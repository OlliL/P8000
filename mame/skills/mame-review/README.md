# mame-review skill v1.1 (Oct. 14, 2026)

This skill allows your agent to do an in-depth review of either a Github pull request or local files.  It's based on MAME's published
C++ coding standards plus years of coordinator Vas Crabb's PR reviews and "Tidy" changelists, so it should represent the actual state of
what Vas and other maintainers are looking for.

**IMPORTANT** mame-review is NOT an official MAMEdev project.  Passing mame-review does not mean your PR will be accepted, but it does give it better odds.

## How to install
Copy this entire mame-review folder into your agent's skills directory.  Or ask your agent how it wants this installed.

* Claude Code on Linux or macOS: `~/.claude/skills/`
* Claude Code on Windows: `C:\Users\<your username>\.claude\skills`
* Codex on Linux or macOS: `~/.codex/skills/`
* Codex on Windows: `C:\Users\<your username>\.codex\skills`
* Grok Code on Linux or macOS: `~/.grok/skills/`
* Grok Code on Windows: `C:\Users\<your username>\.grok\skills`

Replace `<your username>` with your Windows name.  If you're not sure what that is, just open C:\Users\ in Windows Explorer and look.

## How to use
In your agent `/mame-review filespec`, where `filespec` can be a local path, a Github mamedev/mame pull request URL, or a Github
mamedev/mame pull request number.

## License
BSD-3-Clause
