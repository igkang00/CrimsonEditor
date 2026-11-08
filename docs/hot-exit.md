# Hot exit and automatic backup

**Status:** implemented on `feature/hot-exit`. Written first as the design, then updated to
record what the implementation changed — see [What changed from the plan](#what-changed-from-the-plan).

Quitting with unsaved documents used to ask about each one: save, discard, or cancel. Modern
editors (VS Code calls it *hot exit*, Notepad++ *session snapshot*) skip the question and simply
bring every unsaved document back, edits included, on the next start. Crimson Editor now does the
same, and the same mechanism also recovers work after a crash or a forced reboot.

## Decisions

| | |
| --- | --- |
| On / off | No new option. It follows **Reload working files on startup**: when that is on, unsaved documents are kept across exit; when it is off, exit prompts per document exactly as before |
| Automatic backup | Documents up to **1 MB**: 1 s after typing stops. Larger documents: once 30 s have passed since their last backup *and* typing has stopped |
| Format | One file per document: a one-line metadata header, then the body in UTF-8. Written to a temporary file and renamed into place |
| Location | `%APPDATA%\Crimson Editor\backup\`, with `file\`, `remote\` and `untitled\` subfolders and a `session.xml` |
| Discarded | The moment the document is saved, its edits are reverted, or it is closed |
| Scope | Local files, new (untitled) documents, FTP documents, and Windows sign-out / shutdown |

Unchanged: closing one tab, *Close All*, or switching projects still asks about each unsaved
document, as VS Code does. The undo history is not restored, also as in VS Code.

## Reference: how VS Code does it

From [workingCopyBackupTracker.ts](https://github.com/microsoft/vscode/blob/main/src/vs/workbench/services/workingCopy/common/workingCopyBackupTracker.ts)
and [workingCopyBackupService.ts](https://github.com/microsoft/vscode/blob/main/src/vs/workbench/services/workingCopy/common/workingCopyBackupService.ts):

- **Backups are continuous, not taken at exit.** A content change schedules a backup
  `DEFAULT_BACKUP_SCHEDULE_DELAYS.default = 1000` ms later, re-armed while the user keeps typing.
  When auto-save is set to a short delay, it waits `delayed = 2000` ms instead, to avoid racing the
  save. The backup is discarded as soon as the document stops being dirty or is closed.
- So crash recovery is not a separate feature. Every dirty document always has a recent backup, and
  a clean exit only has to leave without asking.
- **One file per document, metadata inline.** The first line is `<resource URI> <JSON meta>\n`
  (capped at 10,000 characters); the content follows. The path is
  `<backup home>/<scheme>/<hash of resource>`, with scheme folders such as `file` and `untitled`.
  Writes go to a temporary file first and are renamed into place.
- Backups live in the roaming profile, `%APPDATA%\Code\Backups`, one hashed folder per workspace
  ([VS Code docs](https://code.visualstudio.com/docs/editing/codebasics)).

## Backup file format

Metadata and body share one file, as in VS Code. With automatic backup, documents are backed up at
different moments. If metadata lived in a shared index, every document's backup would also rewrite
that index, and a crash between the two writes would leave them disagreeing. A file that describes
itself cannot disagree with anything.

```
<cedtbackup version="1" kind="file" path="E:\Work\a.txt" encoding="4" format="1" mtime="1767225600" size="5120" linenum="42" />
...body, always UTF-8...
```

- The header uses the same `<tag name="value" />` attribute syntax as the workspace and project
  files. Values are escaped (`&amp; &quot; &lt; &gt; &#13; &#10;`), so a remote path or an untitled
  title holding quotes or line breaks still fits on one line.
- `kind="file"` — a local file. `path` is the original path.
- `kind="remote"` — an FTP document: `account="0" path="/home/..."` (account number and remote path,
  as the workspace stores them).
- `kind="untitled"` — a new document: `id="..."` (a GUID that names its backup file) and
  `title="Untitled2"`.
- `encoding` and `format` (line endings) are the document's own settings. The encoding value
  already tells a UTF-8 file with a BOM from one without, so there is no separate flag. The body is
  written in UTF-8 regardless, so a CP949 document holding a character CP949 cannot express loses
  nothing. The original encoding is applied again only when the user saves.
- `mtime` and `size` are the original file's status as last loaded or saved (`m_clsFileStatus`).
  Restoring them is what lets the existing *modified outside* check notice a file that changed on
  disk while the editor was closed.
- `linenum` is the caret line.
- A header with a newer `version`, another tag, or no `kind` is not restored, and the file is left
  on disk. Unknown attributes are ignored, so a later build can add some.

Writing uses plain Win32 calls, not `CFile` and not the editor's own `FileSave`: those report a
failure in a message box, and this runs from a timer every half second. Reading reuses the
ordinary UTF-8 loader — the header is simply the first line, dropped afterwards.

## Directory layout

```
%APPDATA%\Crimson Editor\backup\
├── file\        <hash of the lower-cased local path>.bak
├── remote\      <hash of account + remote path>.bak
├── untitled\    <document id>.bak
└── session.xml  the tab order and the active tab
```

- Local paths are lower-cased and slash-normalised before hashing, because Windows paths are
  case-insensitive. Remote paths are hashed as they are, with the account number: a Unix server
  tells `a.txt` from `A.txt`.
- `session.xml` lists every tab by a key — `file:<lower-cased path>`, `remote:<account>:<path>`,
  `untitled:<id>` — that names a document the same way from its open tab and from its restored
  backup. It is a convenience, not a source of truth: without it every backup is still restored,
  only the tab order differs. It is kept current while the editor runs (see *Lifecycle*), read once
  at startup and deleted, so a stale one is never applied.
- No per-workspace subfolder as in VS Code. VS Code runs many windows at once. Crimson Editor saves
  its workspace only from the first instance, so one set of backups is enough.
- Roaming rather than Local AppData, as VS Code does. It keeps every piece of Crimson's data in one
  folder, and the [portable edition](portable-edition.md) can then relocate all of it by changing
  `m_szAppDataDirectory` alone. Backups are deleted on save or close, so the folder is normally
  near-empty and adds little to roaming-profile sync.

## Lifecycle

**While editing.** Every edit goes through `CCedtDoc::SetModifiedFlag(TRUE)`, and every open, new,
reload, save and undo-back-to-saved through `SetModifiedFlag(FALSE)`. The override there is the one
hook: `TRUE` arms the document's backup, `FALSE` deletes it. A 500 ms main-frame timer writes the
documents whose backup has fallen due:

| Last backup | When the next one is written |
| --- | --- |
| none yet, or ≤ 1 MB | 1 s after the last change |
| > 1 MB | 1 s after the last change, but no sooner than 30 s after the previous backup |

The size is that of the previous backup file, which is known for free once written — measuring the
document would mean walking it. The reasoning is from measured save speed: a 100 MB document saves
in 0.14 s ([refactoring-large-file-perf.md](refactoring-large-file-perf.md)), so a backup costs
about 1.4 ms per MB. That is invisible at 1 MB and nears one 16 ms frame at 10 MB. Hand-edited
source files are almost always below 1 MB; larger ones are logs, dumps and generated files, which
do not need second-by-second backups. The backup runs only once typing has paused, so the only cost
a user can feel is when typing resumes during a large write.

The same tick keeps `session.xml` in step with the tabs: it is rewritten when a tab opens, closes,
moves or is activated while any backup exists, and deleted once none is left. So a crash, a power
cut or `taskkill /F` restores the tab order too, not only a clean exit.

**On save, revert, or close.** The document's backup file is deleted. A document that goes back to
unmodified through undo also loses its backup. A Save As that renames a document moves its backup
to the new name and deletes the old one.

**On exit** (`CMainFrame::OnClose`), when *Reload working files on startup* is on:

1. Write now every modified document's backup that is not current, so keys typed just before
   quitting are kept.
2. Write `session.xml`.
3. Set `CCedtDoc::m_bHotExiting`. While it is set, `SaveModified` skips the prompt for a document
   whose backup is current, and `OnCloseDocument` keeps that backup instead of deleting it.
4. **A document whose backup could not be written is not "backed up", so it simply prompts as
   before.** Losing data is never an acceptable outcome of skipping a question.
5. If the main window survives `OnClose`, the user cancelled at a prompt: clear the flag and delete
   `session.xml`.

The decision rests on a separate "backed up" state (`IsBackedUp()`: a backup exists and nothing was
typed since), never on the modified flag, which stays true throughout. A cancelled exit therefore
has nothing to put back.

When the option is off, the exit path is the old one: no backups are kept, every document prompts.

**On Windows sign-out or shutdown** (`WM_QUERYENDSESSION`): the same steps. MFC used to answer that
message by prompting, which held up the shutdown. The state `OnClose` saves — bar layout,
configuration, workspace — is saved here too, since `OnClose` never runs on this path; it was lost
on shutdown before. If another application vetoes the shutdown, `WM_ENDSESSION(FALSE)` cancels as
in step 5.

## Restore

The restore runs in the first instance whenever backups are on disk — even if *Reload working files
on startup* was turned off since, because they hold the user's unsaved work. It is split around the
workspace reload in `InitInstance`:

1. **FTP documents, before the workspace.** Opened from the local cache, not downloaded, then
   given the backup's text. The workspace reload skips an FTP document that is already open —
   otherwise it would download it again, possibly ask for a password, and open a second window.
2. **The workspace**, as before.
3. **Local and untitled documents, after the workspace.** The workspace opens files through
   `SpawnDocumentFile`, which gives an already-open document a *second* window, so these must not be
   open before it runs. A document the workspace opened just has its text replaced.
4. **The tab order** from `session.xml`: tabs it knows go to their recorded places, the rest follow
   in their own order, and the recorded tab is activated.

| Situation | Result |
| --- | --- |
| Original file unchanged | Opened if not already open, text replaced from the backup, marked modified |
| Original changed on disk meanwhile | Restored the same way, with `m_clsFileStatus` set from the backup's `mtime`/`size`, so the existing *modified outside* check reports it |
| Original deleted | Restored as a modified document with that path; saving recreates the file |
| Untitled | Recreated as a new document with its title and id |
| FTP document | Restored with its account and remote path, **without downloading**; the server is contacted only on save |
| FTP account since removed | Not restored; the backup stays on disk |

A restored document adopts its backup file: it counts as backed up, a save deletes the file, and an
edit rewrites it in place. The undo history starts empty, and `m_nSavedUndoCount` is set to `-1`, so
no undo step counts as "back to saved" — without that, one edit and one undo would clear the
modified flag and delete the backup of a document that still differs from the disk.

## Where the code is

| File | What it holds |
| --- | --- |
| [src/core/cedtBackup.{h,cpp}](../src/core/cedtBackup.cpp) | Everything testable without a document: the header, writing and reading a backup, file names, `IsBackupDue`, `CBackupState` (per-document bookkeeping), `FindBackupFiles`, the `session.xml` format, `OrderBySession`, and `CSessionRecord` (when to rewrite `session.xml`) |
| [src/doc/cedtDocBackup.cpp](../src/doc/cedtDocBackup.cpp) | The document side: the `SetModifiedFlag` hook, writing its backup, `RestoreFromBackup` |
| [src/app/cedtAppBackup.cpp](../src/app/cedtAppBackup.cpp) | The restore pass, in its two halves, and `ApplyBackupSession` |
| [src/frame/MainFrm.cpp](../src/frame/MainFrm.cpp) | The backup timer, `BeginHotExit` / `CancelHotExit`, `OnQueryEndSession` / `OnEndSession`, and `SaveSessionState` (split out of `OnClose`) |
| `CCedtDoc::SaveModified`, `OnCloseDocument` | Skip the prompt / keep the backup while `m_bHotExiting` is set and the document is backed up |

## Testing

**Unit** ([tests/cedtBackup_test.cpp](../tests/cedtBackup_test.cpp), 60 tests, written before the
code they test): the header round trip with quotes, line breaks, Korean and 64-bit values; rejecting
foreign, malformed and newer headers; the body round trip with empty lines, a trailing empty line,
surrogate pairs and characters the original encoding cannot hold; a failed write returning FALSE
quietly and keeping the previous backup; file names; the 1 s / 1 MB / 30 s timing; `CBackupState`
including Save As and adoption; `IsBackedUp`; the folder scan skipping `.tmp` and foreign files;
the session file and tab ordering; `CSessionRecord`.

**Manual** (all passed): quit with modified local, untitled and FTP documents and restart; quit with
the FTP server stopped and restart (no download, no password prompt, no second window); save a
restored document and see its backup go; edit and undo a restored document and stay modified;
change and delete files while the editor is closed; `taskkill /F` mid-edit and restart, tab order
included; turn the option off and see the prompts return. Note that Task Manager's *End task* asks
the window to close first, so it takes the clean-exit path; only `taskkill /F` (or a real crash)
exercises crash recovery.

## What changed from the plan

- **The exit no longer clears the modified flag.** The plan marked backed-up documents unmodified
  so MFC would skip their prompt. That made the flag lie for the length of the exit, collided with
  the `SetModifiedFlag(FALSE)` hook (which deletes backups), and left a cancelled exit with flags to
  put back. A separate "backed up" state replaced it.
- **The restore is split in two around the workspace reload**, rather than running after it: FTP
  documents first, so the workspace does not download them again.
- **`session.xml` is kept current while editing**, not only written at exit, so the tab order
  survives a crash too.
- **No separate `bom` attribute** — the encoding value already carries it.
- **The header parser is the backup module's own**, not the project file's `ParseItemAttribute`,
  which lives in a panel class the unit tests cannot link.
