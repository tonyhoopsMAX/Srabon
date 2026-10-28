# Srabon converter harness (Stage 1)

**Status:** The converter and browser harness are implemented. The automated Node.js test suite has been run in the current Linux environment; see the recorded command/results in the delivery summary. This is a browser-openable test harness, not a Windows executable and not a Windows input method.

## Run on Windows

No package installation or compilation is needed to use the UI:

1. Copy/checkout this repository on a Windows 11 machine.
2. Open `prototype/index.html` in Edge, Chrome, or another current browser (double-click it in Explorer, or run `Start-Process .\index.html` from PowerShell while in this directory).
3. Type Roman text in the input area. Click a candidate to change the output. **Load sample** restores the exact regression example.

The page uses local files only: it makes no network requests and does not read/write the clipboard. A local server is optional; from this directory run `py -m http.server 8000`, then open `http://127.0.0.1:8000` in the same Windows machine's browser.

## Run automated tests

Install a current Node.js LTS release on the test machine, then from this directory:

```powershell
npm test
```

No npm dependencies are used. Equivalently, run:

```powershell
node --test tests/converter.test.js
```

The test suite covers the exact reviewed fixture, sample word pieces, longest digraph handling, case-sensitive retroflex notation, whitespace/punctuation preservation, same-input candidate alternatives, bounded deterministic candidate generation, empty input, and literal passthrough.

## Converter scope and limitations

The implementation is a deliberately small, deterministic rule table. It includes basic vowels/consonants, selected digraphs, `ng`, and a small set of rule alternatives. It does **not** establish a reviewed Bangla phonetic standard, nor does it claim broad conversion accuracy. It has no contextual predictive suggestions, spell correction, learning, or user dictionary. Unknown characters pass through. A human-reviewed word/phrase corpus and approved phonetic conventions are required before judging quality.

The exact regression test is:

```text
ami banglay likhi -> আমি বাংলায় লিখি
```

The harness only previews strings in its own browser page. It does not insert text into Notepad or any other app.

## Stage 2 — native TSF feasibility (not implemented or tested)

Stage 2 is intentionally separate from this runnable converter harness. No TSF DLL, Windows installer, service registration, or Windows Notepad test is included yet.

A real system-wide input method should use a TSF text service. Microsoft describes the text service as a registered in-process COM server and requires an application/profile registration path; a service needs to implement a TSF text-input processor and register a language profile for the language it supports. Follow the current [TSF architecture](https://learn.microsoft.com/en-us/windows/win32/tsf/architecture), [text-service registration](https://learn.microsoft.com/en-us/windows/win32/tsf/text-service-registration), and [custom IME requirements](https://learn.microsoft.com/en-us/windows/apps/develop/input/input-method-editor-requirements) documentation. Do not write directly to the registry to force Srabon as the default input profile; activation should be user-controlled through Windows input settings/profile selection.

A production-oriented TSF service must be code-signed before distribution. Microsoft specifically calls out signatures because a registered text service can be loaded into application processes. Keep the service small and local; avoid networking and unrelated settings/update logic inside the in-process component. See [TSF security considerations](https://learn.microsoft.com/en-us/windows/win32/tsf/tsf-security-information). No signing certificate or Windows installer is available in this environment.

The eventual installation path must install the COM in-process server, register the text service with TSF (including its language profile), and offer clean unregister/removal on uninstall. Windows/profile activation should remain user-controlled through the Windows input/profile UI; Srabon must not silently force itself as the default. Installer failure and rollback need tests. See Microsoft's [registration API guidance](https://learn.microsoft.com/en-us/windows/win32/tsf/text-service-registration) and [custom IME requirements](https://learn.microsoft.com/en-us/windows/apps/develop/input/input-method-editor-requirements).

Stage 2 needs a Windows build/test host with the Windows SDK and MSVC (or another verified compiler setup with complete TSF headers/libs), an install/registration harness, and real Notepad testing. Validation must include composition/pre-edit, candidate selection, commit, Backspace/space/punctuation, focus loss, Windows profile activation, and uninstall. Testing only this converter page is not evidence that a TIP works system-wide.

## Next engineering gate

The current environment did not include Windows, the Windows SDK, MSVC, MinGW, or Wine. Consequently a Windows `.exe`/`.dll` could not be compiled or exercised here. Do not describe Stage 2 as implemented, built, signed, installed, or working until it is built and tested on Windows. First run the harness and tests, then perform the TSF work as a separate Windows-verifiable milestone.
