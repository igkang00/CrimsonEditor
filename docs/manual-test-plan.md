# Manual test plan

A hands-on walk through the editor's features, done as a user rather than as a developer. For
what runs without a human — the unit tests, and the roadmap toward integration and E2E coverage
— see [automated-test-plan.md](automated-test-plan.md).

The split is not about effort, it is about reach. The automated suite is 150 tests green and
they check algorithms and containers: *what a container holds, not what it costs*, and nothing
that needs a device context, a font, or an eye. Every bug in this document's §A got past them.

## Why this exists

Six refactorings have landed since the last time anyone used the editor end-to-end on purpose:
x64, Unicode, surrogate pairs, large-file performance, the line container, and column mode.
Each was verified against *its own* concern. Nothing has checked what they did to each other,
or to the features nobody was thinking about at the time.

The Unicode write-up says so in as many words, and then that walk-through never happened:

> The remaining work is release mechanics — deliberately paused until a full manual feature
> walk-through of the Unicode build has been done end-to-end from the user side. **That
> walk-through is not tracked in this document.**
> — [refactoring-unicode-migration.md](refactoring-unicode-migration.md)

This is that document. Four more refactorings have landed since it was written.

**The evidence base is thinner than the write-ups make it look.** Worth knowing before trusting
any of them:

- [refactoring-memory-safety.md](refactoring-memory-safety.md) records **no testing at all** —
  the only evidence offered is that it compiles. It also introduced deliberate *truncation* in
  several places.
- [refactoring-surrogate-pairs.md](refactoring-surrogate-pairs.md) and
  [refactoring-x64-migration.md](refactoring-x64-migration.md) have "Verification" sections that
  are **proposals, not results**. No pass/fail is recorded in either.
- Two documents are **stale about their own status**: x64's checklist shows phases 1–7 unticked
  though the work plainly shipped; memory-safety's header says the fixes "are not yet applied"
  while its body says they are.

So: absence of a known bug here means nobody looked, not that it works.

---

## Where to test

**Default: the Release build, installed.** This is not a formality — two classes of bug are
invisible anywhere else.

**Release-only.** Two safety nets are compiled out of Release by design, and both fail *silently*
there:

- `CLineList`'s stale-POSITION assert. The line-container doc: *"Debug builds assert if you get
  it wrong; **release builds do not, so do not develop against release only.**"* In Release a
  stale POSITION edits the **wrong line**, with no crash.
- `/RTC1` stack checks. The memory-safety doc: *"`/RTC1` catches it in Debug; **Release silently
  corrupts the stack**."*

**Installed-only.** The Unicode migration truncated a `REG_SZ` so that
`"C:\Program Files\Crimson Editor"` became `"C"`, and *"every install-dir-relative lookup (syntax
spec link files, colour scheme, templates, schemes) then failed **on the installed build**"* —
most visibly, `.c` files got no syntax highlighting. A dev build reads those from the source
tree and never noticed. **Clean-box install verification was left undone by both the x64 and the
Unicode work** (their phases 7 / 7b), so it has never been done at all.

Almost everything below should therefore be done on an installed Release build. Use Debug only
where a step says so — those steps want the assert to fire.

| Build | Use for |
| --- | --- |
| **Release-KR, installed** | Everything, unless noted |
| Debug-KR | Steps marked `[Debug]` — the asserts are the point |
| Release-US | The English-edition pass (§D) |

---

## Risk axes

Tags mark which of our changes a feature is exposed to. **Tag count is the priority order** —
more tags means more ways it can be wrong.

| Tag | Means | Comes from |
| --- | --- | --- |
| `[wrap]` | Behaves differently with word wrap ON | **The repeated blind spot** — see below |
| `[cjk]` | Korean/CJK text may differ from ASCII | Unicode migration |
| `[emoji]` | Astral (surrogate pairs) and BMP emoji | Surrogate-pair work |
| `[enc]` | Encoding round-trip (UTF-8/16, CP949, BOM) | Unicode migration |
| `[big]` | 900k-line file: slow, or broken | Large-file perf + line container |
| `[col]` | Column mode differs | Column-mode work |
| `[ime]` | Korean IME composition | x64 (*"the most likely place to hit a subtle x64 issue"*) |
| `[undo]` | Undo/redo entanglement | Everywhere; column-mode undo is known-weak |
| `[trunc]` | Memory-safety pass added deliberate truncation here | Memory-safety audit |

**`[wrap]` is first among these.** It is the axis our measurements keep missing:

> **16× slower, and it got all the way to the end of the branch.** It did not ship — it was
> caught before the release, but only because someone said the editor *felt* slower. Nothing
> else was going to catch it… **Phase 5 measured with word wrap off**, where a line is exactly
> one row and nothing is ever inserted… **The one path that inserts was the one path not
> measured.**
> — [refactoring-line-container.md](refactoring-line-container.md)

The large-file work has the same habit: its headline numbers are the wrap-off path. **Turn word
wrap on for anything you are unsure about.**

---

## Fixtures

All in [../tests/data/](../tests/data/), committed except where noted. Their **bytes** are the
fixture — a BOM-less CP949 file is only a test if it really has no BOM — so if one ever needs
recreating, use the script rather than an editor that might add one:

```
python tests/data/make-fixtures.py          # the small ones (idempotent)
python tests/data/make-fixtures.py --big    # also big.txt, ~56 MB, gitignored
```

| File | What | For |
| --- | --- | --- |
| `ascii.c` | plain C, ~200 lines | baseline |
| `korean.c` | Korean comments, identifiers and strings — **CP949, no BOM** | `[cjk]` `[enc]` |
| `utf8-nobom.txt`, `utf8-bom.txt`, `utf16le.txt`, `utf16be.txt`, `cp949.txt` | the same text in five encodings, for round-trip comparison | `[enc]` |
| `astral.txt` | 😀, CJK Ext-B, ✅ U+2705, ⭐ U+2B50, Hangul — caret and delete over astral | `[emoji]` |
| `emoji.c` | the same characters **inside C string literals**, including `"\😀"` — the analyzer's escape branch | `[emoji]` |
| `long-line.txt` | one line of 41,000 chars — crosses both the 2,048 and 32,767 boundaries | `[trunc]` |
| `long-path.prj` | a project entry whose `path` attribute is ~5,200 chars — past the 4,096 `getline` in the `.prj` reader | `[trunc]` |
| `truncated.prj` | a project file ending exactly at `<localfile`, no attributes, no `>` | `[trunc]` |
| `long-token.key`, `long-token.dic` | keyword and dictionary entries of 254 / 255 / 300 chars, straddling `MAX_WORD_LENGTH` | `[trunc]` |
| `long-token.ada` | the document to open against `long-token.key` — one line per boundary | `[trunc]` |
| `blockcomment.c` | a `/*` held open across thousands of lines | `[wrap]` `[big]` |
| `big.txt` | 900,000 lines, ~14% carrying Korean. **Not committed** — generate it | `[big]` |

`cp949.txt` deliberately drops the em-dash/smart-quote line the other four carry: CP949 cannot
hold it, and that is the point — it is what an ANSI file can actually contain.

---

## A. Where the documents point

Start here. Every item is a place one of the write-ups says something is fragile, or admits it
was never checked. This is the highest-yield section in the plan and should be finished before
§B is started.

### A1. Word wrap, everywhere `[wrap]`

- [x] Open `big.txt`, **turn word wrap on**, and time it. The line-container fix brought this
      from 2,779 ms back to 194 ms on 90k lines — confirm it still feels immediate on 900k.
      Reformat ~1 s on 900k; feels immediate.
- [x] With wrap on, edit near the **bottom** of `big.txt`: type, paste, delete a large block.
      Edit and scroll held.
- [x] With wrap on, open `blockcomment.c` and scroll to the middle. Syntax colour must survive.
      Type `*/` somewhere inside the comment and watch the rest of the file recolour; undo it.
      Colour survived; recolour and undo correct.
- [x] Toggle wrap on/off repeatedly on `korean.c`. Caret must stay on the same character.
      Caret stayed on the same character.
- [x] Resize the window with wrap on, on a large file. (This is what `OnSize` reformats.)
      Found a drag-jank bug (row 7) — now fixed with a debounce.

### A2. Print preview `[big]`

- [x] Open print preview, then **resize the main window**, then close it. This crashed with an
      access violation before `9f8725e`; the guard that used to hide it is still there but no
      longer needed for that reason. No crash — the `GetNextCedtView` walk holds.
- [x] Print preview a file with Korean and with wrap on. The printer path deliberately does
      **not** use the column grid (`_bGridLayout` is off for it) — check CJK spacing looks right.
      Found a CJK preview bug (row 8) — Korean overlapped/garbled in preview with a Latin printer
      font; actual print was always correct. Fixed with explicit font linking.
- [x] Print preview while **column mode is on**. Works. Print-in-selection prints the full lines
      the block spans (a row range), not just the boxed columns — that is how `PD_SELECTION`
      works here (`OnBeginPrinting` uses `nBegY..nEndY`), same as line mode. Not a bug.

### A3. Column mode `[col]` `[undo]`

- [x] **Undo a column edit.** The doc admits: *"Undo does not know about column mode… a caret
      parked in virtual space is not restored."* Shipped known-weak — find out how bad.
      Undo worked correctly — no visible virtual-space caret problem in the cases tried.
- [x] Block-select across Korean, copy, paste into a new file. What was blue must be what lands.
      Held — the selection round-tripped.
- [x] Type Korean into a multi-row block; **abandon a composition with Escape** mid-way, then
      type again. (The IME/column seam — `OnImeCompositionEnd` runs *before* `Result`.)
      Esc commits the last composed character and drops the multi-row selection; no corruption,
      typing again is fine.
- [x] Comment/uncomment a block in a `.c` file; then uncomment a block that is **not** commented
      — the block must not creep left. Held — no leftward creep.
- [x] Click the **left half** of a Hangul syllable in column mode; the caret must land before it,
      not after. (Block edges and carets snap to opposite sides on purpose.) Lands before it.
- [x] Column mode with a **proportional screen font** — it should substitute the fixed-pitch one.
      Substitutes the fixed-pitch font.

### A4. Encoding detection `[enc]` `[cjk]`

- [x] Open **CP949 Korean without a BOM**. The migration's own list records this failing once:
      *"every Korean byte became U+FFFD."* `korean.c` and `cp949.txt` opened correctly — no U+FFFD.
- [x] Round-trip each of the four encodings: open, edit one character, save, reopen, compare.
      Byte-verified: content identical after add-char/save/delete/save on all six fixtures; BOM
      preserved (utf8-bom kept it, utf8-nobom stayed BOM-less); UTF-16 LE/BE byte-identical.
- [x] `Document > Encoding Type` — convert a Korean file UTF-8 → UTF-16LE → CP949 and back.
      Byte-verified via saved samples: UTF-8 (no BOM), CP949 (genuinely CP949 — fails a UTF-8
      decode), UTF-16 LE (`FF FE`) and BE (`FE FF`) all decode to the identical text.
- [x] Save with each of DOS/Unix/Mac line endings and reopen. Byte-verified: DOS = CRLF, Unix =
      LF only, Mac = CR only, each with identical content.

### A5. IME `[ime]` `[cjk]`

The x64 doc names this twice as the most suspicious surviving path.

- [x] Type Korean into a normal document; check composition, Backspace mid-composition, Escape.
      Composition, mid-composition Backspace, and Escape all behaved.
- [x] Type Korean at the **end of a long line** and near the **bottom of a big file**. The long-line
      end surfaced the max-line-length overflow bug (row 9), now fixed; big-file bottom held after that.
- [x] Type Korean into a **column block** (see A3). Covered by §A3 item 3 — held (Esc mid-composition
      commits the last char and drops the block selection; no corruption).
- [x] Record a macro that types Korean, then replay it. Recorded and replayed correctly — the Korean
      characters land on replay.

### A6. Emoji and astral `[emoji]`

- [x] `astral.txt`: arrow past 😀 — one press per character, not two. Backspace deletes the whole
      thing. Save and reopen: it must survive (a half pair *"is not a valid character in any
      encoding… the data is permanently destroyed"*). One press per emoji; Delete and Backspace
      remove the whole pair; survived save/reopen.
- [x] `emoji.c`, line 3: `"\😀"` — a backslash then an astral pair. The analyzer's escape branch
      does `fwd += 2` unconditionally and can split it. Lines 4–5 vary the shape. No split — the
      pair stays intact.
- [x] ✅ U+2705 and ⭐ U+2B50 (`astral.txt` line 3, `emoji.c` line 6) in a fixed-pitch font: the
      caret must not land inside the glyph. (These are BMP, not astral — they are what killed
      the previous attempt.) Caret never landed inside the glyph.
- [x] `astral.txt` lines 5–6: a whole line of ✅, and a whole line of 😀 — column positions and
      End-key behaviour. Column positions and End behaved.
- [x] Double-click an emoji to select it; drag-select across one. Double-click, drag-select, and
      column-mode selection all held.

### A7. Long lines and truncation `[trunc]`

Every item here is a place the memory-safety pass chose truncation over an overrun, and then
tested none of it.

- [x] `Evaluate Line` (`Ctrl+Enter`) on a line of 2,048+ and 32,767+ characters. Two boundaries:
      the whole line (a long *expression* of small tokens) and a single *token* > the evaluator's
      2048-char scratch buffer. Expressions of 3,999 and 32,759 chars evaluated correctly (2000,
      16380 — no line-copy truncation). A single > 2048-char token **crashed** (stack overrun,
      row 10); now reported as "token too long" with a beep (not a truncated wrong answer), with
      a unit-test regression.
- [x] A **user tool** whose command line, or whose piped stdin, is multi-KB. *"Truncation can now
      occur on multi-KB single-line input to a child process."* Pass a long selection via
      `$(CurrWord)` (when text is selected, it *is* the selection). The command line is now built
      in a `CString` (no arbitrary cap — passed in full up to the OS ~32767 limit) instead of the
      old fixed 2048 buffer. Two bugs surfaced and were fixed: **Capture Output garbled the
      child's output** (row 12) and **Copy All crashed** on the long line (row 13).
- [x] Open a project whose entries are absurdly long paths; a keyword/dictionary file with a very
      long token. (`stream.width(N)` was added to 20 parse sites.) The two readers behaved
      differently, and the difference is the lesson: **`width()` truncates the token but does not
      discard the tail**, which comes back as the next token.
      - `long-path.prj` — the path lives in a `getline(szText, 4096, L'>')`, not a `>>`, so it
        fails closed: the truncated text has no closing quote, `ParseItemAttribute` rejects it,
        and the open aborts with an explicit error. No hang, no half-loaded tree. (Losing the
        previously open project on a failed open is by design.)
      - `truncated.prj` — a `.prj` ending at `<localfile` reaches
        `if( szText[nLen-1] == '/' )` with `nLen == 0`, reading (and on a chance match, writing)
        `szText[-1]`. **Row 14**, fixed with an `nLen > 0` guard at all four sites. Note Debug
        cannot show this: `/RTC1` guard bytes never compare equal to `'/'`, so the write never
        fires there — the explicit error dialog is what proves the line ran.
      - `long-token.key` — a 300-char keyword registered *twice*: truncated at 255, and again as
        its 45-char tail, which then coloured in the editor as a keyword nobody wrote. **Row 15**,
        fixed by discarding the tail in both `CKeywords::FileLoad` and `CDictionary::FileLoad`,
        with unit-test regressions confirmed to fail without the fix.
- [x] Directory panel: **copy / move / rename / delete** a file with a near-`MAX_PATH` path. These
      are destructive and the fix was off-by-one-shaped. It was — twice, at the same length.
      Fixtures: a tree of files at 200 / 258 / **259** characters, 259 being the longest path
      Windows allows. Getting to the test took two prior fixes (rows 16 and 17); the operations
      themselves then failed exactly as row 18 describes, and all four now behave identically at
      all three lengths. A file **copied into** one of those deep directories lands past 259
      characters, where the panel lists it but cannot open it (row 19).

### A8. The installed build `[enc]`

Run on a clean Windows VM (VirtualBox), 3.93, first pass 2026-07-18.

- [x] **Install from `dist/cedt-393-setup.exe` onto a machine (or user) with no prior config.**
      Installed cleanly.
- [x] Open a `.c` file — it must get syntax colouring. (This is the exact symptom of the
      registry-truncation bug: it worked in dev and failed installed.) **Coloured — the
      registry path resolves on a clean install. First time this has been confirmed.**
- [x] Colour schemes, templates, syntax `Customize…` — all resolve through the install dir. Work.
- [x] Explorer right-click → "Edit with Crimson Editor"; `launch.exe`; a file passed on the
      command line; a filename containing Korean. All work.
- [x] Uninstall, then reinstall over the top. Reinstall succeeds; settings under `HKCU` and
      `%APPDATA%` survive the uninstall (README's stated behaviour) — the reinstalled build read
      the previous workspace on first run, which is how bug #6 surfaced. Config is not reset.

**§A8 complete.** Two confirmations (rows 1–4) and two bugs (rows 5–6, both fixed) — the whole
installed-build section has now been exercised on a clean VM, the first time it ever has.

### A9. Release-only hazards `[big]` `[Debug]`

- [ ] In **Release**: delete a 100,000-line selection out of `big.txt`; paste it back. Then the
      same in **Debug** — if a POSITION assert fires, the Release run was silently corrupting.
- [ ] In **Release**: heavy block edits in column mode on a large file.
- [ ] First run with **no config**: the x64 work resets every config deliberately. Confirm
      defaults load and no "config corrupted" popup appears.

---

## B. Feature sweep

Menu order, so nothing is skipped. Untagged items are plain toggles — click, confirm, move on.
Items that are **one code path behind many slots** (Tools 1–0, Macros 1–0, font Custom 1–5, tab
sizes, colour schemes) are grouped: test one, then confirm a second behaves the same.

### File

- [ ] New `Ctrl+N` · Open `Ctrl+O` `[enc]` `[big]` · Open Template `Alt+Shift+O`
- [ ] Close `Ctrl+F4` · Close All — `[big]` (closing a large file used to reformat it first)
- [ ] Reload · Reload As… `[enc]`
- [ ] Save `Ctrl+S` `[enc]` · Save As `Alt+Shift+S` · Save All — `[big]` (block writes)
- [ ] Print `Ctrl+P` `[cjk]` `[wrap]` · Print Preview (→ A2) · Print Setup
- [ ] FTP: Open Remote `Ctrl+Shift+O` · FTP Settings — (x64 smoke list; needs a server)
- [ ] Recent Files (MRU) — including a path with Korean in it
- [ ] Exit `Alt+F4` with unsaved changes

### Edit

- [ ] Undo `Ctrl+Z` · Redo `Ctrl+Y` — `[undo]` `[cjk]` `[emoji]` `[col]`
- [ ] Cut `Ctrl+X` · Copy `Ctrl+C` · Paste `Ctrl+V` · Delete — `[cjk]` `[emoji]` `[col]`,
      and paste **to and from another application**
- [ ] Cut Append `Ctrl+Shift+X` · Copy Append `Ctrl+Shift+C` — `[col]` refuses these; confirm beep
- [ ] Select All `Ctrl+A` `[col]` (refused) · Select Line `Ctrl+E` · Select Word `Ctrl+D` `[cjk]` ·
      Select Block `Ctrl+B` `[col]` (refused)
- [ ] Upper/Lower/Capitalize/Invert Case — `[cjk]` (Korean must pass through untouched) `[col]`
- [ ] Insert Date `Alt+Shift+D` · Insert Time `Alt+Shift+T` · Insert File `Alt+Shift+F` `[enc]` `[trunc]`
- [ ] Increase/Decrease Indent `Ctrl+.` `Ctrl+,` — `[col]` refuses; `[wrap]`
- [ ] Delete Line `Alt+Del` · Duplicate Line `Alt+Ins` · Delete Word `Ctrl+Del` `[cjk]`
- [ ] Join Lines `Alt+J` · Split Line `Alt+K` — `[wrap]`
- [ ] Make Comment `Ctrl+M` — `[col]` (→ A3), and in a language with **no** block comment (`.py`)
- [ ] Column Mode `Alt+C` — the toggle now always reformats; check on a big file `[big]` `[wrap]`

### Search

- [ ] Find `Ctrl+F` `[cjk]` `[emoji]` — regex too (the regex engine broke once under Unicode:
      *"5 of 5 failing tests were regex"*)
- [ ] Replace `Ctrl+R` `[cjk]` `[undo]` — replace-all in selection, and in a `[big]` file
- [ ] Find Next `F3` · Find Prev `Shift+F3` `[big]` (search from the bottom of a huge file)
- [ ] Find in Files `Ctrl+Shift+F` `[enc]` — over a folder of mixed encodings (x64 smoke list)
- [ ] Go To `Ctrl+G` `[big]` — line 900,000
- [ ] Toggle/Next/Prev Bookmark `Ctrl+F2` `F2` `Shift+F2` — `[big]` `[wrap]`
- [ ] Prev Editing Position `Ctrl+\` · Pairs Begin/End `Ctrl+[` `Ctrl+]` `[cjk]`

### View

- [ ] Toolbar · MDI File Tabs · Status Bar · Remote/Project/Output Window — `[trunc]` (tab titles
      and project items were `lstrcpyn`-limited)
- [ ] **Word Wrap** `Alt+Shift+W` → A1
- [ ] Spell Check `Alt+Shift+K` · Line Numbers `Alt+Shift+L` `[big]` · Column Markers `[cjk]`
- [ ] Screen Fonts: Default + one Custom + Set Fonts — `[cjk]` (fixed vs proportional, and
      **column mode substitutes** a fixed one), then confirm a second Custom slot behaves alike
- [ ] Printer Fonts: same, one slot + Set Fonts
- [ ] Line Spacing: one of the five + confirm a second
- [ ] Tab Size: one of the four + Custom Tab Size — `[wrap]` `[col]` (tab stops are the grid)
- [ ] Colour Schemes: one preset + Load Saved + Set Colors — `[enc]` (scheme files resolve
      through the install dir → A8)
- [ ] Show Spaces `Ctrl+Shift+E` — `[cjk]` `[col]`

### Document

- [ ] Syntax Type: Auto Detect / Plain Text / Customize — **`.c` must colour on the installed
      build** (→ A8)
- [ ] Properties · Reload Document · Lock Document
- [ ] Encoding Type (5 items) → A4
- [ ] File Format: DOS / Unix / Mac → A4
- [ ] Convert Tabs↔Spaces, Leading Spaces to Tabs, Remove Trailing Spaces — `[cjk]` `[undo]` `[big]`
- [ ] Summary — `[cjk]` (character counts vs code units)

### Project

- [ ] New / Open / Close Project — `[enc]` (a project referencing **CP949 filenames** has no
      migration: *"re-open the file by hand"*) `[trunc]`
- [ ] New Category · Add to Project · Add Active File · Add All Open Files

### Tools

- [ ] Preferences — open, change something, OK. This writes the whole config (x64 smoke list).
      Then restart and confirm it stuck.
- [ ] Evaluate Line `Ctrl+Enter` `[trunc]` `[cjk]`
- [ ] MS-DOS Shell `F10` · View in Browser `Alt+B` `[trunc]` (paths)
- [ ] Load User Tools · Conf. User Tools — configure **one** slot, run it, capture output
      `[trunc]`, then confirm a second slot runs
- [ ] Menu labels render correctly — `- Empty -` once printed *"`- Empty -\t<garbage wchars>`"*

### Macros

- [ ] Begin/End Recording, Replay `Alt+Enter` — record something with **Korean** `[ime]`, with an
      **emoji** `[emoji]`, and with a **column edit** `[col]`; replay each
- [ ] Load User Macros · Conf. User Macros — one slot, then confirm a second
- [ ] Menu labels (same `- Empty -` hazard)

### Window / Help

- [ ] Window: new window, split, cascade, tile, arrange, close — `[big]` `[wrap]` (a second view
      of the same document reformats independently)
- [ ] Help: contents/index (the x64 work swapped in the SDK `HtmlHelp.lib`), About

---

## C. Cross-cutting

- [ ] **Two documents open with different syntax types.** `GetCharType` reads the table of
      *"whichever document `AnalyzeText` last ran on"* — colour one `.c` and one `.py` and switch
      between them.
- [ ] **Two views of one document** (Window > New Window), one wrapped and one not.
- [ ] A file that is **modified on disk** while open → reload prompt.
- [ ] **Read-only** file: open, edit, save.
- [ ] Drag and drop: a file onto the editor; text between two views; text to another app.

## D. English edition

- [ ] Run `cedt_us.exe` and repeat §A4 and one pass of §B. Both `.rc` files carry the same
      resources; the KR one is CP949 and is the one that gets hand-edited.

---

## The test environment

The installed-build passes need a clean machine, so they run on a VirtualBox Windows VM with no
prior Crimson Editor config, registry keys, or `%APPDATA%` state. That absence is the fixture:
the registry-truncation bug (§A8) was invisible on any machine that had ever run the editor, and
only a first run on a clean box exercises the install-dir path resolution at all.

Keep the VM (or a snapshot of its pre-install state) so the next release can repeat §A8 the same
way, rather than rebuilding the clean condition each time.

## Recording what you find

Add findings here as you go — file, steps, expected vs actual, and which build. A bug found on
Release-installed and not reproducible in Debug is the most valuable kind and should say so.
Confirmations of the fragile spots are worth a row too: "checked, holds" tells the next person
where not to look again.

| # | Where | What happened | Build | Status |
| --- | --- | --- | --- | --- |
| 1 | §A8 · `.c` syntax highlighting on a clean install | Coloured correctly. The Unicode registry-truncation bug (path → `"C"`, killing install-dir lookups) does **not** reproduce on a clean box. First time this path has been verified. | 3.93 Release-KR, VM | ✅ holds |
| 2 | §A8 · install, Explorer "Edit with…", Korean file display | All worked. | 3.93 Release-KR, VM | ✅ holds |
| 3 | §A8 · colour schemes, templates, syntax Customize (all resolve through the install dir) | All applied/opened correctly. Same install-path resolution as row 1, other consumers. | 3.93 Release-KR, VM | ✅ holds |
| 4 | §A8 · `launch.exe`, command-line file open, Korean filename | All opened. | 3.93 Release-KR, VM | ✅ holds |
| 5 | §A8 / §Project · open a file with a **Korean path**, close the editor, reopen | **BUG.** On exit the workspace is saved to `cedt.wks`, and `wofstream` (no `imbue`, [FileWndProject.cpp:128](../src/panels/FileWndProject.cpp#L128)) fails on the first non-ASCII character of a path — so the file is **truncated mid-attribute**, right after `path="C:\Temp\`. The next launch reads the truncated `.wks`, hits an unterminated attribute, and shows "프로젝트 속성을 읽어들이면서 에러가 발생했습니다" (`IDS_ERR_PARSE_PRJ_ATTR`). Non-fatal — the editor opens — but the workspace does not restore, and every file after the Korean-path one is silently dropped from it. **Reproduces without reinstalling: a Korean-path file open at exit is enough.** Root cause is the same missing `imbue` on the read side (`wifstream`, [FileWndProject.cpp:77](../src/panels/FileWndProject.cpp#L77)), so even an intact Korean path would not load. This is the `.prj`/`.wks` path the Unicode migration flagged as unmigrated and the plan tagged `[enc]` `[trunc]` under §Project. | 3.93 Release-KR, VM (and dev) | **fixed** — `Utf8FileLocale()` imbued on all four workspace streams ([fstream_compat.h](../src/include/fstream_compat.h), [FileWndProject.cpp](../src/panels/FileWndProject.cpp)). A Korean-path file now saves and restores intact; ASCII workspaces round-trip unchanged. Verified on the VM. For 3.94. |
| 6 | The fix for #5, first cut | **The fix crashed the app on startup — but only on the VM, never on the dev machine.** The first version imbued `std::locale(".65001")`, a named code-page locale. That depends on the CRT actually having UTF-8 code-page locale data, which the VM's Windows/UCRT lacked, so the constructor threw — and it runs before `open()` on every startup workspace load, so an unhandled exception took the process down. The dev machine's newer UCRT had the data and never reproduced it. Refixed with the `codecvt_utf8_utf16` **facet**, which is STL code and constructs the same on every machine. **This is the whole thesis of §A8 biting the fix itself**: a dev-only pass would have shipped a guaranteed first-run crash. | 3.93 Release-KR — crash **VM-only**, invisible on dev | **fixed** — verified crash-free on the VM |
| 7 | §A1 · resize the window with wrap on, on a large file | **BUG (jank, not corruption).** Word wrap is inherently eager — the row count depends on every line's width, so a re-wrap is a whole-document pass (~1 s on a large file). `OnSize` reformatted inline on **every** `WM_SIZE`, and a resize *drag* sends one per pixel, so the drag crawled and flashed the "Formatting…" progress bar on every step. The debounce that existed keyed off **screen rows**, which mid-3k.txt (3,000 long lines → >5,000 wrapped rows) tripped wrongly. | 3.93 Release-KR (and dev) | **fixed** — `OnSize` now defers to a 120 ms one-shot timer (`ID_TIMER_WRAP_REFORMAT`, re-armed each size step) when the doc exceeds `LARGE_FILE_LINE_COUNT` (1,000 **logical** lines, the same threshold the progress bar uses); small files still wrap inline live. Verified: `mid-3k.txt` reformats once after the drag stops, `ascii.c` tracks the drag live. For 3.94. |
| 8 | §A2 · print preview a Korean file with a Latin printer font (Consolas is the default; Courier New too) | **BUG (preview-only, not the printed output).** In print preview, Korean glyphs overlapped and long lines showed garbled fallback glyphs; a **D2Coding** (Hangul-bearing) printer font previewed cleanly, and **actual print (PDF/paper) was always correct**. Root cause: a Latin printer font has no Hangul, so it leans on GDI's *implicit* font linking, which misrenders CJK in MFC's scaled preview DC (`CPreviewDC`); real printing links fine at full resolution. Confirmed by probes (per-char vs whole-run extent, `IMLangFontLink2` mapping) rather than assumed. | 3.93 Release-KR | **fixed** — explicit font linking in the print draw path ([DrawPrintWord](../src/view/cedtViewDraw.cpp), [FillPrintCharDx](../src/view/cedtViewFormat.cpp)): split each word into runs the base font can/can't render (`GetStrCodePages`), map the uncovered runs to the correct linked font (`MapFont`, sized off the **printer** DC and selected **through** `CPreviewDC` so it scales like the base font), and draw every run with an `ExtTextOut` dx array so glyph advances match the laid-out widths. The user's font choice is kept — no default change. Korean now renders correctly at the right size. **Residual:** preview glyphs are still *slightly* larger than the printed output — `CPreviewDC`'s scaling rounding, cosmetic and preview-only; the print is exact. For 3.94. |
| 9 | §A5 / §A7 · edit a line at the maximum length (`MAX_STRING_LENGTH`) | **BUG (hidden overflow → corruption).** Typing *looked* blocked at the limit, but only because the analyzer stops laying out words at the ceiling — the physical `CString` kept growing past it via an unguarded `Insert` (no length check on any insert path). The extra characters became invisible and unreachable (caret/geometry stop at the limit), so **Delete/Backspace then acted on the wrong character** and a line **join** (Delete at EOL, Backspace at BOL, Alt+J, Ctrl+Del, Ctrl+Backspace) could merge two lines well past the limit. IME input hit the same wall, and refusing it left a **dangling composition** in the IME. Also an off-by-one: the ceiling was `MAX_STRING_LENGTH-1` (32766) though the constant, buffer, and `SHORT` all say 32767. Traced with a subagent across the insert/delete/analyze paths. | 3.93 Release-KR | **fixed** — `WouldExceedLineLimit` guards every text-entry and line-join path (`ActionInsertChar`, `ActionInsertString`, IME `ActionCompositionCompose`/`Result`, `ActionPasteLineSelection`/`ActionPasteColumnSelection`, and the seven join sites): over the limit it beeps and refuses the whole operation (paste included) instead of building a hidden-overflow line. IME refusal now also cancels the composition (`CancelComposition` → `ImmNotifyIME` `CPS_CANCEL`). The analyzer ceiling is corrected to `MAX_STRING_LENGTH` (32767), matching the constant/buffer/`SHORT`. Physical and analyzed lengths can no longer diverge. For 3.94. |
| 10 | §A7 · `Evaluate Line` (Ctrl+Enter) on a line with a single token longer than 2048 chars | **BUG (stack buffer overrun → crash).** `EvalConstant`/`EvalVariable`/`EvalFunction` copied a number/name token into a fixed `TCHAR[2048]` with `_tcsncpy(buf, p, nLen)` and `buf[nLen] = '\0'` — no cap on `nLen`. A token ≥ 2048 chars (e.g. a 3000-digit number) overran the stack and crashed. The memory-safety pass bounded ~20 parse sites but missed these three. The *line-copy* side was already safe (Evaluate reads the whole line directly), so a long **expression** of small tokens is fine — verified `1+…+1` (3999 chars) = 2000 and (32759 chars) = 16380; only an over-long single **token** crashed. | 3.93 Release-KR | **fixed** — all three ([evaluate.cpp](../src/util/evaluate.cpp)) now reject an over-long token with a new `EVAL_ERROR_TOKEN_TOO_LONG` ("token too long") instead of copying it — no overrun, and no silently-truncated wrong answer. `Evaluate Line` also **beeps** on any evaluation error now (the inserted `error(...)` line was easy to miss). Unit-test regression added (`OverLong{Number,Variable,Function}Token_ReportsErrorNotOverrun`, `LongSumEvaluatesFully`) — evaluate.cpp is in the test harness. For 3.94. |
| 11 | §A7 · `Evaluate Line` on a malformed expression with a missing operand (`1++`, `1+`, `1*`, empty) | **BUG (accepts invalid input; reads uninitialised memory).** `1++` returned `1` with no error. `EvalFactor` returned at end-of-string **without setting an error**, so the caller added in an **uninitialised `double`** (it happened to be 0, so the answer looked like 1). Pre-existing, unrelated to the token fix (row 10) — surfaced while testing it. | 3.93 Release-KR | **fixed** — `EvalFactor` now reports `EVAL_ERROR_WRONG_SYNTAX` when an operand is missing ([evaluate.cpp](../src/util/evaluate.cpp)); the caller already returns on error before touching the value, so the uninitialised read is gone too. Regression test `MissingOperand_IsSyntaxError` (`1++`, `1+`, `1*`, empty). For 3.94. |
| 12 | §A7 / §Tools · a user tool with **Capture Output** on | **BUG (Unicode-migration regression — all captured output garbled).** Any tool's output came back as garbage (`echo hello` → `敨汬൯`): `OnTimerCaptureOutput` reads the child's **bytes** (console/OEM code page) straight into a `TCHAR[]` **wide** buffer with no conversion, so every two bytes became one wrong wide char. The migration widened the buffer but not the byte handling. The stdin **write** side had the mirror bug (wide buffer written to the child's pipe as-is). Not specific to long input — surfaced while testing §A7 item 2. `dwSave` also conflated byte counts with wide indices. | 3.93 Release-KR | **fixed** — [OnTimerCaptureOutput](../src/view/cedtViewCommand.cpp): read child output into a `CHAR` buffer and `MultiByteToWideChar(CP_OEMCP, …)` into the wide buffer; write child input via `WideCharToMultiByte(CP_OEMCP, …)`. Byte/wide counts kept distinct. (Capture uses a pipe + child process + timer, so no unit test; verified by running a tool.) For 3.94. |
| 13 | §A7 / §Tools · output-window **Copy All** / **Copy** after a long captured line | **BUG (stack overrun → crash).** With an output line at or past 2048 chars (e.g. a long command echo), Copy All crashed: `OnOutputWindowCopyAll`/`OnOutputWindowCopy` read each listbox item into a fixed `TCHAR[2048]` via `CListBox::GetText(int, LPTSTR)`, which copies the whole item with no length cap. | 3.93 Release-KR | **fixed** — use the self-sizing `CListBox::GetText(int, CString&)` overload in both ([OutputWindow.cpp](../src/panels/OutputWindow.cpp)); no fixed buffer, no overrun. Found while testing finding 12 (a >2048-char capture then Copy All). For 3.94. |
| 14 | §A7 · a truncated `.prj` that ends at `<localfile`, with no attributes and no `>` | **BUG (out-of-bounds stack access).** `getline(szText, 4096, L'>')` extracts nothing at EOF, so `nLen` is 0 and `if( szText[nLen-1] == '/' ) szText[nLen-1] = '\0'` **reads `szText[-1]`** — and on a chance match writes it, corrupting whatever precedes the buffer (`CMapStringToString mapAttr` is declared right there). Four identical sites. **Debug cannot demonstrate this**: `/RTC1` fills the guard zone with `0xCC`, which never equals `'/'`, so the write is not taken; the `IDS_ERR_WRONG_PRJ_FILE` dialog that *does* appear is the proof the line executed, since reaching it requires passing through. Release has no guard bytes and reads real adjacent stack. | 3.93 Debug-KR (read confirmed; write is Release-only and data-dependent) | **fixed** — `nLen > 0` guard at all four sites ([FileWndProject.cpp](../src/panels/FileWndProject.cpp)), with a header comment recording why a passing Debug run is not evidence here. For 3.94. |
| 15 | §A7 · a `.key` (or `.dic`) holding a token longer than `MAX_WORD_LENGTH` (255) | **BUG (a keyword nobody wrote).** `stream.width(N)` bounds the read but does **not** discard what did not fit: the tail stays in the stream and comes back as the *next* token. A 300-char keyword therefore registered twice — truncated at 255, and again as a 45-char remainder — and that phantom entry then **coloured in the editor** (the dictionary equivalent makes a real misspelling read as correct). The truncated head is harmless on its own, since `LookupTable` refuses anything over 255 outright. Found with `long-token.key` + `long-token.ada`. | 3.93 Debug-KR | **fixed** — `_DiscardOverlongTokenTail` drops the remainder in both `CKeywords::FileLoad` and `CDictionary::FileLoad` ([cedtElement.cpp](../src/core/cedtElement.cpp)), called only when the token came back exactly 255 long (a genuinely-255 token is followed by whitespace, so it is a no-op). Regression tests `FileLoad_Overlong{Keyword,Word}_DoesNotRegisterItsTail`, confirmed to fail without the fix. For 3.94. |
| 16 | §A7 · directory panel showing a file whose full path is exactly 259 chars (the legal maximum) | **BUG (the file is invisible).** `InsertDirectoryTreeItem` copied the path with `lstrcpyn(szTemp, lpszPath, MAX_PATH - 1)`, keeping only **258** characters — one short of what Windows allows. `…\f.txt` became `…\f.tx`, `SHGetFileInfo` failed on a name that does not exist, and the item was never inserted. Explorer lists the file fine. Probed `SHGetFileInfo` directly to confirm the mechanism: a 259-char path succeeds as-is, and only the truncated-then-backslashed form fails. Not a memory-safety bug — the off-by-one loses data instead of overrunning — but it is the `MAX_PATH - 1` shape this row was written to look for. | 3.93 Debug-KR | **fixed** — `InsertDirectoryTreeItem` and `InsertDirectoryTreeRoot` build the path in a `CString`, no cap at all; `ExpandDirectoryTreePath` keeps its in-place tokenising but gets `TCHAR[MAX_PATH + 2]` and `lstrcpyn(..., MAX_PATH)`, without which a maximum-length **directory** cannot be navigated to ([FileWndDirectory.cpp](../src/panels/FileWndDirectory.cpp)). Its `szPathName[_tcslen(...)-1]` on an empty path — the row-14 shape again — is guarded too. Verified: all three fixture files now appear. For 3.94. |
| 17 | §A7 · start the editor with a workspace whose file was deleted since last run | **BUG (crash on startup, Debug *and* Release).** Restoring `cedt.wks` calls `SpawnDocumentFile` on every recorded path with no existence check. `OnOpenDocument` fails at [cedtDoc.cpp:359](../src/doc/cedtDoc.cpp#L359), and its error box **pumps messages** — which dispatches the `ID_FILE_TAB_REFRESH` that `Insert`/`DeleteMDIFileTab` had posted. `UpdateMDIFileTab` then runs against a child frame whose document is not attached yet, so `GetActiveDocument()` returns NULL and `pDoc->GetTitle()` dereferences it. A workspace entry going stale is completely ordinary (file deleted, renamed, drive not mounted), so this is reachable in normal use — hit here by deleting a fixture tree the editor still had open. | 3.93 Debug-KR **and** Release-KR | **fixed** — two layers. `LoadWorkspaceItem` skips entries whose file is gone ([FileWndProject.cpp](../src/panels/FileWndProject.cpp)), so the failing path is never entered for the common cause; and `UpdateMDIFileTab` returns early when `GetActiveDocument()` is NULL ([FileTab.cpp](../src/panels/FileTab.cpp)), covering the other ways an open can fail (permissions, a lock, a race between the check and the open). Verified: the same stale `cedt.wks` now restores the surviving files and drops the missing one silently. For 3.94. |
| 18 | §A7 · directory panel **rename / copy / move / delete** on a file whose path is exactly 259 chars | **BUG (the shell operated on a phantom second file).** `SHFileOperation`'s `pFrom`/`pTo` are **double**-null-terminated lists. `TCHAR szFrom[MAX_PATH]` with `lstrcpyn(..., MAX_PATH)` holds 259 characters plus their NUL — filling all 260 slots, leaving nowhere for the list terminator. The shell read past the buffer and took the trailing stack bytes as a second source: **rename** failed with "select only one file to rename", **copy** silently did nothing. Shorter paths left a spare zeroed slot, so only maximum-length paths misbehaved. Seven buffers across the four handlers. **Delete was not attempted before the fix** — `FO_DELETE` would have tried to delete that phantom second path. | 3.93 Debug-KR | **fixed** — the buffers are `kShellOpPathBufSize` (`MAX_PATH + 2`), so `memset(0)` supplies both terminators; the copy length stays `MAX_PATH`, so a 259-char path is still not truncated ([FileWndDirectory.cpp](../src/panels/FileWndDirectory.cpp)). Verified: rename, copy, move and delete now behave identically at 200, 258 and 259 characters, in Debug-KR and Release-KR. For 3.94. |
| 19 | §A7 · double-click a file whose path is longer than `MAX_PATH` (copy a file into a near-limit directory to make one) | **BUG (silent no-op).** Nothing happened at all — no window, no error. `OpenDirectoryItem` returns FALSE when `VerifyFilePath` fails and nobody reports it. The panel can *list* such a file because the tree is built through shell APIs, which tolerate over-long paths, while `VerifyFilePath` uses `CFileFind`/`FindFirstFile`, which does not — so the item is visible and unopenable, and from the user's side a double-click simply does nothing. Notepad opens the same file, because it declares `longPathAware` and this machine has `LongPathsEnabled = 1`; [res/cedt.manifest](../res/cedt.manifest) does not declare it. | 3.93 Debug-KR | **fixed (reporting only)** — `OpenDirectoryItem` now shows `IDS_ERR_FILE_NOT_FOUND` with the path ([FileWndDirectory.cpp](../src/panels/FileWndDirectory.cpp)), reusing an existing string so neither `.rc` had to be touched. Directories are filtered out first: a double-click reaches the same function for a folder, and `VerifyFilePath` rejects those too, so without the guard every folder expansion would have popped an error box. **Deliberately not made to work**: `longPathAware` would let >260-char paths into the `TCHAR[MAX_PATH]` buffers this pass keeps finding bugs in (rows 16 and 18 today), turning a safe "cannot open" into truncated paths in destructive operations. That needs a full audit of the path-handling routes first — out of scope for 3.94. |
