# Basic screens and user flows

**Basic release scope:** four surfaces and the essential language/input workflow. The screen descriptions define intent; they do not assert that native input, tray integration, or candidate placement already works.

## Shared conventions

- Use the text-only **Srabon** wordmark; no app icon in this phase.
- Use the single working tagline “Bangla typing, made simple.” only where a tagline is useful (first-run setup is the preferred location).
- Quiet Fluent is the visual foundation: calm neutral surfaces, restrained teal, clear hierarchy, compact but readable controls.
- Show the current language in text. Bangla script may accompany a language name only after that label is human-reviewed.
- The only specified illustrative example is `ami banglay likhi` → `আমি বাংলায় লিখি`.
- Transliteration candidates are alternate outputs for the current Roman input. Predictive word suggestions are a separate, context-inferred feature and are not promised in Basic V1.

## 1. First-run setup

**Goal:** explain what Srabon does and get a new user to a usable state with minimal effort.

**Layout:** small, focused setup window with wordmark, one purpose sentence, short explanation, primary Continue/Finish action, and an unobtrusive Skip/Not now path where skipping is safe. Use a progress indication only if more than one required step exists. No account creation.

**Content rules:**
- State the core promise in plain language: Bangla phonetic typing from English/Latin input, subject to the final supported input behavior being validated.
- Explain the language-switch shortcut only after it is confirmed; display the real shortcut in a keycap-style control.
- Startup or default-input choices are optional and should appear only if they are implemented, understandable, and reversible. Do not preselect a consequential choice without explicit explanation.
- Avoid unsupported claims such as “works everywhere,” “smart suggestions,” or guaranteed compatibility.

**Flow:** first launch → welcome/explanation → (optional, only when necessary) input registration/permission status → completion → clear “how to switch language” recap once shortcut is approved → close setup and keep Srabon available from its actual entry point.

**States:** normal; busy while OS registration is in progress; success with a clear next step; recoverable registration failure with retry and Help; skipped setup with a route back to settings/help. Do not show a successful registration state unless the OS confirms it.

## 2. System-tray controls

**Goal:** show current input mode and make switching and basic access straightforward.

**Compact menu hierarchy:**
1. Srabon label and explicit current mode (Bangla or English).
2. Switch action, showing the confirmed keyboard shortcut once known.
3. Basic Settings.
4. Help/About.
5. Exit Srabon, only if the tray utility is the actual running process and exit behavior is defined.

Menu item order and wording must be consistent. Avoid visual-only mode indication. If native Windows input APIs expose a separate system status indicator, do not imply Srabon's tray menu is itself composing text. The app must explain when an input switch could not be applied.

**States:** Bangla active; English active; switching/in-progress if the platform has an asynchronous state change; unavailable/error state with reason and recovery; disabled item states with a reason if relevant. Keyboard invocation, focus return, and dismissal behavior need native validation.

## 3. Typing, composition, and candidate preview

**Goal:** demonstrate the defining typing workflow with clear separation between in-progress composition and committed text.

**Illustrative workflow:** user types `ami banglay likhi`; the composition/pre-edit state may expose that Roman string while the engine proposes Bangla output; user accepts the intended output; committed text is `আমি বাংলায় লিখি`. This is a design example, not a claim about current software behavior.

**Required behavior definition before implementation:**
- **Roman pre-edit:** define whether it appears inline in the focused application, in a native composition string, or in another architecture-supported form. If the chosen input method does not expose a separate Roman pre-edit, do not draw a permanent Roman line in the candidate mockup.
- **Candidate set:** show alternate transliterations for the current composition only. Identify selection clearly, expose candidate count/order where feasible, and allow keyboard navigation (proposed: Up/Down or dedicated keys, then Enter; exact keys depend on native integration and must be tested). Number-key shortcuts are optional and must not conflict with host applications.
- **Commit:** define when candidate selection commits and how the composition is finalized. Enter should accept the selected candidate only when that behavior is safe in the host app; otherwise define a conflict-safe alternative.
- **Space:** determine if it commits the current candidate, inserts a space after commit, advances a candidate, or has context-dependent behavior. The rule must be consistent and documented before launch.
- **Backspace:** edit the active Roman composition before affecting already committed text. Verify behavior when the caret is not at the end or the host app owns a selection.
- **Punctuation:** specify whether punctuation commits composition first, passes through, or participates in transliteration. Preserve expected host application behavior.
- **Cursor movement / selection:** define how Left/Right, Home/End, selection replacement, and caret relocation resolve or cancel composition without losing/duplicating text.
- **Mixed language:** switching input mode should not retroactively alter committed text. Define whether an in-progress composition is committed or canceled on mode switch, focus loss, and application changes.
- **Predictive suggestions:** excluded from Basic by default. If separately approved later, show these in a distinguishable area and never confuse them with transliteration candidates.

**Candidate surface design:** compact floating panel anchored relative to the active composition/caret if the native architecture allows. Candidate selection must not steal focus from the text control unless the platform's input API requires it and has a safe return path. Placement, anchor fidelity, clipping, monitor-edge behavior, z-order, multi-monitor handling, DPI scaling, and app compatibility are blockers for visual sign-off; mockups cannot establish these behaviors.

**States:** no active composition (candidate surface hidden); composition with candidates; selected candidate; one/no candidate; conversion error/fallback; app/focus unavailable; surface cannot be placed; disabled mode. Recovery should preserve user text and make clear whether composition remains pending. No invented Bangla candidate strings beyond the approved example.

## 4. Basic settings

**Goal:** expose only essential first-release controls, without making a new user manage engine internals.

**Proposed grouping:**
- **General:** input method/mode status (read-only where the method is fixed); Start Srabon with Windows toggle only if supported; current app version and About/Help.
- **Input:** essential candidate-window visibility or composition behavior only if needed and fully implemented. A setting must include a concise explanation and a useful default.
- **Switching:** language-switch shortcut display; configurability only if collision handling, validation, reset/recovery, and OS registration behavior are supported.

Use clear Apply/Save behavior if settings are staged; otherwise apply immediately and show the resulting state. Avoid redundant Save buttons for immediate settings. Startup toggle failures should explain the OS-level issue and preserve the prior state. Shortcut capture should support Escape to cancel and recover from reserved/conflicting key combinations.

**Out of Basic scope:** custom dictionaries, rule editors, profiles, theme editors, advanced productivity features, and predictive suggestions unless explicitly re-scoped through product review. The information architecture may reserve an extension point for Advanced later, but should not show a disabled Advanced tab or empty category in Basic.

## End-to-end flows

### First launch to first committed text
1. Explain purpose in setup.
2. Enable/register only the confirmed Windows input component; communicate errors truthfully.
3. Show current mode and confirmed switch shortcut.
4. User enters Roman input in a supported host app.
5. Composition/candidates appear only as supported by the chosen native integration.
6. User selects and commits output; verify actual inserted text and that ordinary host editing remains intact.
7. User can reach settings/help from a confirmed app entry point.

### Mode switching
1. Invoke the confirmed shortcut or tray switch.
2. Display the new mode in a textual status.
3. Define pending-composition behavior and announce failure if the state did not change.

### Recovery
If input registration, shortcut registration, candidate display, or startup configuration fails, retain a route to help/settings, offer a safe retry where appropriate, and avoid claiming the feature is active. Text already committed must not be silently rewritten or lost.
