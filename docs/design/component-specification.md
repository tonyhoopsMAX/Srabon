# Shared component specification

**Status:** Interaction contract and proposed states for design/engineering discussion. Dimensions are provisional; native platform controls may be preferable where they provide better accessibility, keyboard semantics, and Windows behavior.

## State model

Every interactive control should distinguish these states where applicable:

- **Default:** neutral surface, readable label.
- **Hover:** subtle surface or border change; never the only indication of interactivity.
- **Pressed:** visible transient feedback.
- **Selected/active:** persistent, high-clarity state (for example, active language or selected candidate).
- **Keyboard focus:** explicit outline/ring, distinct from hover and selection.
- **Disabled:** non-interactive appearance plus an explanation when the disabled state is not self-evident.
- **Error:** visible message and recovery action; not red color alone.
- **Busy:** progress/disabled duplicate action with an accessible status announcement.

All states must use semantic color tokens from [the design system](design-system.md), not one-off hard-coded colors.

## Controls

| Component | Required behavior and states |
|---|---|
| Primary button | One prominent action per surface; concise verb label; default/hover/pressed/focus/disabled/busy/error. Enter/Space activate when focused. Avoid ambiguous “OK” where the action can be named. |
| Secondary button / link | Lower emphasis; still visible focus; avoid color-only distinction. Cancel/Skip must not be styled as equivalent to irreversible exit. |
| Checkbox | Binary preference, label clickable, checked/unchecked/focus/disabled/error; accessible checked state; Space toggles. Use only where independent choices are valid. |
| Radio group | One-of-many choices, labeled group, selected/unselected/focus/disabled; arrow keys move according to platform conventions. Avoid using radios for a simple on/off preference. |
| Dropdown | Current value always readable; keyboard open/navigation/commit/cancel; focus and disabled/error states. Use native accessible control where possible. |
| Toggle | Clear text label and on/off status; do not rely on switch color or thumb position alone. Startup and feature-registration errors must not leave UI claiming an unregistered setting is active. |
| Menu item | Text plus optional familiar icon; selected/checked state where meaningful; visible focus; arrow-key navigation and Escape dismissal; separator only for meaningful groups. Current language remains textual. |
| Shortcut field | Shows current shortcut as readable key names. Edit/capture state, Escape cancel, conflict/reserved-key error and recovery/reset. Never display a shortcut before it is confirmed in the running system. |
| Text/input field | Persistent accessible label, optional helper/error text, clear focus ring, adequate Bangla line height, and no clipped glyphs. Error announced to assistive technology. |
| Candidate item | Displays one transliteration alternative, ordinal if helpful, selected/focus state, keyboard navigation, and commit feedback. Must not resemble predictive suggestions unless that feature is separately approved. |
| Candidate panel | Compact, readable floating surface; placement/focus/monitor/DPI behavior is explicitly an engineering dependency. Do not capture focus without a validated return path. |
| Status indicator | Text such as Bangla or English, optionally accompanied by a meaningful icon; accessible name/state; changes announced only when appropriate to avoid excessive speech. |
| Dialog/notification | Short title, direct explanation, clear primary recovery action, and a non-destructive dismissal path. Avoid modal dialogs for routine input-mode changes. |

## Navigation and keyboard interaction

- Settings use a deterministic tab order matching visual reading order. If there is a navigation rail, it must be keyboard-reachable and have a distinct selected state.
- Menu behavior should follow expected Windows keyboard conventions. Exact keys for candidate traversal and commit remain unresolved until native input prototype testing.
- Escape closes or cancels the current transient interaction without deleting committed text. For shortcut capture it cancels capture.
- Focus is never hidden beneath a candidate panel, tray menu, or dialog. Dismissal returns focus to the originating context where the platform supports it.
- Avoid global hotkeys that conflict with Windows or common applications; register/validate before presenting the shortcut as active.

## Sizing and scaling

Provisional targets from the design system: common settings controls approximately 36–40 px high; compact menu rows 36–44 px; candidate rows 32–40 px. These values are starting points, not pixel measurements. Confirm final native logical units and interaction targets on supported Windows builds. Preserve usable target sizes at 100%, 150%, and 200% scale and with enlarged text. No fixed-height control may crop Bengali vowel signs or other marks.

## Accessibility and error criteria

- Each control has a programmatic name, role, value/state, and grouping where required.
- Focus, selection, disabled, and error are perceivable without color alone.
- Text and essential UI boundaries meet validated contrast targets in both themes and high-contrast modes.
- Screen-reader announcements identify active language changes, setup status, settings errors, and candidate selection without repeating on every keystroke unnecessarily.
- Test keyboard-only use, at least one Windows screen reader, magnification/text scaling, high-contrast/contrast themes, and Bangla rendering before release.
- Error text states what happened, what remains active, and a safe next action. Never report a preference as saved if the OS rejected it.
