param(
    [Parameter(Mandatory)][string] $ManifestPath,
    [Parameter(Mandatory)][string] $ProjectDirectory
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$root = [IO.Path]::GetFullPath($ProjectDirectory).TrimEnd('\', '/') + [IO.Path]::DirectorySeparatorChar
$entries = Get-Content -LiteralPath $ManifestPath -Raw | ConvertFrom-Json
foreach ($entry in $entries) {
    $file = [IO.Path]::GetFullPath([IO.Path]::Combine($root, $entry.path))
    if (-not $file.StartsWith($root, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Timestamp path is outside the project: $file"
    }
    [IO.File]::SetLastWriteTimeUtc($file, [DateTime]::FromFileTimeUtc([long] $entry.time))
}
