param(
    [Parameter(Mandatory)][string] $ProjectDirectory,
    [Parameter(Mandatory)][string] $LudorkDirectory,
    [Parameter(Mandatory)][string] $OutputDirectory,
    [ValidateSet('package', 'standalone')][string] $Mode = 'package',
    [string] $CompilerPath
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$ci = Join-Path $ProjectDirectory '.github/scripts/package-ci.py'
& python $ci validate-windows-project $ProjectDirectory
if ($LASTEXITCODE -ne 0) { throw 'Windows CI requires the project MSVC compiler setting.' }
if ($Mode -eq 'standalone') {
    if (-not $CompilerPath -or -not [IO.Path]::IsPathFullyQualified($CompilerPath)) {
        throw 'Standalone CI requires -CompilerPath with an absolute clang-cl.exe path.'
    }
    $CompilerPath = (Get-Item -LiteralPath $CompilerPath -ErrorAction Stop).FullName
    if ([IO.Path]::GetFileName($CompilerPath) -ine 'clang-cl.exe') {
        throw 'Standalone CI requires the LLVM clang-cl.exe compiler.'
    }
    foreach ($name in @('clang-cl.exe', 'lld-link.exe', 'llvm-lib.exe')) {
        if (-not (Test-Path -LiteralPath (Join-Path (Split-Path -Parent $CompilerPath) $name) -PathType Leaf)) {
            throw "LLVM installation is missing $name"
        }
    }
    Get-Command ninja.exe -ErrorAction Stop | Out-Null
} elseif ($CompilerPath) {
    throw '-CompilerPath is only supported for Standalone CI; game packaging uses MSVC.'
}

foreach ($variable in @('LUDORK_SERVER_URL', 'LUDORK_SERVER_KEY')) {
    if (-not [Environment]::GetEnvironmentVariable($variable)) {
        throw "Missing required environment variable: $variable"
    }
}
if ($env:LUDORK_SERVER_ENABLED -ne '1') {
    throw 'CI packaging requires LUDORK_SERVER_ENABLED=1.'
}

if ($env:LUDORK_SHA -cnotmatch '^[0-9a-f]{40}$') {
    throw 'BuildTools extraction requires a valid LUDORK_SHA.'
}
$archive = Join-Path $LudorkDirectory "Ludork-BuildTools-windows-x64-$env:LUDORK_SHA.tar.gz"
if (-not (Test-Path -LiteralPath $archive -PathType Leaf)) {
    throw "Ludork BuildTools archive was not found: $archive"
}
& tar.exe -xzf $archive -C $LudorkDirectory
if ($LASTEXITCODE -ne 0) {
    throw "Ludork BuildTools extraction failed with exit code $LASTEXITCODE"
}

foreach ($command in @('cmake', 'cl', 'python')) {
    Get-Command $command -ErrorAction Stop | Out-Null
}
foreach ($relativePath in @(
    'BuildTools/pack_project.bat',
    'BuildTools/build_cpp.bat',
    'BuildTools/build_standalone.bat',
    'BuildTools/Toolchain/ludork-build.exe',
    'BuildTools/Toolchain/ludork-pack.exe',
    'BuildTools/Toolchain/ludork-assets.exe',
    'BuildTools/luac.exe',
    'BuildTools/gnu-make/gnumake.exe'
)) {
    if (-not (Test-Path -LiteralPath (Join-Path $LudorkDirectory $relativePath) -PathType Leaf)) {
        throw "Ludork artifact is missing $relativePath"
    }
}
foreach ($variable in @('PROJECT_SHA', 'LUDORK_SHA', 'LUDORK_CHECKED_SHA', 'LUDORK_RUN_ID', 'LUDORK_ARTIFACT_ID')) {
    if (-not [Environment]::GetEnvironmentVariable($variable)) {
        throw "Missing build metadata: $variable"
    }
}

if ($Mode -eq 'standalone' -and -not (Test-Path -LiteralPath (Join-Path $LudorkDirectory 'BuildTools/pack_standalone.bat') -PathType Leaf)) {
    throw 'This Ludork artifact lacks BuildTools/pack_standalone.bat. Publish a new Ludork BuildTools artifact with Standalone project packaging support.'
}

$toolchain = Join-Path $LudorkDirectory 'BuildTools/Toolchain'
& (Join-Path $toolchain 'ludork-pack.exe') server-config --help *> $null
if ($LASTEXITCODE -ne 0) {
    throw 'This Ludork artifact lacks server-config support. Publish a new Ludork BuildTools artifact with server-config support.'
}
& (Join-Path $toolchain 'ludork-build.exe') native-build --help *> $null
if ($LASTEXITCODE -ne 0) {
    throw 'This Ludork artifact lacks native-build support. Publish a new Ludork BuildTools artifact with native-build support.'
}

$packer = Join-Path $LudorkDirectory 'BuildTools/pack_project.bat'
$env:LUDORK_SERVER_URL = & python $ci server-url
if ($LASTEXITCODE -ne 0) { throw 'Server URL preparation failed.' }
& python $ci clean $ProjectDirectory $OutputDirectory
if ($LASTEXITCODE -ne 0) { throw 'Clean build preparation failed.' }
$projectFile = Join-Path $ProjectDirectory 'Main.proj'
$originalProjectBytes = [IO.File]::ReadAllBytes($projectFile)
try {
    if ($Mode -eq 'standalone') {
        $projectConfig = Get-Content -LiteralPath $projectFile -Raw | ConvertFrom-Json -AsHashtable
        $projectConfig['compiler'] = $CompilerPath
        [IO.File]::WriteAllText($projectFile, ($projectConfig | ConvertTo-Json -Depth 100) + "`n", [Text.UTF8Encoding]::new($false))
        & python $ci validate-windows-project $ProjectDirectory --standalone --compiler-path $CompilerPath
        if ($LASTEXITCODE -ne 0) { throw 'Standalone compiler configuration validation failed.' }
        $standalonePacker = Join-Path $LudorkDirectory 'BuildTools/pack_standalone.bat'
        $standaloneDirectory = Join-Path $OutputDirectory 'ProjectMT3'
        & $standalonePacker $ProjectDirectory $standaloneDirectory
        if ($LASTEXITCODE -ne 0) {
            throw "Ludork Standalone project packaging failed with exit code $LASTEXITCODE"
        }
    } else {
        & $packer --dev --encrypt-data --compile-lua --use-ldpak $ProjectDirectory $OutputDirectory
        if ($LASTEXITCODE -ne 0) {
            throw "Ludork packaging failed with exit code $LASTEXITCODE"
        }
    }

    # Game packages use the application name; Standalone projects use the requested ProjectMT3 directory.
    $packages = @(Get-ChildItem -LiteralPath $OutputDirectory -Directory)
    if ($packages.Count -ne 1) {
        throw 'Expected exactly one game directory in the package output.'
    }
    $gameDirectory = $packages[0].FullName
    foreach ($relativePath in @('Main.exe', 'Binaries/Main.exe')) {
        if (-not (Test-Path -LiteralPath (Join-Path $gameDirectory $relativePath) -PathType Leaf)) {
            throw "Game package is missing $relativePath"
        }
    }
    $metadataOptions = @()
    if ($Mode -eq 'standalone') {
        & python $ci validate-standalone $ProjectDirectory $gameDirectory
        if ($LASTEXITCODE -ne 0) { throw 'Standalone project validation failed.' }
        & $scriptTools preview validate $gameDirectory
        if ($LASTEXITCODE -ne 0) { throw 'Standalone preview validation failed.' }
        $metadataOptions = @('--standalone')
    } else {
        & python $ci validate $gameDirectory
        if ($LASTEXITCODE -ne 0) { throw 'Packed resource validation failed.' }
    }
    if (-not (Get-ChildItem -LiteralPath (Join-Path $gameDirectory 'Binaries') -Filter '*.dll' -File)) {
        throw 'Game package contains no runtime DLLs.'
    }

    & python $ci metadata $ProjectDirectory $OutputDirectory windows-x64 @metadataOptions
    if ($LASTEXITCODE -ne 0) { throw 'Build metadata generation failed.' }
} finally {
    if ($Mode -eq 'standalone') {
        [IO.File]::WriteAllBytes($projectFile, $originalProjectBytes)
    }
}
