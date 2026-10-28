param(
    [Parameter(Mandatory = $true)]
    [string] $DllPath
)
$ErrorActionPreference = 'Stop'

$identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = [Security.Principal.WindowsPrincipal]::new($identity)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    throw 'Run Install.ps1 from an elevated PowerShell session.'
}
if (-not (Test-Path -LiteralPath $DllPath -PathType Leaf)) { throw "TSF DLL not found: $DllPath" }

$destination = Join-Path $env:ProgramFiles 'Srabon\TSF'
New-Item -ItemType Directory -Path $destination -Force | Out-Null
$installedDll = Join-Path $destination 'Srabon.Tsf.dll'
Copy-Item -LiteralPath $DllPath -Destination $installedDll -Force

try {
    & "$env:WINDIR\System32\regsvr32.exe" /s $installedDll
    if ($LASTEXITCODE -ne 0) { throw "TSF profile registration failed (regsvr32 exit $LASTEXITCODE)." }
    Write-Host "Srabon Bangla phonetic TSF profile installed for $([Environment]::UserName)."
    Write-Host 'Select Srabon Bangla Phonetic from the Windows input-language switcher (Win+Space).'
} catch {
    Remove-Item -LiteralPath $installedDll -Force -ErrorAction SilentlyContinue
    throw
}
