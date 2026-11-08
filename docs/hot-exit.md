# Hot exit and automatic backup — a design

**Status:** design agreed, implementation not started (it will happen on its own branch).

Today, quitting with unsaved documents asks about each one: save, discard, or cancel. Modern
editors (VS Code calls it *hot exit*, Notepad++ *session snapshot*) skip the question and simply
bring every unsaved document back, edits included, on the next start. This records how Crimson
Editor will do the same, and how the same mechanism also recovers work after a crash or a forced
reboot.

## Decisions

| | |
| --- | --- |
| On / off | No new option. It follows **Reload working files on startup**: when that is on, unsaved documents are kept across exit; when it is off, exit prompts per document exactly as today |
| Automatic backup | Documents up to **1 MB**: 1 s after typing stops. Larger documents: once 30 s have passed since their last backup *and* typing has stopped |
| Format | One file per document: a one-line metadata header, then the body in UTF-8. Written to a temporary file and renamed into place |
| Location | `%APPDATA%\Crimson Editor\backup\`, with `file\`, `remote\` and `untitled\` subfolders and a `session.xml` |
| Discarded | The moment the document is saved, its edits are reverted, or it is closed |
| Scope | Local files, new (untitled) documents, FTP documents, and Windows shutdown / restart |

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
<cedtbackup version="1" kind="file" path="E:\Work\a.txt" encoding="2" bom="1" format="0" mtime="133771234567890000" size="5120" linenum="42" />
...body, always UTF-8...
```

- The header uses the attribute syntax of the workspace and project files, so the existing
  `ParseItemAttribute` reads it — no JSON parser needed.
- `kind="file"` — a local file. `path` is the original path.
- `kind="remote"` — an FTP document: `account="0" path="/home/..."` (account number and remote path,
  as the workspace stores them).
- `kind="untitled"` — a new document: `title="Untitled2"`.
- `encoding`, `bom`, `format` (line endings) are the document's own settings. The body is written
  in UTF-8 regardless, so a CP949 document holding a character CP949 cannot express loses nothing.
  The original encoding is applied again only when the user saves.
- `mtime` and `size` are the original file's status as last loaded or saved (`m_clsFileStatus`).
  Restoring them is what lets the existing *modified outside* check notice a file that changed on
  disk while the editor was closed.
- `linenum` is the caret line.

## Directory layout

```
%APPDATA%\Crimson Editor\backup\
├── file\        <hash of the lower-cased local path>.bak
├── remote\      <hash of account + remote path>.bak
├── untitled\    <document id>.bak        an id given to a new document on its first backup
└── session.xml  written only on a clean exit: tab order and the active tab
```

- Local paths are lower-cased before hashing, because Windows paths are case-insensitive.
- `session.xml` is a convenience, not a source of truth. After a crash it is missing or stale; the
  restore then appends every backup found in the folders after the restored workspace. Only the tab
  order differs; no data is lost. Named files already keep their order in the workspace (the
  project file or the regular workspace), so what `session.xml` really adds is where the untitled
  documents sat.
- No per-workspace subfolder as in VS Code. VS Code runs many windows at once. Crimson Editor saves
  its workspace only from the first instance, so one set of backups is enough.
- Roaming rather than Local AppData, as VS Code does. It keeps every piece of Crimson's data in one
  folder, and the [portable edition](portable-edition.md) can then relocate all of it by changing
  `m_szAppDataDirectory` alone. Backups are deleted on save or close, so the folder is normally
  near-empty and adds little to roaming-profile sync.

## Lifecycle

**While editing.** A document change (re)arms a per-document backup timer. When it fires, the
document is written to its backup file. Size decides the timer:

| Document size | When the backup is written |
| --- | --- |
| ≤ 1 MB | 1 s after the last change |
| > 1 MB | 1 s after the last change, but no sooner than 30 s after its previous backup |

The reasoning is from measured save speed. A 100 MB document saves in 0.14 s
([refactoring-large-file-perf.md](refactoring-large-file-perf.md)), so a backup costs about
1.4 ms per MB. That is invisible at 1 MB and nears one 16 ms frame at 10 MB. Hand-edited source
files are almost always below 1 MB; larger ones are logs, dumps and generated files, which do not
need second-by-second backups. The backup runs only once typing has paused, so the only cost a
user can feel is when typing resumes during a large write.

**On save, revert, or close.** The document's backup file is deleted. A document that goes back to
unmodified through undo also loses its backup.

**On exit** (`CMainFrame::OnClose`), when *Reload working files on startup* is on:

1. Bring every modified document's backup up to date (write any pending ones now).
2. Write `session.xml`.
3. Mark each document whose backup is on disk as unmodified, so MFC's `SaveAllModified` lets it go
   without a prompt.
4. **A document whose backup failed keeps its modified flag and gets today's prompt.** Losing data
   is never an acceptable outcome of skipping a question.

When the option is off, the exit path is today's: no backups are kept, every document prompts.

**On Windows shutdown / restart** (`WM_QUERYENDSESSION`): the same steps. Today MFC answers that
message by prompting, which holds up the shutdown. With backups in place it can answer at once.

## Restore

After the workspace has been reloaded at startup, each backup is applied:

| Situation | Result |
| --- | --- |
| Original file unchanged | Opened if not already open, body replaced from the backup, marked modified |
| Original changed on disk meanwhile | Restored the same way, with `m_clsFileStatus` set from the backup's `mtime`/`size`, so the existing *modified outside* check reports it |
| Original deleted | Restored as a modified document with that path; saving recreates the file |
| Untitled | Recreated as a new document with its title |
| FTP document | Restored from the backup with its account and remote path, **without downloading**; the server is contacted only on save |

The backup files are not deleted on restore. The documents are still modified, so the timers keep
the backups current. If the editor crashes right after restoring, the backups are still there.

## Where the work lands

| Area | Change |
| --- | --- |
| `CCedtDoc` | Backup id and path, `WriteBackup()` / `DeleteBackup()`, last-backup time; hook the modify points (`SetModifiedFlag` callers in `cedtDocEdit.cpp`, `CheckIfAllActionsAreUndone`) to arm the timer; delete on save and close |
| Timer | One frame-level timer that scans for documents due a backup, rather than one per document |
| `CMainFrame::OnClose` | Steps 1–4 above, gated on `m_bReloadWorkingFilesOnStartup` |
| `CMainFrame` | `OnQueryEndSession` / `OnEndSession` |
| `CCedtApp::InitInstance` | Restore pass after the workspace reload |
| Remote documents | Restore without a download, keeping account and remote path |

## Testing

- Unit: header write → parse round trip, including paths with spaces and Korean names, and a body
  holding characters the original encoding cannot express.
- Manual: quit with modified local, untitled and FTP documents and restart; kill the process from
  Task Manager mid-edit and restart; change a file in another program while the editor is closed;
  delete a file while the editor is closed; turn the option off and confirm the prompts return;
  sign out of Windows with an unsaved document.
