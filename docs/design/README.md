# Srabon design handoff

**Status:** Consolidated design direction; ready for engineering discovery and native prototypes, **not yet implementation-ready**. Critical input-platform, shortcut, and compatibility decisions remain open.

**Scope:** Basic Windows desktop Bangla phonetic typing experience. Documentation and design specification only; this is not evidence of a working input method.

## The decision in brief

Use **Quiet Fluent** as the visual foundation: calm Windows-like surfaces, restrained teal accents, clear typography, compact controls, and low visual noise. Add a small, purposeful Bangla typographic cue only where it improves recognition or comprehension. Do not use the red-heavy Bengali Contemporary direction as the main palette. Use Precision Tool as a reference for clarity and compactness, not as the default dark visual style.

- **Product name:** Srabon
- **Working tagline:** “Bangla typing, made simple.” Use this alone in product UI.
- **App icon:** explicitly deferred. A text-only Srabon wordmark may be used; do not create or export an icon in this phase.
- **Basic release:** first-run setup, system-tray controls, composition/candidate workflow, and essential settings. Advanced customization remains out of scope.
- **Verified example to use verbatim:** `ami banglay likhi` → `আমি বাংলায় লিখি`.

## Reference boards

The supplied boards are preserved here so this handoff stays self-contained. They are visual references, not approved copy or proof of Windows behavior.

- [Reference A — visual directions and four core screens](references/visual-directions.png): primary reference for information architecture, screen hierarchy, candidate bar, tray menu, settings, states, and beginner clarity.
- [Reference B — design-system overview](references/design-system-overview.webp): primary reference for palette direction, typography hierarchy, spacing/radius examples, component-state patterns, and light/dark treatment.

Both boards contain generated artifacts, including malformed English and potentially distorted Bangla. Do not copy small mockup labels or treat displayed features as approved requirements. Production Bangla UI strings need human review. The sample above is the only Bangla product example specified here as reviewed.

## How to use this handoff

1. Read [screens and flows](screens-and-flows.md) for required surfaces, interaction intent, and the illustrative typing workflow.
2. Read [design system](design-system.md) for semantic tokens and provisional visual/accessibility guidance.
3. Read [component specification](component-specification.md) for shared states and keyboard/accessibility behavior.
4. Read [implementation notes](implementation-notes.md) before prototyping native input, composition, focus, scaling, or tray behavior.
5. Resolve the items in [open decisions](open-decisions.md) before declaring launch scope or implementation readiness.

## Decision status

| Status | Meaning | Examples |
|---|---|---|
| **Approved** | Direction accepted for this handoff | Quiet Fluent foundation; tagline; four Basic surfaces; icon deferred; no automatic commitment to predictive suggestions |
| **Provisional** | Useful design starting point requiring design/engineering validation | Numeric tokens, font stack candidates, minimum target sizes, component sizing, theme behavior |
| **Unresolved dependency** | Must be decided or demonstrated; visuals cannot settle it | Supported Windows versions, native input integration, shortcut, candidate placement/focus/DPI/app compatibility, dark-theme launch scope |

## Readiness

This specification consolidates the visual direction and establishes intended surface behavior. It does **not** satisfy implementation readiness yet: system-wide input architecture and platform support are unknown, candidate-window behavior needs native prototype testing, the exact shortcut is undecided, and dark-theme scope is open. See [open decisions](open-decisions.md) for owners/evidence needed and acceptance gates.
