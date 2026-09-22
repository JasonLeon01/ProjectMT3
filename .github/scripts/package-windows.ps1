param(
    [Parameter(Mandatory)][string] $ProjectDirectory,
    [Parameter(Mandatory)][string] $LudorkDirectory,
    [Parameter(Mandatory)][string] $OutputDirectory
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

foreach ($command in @('cmake', 'ninja', 'cl')) {
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
foreach ($variable in @('PROJECT_SHA', 'LUDORK_SHA', 'LUDORK_RUN_ID', 'LUDORK_ARTIFACT_ID', 'PACKAGE_BUILD_INFO')) {
    if (-not [Environment]::GetEnvironmentVariable($variable)) {
        throw "Missing build metadata: $variable"
    }
}

$packer = Join-Path $LudorkDirectory 'tools/pack_project.bat'
& $packer $ProjectDirectory $OutputDirectory
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
foreach ($relativePath in @('Assets', 'Data', 'Scripts')) {
    if (-not (Test-Path -LiteralPath (Join-Path $gameDirectory $relativePath) -PathType Container)) {
        throw "Game package is missing $relativePath"
    }
}
if (-not (Get-ChildItem -LiteralPath (Join-Path $gameDirectory 'Binaries') -Filter '*.dll' -File)) {
    throw 'Game package contains no runtime DLLs.'
}

$cacheInfo = Get-Content -LiteralPath $env:PACKAGE_BUILD_INFO -Raw | ConvertFrom-Json
[ordered]@{
    project_commit = $env:PROJECT_SHA
    engine_tree = $cacheInfo.engine_hash
    ludork_commit = $env:LUDORK_SHA
    ludork_checked_commit = $cacheInfo.ludork_checked_sha
    ludork_run_id = $env:LUDORK_RUN_ID
    ludork_artifact_id = $env:LUDORK_ARTIFACT_ID
    ludork_run_url = "https://github.com/JasonLeon01/Ludork/actions/runs/$env:LUDORK_RUN_ID"
    configuration = 'Release'
    platform = 'windows-x64'
    tools_cache_hit = $cacheInfo.tools_cache_hit
    tools_cache_reason = $cacheInfo.tools_reason
    build_cache_hit = $cacheInfo.build_cache_hit
    build_cache_reason = $cacheInfo.build_reason
    build_environment_hash = $cacheInfo.environment_hash
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $OutputDirectory 'build-info.json') -Encoding utf8
