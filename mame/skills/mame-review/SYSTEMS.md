# MAME review rules: inputs, ROMs and system metadata

Read when the change touches input port definitions (`INPUT_PORTS_START`, `PORT_*`), ROM definitions (`ROM_START`), or system definition lines (`GAME`, `SYST`, `CONS`, `COMP`) — most driver changes under `src/mame`, and devices with their own input ports. The legend, tiers and source notation are described at the top of [RULES.md](RULES.md); the calibration in [SKILL.md](SKILL.md) governs every rule.

## I. Inputs and metadata

### I-1 Input types · Tidy
Use the specific `IPT_*` type rather than `IPT_BUTTONn`/`IPT_OTHER` plus a `PORT_NAME`: coins start at `IPT_COIN1`; `IPT_SERVICE`, `IPT_MEMORY_RESET`, `IPT_TILT`, `IPT_START1`, `IPT_SLOT_STOP1`…, `IPT_POKER_HOLD1`…, `IPT_GAMBLE_PAYOUT` and the other `IPT_GAMBLE_*`, `IPT_AD_STICK_Z`; mahjong panels use `PORT_INCLUDE(mahjong_matrix_2p)`. `IPT_SERVICE` is the service-mode switch itself (what `PORT_SERVICE` wraps, on F2); any other service, test, refill or setup key or switch is `IPT_SERVICE1`…`IPT_SERVICE4`. Latching switches get `PORT_TOGGLE`. Standard controls carry no `PORT_CODE` and keep their default names, which would otherwise override the user's general assignments. A user-pressed key with no standard type is `IPT_OTHER` (or `IPT_KEYPAD`); `IPT_CUSTOM` is only for bits that code supplies. Adjacent unused bits merge into one `PORT_BIT`; shared port blocks use `PORT_INCLUDE`/`PORT_MODIFY`. A key or switch matrix read ANDs together all active rows ("It isn't a multiplexer").
- Grep: `IPT_COIN2` without `IPT_COIN1`; `IPT_SERVICE \).*PORT_NAME`; `IPT_(BUTTON\d+|OTHER|SERVICE|COIN\d)\b.*PORT_NAME\("[^"]*(Tilt|Start|Stop|Hold|Bet|Payout|Take|Double|Service)`; `IPT_(JOYSTICK|PADDLE|BUTTON|AD_STICK|DIAL|TRACKBALL)\w*.*PORT_CODE`; `IPT_CUSTOM.*PORT_(CODE|IMPULSE)`; row `switch` statements returning a single row's port
- Not when: slot devices, which keep the default assignments; an undocumented but normally wired button, which can be a plain `IPT_BUTTONn`.
- Source: a5889c83221, 7f3d4ebb232, 016e0fd5d8a, f32fee0e538, 272ed7c2767; #14280, #13004, #10862

### I-2 DIP switches and configuration settings · Tidy
`PORT_DIPLOCATION` goes on the `PORT_DIPNAME` line, with the settings tabulated beneath. `DEF_STR()` strings are used only in their exact meaning, other names are plain unabbreviated English, and gambling games use the standard terms ("Maximum Bet", "Main Game Payout Rate" with `%`). Settings are ordered off before on, least to most generous, smallest to biggest. A switch of unknown function is `DEF_STR( Unknown )`; `DEF_STR( Unused )` only once the code confirms it does nothing ("fixed on" is not unused). An on/off configuration setting has a descriptive name and `DEF_STR( No )`/`DEF_STR( Yes )`.
- Grep: `PORT_DIPLOCATION` on a line of its own; `"(Demo Sounds|Unused|Unknown|Free Play|Service Mode|Flip Screen)"` quoted instead of `DEF_STR`; `PORT_(CONF|DIP)SETTING\([^,]+,\s*"(?i:enabled|disabled|on|off|yes|no)"`; `dipswitch`
- Not when: wording taken verbatim from the operator's manual; duplicate settings; configuration inputs that don't expose every bit pattern.
- Source: 780490d9ac6, 24154bc1f00, 016e0fd5d8a; #14483, #14151, #13734, #13138

### I-3 Keyboards · Tidy
Keys are `IPT_KEYBOARD` with `PORT_CHAR`, the standard `KEYCODE_*` for the key's physical position, and real characters — `PORT_CHAR(U'ß')`, `PORT_NAME(u8"¥")` — not UTF-8 byte escapes or decimal codes. `PORT_NAME` is dropped where `PORT_CHAR` already names the key. In string and character literals, Latin, Greek and Cyrillic characters (up to U+052F) may be written directly; anything beyond is written as a `\uXXXX` escape with a trailing comment showing the glyph (`PORT_NAME(u8"Ⅱ") // Ⅱ`), which is what srcclean would turn it into.
- Grep: `PORT_NAME\("(\\x[0-9a-fA-F]{2})+`, `PORT_CHAR\(\d{3,}\)`, `\\u[0-9a-fA-F]{4}` with no `//` on the line; `IPT_OTHER.*PORT_CODE\(KEYCODE_` on a keyboard
- Source: 79cea2cbf54, cdbbacec859, 5f27f5e9c3f; #13342, #13340

### I-4 System metadata · Tidy
The macro fits the machine: `GAME` arcade, `CONS` console, `COMP` computer, `SYST` anything else (synthesisers, calculators, test equipment). The title is the product's real one, styled as the product styles it, with versions as printed ("VER.2.40A"); loanwords are written in their source language ("Battle", "Slot"), not romanised back from katakana. Disambiguating text is lowercase ("(set 2)", "(Japan, rev 1)"), and a region or version is added only when needed to tell sets apart. Romanised particles are lowercase ("no", "de"). Descriptions use characters with good Latin-font coverage — no dingbats. The manufacturer is the brand the product was sold under, spelled as the company spells it, or `"<unknown>"` — never `"Unknown"`. An unknown year has a `?` (`199?`). Metadata changes cite a source (box, PCB, title screen photo).
- Grep: `"Unknown"` as a manufacturer; `"Unknown ` at the start of a title; capitalised words inside `(…)` in titles; `CONS\(` for non-consoles; non-ASCII symbols in title strings
- Not when: stylised capitalisation the product itself uses.
- Source: a17c87d7e5c, ed77088c171, 05e8f264415, 0bdb4f0669a; #14280, #14136, #15706, #13342

### I-5 ROM definitions · Tidy
ROMs are named `"<label>.<pcb location>"` in lowercase — the label or mask code before the final dot, the board location after — not `"….bin"`. ROM sizes match the part: a truncated or odd-sized image needs a reason (a PIC program is whole words, so an odd byte count makes no sense), otherwise it's a bad dump. A region is sized to its contents, or carries `ROMREGION_ERASE00`/`ROMREGION_ERASEFF` when larger; mirroring is done in the map, not by `ROM_COPY` into the same region. Bad and missing dumps are `BAD_DUMP` and `NO_DUMP`. No made-up ROMs ("MAME is about preservation, making up ROMs is not"), no required-but-unused ROMs, no `ROM_OPTIONAL`. BIOS short names have no punctuation. Handcrafted NVRAM carries a comment saying where it came from. `ROM_START` bodies are indented one level.
- Grep: `ROM_LOAD\w*\(\s*"[^"]*\.bin"`; uppercase ROM names; `ROM_OPTIONAL`; `ROM_SYSTEM_BIOS\([^,]+,\s*"[^"]*[.\-]`; `ROM_COPY`
- Not when: `#define rom_x rom_y` for identical sets.
- Source: a5889c83221, 79994c3fee3; #14771, #14636, #14478, #14300
