param(
    [Parameter(Mandatory)][string] $ProjectDirectory,
    [Parameter(Mandatory)][string] $LudorkDirectory,
    [Parameter(Mandatory)][string] $OutputDirectory,
    [ValidateSet('package', 'standalone')][string] $Mode = 'package'
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

foreach ($variable in @('LUDORK_SERVER_URL', 'LUDORK_SERVER_KEY')) {
    if (-not [Environment]::GetEnvironmentVariable($variable)) {
        throw "Missing required environment variable: $variable"
    }
}
if ($env:LUDORK_SERVER_ENABLED -ne '1') {
    throw 'CI packaging requires LUDORK_SERVER_ENABLED=1.'
}

$archives = @(Get-ChildItem -LiteralPath $LudorkDirectory -Filter 'Ludork-editor-*-windows-x64.7z' -File)
if ($archives.Count -gt 0) {
    if ($archives.Count -ne 1) {
        throw 'Expected exactly one Ludork Windows editor archive.'
    }
    & "$env:ProgramFiles\7-Zip\7z.exe" x $archives[0].FullName "-o$LudorkDirectory" -y
    if ($LASTEXITCODE -ne 0) {
        throw "Ludork editor extraction failed with exit code $LASTEXITCODE"
    }
}

foreach ($command in @('cmake', 'ninja', 'cl', 'python')) {
    Get-Command $command -ErrorAction Stop | Out-Null
}
foreach ($relativePath in @(
    'tools/pack_project.bat',
    'tools/build_cpp.bat',
    'tools/build_standalone.bat',
    'tools/ScriptTools/ScriptTools.exe',
    'tools/luac.exe',
    'tools/gnu-make/gnumake.exe'
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

$scriptTools = Join-Path $LudorkDirectory 'tools/ScriptTools/ScriptTools.exe'
& $scriptTools server-config --help *> $null
if ($LASTEXITCODE -ne 0) {
    throw 'This Ludork artifact lacks server-config support. Publish a new Ludork tools artifact with server-config support.'
}

$packer = Join-Path $LudorkDirectory 'tools/pack_project.bat'
$ci = Join-Path $ProjectDirectory '.github/scripts/package-ci.py'
$env:LUDORK_SERVER_URL = & python $ci server-url
if ($LASTEXITCODE -ne 0) { throw 'Server URL preparation failed.' }
& python $ci clean $ProjectDirectory $OutputDirectory
if ($LASTEXITCODE -ne 0) { throw 'Clean build preparation failed.' }
if ($Mode -eq 'standalone') {
    $standaloneBuilder = Join-Path $LudorkDirectory 'tools/build_standalone.bat'
    $standaloneDirectory = Join-Path $OutputDirectory 'ProjectMT3'
    try {
        & $standaloneBuilder $ProjectDirectory $standaloneDirectory Release
        if ($LASTEXITCODE -ne 0) {
            throw "Ludork standalone build failed with exit code $LASTEXITCODE"
        }
    } finally {
        & $scriptTools server-config cleanup $ProjectDirectory
        if ($LASTEXITCODE -ne 0) { throw 'Server configuration cleanup failed.' }
    }
} else {
    & $packer --dev --encrypt-data --compile-lua --use-ldpak $ProjectDirectory $OutputDirectory
    if ($LASTEXITCODE -ne 0) {
        throw "Ludork packaging failed with exit code $LASTEXITCODE"
    }
}

# Ludork chooses the game directory name from the project's application name.
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
    & python $ci prepare-standalone $ProjectDirectory $gameDirectory
    if ($LASTEXITCODE -ne 0) { throw 'Standalone project preparation failed.' }
    & $scriptTools ui-preview validate $gameDirectory
    if ($LASTEXITCODE -ne 0) { throw 'Standalone UI preview validation failed.' }
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
