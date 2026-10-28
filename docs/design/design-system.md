# Design system

**Status:** Direction approved; numeric values and font choices below are **provisional starting points**, not measurements sampled from the reference images and not verified implementation values. Validate in a native Windows prototype at intended DPI and with real Bangla text.

## Principles

1. **Quiet by default:** native-feeling neutral surfaces, thin separators, restrained teal, and minimal decoration.
2. **Language is explicit:** show the active input language in text, not color alone. Bangla typography should be legible and correctly shaped.
3. **Compact, not cramped:** keep utility surfaces efficient while preserving readable text and usable targets.
4. **Keyboard-first:** predictable focus order, visible focus, and keyboard operation for all essential actions.
5. **State is honest:** distinguish active input mode, Roman pre-edit, committed text, transliteration candidates, and any future predictive suggestions.
6. **Theme parity:** if dark mode ships, it must preserve the same hierarchy, contrast, selection, and focus affordances as light mode.

## Semantic color tokens

The table defines names and intended roles. Hex values are deliberately not specified: the images are not a reliable color-measurement source. Select and validate actual values in implementation against contrast requirements and Windows rendering.

| Token | Light intent | Dark intent | Notes |
|---|---|---|---|
| `color.canvas` | Window/background neutral | Deep neutral canvas | Main app/settings background |
| `color.surface` | White or near-white surface | Raised dark neutral surface | Cards, menus, dialogs |
| `color.surface-subtle` | Slightly tinted neutral | Slightly lifted/different neutral | Secondary groups, hover backgrounds |
| `color.primary` | Restrained teal | Accessible teal adjusted for dark surface | Primary actions and selected emphasis; do not use as sole state signal |
| `color.accent` | Teal/blue-green secondary emphasis | Adjusted accent | Links, active navigation, small brand details |
| `color.text` | Near-black neutral | Near-white neutral | Primary text |
| `color.text-muted` | Secondary gray | Muted light gray | Secondary descriptions; must remain readable |
| `color.border` | Low-contrast neutral line | Visible dark-surface separator | Must still define boundaries |
| `color.focus` | Distinct high-visibility focus ring | Distinct high-visibility focus ring | Separate from hover and selection |
| `color.success` | Green semantic status | Adjusted green | Pair with text/icon; never color only |
| `color.warning` | Amber semantic status | Adjusted amber | Pair with text/icon |
| `color.error` | Red semantic status | Adjusted red | Pair with text/icon and recovery action |
| `color.disabled-text` | Readable disabled gray | Readable disabled gray | Avoid opacity-only treatment that destroys contrast |
| `color.disabled-surface` | Muted neutral | Muted neutral | Disabled controls remain recognizable |
| `color.selection` | Subtle teal-tinted selection | Teal-tinted selection with adequate contrast | Selection/focused candidate |

**Contrast acceptance:** calculate contrast from actual chosen color pairs before ship. As a baseline, target WCAG 2.2 AA contrast for text (4.5:1 normal text; 3:1 large text) and non-text UI boundaries/focus indicators where applicable. Validate in Windows high-contrast/contrast themes as well; do not claim conformance before testing.

## Typography and Bangla rendering

Font availability, glyph coverage, shaping, fallback, and licensing must be checked on supported Windows versions. A fallback stack is a proposal, not a guarantee:

- Latin UI: system UI font (Windows Segoe UI where available), with platform fallback.
- Bangla UI and candidate text: prefer an installed Bengali-capable UI font such as Nirmala UI on Windows; evaluate Noto Sans Bengali as an alternative only after confirming version availability, shaping quality, licensing, and whether redistribution is intended.
- Do not bundle a font until its license and packaging implications are reviewed. Do not use a font merely because the reference image appears to use it.

| Role | Provisional size / weight | Use |
|---|---|---|
| `type.title` | 22–26 px, semibold | Setup heading / top-level settings title |
| `type.section` | 16–18 px, semibold | Section headings, current mode |
| `type.body` | 14–16 px, regular | Instructions, menu and settings labels |
| `type.supporting` | 12–14 px, regular | Secondary explanation, version/help metadata |
| `type.control` | 13–15 px, regular | Buttons, dropdowns, shortcut display |
| `type.composition` | 18–22 px, regular | Roman pre-edit and Bangla composition preview |
| `type.candidate` | 15–18 px, regular | Candidate choices; ensure Bengali glyphs do not clip |

Values are nominal CSS-like pixels for design communication only. Native framework units, text scaling, and rendering measurements must be established for the chosen implementation technology. Use a line height around 1.35–1.5 for body copy and allow Bangla diacritics/marks enough vertical room; do not crop glyph ascenders, descenders, or combining marks.

## Spacing, geometry, and elevation

Provisional scale: `space-1=4`, `space-2=8`, `space-3=12`, `space-4=16`, `space-5=24`, `space-6=32` px. Prefer the scale; exceptions should be documented rather than scattered arbitrary values.

- Main settings window padding: 20–24 px provisional.
- Compact menu row: 36–44 px high provisional, depending on target framework and text scaling.
- Standard button/input: 36–40 px high provisional.
- Candidate row: 32–40 px high provisional, with larger touch/pen targets where relevant.
- Corner radii: 4 px for compact controls, 6–8 px for cards/dialogs, 999 px only for deliberate pill toggles; all provisional.
- Borders: one-pixel-equivalent separators as a starting point. Avoid relying on shadow alone to separate surfaces.
- Elevation: use a restrained, soft shadow for floating candidate/menu surfaces only if the OS surface system permits. Keep settings and onboarding mostly flat. The reference's elevation examples are illustrative, not sampled values.

## Layout and responsive behavior

- Keep onboarding to one concise first-run surface and a small number of clearly labeled steps only if actual configuration requires them. Avoid a long wizard.
- Settings: stable navigation rail plus one content pane at typical desktop widths; at narrower widths, collapse navigation without clipping controls.
- Tray menu and candidate window are separate native surfaces, not miniature app windows assumed to be web popovers.
- Minimum window sizes, candidate width, anchor/placement rules, and screen-edge handling are implementation decisions to prove with a prototype.
- Support Windows display scaling and text scaling. Test at 100%, 150%, and 200% DPI/scaling where available, plus a narrow/low-resolution layout. Avoid fixed pixel geometry that clips Bangla or localized text.

## Theme and Windows preferences

Light is the launch visual default for this specification. Whether dark theme ships in Basic V1 is unresolved. If both themes ship, follow Windows app/system theme preference by default, offer no theme editor in Basic, and ensure an accessible system override/high-contrast setting takes precedence. Do not silently invert colors; map each semantic token deliberately. Persist an explicit user override only if that product choice is approved. If dark follows later, keep token semantics ready but do not expose a nonfunctional theme toggle.

## Accessibility baseline

- All actions operable via keyboard; predictable tab order and arrow-key behavior for menus/candidates where appropriate.
- Always-visible keyboard focus distinct from hover, pressed, and selection.
- Text labels for language state and status; never rely on color or icon alone.
- Accessible names and state announcements for toggles, radio/checkbox controls, candidate position/selection, and input mode changes.
- Respect Windows text scaling and high-contrast/contrast themes; check focus and selection visibility in those modes.
- Do not disable system text selection or screen-reader access in settings surfaces.
- Reduced animation/motion preference should be respected if animation is introduced; animation is not needed for core flows.
- Verify contrast using final token values and test with screen readers, keyboard only, and magnification.
