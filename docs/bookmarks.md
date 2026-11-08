# Persistent bookmarks — a design

**Status:** design agreed, implementation not started (it will happen on its own branch).

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

- When a document closes, is saved, or is reloaded, and when the editor exits.
- A document with no bookmarks writes nothing — unless it had an entry, which is then removed.
- Writes are whole-file and small (100 lines at most), to a temporary file renamed into place.

## Hot exit

A modified document's line numbers refer to its unsaved text, not to the file on disk, so for a
document with a hot-exit backup the bookmarks travel in the backup header instead (`bookmarks="11,56"`)
and are restored together with the text. Untitled documents get theirs back the same way. Once the
document is saved, its bookmarks move to `bookmarks.xml` like any other file's.

## Where the work lands

| Area | Change |
| --- | --- |
| New `src/core/cedtBookmarks.{h,cpp}` | Everything testable without a document: the line hash, the file format, most-recently-used ordering and the 100-entry cap, and re-finding bookmarks by content |
| `CCedtDoc` | Collect and apply bookmarks; apply after open and reload; record on close, save, reload |
| Hot exit | `bookmarks` attribute in the backup header |
| `CCedtApp` | Load `bookmarks.xml` at startup, write it when it changes |

## Testing

- Unit, written first: hash stability and whitespace trimming; file round trip with Korean paths;
  most-recently-used order and the cap; re-finding with lines inserted above, deleted above, a
  bookmarked line edited (dropped), moved, duplicated and blank lines; an unchanged file applied by
  index.
- Manual: bookmark, close, reopen; bookmark, quit, restart; change the file in another editor while
  it is closed; reload after an outside change; a bookmarked unsaved document across a hot exit; an
  FTP document.
