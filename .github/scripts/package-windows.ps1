param(
    [Parameter(Mandatory)][string] $ProjectDirectory,
    [Parameter(Mandatory)][string] $LudorkDirectory,
    [Parameter(Mandatory)][string] $OutputDirectory
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

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

$packer = Join-Path $LudorkDirectory 'tools/pack_project.bat'
$ci = Join-Path $ProjectDirectory '.github/scripts/package-ci.py'
& python $ci clean $ProjectDirectory $OutputDirectory
if ($LASTEXITCODE -ne 0) { throw 'Clean build preparation failed.' }
& $packer --dev --encrypt-data --compile-lua --use-ldpak $ProjectDirectory $OutputDirectory
if ($LASTEXITCODE -ne 0) {
    throw "Ludork packaging failed with exit code $LASTEXITCODE"
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
& python $ci validate $gameDirectory
if ($LASTEXITCODE -ne 0) { throw 'Packed resource validation failed.' }
if (-not (Get-ChildItem -LiteralPath (Join-Path $gameDirectory 'Binaries') -Filter '*.dll' -File)) {
    throw 'Game package contains no runtime DLLs.'
}

& python $ci metadata $ProjectDirectory $OutputDirectory windows-x64
if ($LASTEXITCODE -ne 0) { throw 'Build metadata generation failed.' }
