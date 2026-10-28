$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Windows.Forms

$dll = Join-Path $PWD 'build/native/Srabon.Tsf/Release/Srabon.Tsf.dll'
$activator = Join-Path $PWD 'build/native/Srabon.Tsf/Release/Srabon.Tsf.Activate.exe'
if (-not (Test-Path $dll)) { throw "TSF DLL missing: $dll" }
if (-not (Test-Path $activator)) { throw "Profile activator missing: $activator" }

$registered = $false
$notepad = $null
try {
    $registration = Start-Process -FilePath "$env:WINDIR\System32\regsvr32.exe" -ArgumentList @('/s', ('"{0}"' -f $dll)) -PassThru -Wait -WindowStyle Hidden
    if ($registration.ExitCode -ne 0) { throw "Srabon TSF registration failed (regsvr32 exit $($registration.ExitCode))." }
    $registered = $true

    $testFile = Join-Path $env:TEMP "Srabon-Tsf-$PID.txt"
    [System.IO.File]::WriteAllText($testFile, '')
    $notepad = Start-Process -FilePath "$env:WINDIR\System32\notepad.exe" -ArgumentList ('"{0}"' -f $testFile) -PassThru
    $shell = New-Object -ComObject WScript.Shell
    $window = $null
    for ($i = 0; $i -lt 30; $i++) {
        Start-Sleep -Milliseconds 500
        $window = Get-Process -Name notepad -ErrorAction SilentlyContinue |
            Where-Object { $_.MainWindowHandle -ne [IntPtr]::Zero } | Select-Object -First 1
        if ($window) { break }
    }
    if (-not $window) { throw 'Notepad did not expose a desktop window.' }

    & $activator
    if ($LASTEXITCODE -ne 0) { throw "Could not activate Srabon profile in the desktop session (exit $LASTEXITCODE)." }
    Start-Sleep -Milliseconds 500
    if (-not $shell.AppActivate($window.Id)) { throw 'Could not focus the Notepad window.' }
    Start-Sleep -Milliseconds 300

    [System.Windows.Forms.SendKeys]::SendWait('ami banglay likhi ')
    [System.Windows.Forms.SendKeys]::SendWait('s')
    [System.Windows.Forms.SendKeys]::SendWait('{UP}')
    [System.Windows.Forms.SendKeys]::SendWait(' ')
    Start-Sleep -Milliseconds 300
    [System.Windows.Forms.SendKeys]::SendWait('^s')
    Start-Sleep -Seconds 1
    $actual = [System.IO.File]::ReadAllText($testFile)
    $actual = $actual.TrimEnd([char[]]@([char]13, [char]10))
    $expected = 'আমি বাংলায় লিখি শ '
    if ($actual -cne $expected) {
        throw "Notepad TSF text mismatch. Expected '$expected', received '$actual'."
    }
    Write-Host "Notepad TSF acceptance passed: $actual"
}
catch {
    $message = $_.Exception.Message -replace '%', '%25' -replace "`r", '%0D' -replace "`n", '%0A'
    Write-Host "::error title=Notepad TSF integration::$message"
    throw
}
finally {
    Get-Process -Name notepad -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
    if ($testFile -and (Test-Path -LiteralPath $testFile)) { Remove-Item -LiteralPath $testFile -Force -ErrorAction SilentlyContinue }
    if ($registered) {
        $unregistration = Start-Process -FilePath "$env:WINDIR\System32\regsvr32.exe" -ArgumentList @('/u', '/s', ('"{0}"' -f $dll)) -PassThru -Wait -WindowStyle Hidden
        if ($unregistration.ExitCode -ne 0) { throw "Srabon TSF unregistration failed (regsvr32 exit $($unregistration.ExitCode))." }
    }
}
