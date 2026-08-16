# Agentic-coding companion features — a design exploration

**Status:** exploration, not started. This records where Crimson Editor could fit in the era of
terminal AI coding agents (Claude Code, aider, Codex-style CLIs), and which features would pay off
without betraying what the editor is.

## The angle: a fast native companion, not an AI IDE

The obvious move — bolt a chat panel and an inline-completion engine onto the editor — is the wrong
one. That space (VS Code + Copilot, Cursor, and the rest) is crowded, heavy, and built on exactly
the runtime weight Crimson Editor exists to avoid. Competing there means giving up the only thing
that makes Crimson worth choosing: it starts instantly, uses almost no memory, is a single native
executable, and now handles large files well.

The open, unclaimed niche is the **companion window**. When you run a coding agent in a terminal, it
edits files on disk, hands you diffs, and produces generated output and logs. The job of a good
companion is to reflect those changes instantly, let you review and undo them, and let you fire a
file or a selection at the agent — all natively, next to the terminal, without becoming the terminal.
That plays *to* Crimson's strengths instead of against them.

Two principles keep the features honest:

1. **Delegate the model, don't embed it.** The editor should shell out to whatever CLI or script
   the user configures (`claude`, `aider`, a wrapper), not link an LLM client, ship API keys, or
   pin a provider. That keeps the binary small and sidesteps the security, auth, and maintenance
   weight of an in-app model client.
2. **Build on what's already there.** User-defined tools with captured output, and external-change
   detection, already exist. The best features extend those rather than add new subsystems.

## Idea 1 — User Tools → AI CLI bridge, with replace-in-place

Crimson already lets you wire an external command to a menu/shortcut and capture its output into the
docked console ([src/view/cedtViewCommand.cpp](../src/view/cedtViewCommand.cpp)). That is most of an
agent bridge already: a tool that runs `claude -p "…"` with the current file or selection on stdin,
and shows the reply in the console, works today.

The missing piece that would make it feel native is an output mode that **replaces the selection in
place** instead of only capturing to the console. With that, prompts like *"refactor this function",
"add doc comments", "explain this log line"* land directly in the buffer, as an undoable edit.

- **Effort:** low — a new "capture mode" (console / replace selection / insert at caret) on the
  existing user-tool definition, plus piping the selection to the child's stdin.
- **Value:** high — turns the whole existing tools mechanism into an agent-transform surface.
- **Fit:** excellent — no model in the app, no new UI subsystem.

Ship a couple of example tool presets (send-selection-to-CLI, replace-with-CLI-output) so the pattern
is discoverable rather than something each user has to invent.

## Idea 2 — Agent-companion mode: seamless reload + local diff

When an agent edits a file you have open, Crimson already notices and offers to reload
(external-change detection / `IsModifiedOutside`, the reload prompt exercised in the §C sweep). Two
extensions turn that from a nuisance prompt into a live view of the agent at work:

- **Opt-in auto-reload** with a clear indicator, so an agent's edits appear as they happen instead
  of behind a dialog per file.
- **A local diff view** — "what changed since I last saw this file", against the last-loaded content
  or against `git HEAD`. Native and fast, this is where Crimson's quick rendering of large / generated
  files is an advantage, not a footnote.

- **Effort:** medium — auto-reload is a small change on top of the existing detection; the diff view
  is the larger part (a diff algorithm and a two-column or inline presentation).
- **Value:** high — this is the core "watch the agent, review, undo" loop.
- **Fit:** strong — it is the companion-window thesis, made concrete.

## Idea 3 — Apply a unified diff / patch

Agents frequently hand back a patch rather than editing in place. A **paste-a-diff → preview →
apply** command covers that flow with no file-watching and no model in the loop: paste a unified
diff, see what it touches, apply it to the buffer or the tree.

- **Effort:** medium (a unified-diff parser and a preview/apply step).
- **Value:** solid — complements Idea 2 for agents that propose rather than write.
- **Fit:** good — pure native text manipulation, Crimson's home ground.

## Idea 4 — In-editor AI chat panel / MCP client (deferred)

A streaming chat panel or an MCP client inside the editor is the feature people *picture*, but it is
the wrong first step: a real streaming-chat UI plus networking in MFC is a large build, it pulls the
model back into the app (against principle 1), and it adds exactly the weight the companion thesis
avoids. Worth revisiting only after the lighter, higher-fit features prove the direction.

## Recommendation

Start with **Idea 1 (AI CLI bridge + replace-in-place)** and **Idea 2 (auto-reload + local diff)**.
Both extend mechanisms that already exist, keep the model out of the binary, preserve the fast native
feel, and together stake out the clear niche: *a terminal coding agent, plus a fast native window to
watch, review, and steer it.* Idea 3 slots in alongside; Idea 4 waits.
