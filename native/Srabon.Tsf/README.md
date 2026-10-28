# Srabon native TSF feasibility prototype

This directory is the first native Windows Text Services Framework (TSF) implementation. It is a COM in-process keyboard text service, not a wrapper around the WinForms converter and not a simulated-keystroke production tool. The TSF manager delivers key events; the service inserts and updates an `ITfComposition` through synchronous TSF edit sessions; Space/Enter commit it; Backspace edits Roman composition; Up/Down cycles bounded transliteration candidates.

The native transliterator is a C++ port of the existing provisional C# rule table. The required regression case is `ami banglay likhi` → `আমি বাংলায় লিখি`. Candidate options are deterministic alternatives, not context-predicted suggestions. Candidate navigation updates the active composition in-place; this feasibility slice does not yet draw a candidate-list UI.

## Build and test (Windows x64, Visual Studio C++ tools + CMake)

```powershell
cmake -S native/Srabon.Tsf -B build/native/Srabon.Tsf -A x64
cmake --build build/native/Srabon.Tsf --config Release
ctest --test-dir build/native/Srabon.Tsf -C Release --output-on-failure
```

## Register for a local prototype test

Close apps where you plan to test, then use an elevated PowerShell session:

```powershell
.\native\Srabon.Tsf\Install.ps1 -DllPath .\build\native\Srabon.Tsf\Release\Srabon.Tsf.dll
```

Choose **Srabon Bangla Phonetic** with Win+Space, open Notepad, type Roman text, use Backspace while composing, Up/Down to cycle candidates, and Space/Enter to commit. Uninstall with:

```powershell
.\native\Srabon.Tsf\Uninstall.ps1
```

The scripts use per-user TSF profile registration and put the x64 DLL under `%ProgramFiles%\Srabon\TSF`. Close all hosting applications before uninstalling because Windows can keep an in-process text-service DLL loaded until those applications exit.

## Feasibility limits (not production sign-off)

- This is an unsigned x64-only prototype. Microsoft requires third-party IMEs to be digitally signed; production packaging needs signing and Windows validation. A 32-bit companion DLL is also required for 32-bit host applications on 64-bit Windows.
- The profile currently relies on the Windows language ID `0x0845` (Bengali Bangladesh). Real-world language-pack/input-switcher behavior must be verified on supported Windows releases.
- No installer executable, signed release package, polished candidate UI, accessibility/touch/secure-desktop compatibility, or clean-uninstall validation across all hosts is claimed by this prototype.
- The Notepad integration test in CI uses UI automation to send key events into a real Notepad window; the production DLL itself uses only TSF key events, compositions, and edit sessions (no SendInput or clipboard path). On 2026-10-28, the x64 CMake build and native converter regression passed, but the Notepad test did **not** pass: an earlier run saved `ami banglay likhi s ` unchanged in Notepad, and the current `bn-BD` language/profile activation path returned `E_FAIL` (`0x80004005`). The runner warned that its language-list change takes effect only after the next sign-in. Therefore composition, candidate navigation, and Bangla commit in Notepad are still unverified. The shortest retest is an interactive Windows account with the Bengali (Bangladesh) input language installed and active after sign-out/sign-in, then select Srabon from Win+Space.
- The CI artifact is an unsigned x64 feasibility ZIP containing the TSF DLL and PowerShell setup/removal scripts; it is not a signed release installer.
