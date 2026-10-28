# Implementation notes and validation plan

This document is a list of design-facing engineering dependencies and validation gates. It does not choose a Windows input architecture and does not claim any prototype or compatibility tests have already passed.

## Architecture-sensitive boundaries

1. **Windows support matrix:** decide minimum and supported Windows versions/builds before choosing APIs, fonts, packaging, startup behavior, or accessibility baselines.
2. **Native input integration:** select and prototype the system-wide input method/TSF or other native approach. Design assumptions about pre-edit, commit, focus, application interoperability, and candidate UI depend on this choice.
3. **Shortcut registration:** select the default language-switch shortcut, determine whether it is configurable, detect reserved/conflicting shortcuts, handle registration failure, and define how active state is displayed. The exact shortcut is unresolved.
4. **Candidate surface:** prototype positioning relative to caret/pre-edit, monitor boundaries, focus behavior, z-order, DPI scaling, text scaling, and focus restoration. Validate in a representative set of target applications; compatibility cannot be inferred from a mockup.
5. **Basic feature scope:** decide whether predictive suggestions ship in Basic V1. The default in this handoff is no; do not conflate suggestions with current-input transliteration candidates.
6. **Themes:** decide whether dark mode ships in V1 or follows after the core workflow is proven. Light is the visual default in this specification. If dark ships, validate parity and Windows preference/contrast behavior.

## Prototype validation sequence

### Gate A — environment and API feasibility
- Publish Windows version/build support target.
- Confirm input architecture can enable/disable the mode and report actual active state.
- Confirm packaging, registration, startup, update, and uninstall implications.
- Verify language and Bangla glyph rendering with a supported-font plan.

### Gate B — composition semantics
In a native test harness, document actual behavior for:
- Roman pre-edit visibility and composition lifecycle.
- Candidate generation and candidate traversal/commit.
- Enter, Space, Backspace, punctuation, cursor movement, selection replacement, mode change, and focus loss.
- Mixed English/Bangla editing and preservation of already committed text.
- Host applications with different text frameworks, privilege levels, and input models.

Use the approved sample `ami banglay likhi` → `আমি বাংলায় লিখি` as one test case, not as proof of general transliteration quality.

### Gate C — candidate-window behavior
- Anchor to composition/caret if supported; test when the caret is near each screen edge.
- Test multi-monitor layouts, differing monitor scale factors, 100%/150%/200% scaling, and text enlargement.
- Verify focus does not get stranded or stolen and that dismissal restores the intended focus.
- Test clipping, z-order, popup dismissal, accessibility tree exposure, screen-reader navigation, and windowed/full-screen app behavior.
- Define a graceful fallback if a host application does not provide a reliable caret anchor.

### Gate D — settings/setup and accessibility
- Verify startup and shortcut settings reflect the OS's true state and recover from registration failure.
- Verify setup Skip path, return access, and first successful typing flow.
- Test keyboard-only operation, Windows Narrator (or a documented target screen reader), contrast/high-contrast settings, and color-blind state comprehension.
- Confirm production English and Bangla strings with human review and test localized text expansion.

## Testing matrix (to be filled with actual results)

| Area | Minimum evidence before design sign-off | Status |
|---|---|---|
| Supported Windows versions | Published product support matrix and API checks | Not tested / decision open |
| Input registration and mode changes | Native prototype reports success/failure accurately | Not tested |
| Composition and commit | Host-app matrix covers documented key interactions | Not tested |
| Candidate placement/focus | Monitor-edge, multi-monitor, DPI, and focus tests | Not tested |
| Bangla rendering | Glyph/shaping checks on supported OS/font combinations | Not tested |
| Accessibility | Keyboard, screen reader, scaling, contrast-mode review | Not tested |
| Shortcut conflicts | Registration failures and user recovery exercised | Not tested |
| Light/dark scope | Product decision plus contrast/state review for shipped themes | Decision open |

Do not change “Not tested” or “Decision open” without test evidence or an approved decision record.

## Engineering handoff inputs

Before implementation planning is considered complete, attach or link:
- A supported Windows version/build matrix.
- A short architecture note for native input integration and its focus/pre-edit constraints.
- A resolved shortcut decision and conflict strategy.
- A composition/candidate interaction table based on prototype behavior.
- Candidate-window behavior test results including DPI, monitors, focus, and app compatibility.
- Font coverage and licensing decision.
- Predictive-suggestion and dark-theme scope decisions.
