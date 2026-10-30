param(
    [Parameter(Mandatory)][string] $ProjectDirectory,
    [Parameter(Mandatory)][string] $LudorkDirectory,
    [Parameter(Mandatory)][string] $OutputDirectory,
    [ValidateSet('package', 'standalone')][string] $Mode = 'package'
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$ci = Join-Path $ProjectDirectory '.github/scripts/package-ci.py'
& python $ci validate-windows-project $ProjectDirectory
if ($LASTEXITCODE -ne 0) { throw 'Windows CI requires the project MSVC compiler setting.' }

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

foreach ($command in @('cmake', 'cl', 'python')) {
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

if ($Mode -eq 'standalone' -and -not (Test-Path -LiteralPath (Join-Path $LudorkDirectory 'tools/pack_standalone.bat') -PathType Leaf)) {
    throw 'This Ludork artifact lacks tools/pack_standalone.bat. Publish a new Ludork tools artifact with Standalone project packaging support.'
}

$scriptTools = Join-Path $LudorkDirectory 'tools/ScriptTools/ScriptTools.exe'
foreach ($command in @('server-config', 'windows-native-build')) {
    & $scriptTools $command --help *> $null
    if ($LASTEXITCODE -ne 0) {
        throw "This Ludork artifact lacks $command support. Publish a new Ludork tools artifact with $command support."
    }
}

$packer = Join-Path $LudorkDirectory 'tools/pack_project.bat'
$env:LUDORK_SERVER_URL = & python $ci server-url
if ($LASTEXITCODE -ne 0) { throw 'Server URL preparation failed.' }
& python $ci clean $ProjectDirectory $OutputDirectory
if ($LASTEXITCODE -ne 0) { throw 'Clean build preparation failed.' }
if ($Mode -eq 'standalone') {
    $standalonePacker = Join-Path $LudorkDirectory 'tools/pack_standalone.bat'
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
