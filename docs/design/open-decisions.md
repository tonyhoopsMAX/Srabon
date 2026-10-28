# Open decisions and sign-off gates

These questions are intentionally not resolved by visual reference. Assign an owner and record evidence/decision before implementation readiness. **No launch compatibility, shortcut, or candidate-window behavior is approved by the mockups.**

| ID | Open question | Why it matters | Needed to close |
|---|---|---|---|
| D-01 | Which Windows versions/builds are officially supported at launch? | Determines APIs, packaging, fonts, accessibility behavior, and test coverage. | Product/engineering support matrix. |
| D-02 | What native Windows input integration architecture will be used? | Determines system-wide input, pre-edit, commit semantics, focus, and candidate rendering. | Engineering architecture decision and native feasibility prototype. |
| D-03 | Does predictive word suggestion ship in Basic V1? | Distinct from transliteration candidates; changes product promise, UI, privacy/quality expectations, and scope. | Product decision. Default in this spec: excluded. |
| D-04 | What is the default language-switch shortcut? Is it configurable? | Must be displayed consistently and must not conflict with OS/app shortcuts. | Product decision plus registration/conflict prototype. |
| D-05 | Where/how can candidate UI be placed, and what happens to focus? | Candidate window is central to the typing workflow; visual mockups cannot prove anchoring, focus restoration, or app compatibility. | Native prototype test results for caret placement, edges, multi-monitor, z-order, and host apps. |
| D-06 | What scaling and DPI combinations are supported for candidate and settings UI? | Prevents clipping and inaccessible targets on mixed-DPI displays. | Supported display matrix and test results at common scale factors. |
| D-07 | Does dark theme ship in Basic V1 or follow later? | Affects implementation, preference behavior, test effort, and release acceptance. | Product decision. This spec uses light as default and leaves dark launch scope open. |
| D-08 | Which Bengali-capable font stack is supported, and will any font be bundled? | Glyph coverage, shaping, line height, licensing, and deployment vary by OS. | OS/font validation and license/redistribution review. |
| D-09 | What are exact commit/cancel rules for Enter, Space, Backspace, punctuation, cursor movement, mode switch, and focus loss? | Incorrect assumptions can duplicate, lose, or unexpectedly transform user text. | Composition behavior prototype and approved interaction table. |
| D-10 | What is the failure behavior when startup/shortcut/input registration is rejected? | Settings and current-mode indication must not report a state that the OS did not activate. | Error/recovery design verified against OS behavior. |
| D-11 | Which production Bangla UI labels and support copy are approved? | Reference text may be distorted; this handoff intentionally avoids treating it as verified copy. | Human-reviewed localization/copy set. |
| D-12 | What is the target minimum window size and candidate panel sizing policy? | Required for narrow layouts, text scaling, and screen-edge placement. | Native prototype measurements and accessibility review. |

## Approved choices for this phase

- Srabon is the product name; “Bangla typing, made simple.” is the single working tagline.
- Quiet Fluent is the primary visual direction; Bengali Contemporary contributes only a subtle typographic/cultural cue where useful; Precision Tool contributes compactness/clarity only.
- Light neutral surfaces and restrained teal are the visual default. Red-heavy palette and default dark visual style are rejected.
- Basic includes first-run setup, tray controls, composition/candidate workflow, and essential settings.
- `ami banglay likhi` → `আমি বাংলায় লিখি` is the exact illustrative example.
- Transliteration candidates and predictive suggestions are separate concepts. Predictive suggestions are not automatically part of Basic.
- The app icon is explicitly deferred; text-only wordmark is allowed.
- Advanced custom dictionaries, rule editors, profiles, theme editors, and extra productivity features do not appear in Basic screens.

## Provisional choices requiring validation

- Semantic token names and the proposed spacing, type, target-size, and radius values in [design-system.md](design-system.md).
- Proposed font candidates, including OS-installed Bengali-capable options; coverage and licensing are unverified.
- Provisional component states and candidate-panel interaction intent.
- Light as the design specification's default; actual theme switching and dark-mode launch scope remain open.

## Readiness checklist

The handoff becomes implementation-ready only after:

- [ ] All four Basic surfaces have reviewed layouts and defined states in the target native framework.
- [ ] Production Bangla UI strings are human-reviewed and separated from generated reference text.
- [ ] Windows support and input architecture are approved.
- [ ] Shortcut and conflict/error behavior are decided and demonstrated.
- [ ] Candidate placement, focus, DPI/scaling, and representative host-app behavior are tested.
- [ ] Commit/edit/mixed-language semantics are specified from prototype evidence.
- [ ] Theme scope, font coverage/licensing, and accessibility baselines are resolved.
- [ ] Basic remains focused, with no Advanced controls exposed and a viable extension path.
- [ ] App icon remains out of scope.

Until these blockers close, describe the work as a **consolidated design specification for engineering discovery**, not as verified implementation-ready design.
