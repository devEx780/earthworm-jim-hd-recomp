# Packs the Windows build into private/dist/ewj-hd-windows.zip: exe, runtime DLLs, VC++ runtime
# (app-local, from System32, so it is never older than the build toolset) and a launcher.
# No game data and no ewj_hd.toml (defaults: controller = player 1). Copy your own dump into game\.
$ErrorActionPreference = 'Stop'
$root = Resolve-Path (Join-Path $PSScriptRoot '..\..')
$bin = Join-Path $root 'out\build\win-amd64-release'
$stage = Join-Path $root 'private\dist\ewj-hd-windows'
$zip = Join-Path $root 'private\dist\ewj-hd-windows.zip'
if (Test-Path $stage) { Remove-Item $stage -Recurse -Force }
New-Item -ItemType Directory -Force (Join-Path $stage 'game') | Out-Null
Copy-Item (Join-Path $bin 'ewj_hd.exe'), (Join-Path $bin 'rexruntime.dll'), (Join-Path $bin 'rexgpu-xenos.dll') $stage
foreach ($dll in 'msvcp140.dll', 'msvcp140_atomic_wait.dll', 'vcruntime140.dll', 'vcruntime140_1.dll') {
  Copy-Item (Join-Path $env:SystemRoot "System32\$dll") $stage
}
Copy-Item (Join-Path $root 'launchers\play.cmd') $stage
if (Test-Path $zip) { Remove-Item $zip -Force }
Compress-Archive -Path $stage -DestinationPath $zip
Get-Item $zip | Select-Object FullName, Length
