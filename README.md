# Srabon

A Windows desktop Bangla phonetic typing application.

## Windows desktop prototype

The primary test experience is the native C# Windows Forms application in `src/Srabon.App`. See the [desktop build and run guide](src/Srabon.App/README.md). A self-contained x64 EXE is built by the [Windows desktop GitHub Actions workflow](.github/workflows/windows-desktop.yml); download its uploaded artifact from the successful workflow run.

The earlier [browser converter harness](prototype/README.md) is retained only as a reference and for its existing JavaScript regression tests. It is not the primary application and is not a system-wide input method.

The merged [design handoff](docs/design/README.md) remains preserved. Stage 2 TSF integration is explicitly not part of this task and is not implemented or claimed as working.
