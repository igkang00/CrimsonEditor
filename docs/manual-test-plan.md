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

Run 2026-07-19, first pass.

- [x] In **Release**: delete a 100,000-line selection out of `big.txt`; paste it back. Then the
      same in **Debug** — if a POSITION assert fires, the Release run was silently corrupting.
      Clean in both, and checked by **hash rather than by the assert**: lines 400,000–500,000
      deleted and pasted back, saved out, SHA256 compared against `big.txt` — byte-identical
      (57,644,682 bytes) from Release-KR *and* Debug-KR. The assert is the wrong instrument on
      its own here, since Release does not compile it in at all; a matching hash is the stronger
      claim. No assert fired in Debug either.
- [x] In **Release**: heavy block edits in column mode on a large file. Repeated multi-thousand
      line column insert / delete / paste with undo mixed in, on `big.txt`. Lines outside the
      edited range were untouched, undo restored the original shape, no reformat flicker.
- [x] First run with **no config**: the x64 work resets every config deliberately. Confirm
      defaults load and no "config corrupted" popup appears. Cleared both halves of the user
      state — `%APPDATA%\Crimson Editor` and `HKCU\SOFTWARE\Crimson System` — and launched
      Release-KR. No popup; toolbar, status bar, fonts and colours all came up at their
      defaults; a `.c` file syntax-highlighted (the §A8 install-dir lookup, exercised with no
      config to lean on); and a fresh config was written and survived a restart. This is the
      exact path a real user takes on first launch after upgrading.

---

## B. Feature sweep

Menu order, so nothing is skipped. Untagged items are plain toggles — click, confirm, move on.
Items that are **one code path behind many slots** (Tools 1–0, Macros 1–0, font Custom 1–5, tab
sizes, colour schemes) are grouped: test one, then confirm a second behaves the same.

### File

Run on the installed **3.94** build (rebuilt and reinstalled so the sweep exercises the fixes
from findings 14–19, which the shipped 3.93 did not carry).

- [x] New `Ctrl+N` · Open `Ctrl+O` `[enc]` `[big]` · Open Template `Alt+Shift+O`. All fine.
- [x] Close `Ctrl+F4` · Close All — `[big]` (closing a large file used to reformat it first). No
      reformat, no "Formatting…" bar — `big.txt` closes instantly.
- [x] Reload · Reload As… `[enc]`. Fine.
- [x] Save `Ctrl+S` `[enc]` · Save As `Alt+Shift+S` · Save All — `[big]` (block writes). **Block
      writes verified clean by hash**: `big.txt` edited and saved, then compared to the original —
      byte-identical for all 900,000 lines except the edit, valid UTF-8 throughout, no
      truncation. The block-write path does not corrupt a large file.
- [x] Print `Ctrl+P` `[cjk]` `[wrap]` · Print Preview (→ A2) · Print Setup. Fine (preview covered
      in A2, finding 8).
- [ ] FTP: Open Remote `Ctrl+Shift+O` · FTP Settings — (x64 smoke list; needs a server).
      **Skipped — no server.**
- [x] Recent Files (MRU) — including a path with Korean in it. Korean-path file lands in the MRU
      and reopens.
- [x] Exit `Alt+F4` with unsaved changes. Save prompt appears.

### Edit

- [x] Undo `Ctrl+Z` · Redo `Ctrl+Y` — `[undo]` `[cjk]` `[emoji]` `[col]`. Fine across Korean,
      emoji and column edits; undo-to-origin and redo-to-end both hold.
- [x] Cut `Ctrl+X` · Copy `Ctrl+C` · Paste `Ctrl+V` · Delete — `[cjk]` `[emoji]` `[col]`,
      and paste **to and from another application**. Fine, clipboard round-trips through Notepad.
- [x] Cut Append `Ctrl+Shift+X` · Copy Append `Ctrl+Shift+C` — `[col]` refuses these; confirm beep.
      Refused with a beep in column mode.
- [x] Select All `Ctrl+A` `[col]` (refused) · Select Line `Ctrl+E` · Select Word `Ctrl+D` `[cjk]` ·
      Select Block `Ctrl+B` `[col]` (refused). As specified; Select Word grabs the Korean run.
- [x] Upper/Lower/Capitalize/Invert Case — `[cjk]` (Korean must pass through untouched) `[col]`.
      Korean passes through unchanged (`CString::MakeUpper`/`Lower` are no-ops on Hangul).
- [x] Insert Date `Alt+Shift+D` · Insert Time `Alt+Shift+T` · Insert File `Alt+Shift+F` `[enc]` `[trunc]`.
      Date/Time fine. **Insert File of a non-CP949 file inserted mojibake — finding 20**, fixed.
- [x] Increase/Decrease Indent `Ctrl+.` `Ctrl+,` — `[col]` refuses; `[wrap]`. As specified.
- [x] Delete Line `Alt+Del` · Duplicate Line `Alt+Ins` · Delete Word `Ctrl+Del` `[cjk]`. All fine.
      Ctrl+Del on a Korean run deletes to the end of the run — **standard word-delete, not a bug**
      (investigated, see findings table).
- [x] Join Lines `Alt+J` · Split Line `Alt+K` — `[wrap]`. Fine with wrap on.
- [x] Make Comment `Ctrl+M` — `[col]` (→ A3), and in a language with **no** block comment (`.py`).
      Line-comments a `.py` block; column case covered in A3.
- [x] Column Mode `Alt+C` — the toggle always **invalidates** the layout (`RemoveAll` +
      `InsertGap`), so no stale grid geometry survives; visible rows are then re-laid-out
      lazily, with no full-document pass while unwrapped — and column mode is always unwrapped.
      `[big]` `[wrap]`. Toggles on `big.txt` with no visible stall, which is the lazy path
      working as intended, not a missing reformat. **A proportional-font mid-word selection crept
      one char left per toggle — finding 21**, fixed.

### Search

- [x] Find `Ctrl+F` `[cjk]` `[emoji]` — regex too (the regex engine broke once under Unicode:
      *"5 of 5 failing tests were regex"*). Korean, emoji and regex all search correctly; the
      regex unit tests are green.
- [x] Replace `Ctrl+R` `[cjk]` `[undo]` — replace-all in selection, and in a `[big]` file. Fine;
      Korean replace-all undoes as one step.
- [x] Find Next `F3` · Find Prev `Shift+F3` `[big]` (search from the bottom of a huge file). Fine.
- [x] Find in Files `Ctrl+Shift+F` `[enc]` — over a folder of mixed encodings (x64 smoke list).
      **Hung on `big.txt` and returned nothing — finding 22**, fixed (byte-encoded files were read
      as one giant line). The output-window flood then exposed **finding 23** (scrollbar flicker +
      silent 1,024-line cap), also fixed. Mixed-encoding folder now matches correctly.
- [x] Go To `Ctrl+G` `[big]` — line 900,000. Jumps to the exact line.
- [x] Toggle/Next/Prev Bookmark `Ctrl+F2` `F2` `Shift+F2` — `[big]` `[wrap]`. Fine.
- [x] Prev Editing Position `Ctrl+\` · Pairs Begin/End `Ctrl+[` `Ctrl+]` `[cjk]`. Fine.

### View

- [x] Toolbar · MDI File Tabs · Status Bar · Remote/Project/Output Window — `[trunc]` (tab titles
      and project items were `lstrcpyn`-limited). Toggles fine; long-filename tab renders without
      the earlier `cchTextMax` overrun (row 17).
- [x] **Word Wrap** `Alt+Shift+W` → A1. Covered.
- [x] Spell Check `Alt+Shift+K` · Line Numbers `Alt+Shift+L` `[big]` · Column Markers `[cjk]`. Fine.
- [x] Screen Fonts: Default + one Custom + Set Fonts — `[cjk]` (fixed vs proportional, and
      **column mode substitutes** a fixed one), then confirm a second Custom slot behaves alike. Fine.
- [x] Printer Fonts: same, one slot + Set Fonts. Fine.
- [x] Line Spacing: one of the five + confirm a second. **Decreasing it with the caret on the
      last line crashed — finding 24**, fixed. Also, below 100% from the **View menu** clipped the
      current-line highlight and stuck — **finding 25**, fixed. Both paths now clean.
- [x] Tab Size: one of the four + Custom Tab Size — `[wrap]` `[col]` (tab stops are the grid). Fine.
- [x] Colour Schemes: one preset + Load Saved + Set Colors — `[enc]` (scheme files resolve
      through the install dir → A8). Applies and saves; install-dir resolution as verified in A8.
- [x] Show Spaces `Ctrl+Shift+E` — `[cjk]` `[col]`. Fine.

### Document

- [x] Syntax Type: Auto Detect / Plain Text / Customize — **`.c` must colour on the installed
      build** (→ A8). `.c` colours. `.txt` showed "Auto Detect" checked but no colour, which
      read as auto-detect doing nothing — **finding 26** (a deliberate default-mapping change,
      not a bug).
- [x] Properties · Reload Document · Lock Document. Properties and Reload fine; **Lock Document is
      an unimplemented, disabled menu item** (see findings table).
- [x] Encoding Type (5 items) → A4. Fine (covered in A4).
- [x] File Format: DOS / Unix / Mac → A4. Fine (covered in A4).
- [x] Convert Tabs↔Spaces, Leading Spaces to Tabs, Remove Trailing Spaces — `[cjk]` `[undo]` `[big]`.
      Korean is preserved (untouched — verified byte-identical on `korean.c`); undo works. One quirk
      noted below (**finding —, deferred**): Convert Spaces↔Tabs marks the document modified even
      when it makes no net change.
- [x] Summary — `[cjk]` (character counts vs code units). The "Byte count" was UTF-16 code units
      (neither bytes nor characters). **Finding 27**: replaced with a logical **Characters** field
      (surrogate pair = 1); also added thousands-comma formatting and made the attribute checkboxes
      read-only display. Verified on `korean.c`, `astral.txt`, `big.txt`.

### Project

- [x] New / Open / Close Project — `[enc]` `[trunc]`. Korean-path workspace saves and restores
      (finding 5, fixed). Older `.prj`/`.wks` are now refused by a version compare (finding 28).
      Opening / executing / properties on a project item whose file was deleted from disk used to
      fail silently and now reports it (finding 29). New / open / close all work.
- [x] New Category · Add to Project · Add Active File · Add All Open Files — all work.

### Tools

- [x] Preferences — open, change something, OK; restart and confirm it stuck. Works. Spotted the
      dialog's right margin was clipped (finding 30, fixed).
- [x] Evaluate Line `Ctrl+Enter` `[trunc]` `[cjk]` — covered by findings 10–11 (§A7).
- [x] MS-DOS Shell `F10` · View in Browser `Alt+B` `[trunc]` (paths) — work.
- [x] Load User Tools · Conf. User Tools — configure **one** slot, run it, capture output
      `[trunc]`, then confirm a second slot runs. Works (capture output was finding 12).
- [x] Menu labels render correctly — verified by inspection (all CString-based, no fixed buffer);
      the old `- Empty -\t<garbage wchars>` hazard does not reproduce. Renders cleanly.

### Macros

- [x] Begin/End Recording, Replay `Alt+Enter` — record something with **Korean** `[ime]`, with an
      **emoji** `[emoji]`, and with a **column edit** `[col]`; replay each. Recording is Unicode-safe
      (emoji + IME recorded via `MacroRecordString`, not lone code units); all replay correctly.
- [x] Load User Macros · Conf. User Macros — one slot, then confirm a second. Works. Corrupt/failed
      loads now report (finding 31); Save…/Load… round-trips (finding 32); extension is now `.macros`
      (finding 33).
- [x] Menu labels (same `- Empty -` hazard) — verified clean (CString-based, no fixed buffer).

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
| 20 | §B Edit · **Insert File** `Alt+Shift+F` of a file whose encoding differs from the system code page | **BUG (Korean inserted as mojibake).** `CMemText::FileLoad` — the Insert File / drag-drop "insert as block" path — hard-coded `ENCODING_TYPE_ASCII` (→ `CP_ACP`), so it never looked at the file's actual encoding. On a Korean system (`CP_ACP` = 949) a CP949 file happened to come in right, but a **UTF-8 or UTF-16** file was decoded as CP949 and its Korean turned to garbage. The main open path (`CAnalyzedText::FileLoad`) detects and decodes correctly; only Insert File was left behind, even though its own comment already claimed to guard against a mangled CP949 pair. | 3.94 Release-KR | **fixed** — `CMemText::FileLoad` now calls `DetectEncodingTypeAndFileFormat` and runs the same encoding-aware split as the main loader: BOM skip, UTF-16 read in 2-byte units, DOS/Unix/Mac line ends, `_DecodeLine` with the detected type ([cedtElement.cpp](../src/core/cedtElement.cpp)). Regression tests `CMemTextTest.FileLoad_Utf8Bom_*` / `_Utf16LE_*`, confirmed to fail without the fix; they use self-describing encodings so they pin the behaviour on any system code page. For 3.94. |
| — | §B Edit · **Delete Word** `Ctrl+Del` from the middle of a Korean run | **Investigated — not a defect.** Reported as "Ctrl+Del deletes all Korean after the caret." That is standard delete-word behaviour, identical to English: the editor treats a run of one character type as a single word ([GetNextSegmentIdxX](../src/view/cedtViewMapAdv.cpp)), so `가나다\|라마` deletes `라마` just as `abc\|def` deletes `def`. Notepad behaves the same. Left unchanged: making Korean delete syllable-by-syllable would diverge from the editor's word model and from every other language. | 3.94 Release-KR | not a bug — no change |
| 21 | §B Edit · **Column Mode** `Alt+C` toggle with a **proportional** screen font and a mid-word selection | **BUG (selection creeps left one char per toggle).** In a proportional font, selecting with start and end inside English words and toggling column mode repeatedly slid both endpoints left by one character each line→column switch, accumulating. Root cause: `OnEditColumnMode` flipped `m_bColumnMode` **before** saving the selection, but `SaveCaretAndAnchorPos` → `GetIdxXFromPosX` picks its measurement branch off that flag (via `_bGridLayout` in `GetWordIndex`). So the save measured the still-proportional layout with **grid-cell** logic — a mismatch that mapped the pixel position to the wrong character index. | 3.94 Debug-KR **and** Release-KR | **fixed** — move `SaveCaretAndAnchorPosAllViews()` ahead of the mode flip in `OnEditColumnMode` ([cedtAppView.cpp](../src/app/cedtAppView.cpp)), so the flag still describes the layout whose positions are being converted. Restore stays after the reformat, where the flag correctly describes the new layout. Verified in both builds: mid-word selection holds across repeated toggles, proportional and with Korean. For 3.94. |
| 22 | §B Search · **Find in Files** `Ctrl+Shift+F` over a folder that contains a large file | **BUG (app hangs; large files not searchable) — a Unicode-migration regression.** `FindInFilesInFile` read each file with `CArchive::ReadString`, which scans for `L'\n'` in `sizeof(TCHAR)` units. In the Unicode build `TCHAR` is 2 bytes, and a byte-encoded file (ANSI / UTF-8 — most files) contains no such unit, so the **whole file came back as one gigantic line**. On a large file that single `ReadString` froze the app — no pump point inside it, window went "Not Responding", force-quit — and the over-long line was then skipped by the `MAX_STRING_LENGTH` guard, so big files could not be searched at all (0 matches). The exact hazard the `[enc]` tag flagged: the doc loaders were moved to block reading for this reason, but Find in Files stayed on the old path. | 3.94 Debug-KR | **fixed** — read with `CMemText::FileLoad`, the same encoding-aware, block-at-a-time loader the editor opens files with ([cedtAppSearch.cpp](../src/app/cedtAppSearch.cpp)): detects the encoding, splits lines correctly, returns fast. Large files are now searchable and quick, and mixed-encoding folders (CP949 / UTF-8 / UTF-16) match correctly. Also pump the queue periodically during the scan and let **Esc** abort, so a large folder stays responsive. For 3.94. |
| 23 | §B Search · output window while a **Find in Files** result floods it | **BUG (scrollbar flicker) + two usability gaps.** At the line cap, `AddStringToTheLast` ran `DeleteString(0)`, `AddString` and `SetTopIndex` each with redraw on, so the scrollbar recomputed three times per line — thumb up (oldest dropped), down (new added), then to the bottom — flickering rapidly during a big search. Separately, the window silently kept only the most recent 1,024 lines, so the "N found" count and the visible list disagreed with no explanation. | 3.94 Debug-KR **and** Release-KR | **fixed** — freeze redraw across the whole delete/add/scroll sequence, one update per line ([OutputWindow.cpp](../src/panels/OutputWindow.cpp)), which also cut repaint work enough to speed the search up noticeably. Raised `OUTPUT_MAX_LINE_COUNT` 1024 → 4096, and when the ring buffer drops lines the search now ends with a note that only the most recent N lines are shown (`IDS_OUT_SEARCH_TRUNCATED`). For 3.94. |
| 24 | §B View · **Line Spacing** — decrease it with the caret on (or near) the last line | **BUG (NULL-deref crash).** `GetFirstPosX` did `rLine.m_pWord[0]` on a row whose `m_pWord` was NULL. Path: the Preferences OK writes the new `m_nLineSpacing` first, then `ApplyPreferencesToAllViews` runs `SaveCaretAndAnchorPos` → `GetIdxXFromPosX` → `GetFirstPosX`. `GetLineHeight` scales by `m_nLineSpacing`, so with the spacing already reduced, `m_nCaretPosY / GetLineHeight()` (an old-height pixel over the new, smaller height) overshot the last row. `GetLineFromPosY` no-op'd `EnsureFormattedAt` on the out-of-range index and returned an **unformatted** `GetTail()` row (lazy layout leaves off-screen rows with `m_pWord == NULL`), which the geometry helper dereferenced. Same family as row 21 — Save/Restore colliding with a layout-metric change. | 3.94 Debug-KR **and** Release-KR | **fixed** — `GetLineFromPosY` now clamps the index into `[0, count)` before formatting and fetching ([cedtViewMap.cpp](../src/view/cedtViewMap.cpp)), so the row it formats is always the row it returns and no unformatted row escapes the gateway. Matches the clamp `GetIdxYFromPosY` already applies. Verified in both builds: line spacing up/down with the caret at end / middle / start no longer crashes, and the caret holds. For 3.94. |
| 25 | §B View · **Line Spacing** from the **View menu** (not the Preferences dialog), below 100% | **BUG (current-line highlight clipped, and stuck).** Setting spacing under 100% (e.g. 90%) from the View menu clipped the bottom of the active-line highlight, and raising it back above 100% left it clipped — yet the Preferences dialog applied the same value cleanly. Cause: each view's active-line highlight buffer is sized `2 * (GetLineHeight() + 1)` in `OnScreenFontChange`, and `ApplyCurrentScreenFont` is the only thing that drives `OnScreenFontChange` across the views. The Preferences and screen-font paths call it; `OnViewLineSpacing` did not, so the buffer kept its old height and the highlight was drawn against a stale size. | 3.94 Debug-KR **and** Release-KR | **fixed** — `OnViewLineSpacing` now goes through `ApplyCurrentScreenFont` (then `FormatScreenTextAllViews`) like the other line-height-changing paths ([cedtAppHndr.cpp](../src/app/cedtAppHndr.cpp)), so the active-line buffer is rebuilt at the new line height. The font is unchanged; only the line-height-dependent buffers and layout are refreshed. Verified: 90% no longer clips, and raising back to 100% recovers, matching the Preferences path. For 3.94. |
| 26 | §B Document · **Syntax Type** menu / `.txt` default mapping | **Not a bug — a deliberate change.** The Syntax Type radio reflects the current (spec, keywords) pair, not an auto/manual flag. `.txt` linked (via `link/extension.txt`) to `DEFAULT.SPC` / `DEFAULT.KEY`; `DEFAULT.KEY` has no keywords, so `.txt` got no colour, yet a pair *was* assigned and matched no named language, so the menu showed **Auto Detect** checked — reading as "auto-detect ran but did nothing". | 3.94 Release-KR | **changed** — (1) removed `link/extension.txt` so `.txt` falls through to an empty spec and reads as **Plain Text** ([runtime/link/](../runtime/link/)); (2) seeded default bracket pairs `()[]{}` in `CLangSpec::ResetContents` so pair matching keeps working on any spec-less file ([cedtElement.cpp](../src/core/cedtElement.cpp)) — word delimiters already had a built-in default; (3) audit found only `default.spc` set `$PAIRS4=<>` and the loader only ever supported three pair slots, so `<>` never worked — removed the dead `$PAIRS4` lines from `default.spc`, `d.spc` and the `langspec.key` directive list. `<>` intentionally not added (no slot; would misfire on comparison operators). For 3.94. |
| 27 | §B Document · **Summary** dialog — byte count, number formatting, attribute checkboxes | **BUG (mislabeled count) + two UX fixes.** The "Byte count" field summed `rLine.GetLength()`, i.e. UTF-16 **code units** — an MBCS-era assumption that a char equals a byte. Post-Unicode it is neither bytes (a CP949/UTF-8 Korean char is 2–3 bytes) nor characters (a surrogate-pair emoji counts as 2). The file-size field already reports on-disk bytes. | 3.94 Debug-KR | **fixed** — new **Characters** field (`IDC_CHAR_COUNT`, `CCedtDoc::GetCharCount` → `CAnalyzedText::GetCharCount`) reporting logical code points (a surrogate pair is one character) plus one per line break; the old byte symbols are left in place but unused ([cedtElement.cpp](../src/core/cedtElement.cpp), [DocumentSummary.cpp](../src/dialogs/DocumentSummary.cpp)). Regression tests `AnalyzedTextFileTest.CharCount_*` (emoji = 1, not 2). Also, while here: (a) the four numeric fields now group thousands with commas (`57,644,723`); (b) the read-only/hidden/system attribute checkboxes are shown as `WS_DISABLED` boxes with **separate** static labels, so they read as non-settable without greying the label text. **Follow-up:** the status-bar caret "character" position had the mirror problem — it showed `nIdxX`, the code-UNIT index, so it stepped by 2 across a surrogate pair while the new Characters field counted it as 1. Made the status bar count code points too ([cedtView.cpp](../src/view/cedtView.cpp)): a display-only conversion at the one `SetCaretPositionInfo` site (`nIdxX` untouched — it still drives caret positioning), so an emoji advances the position by 1, matching the Summary. Column mode still reports the display column by design. For 3.94. |
| — | §B Document / View · unimplemented menu items **Lock Document** and **Remote Window** | **Investigated — deliberately left alone.** Both are greyed out because they have **no handler** at all (no `ON_COMMAND`, no `ON_UPDATE_COMMAND_UI`), so MFC auto-disables them. **Lock Document** (`ID_DOCU_LOCK`) was meant to lock the buffer from edits — distinct from the file's read-only attribute, which the editor honours for *saving* only (typing is never blocked) and which has no in-editor toggle either. **Remote Window** (`ID_VIEW_REMOTE_WINDOW`) is the one window toggle of four with no handler, though the panel class (`FileWndRemote`) exists — only the View-menu wiring is missing. Not touched: this branch verifies the Unicode migration against existing features, not build new ones. Left for a separate branch. | 3.94 | not implemented — out of scope |
| — | §B Document · **Convert Spaces↔Tabs** marks the document modified with no net change | **Investigated — pre-existing, deferred.** On `korean.c`, Convert Spaces to Tabs set the modified (red-bullet) flag but nothing looked different. Traced live: the file's leading tabs round-trip — `FastConvertSpacesToTabs` first expands every tab to spaces (`FastConvertTabsToSpaces`) then re-collapses, so a leading tab becomes 4 spaces and back to a tab. The result is **byte-identical** (verified 491→491 bytes), but each edit primitive (`FastInsertChar`/`FastDeleteString`) unconditionally calls `SetModifiedFlag(TRUE)` and records an undo step, so the doc is flagged dirty and the undo buffer gets no-op edits. Not `[cjk]`-specific (Korean is never touched) and not a Unicode regression — it happens on any tab-indented file. A proper fix (compute the target per line, edit only if different) is ~30 lines across the convert functions plus undo testing; out of scope here. Recorded so it is not re-investigated. | 3.94 Debug-KR | pre-existing — out of scope |
| 28 | §B Project · open a `.prj` / `.wks` written by an **older format version** | **Not a bug — a hardening change.** The `version` attribute in a project/workspace file was parsed and then **ignored**, so a file from any earlier version loaded regardless. Because pre-Unicode editors wrote these files in CP949 while this build reads them as UTF-8, an old file's Korean paths broke silently against the reader — the same class of failure as finding 5. | 3.94 Release-KR | **fixed (compatibility check)** — all three load sites (`<project>`, `<workspace>`, and the workspace-item root) now compare the file's `version` against `STRING_PROJECTFILEVER` (`"Crimson Editor 3.90"`); a mismatched or missing version is refused with a clear message — new `IDS_ERR_PRJ_VERSION` "This project file's version (%s) is not compatible…" ([FileWndProject.cpp](../src/panels/FileWndProject.cpp)). Save writes the same `STRING_PROJECTFILEVER`, so files created by this build round-trip; only genuinely older files are rejected. Verified with a hand-edited old-version `.prj`. For 3.94. |
| 29 | §B Project · open / execute / properties on a project item whose file was **deleted from disk** | **BUG (silent no-op).** A file still listed in the project but removed in Explorer did nothing when opened — no window, no error — because `OpenProjectItem` returns FALSE when `VerifyFilePath` fails and nobody reported it. The right-click **Execute** and **Properties** actions failed the same silent way. Same shape as finding 19 (directory panel). | 3.94 Debug-KR **and** Release-KR | **fixed (reporting only)** — all three handlers (`OpenProjectItem`, `ExecuteProjectItem`, `ShowPropProjectItem`) now show `IDS_ERR_FILE_NOT_FOUND` with the path and a STOP icon on a missing file ([FileWndProject.cpp](../src/panels/FileWndProject.cpp)), reusing the existing string so neither `.rc` was touched. `VerifyFilePath` is kept as the existence check (it rejects directories, which the directory helper's `GetFileAttributes` would accept). For 3.94. |
| 30 | §B Tools · **Preferences** dialog — right margin clipped | **BUG (layout).** The dialog's content sits flush against the right edge with no margin, while the categories tree on the left has an 8px margin and the buttons have room below. `SizeAllPrefPages` lays out every page to `x=538` in client coordinates but sized the dialog with `MoveWindow(…, 546, 440)` — 546 is a **window** width, so the frame ate ~8px and the client came out ~538 wide, leaving zero margin on the right. Pre-existing; more visible now that the frame/DPI metrics differ from the original. | 3.94 Release-KR | **fixed** — size the window from the intended **client** width via `CalcWindowRect` on a `546`-wide client rect, so the client is exactly 546 and the right margin matches the left 8px regardless of theme/DPI border ([prefdialog.cpp](../src/dialogs/preferences/prefdialog.cpp)). Height left at 440 (vertical spacing was already fine). For 3.94. |
| 31 | §B Macros / Tools · loading a corrupt or truncated `.macro` / `.command` file | **BUG (stack buffer overrun on load).** `CMacroBuffer::StreamLoad` and `CUserCommand::StreamLoad` read a char-count `nLength` straight off the file and pass it to `_READ_WIDE_STR(fin, szBuffer, nLength)` into a `TCHAR szBuffer[4096]`, then write `szBuffer[nLength] = '\0'` — with **no bounds check**. A garbage or oversized length (a truncated `cedt.macro`, a hand-edited file) overruns the stack. Both files are loaded at startup and via Load User Macros / Load User Tools. Same load-path family as findings 14–16; a previous pass guarded the other `StreamLoad`s in this file (`nLength >= 4096`) but missed these two. Found by inspection while reviewing the Macros record/replay path (which is itself Unicode-safe — emoji and IME results are recorded via `MacroRecordString`, not lone code units). | 3.94 Release-KR | **fixed** — `_CHECK_WIDE_LEN` rejects `nLength` outside `[0, 4095]` before every `szBuffer` read in both loaders ([cedtElement.cpp](../src/core/cedtElement.cpp)), matching the existing `>= 4096` guards; returns FALSE like finding 14. **Plus** the silent-ignore that FALSE used to cause: a load the user picked from **Load User Macros / Load User Tools** now reports `IDS_ERR_LOAD_USER_FILE` ("Could not load '%s' … incompatible version or corrupt") instead of doing nothing — routed through new `LoadUserMacroFile` / `LoadUserToolFile` helpers that also stop overwriting the AppData copy on a failed load; the **startup** auto-load stays silent so a missing config on first run does not nag ([cedtAppHndr.cpp](../src/app/cedtAppHndr.cpp)). The **Preferences dialog's own** Load Tools… / Load Macros… buttons had the same silent-ignore — `OnCommandLoadTools` / `OnMacroLoadMacros` dropped the `FileLoad…` return — and now report the same message ([PrefDialogCommands.cpp](../src/dialogs/preferences/PrefDialogCommands.cpp), [PrefDialogMacros.cpp](../src/dialogs/preferences/PrefDialogMacros.cpp)); those dialog loaders were already overrun-safe (they call the same guarded `StreamLoad`). Verified with `tests/data/corrupt-name-length.macro` (name length field = 999999). No unit test on the stream loaders (no fixture harness), but the guard matches the loaders already covered. For 3.94. |
| 32 | §B Tools · Preferences · **Save User Tools… then Load User Tools…** round-trip | **BUG (Unicode-migration regression — a just-saved tools file will not reload).** Save user tools, then load them back → "invalid tool file". `FileSaveUserCommands` / `FileLoadUserCommands` wrote and read the version header as `_tcslen(STRING_USERTOOLSVER)` **bytes** of the *wide* string via `(const char *)STRING_USERTOOLSVER` — a byte count where the string is 2 bytes/char, so it wrote ~half the header and read ~half, and the wide `_tcscmp` against the full `STRING_USERTOOLSVER` never matched → every load failed. In the MBCS era `_tcslen` bytes *was* the whole string, so it worked pre-migration. The **macro** dialog path (`FileSave/LoadMacroBuffers`) and the AppData path (`CCedtApp::Save/LoadUserCommands`) were already migrated to a `CStringA` ASCII header; only the Preferences **tools** path was missed. Surfaced now because finding 31 added the error message that this failure had been silently swallowing. | 3.94 Release-KR | **fixed** — write/read the header as `CStringA sVer(STRING_USERTOOLSVER)` (ASCII, `sVer.GetLength()` bytes, `strcmp`), byte-identical to the macro and AppData paths ([PrefDialogCommands.cpp](../src/dialogs/preferences/PrefDialogCommands.cpp)). Tools now round-trip through Save…/Load…, and a file saved by either the dialog or the AppData path is interoperable across all load paths. For 3.94. |
| 33 | §B Macros · user macro file extension `.macro` → `.macros` | **Consistency change (not a bug).** User **tools** save as `.tools` (plural) but macros saved as `.macro` (singular). Renamed the macro extension to `.macros` everywhere it is user-facing: the Save/Load filter (`IDS_FILTER_MACRO_BUFFER`, US + KR `.rc`), the Save dialog's default extension, and the Load User Macros menu scan (`\tools\*.macros`). The internal AppData store was also renamed `cedt.macro` → `cedt.macros` to match `cedt.tools`. No backward-compat migration: 3.90 is the only public release, so no post-3.90 `cedt.macro` files exist to preserve. | 3.94 Release-KR | **changed** — extension + filter + default ext + menu scan + AppData store (the three save sites and the startup load all use `cedt.macros`) ([cedtapp.cpp](../src/app/cedtapp.cpp), [cedtViewMacro.cpp](../src/view/cedtViewMacro.cpp), [PrefDialogMacros.cpp](../src/dialogs/preferences/PrefDialogMacros.cpp), [cedtAppHndr.cpp](../src/app/cedtAppHndr.cpp), [prefdialog.cpp](../src/dialogs/preferences/prefdialog.cpp), [cedtViewHndrMisc.cpp](../src/view/cedtViewHndrMisc.cpp)). For 3.94. |
| 34 | §B View · color scheme file extension `.color` → `.colors` | **Consistency change (not a bug).** Same as finding 33, for color scheme files — `.color` → `.colors`, matching `.tools` / `.macros`. Renamed the Save/Load filter (`IDS_FILTER_COLOR_SCHEME`, US + KR `.rc`), the Save dialog's default extension, and the AppData/install store `cedt.color` → `cedt.colors` (startup load, its install-dir fallback, and every save site). Color schemes have no directory menu scan and no `.color` file is shipped in the repo, so nothing else needs touching; no backward-compat migration (3.90 is the only public release). | 3.94 Release-KR | **changed** — filter + default ext + `cedt.colors` at every load/save site ([cedtapp.cpp](../src/app/cedtapp.cpp), [cedtAppHndr.cpp](../src/app/cedtAppHndr.cpp), [prefdialog.cpp](../src/dialogs/preferences/prefdialog.cpp), [PrefDialogColors.cpp](../src/dialogs/preferences/PrefDialogColors.cpp)). For 3.94. |
