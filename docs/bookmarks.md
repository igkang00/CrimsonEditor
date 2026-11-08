# Persistent bookmarks — a design

**Status:** implemented on `feature/bookmarks`. Written first as the design; the sections on
writing, the code and testing record what the implementation found.

A bookmark is one bit on a line (`LI_HAVEBOOKMARK` in `CAnalyzedString::m_usLineInfo`). Because it
lives on the line object, it already follows its line through edits — insert or delete lines above
it and it moves with them — and it survives re-analysis. Toggling it does not mark the document
modified and is not an undo step.

What it does not survive is the document going away: closing the file, reloading it, or quitting
loses every bookmark, because they are stored nowhere. This records how they will be kept.

## Decisions

| | |
| --- | --- |
| Scope | **Per file, permanently.** Reopen a file days later and its bookmarks are back. Restoring a session gets them for free, since the restore opens files |
| Changed on disk | **Found again by content.** Each bookmark keeps a hash of its line; if the file changed, the bookmark moves to the nearest line with the same content, or is dropped if there is none |
| Reload | The same: a reload keeps the bookmarks, re-found by content |
| How many files | The **100** most recently used files with bookmarks |
| Unsaved documents | Kept in the hot-exit backup header, against the backup's own text |

## Storage

`%APPDATA%\Crimson Editor\bookmarks.xml`, one line per file, most recently used first:

```
<cedtbookmarks version="1" />
<file key="file:c:\work\a.c" mtime="1767225600" size="5120" marks="11:3f2a9c0e5b1d7a44,56:0c91..." />
<file key="remote:0:/srv/b.txt" mtime="0" size="0" marks="..." />
```

- `key` is the document key hot exit already uses: `file:<lower-cased path>` or
  `remote:<account>:<path>`. Untitled documents have no path, so they have no entry here.
- `mtime` and `size` are the file's status when the bookmarks were recorded.
- `marks` lists `<line>:<hash>` pairs: the 0-based line index and a 64-bit FNV-1a hash of the line's
  text, trimmed of leading and trailing white space, so re-indenting a line does not lose its
  bookmark. A hash rather than the text itself, so no source code is copied into the settings folder.
- A file whose bookmarks are all cleared loses its entry. Beyond 100 entries, the least recently
  used is dropped.
- The header and the attribute syntax are the ones the backup and session files use, so the same
  parser reads them.

## Finding a bookmark again

When a file is opened or reloaded and has an entry:

1. **Unchanged** — `mtime` and `size` match the file: every bookmark goes back on its line index.
2. **Changed** — for each bookmark, find the lines whose hash equals the recorded one and take the
   one nearest the recorded index (the earlier one on a tie). No such line: the bookmark is dropped.
   Two bookmarks landing on one line become one.

The line hashes are computed once per open, so the cost is one pass over the file plus one scan per
bookmark — negligible next to loading the file. An FTP document is always treated as changed: its
local cache file's time says nothing about the server's copy.

Blank and duplicated lines are where content cannot tell lines apart. "Nearest to where it was"
resolves them as well as anything can, and is right whenever the edit was elsewhere in the file.

## When it is written

- When a document closes, is saved, or is reloaded, and when the editor exits (which closes them).
  Not on every toggle: a bookmark set on an unmodified file and followed by a crash, before the
  file is closed, is lost.
- A document with no bookmarks writes nothing — unless it had an entry, which is then removed.
- A document whose lines do not match the file's status — modified, or an FTP document — is
  recorded with `mtime="0"`, so the next open always re-finds its bookmarks by content.
- Only a document that really loaded is recorded: one closing after a failed open (the file was
  missing for a moment) does not wipe its entry.
- Writes are whole-file and small (100 lines at most), to a temporary file renamed into place.

## Hot exit

A modified document's line numbers refer to its unsaved text, not to the file on disk, so for a
document with a hot-exit backup the bookmarks travel in the backup header instead (`bookmarks="11,56"`)
and are restored together with the text. Untitled documents get theirs back the same way. Once the
document is saved, its bookmarks move to `bookmarks.xml` like any other file's. Toggling a bookmark
on a modified document reschedules its backup, as an edit would, so a crash keeps it too.

## Where the code is

| File | What it holds |
| --- | --- |
| [src/core/cedtBookmarks.{h,cpp}](../src/core/cedtBookmarks.cpp) | Everything testable without a document: the line hash, re-finding by content, and `CBookmarkStore` (the file format, most-recently-used order, the 100-entry cap) |
| [src/doc/cedtDocBookmarks.cpp](../src/doc/cedtDocBookmarks.cpp) | The document side: collect and set bookmarks, `RestoreBookmarks` after an open, `RecordBookmarks` |
| `CCedtDoc::OnOpenDocument` / `OnReloadDocument` | Restore after an open; re-find by content across a reload |
| `SaveDocumentFile` / `SaveRemoteDocumentFile`, `OnCloseDocument` | Record — after a save only once the document carries its new name, which MFC sets after `OnSaveDocument` |
| Hot exit (`cedtBackup`, `cedtDocBackup.cpp`) | The `bookmarks` attribute in the backup header |

One trap, met in testing: MFC also sets a document's path only **after** `OnOpenDocument` returns,
so during the open `GetPathName()` is empty and the document looks untitled. The key is therefore
built from the path `OnOpenDocument` was given. The first version used `GetPathName()`, skipped every
file as untitled, and recorded nothing.

## Testing

- **Unit** ([tests/cedtBookmarks_test.cpp](../tests/cedtBookmarks_test.cpp), 24 tests, plus 2 for the
  backup header; written first): hash stability and whitespace trimming; re-finding with lines
  inserted above, deleted above, a bookmarked line edited (dropped), re-indented, duplicated and
  blank lines, ties, and result order; an unchanged file applied by index; the store's round trip
  with a Korean path and 64-bit values, most-recently-used order, the cap, and removal.
- **Manual** (all passed, Debug-KR): bookmark only, no edit, close and reopen, and quit and restart;
  lines added above in another editor while closed; a bookmarked line changed (that bookmark gone);
  only its indentation changed (kept); reload after an outside change; an unsaved document's
  bookmarks across a hot exit; `bookmarks.xml` gets an entry per file.
