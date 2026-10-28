$ErrorActionPreference = 'Stop'
Add-Type @'
using System;
using System.Runtime.InteropServices;
public static class SrabonTestInput {
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr window);
    [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr window, out uint processId);
    [DllImport("user32.dll")] public static extern void keybd_event(byte virtualKey, byte scanCode, uint flags, UIntPtr extraInfo);
    public static void Down(byte key) { keybd_event(key, 0, 0, UIntPtr.Zero); }
    public static void Up(byte key) { keybd_event(key, 0, 2, UIntPtr.Zero); }
    public static void Tap(byte key) { Down(key); Up(key); }
}
'@

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
    [void][SrabonTestInput]::SetForegroundWindow($window.MainWindowHandle)
    Start-Sleep -Milliseconds 300
    $foreground = [SrabonTestInput]::GetForegroundWindow()
    [uint32]$foregroundProcess = 0
    [void][SrabonTestInput]::GetWindowThreadProcessId($foreground, [ref]$foregroundProcess)
    if ($foregroundProcess -ne $window.Id) { throw "Notepad did not retain foreground focus (foreground PID $foregroundProcess, expected $($window.Id))." }
    # InstallLayoutOrTip enabled the profile; switch the focused Notepad thread to it as a user would.
    [SrabonTestInput]::Down(0x5B) # Left Windows key
    [SrabonTestInput]::Tap(0x20)  # Win+Space cycles to Srabon Bangla Phonetic
    [SrabonTestInput]::Up(0x5B)
    Start-Sleep -Milliseconds 500

    foreach ($character in 'ami banglay likhi '.ToCharArray()) {
        if ($character -eq ' ') { [SrabonTestInput]::Tap(0x20) }
        else { [SrabonTestInput]::Tap([byte][char]::ToUpperInvariant($character)) }
    }
    [SrabonTestInput]::Tap(0x53) # S: candidate with two choices
    [SrabonTestInput]::Tap(0x26) # Up: select the second candidate
    [SrabonTestInput]::Tap(0x20) # Space: commit
    Start-Sleep -Milliseconds 300
    [SrabonTestInput]::Down(0x11) # Ctrl+S saves the Notepad test document.
    [SrabonTestInput]::Tap(0x53)
    [SrabonTestInput]::Up(0x11)
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
