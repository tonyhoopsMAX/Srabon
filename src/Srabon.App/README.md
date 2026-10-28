# Srabon Windows desktop prototype

This project is the primary Stage 1 test interface: a native Windows Forms desktop window. It is not a TSF input method and does not type into other applications. The converter is offline; the only clipboard operation is the explicit **Copy output** button.

## Requirements

- Windows 11 x64 for running the application.
- .NET 10 SDK for local build/run, or download the self-contained `win-x64` artifact from a successful Windows desktop workflow run.
- No app icon is included. Bangla display uses an installed Windows Bangla-capable font where available (Nirmala UI, then Vrinda, then Segoe UI fallback).

## Local build and run (PowerShell)

Run these from the repository root:

```powershell
dotnet restore tests/Srabon.Converter.Tests/Srabon.Converter.Tests.csproj
dotnet run --project tests/Srabon.Converter.Tests/Srabon.Converter.Tests.csproj --configuration Release
dotnet run --project src/Srabon.App/Srabon.App.csproj --configuration Release
```

To publish a self-contained, single-file x64 executable:

```powershell
dotnet publish src/Srabon.App/Srabon.App.csproj `
  --configuration Release `
  --runtime win-x64 `
  --self-contained true `
  -p:PublishSingleFile=true `
  -p:IncludeNativeLibrariesForSelfExtract=true `
  -p:EnableCompressionInSingleFile=true `
  --output artifacts/publish

.\artifacts\publish\Srabon.exe
```

The app uses native Windows Forms controls. Type Romanized text to see a live conversion and candidate list; use arrow keys to move through the candidate list and Enter/Space according to normal ListBox/button behavior. **Load sample** fills the exact fixture `ami banglay likhi`; choosing a candidate updates the preview. **Copy output** writes only the selected output to the clipboard after explicit user action. **Clear input** empties the Roman field.

## Automated tests

The no-third-party-package C# test runner covers the exact fixture, mapped components, digraph matching, casing, punctuation, alternative candidates, candidate ordering/limit, empty input, literal passthrough, and candidate-selection behavior. The existing JavaScript tests remain in `prototype/tests` and are run by CI too.

```powershell
dotnet run --project tests/Srabon.Converter.Tests/Srabon.Converter.Tests.csproj --configuration Release
npm test --prefix prototype
```

## Provisional conversion behavior

The converter is a small port of the previous harness rule table, not an approved complete romanization standard. It uses selected vowel/consonant digraphs, treats `ng` as a nasal mark with an alternate, and offers limited same-input alternatives for `s` and `j`. Final `ay` is rendered as `য়` to satisfy the reviewed fixture. These rules are provisional; the sample and test suite do not establish broad Bangla transliteration accuracy. Predictive suggestions, learning, spell correction, user dictionaries, and TSF input integration are not included.

## Build status

The development sandbox is Linux and has no .NET SDK, Windows SDK, MSVC, or Windows runtime, so it cannot locally build or launch this WinForms application. The GitHub Actions workflow uses a Windows runner to restore, test, publish, verify, and upload `Srabon.exe`. Do not report a Windows build or downloadable artifact as successful until the workflow has actually completed successfully and the artifact is visible on that run.
