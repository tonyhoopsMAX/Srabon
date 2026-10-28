$ErrorActionPreference = 'Stop'
$identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = [Security.Principal.WindowsPrincipal]::new($identity)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    throw 'Run Uninstall.ps1 from an elevated PowerShell session.'
}

$installedDll = Join-Path $env:ProgramFiles 'Srabon\TSF\Srabon.Tsf.dll'
if (Test-Path -LiteralPath $installedDll) {
    & "$env:WINDIR\System32\regsvr32.exe" /u /s $installedDll
    if ($LASTEXITCODE -ne 0) { throw "TSF profile removal failed (regsvr32 exit $LASTEXITCODE)." }
    try {
        Remove-Item -LiteralPath (Split-Path -Parent $installedDll) -Recurse -Force
    } catch {
        throw "The Srabon TSF profile was unregistered, but its files are still in use. Close apps that may host the input service, then remove '$installedDll'."
    }
}
Write-Host 'Srabon Bangla phonetic TSF profile and files removed.'
