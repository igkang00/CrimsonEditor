# analysis/

Throwaway-looking scripts that are not throwaway: the ones that produced a number a design
decision now rests on.

This is deliberately not `tools/` (C++ components that ship inside the product) or `scripts/`
(build automation). Nothing here is built, shipped, or run by CI. These exist so that a claim
in `docs/` can be **re-checked** rather than believed — including by whoever inherits this and
suspects the claim is stale.

Run them from the repository root.

| script | the question it answers | where the answer landed |
| --- | --- | --- |
| `font-cell-width.ps1` | How wide does GDI actually draw a character, per font? | [docs/refactoring-column-mode.md](../docs/refactoring-column-mode.md) |

## font-cell-width.ps1

Measures real GDI advances and reports each character's width as a multiple of the narrow
cell (the advance of a space).

It exists because the column-mode plan turned on a fact nobody had checked: **a fixed-pitch
font is not a dual-width font.** Consolas — the current default Column Mode font — has no
Hangul glyphs. Windows font-links them to another face, and that face's advance is **1.43×**
the Consolas Latin advance, not 2×. Two earlier drafts of the design were built on the
assumption that it was 2× (or near enough to round to it), and both were wrong. The font
cannot hand you a grid; the editor has to impose one.

It also checks the classifier the plan specifies — East Asian Width table **∪** measurement
(`advance > 1.2 × cell`) — by printing where the two halves disagree. Those disagreements are
the point, not a defect: on the fonts measured, each half is the only one that gets some
character right.

```powershell
.\analysis\font-cell-width.ps1
.\analysis\font-cell-width.ps1 -Faces Consolas,"Cascadia Mono" -PointSize 12 -WideThreshold 1.2
```

D2Coding is loaded privately from `runtime/fonts/`, so it is measured whether or not it is
installed on the machine.
