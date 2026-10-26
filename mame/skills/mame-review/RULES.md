# MAME review rules

The calibration in [SKILL.md](SKILL.md) governs every rule here: the file's existing style wins, only lines the change adds or modifies are in scope, and the listed accepted variations are never findings.

Each rule gives an ID, a default tier, the rule, **Grep** (a Perl-compatible regex for `grep -P` or `rg`, run over the lines the change adds; it finds candidates, never findings), **Not when** (exceptions, always including "the file consistently does otherwise" for layout and naming rules), and **Source**. A source is a section of the [C++ Coding Guidelines](https://docs.mamedev.org/contributing/cxx.html) (`cxx: <section>`) or [Contributing](https://docs.mamedev.org/contributing/) page (`contrib: <section>`); a `mamedev/mame` commit in which a core developer applied the rule to someone else's merged code (`https://github.com/mamedev/mame/commit/<hash>`); or a pull request where a core developer asked for it in review (`#<n>`: `https://github.com/mamedev/mame/pull/<n>`).

A Tidy rule becomes Blocking only when step 5 of SKILL.md traces a concrete failure from it.

This file holds the correctness, files, naming and device rules; [STYLE.md](STYLE.md) continues the C++ catalog. [SYSTEMS.md](SYSTEMS.md), [DATAFILES.md](DATAFILES.md) and [SUBMISSION.md](SUBMISSION.md) cover specific areas, as the table in step 2 of SKILL.md describes.

## C. Emulation correctness

### C-1 Reads have no side effects under the debugger · Blocking
A read handler that changes state — clears a status flag, acknowledges an interrupt, pops a FIFO, advances a pointer or latch, drives a line, adjusts a timer, logs — computes the value a real read would return, then applies the changes only under `if (!machine().side_effects_disabled())`. The debugger's memory views and disassembly read through the same handlers ("This makes it undebuggable"). `if (machine().side_effects_disabled()) return 0;` is the wrong fix: the debugger must still see the real value. This covers everything a read reaches, including `PORT_READ_LINE_MEMBER`/`PORT_CUSTOM_MEMBER` functions; a called function suppresses its own side effects rather than relying on its caller. A read whose only side effect is logging is Tidy.
- Grep: in `_r(`/`read(` handler bodies and read lambdas: `m_\w+\s*(=|\+\+|--|[|&^+-]=)`, `set_input_line`, `->\w+_w\(`, `->adjust\(`, `logerror|LOG`; `side_effects_disabled\(\)\)\s*return`
- Not when: the read changes nothing; write handlers (debugger writes are explicit user actions); a recursion guard.
- Source: a17c87d7e5c, fa2deb3e353, bb532d39a12; #15642, #12962, #12739

### C-2 All live state is saved, and only live state · Blocking
Everything the emulation changes after start is registered in `device_start`/`machine_start`, in the function that allocates it: `save_item(NAME(m_x))` for scalars and arrays, `save_item(STRUCT_MEMBER(m_arr, field))` for arrays of structs, `save_pointer(NAME(m_buf), count)` — count in elements — only for `std::unique_ptr<T []>` buffers. Timers, memory banks and memory shares save themselves; configuration isn't saved. Saved enums have an explicit underlying type and `ALLOW_SAVE_TYPE(cls::type)`. Saved members have explicitly sized types (`u8`, `s32`, `bool`, `enum : u8`), never `int`, `unsigned`, `long`, `size_t` or `line_state` ("it's unportable") — that part is Tidy (cxx: Variables and literals). Run-time state held as a pointer becomes an index. Resizable `std::vector`s, `std::deque`/`queue`/`map`/`list` and screen-sized bitmaps don't go in save states. Derived state (bank pointers, lookup caches) is rebuilt in `device_post_load()`, and that is all it does: it never re-drives an output callback or another device's input line from the restored state (`m_out_cb(BIT(m_sr, 9))` in `device_post_load()`), because the receiving device restores its own inputs from the same save state, and pushing the value again only hides missing save registration there ("This would just mask issues with other devices"). A commented-out `save_item` is a missing one. This applies whether or not the system claims `MACHINE_SUPPORTS_SAVE` ("You should be thinking about save state support from the beginning"). Incomplete saving is declared, not hidden: the device returns `flags::SAVE_UNSUPPORTED` from `static constexpr flags_type emulation_flags()`, and no system using it has `MACHINE_SUPPORTS_SAVE`. To check, list the class's data members and tick off each one registered in start.
- Grep: `//\s*save_item`, `_cb\(` or `set_input_line` inside `device_post_load()`, `typedef enum`, `std::(deque|queue|map|list)<`, `save_item\(NAME\(m_\w*bitmap`, `save_pointer` on fixed arrays, `\w+ \*m_\w+;` reassigned outside start
- Not when: configuration values, finder-derived pointers fixed after start, scratch values rebuilt on every call.
- Source: 146c0b9191a, ed77088c171, 7245878a94d, 14e2e3e762b, 272ed7c2767; #13624, #11819, #15002

### C-3 Start sets up, reset resets · Blocking
One-time setup lives in `device_start`/`machine_start`: `configure_entries`, handler installation, timer allocation, save registration, lookups, pushing initial output states. `device_reset`/`machine_reset` changes only what the hardware's reset changes: "Inputs do not magically change state on reset", so remembered input states stay; outputs that belong to other devices aren't driven; child devices reset themselves and are never `->reset()` from the parent. Initial input states are set in the constructor or `device_resolve_objects()`. An override in a derived class calls the base (`williams_state::machine_start();`), even when the base is currently empty; an empty override is omitted. Machine-configuration functions only configure: a live signal write (`m_fdc->dden_w(0)`, `set_input_line`) belongs in `machine_reset` or `machine_start`, because the target isn't started yet. Conversely, configuration setters run only at configuration time: a run-time change of screen timing goes through `screen().configure(…)`, never `set_raw`/`set_refresh_hz`/`set_vblank_time` from `device_reset` or a handler. Devices don't call `schedule_soft_reset()`/`schedule_hard_reset()`. Moving code between start and reset with no behavioural effect is Tidy.
- Grep: `configure_entr|install_(read|write|readwrite|ram|rom|device)|timer_alloc|save_item` inside `_reset\(\)`; `->reset\(\)` in `device_reset`; overrides lacking `::machine_start()`/`::machine_reset()`/`::device_post_load()`; `->\w+_w\(|set_input_line` in `(machine_config &config)` bodies; `schedule_(soft|hard)_reset`
- Not when: for the call-the-base clause only, the class derives directly from `driver_device` or `device_t`; a one-shot install at first reset guarded by a flag that is really set; deasserting outputs the device itself asserted.
- Source: 459b91e55fb, 387a494c637, f32fee0e538, 65b5718e0c0, 1ed09f8975d; #15697, #11330, #15625, #11114

### C-4 No mutable static or global state · Blocking
A function-local or file-scope `static` that isn't `const`/`constexpr` — including static `std::string`s, containers and pointers to devices or their data — is shared by every instance, never saved, and unsafe when MAME runs threads (`-validate`, UI caching). State is a member; constant data is `constexpr`. Headers declare no non-`const` variables.
- Grep: `^\s+static (?!const|constexpr)[\w:<>]+[ *&]+\w+`; `static std::(string|vector|map)`
- Source: fa2deb3e353, 32c259cc994, d7492dd3cd8; #13712, #14826, #16168

### C-5 Undefined behaviour and classic slips · Blocking
MAME builds as C++20 (`-std=c++20` in `scripts/genie.lua`): signed left shifts and out-of-range integer conversions are defined there, signed arithmetic overflow is not. Check every changed expression for:
- a member or local read before anything writes it — trace each member from the constructor's initialiser list through `device_start` and `device_reset`;
- an array index or pointer that emulated data can drive out of bounds (a register value used as an index without masking to the array size);
- a format string whose conversions don't match its arguments (`logerror("%s: %02x\n", value)`) — MAME's formatter asserts on these in debug builds;
- signed overflow in `+`, `-`, `*`, `/`, including `INT_MIN / -1` (which traps on x86), and division or modulo by a value that can be zero;
- a shift count that can be negative or at least the width of the promoted operand; `1 << 31` on `int`;
- `~` on a flag where `!` is meant (`m_enable = ~(data & 0x08)` is always true); `~0xff00` applied to a wider type;
- one buffer used as both `u8 *` and `u16 *` (endianness bug — use `get_u16le`, `util::little_endian_cast`);
- `memset`/`memcpy` offsets or lengths that can overrun the buffer;
- a write handler on a bus with partial writes that ignores `mem_mask`;
- a parameter or local that shadows a member (`emulation_mode = uint8_t(emulation_mode);` assigns to itself);
- comparisons that are always true or false (`unsigned >= 0`);
- a `-1` sentinel in an unsigned type, or a sign test on it, when the API defines its own invalid value (SDL3 joystick instance IDs are unsigned and 0 means failure, so 0 is the unset value);
- "cosmetic" rewrites of masks and constants — `0x80` becoming `0x8` has happened in a tidy commit.

Building a signed value by shifting the unsigned form first (`int16_t(uint16_t(x) << 8)`) states the intent; asking for it is Tidy.
- Grep: `= ~\(`, `1 << 31`, `\((u8|uint8_t) ?\*\)` on wider data, `reinterpret_cast<u(16|32)` on byte buffers, `mem_mask` parameters never read, `>= 0\b`, `sscanf\(.*== ?EOF`, `%s` in a `logerror` with one argument
- Source: fe4cd123b98, c43a83bbdcd, 00e80e88610, f4b165a6313, 272ed7c2767; #15660, #14846

### C-6 Portable, warning-free C++ · Blocking
Standard C++ only: no GNU `?:` with an empty middle operand, `case` ranges, or computed `goto` without a portable fallback. Warnings get fixed, not silenced (`#pragma warning(disable…)`, `-Wno-…` added to build scripts or CI); suppression for third-party code belongs in the Lua build scripts. No workarounds for broken compilers. `emu.h` is never included from a header or from `src/lib`/`src/osd`.
- Grep: `\?:`, `case .*\.\.\.`, `goto \*`, `#pragma warning`, `-Wno-`, `#include "emu.h"` in `.h`
- Source: #13317, #15663, #10870, #13560

### C-7 Bad media and bad configuration fail loudly · Blocking
An image-load callback validates the image's size and layout and returns an error (`return std::make_pair(image_error::INVALIDLENGTH, "Unsupported ROM size (…)");`) for anything it can't use; it never reports success on an unrecognised image. The code is specific: `INVALIDIMAGE` for malformed content (a bad Intel HEX record, a parse or checksum failure), `INVALIDLENGTH` for a wrong size, `UNSUPPORTED` for a valid image the device can't use, and `BADSOFTWARE` only for a software-list item whose metadata or parts are inconsistent — never for a loose file ("BADSOFTWARE is for bad software list items"). An unrecoverable configuration error throws `emu_fatalerror`; nothing follows a `fatalerror()` or `throw`. Unexpected behaviour during emulation is logged, not fatal.
- Grep: `rom_alloc` without a preceding size check; `image_error::BADSOFTWARE` in a loader that also takes loose files; `fatalerror\(.*\);\s*(return|break)`
- Source: a17c87d7e5c, ef3b2ab1aab, 0b4076983d8, 367b2d3e742

### C-8 Status flags tell the truth · Blocking
`MACHINE_NOT_WORKING` stays unless the system really works and was tested ("Not working means all bets are off"); a synthesiser with no sound output is not working. `MACHINE_SUPPORTS_SAVE` only when save states work (C-2). Each flag is used in its specific meaning: `MACHINE_NO_COCKTAIL` is only for broken cocktail (alternating-player flip) support; imperfect timing is not imperfect sound. Imperfect flags aren't stacked on a not-working set. A device with unemulated or imperfect parts declares `static constexpr feature_type unemulated_features()` / `imperfect_features()`. The flags agree with the commit message and PR description.
- Grep: removed `MACHINE_NOT_WORKING`; `MACHINE_SUPPORTS_SAVE` in a system using a `SAVE_UNSUPPORTED` device; `MACHINE_NO_COCKTAIL`
- Source: 9e463d2b89c, 79c2365979c, ded80654008, 14e2e3e762b; #15095, #14928

### C-9 Emulate the hardware; don't paper over problems · Blocking
Model the real mechanism — open bus and pull-ups, what a chip latches from a bus nobody is driving, edge versus level sensitivity, the order of events when a latch and a shift happen on the same edge, the actual wiring — rather than fixing a symptom. Logic the hardware doesn't have (a bit counter in front of a plain shift register) is a symptom fix. No fake signals, invasive HLE shortcuts, ROM patches presented as fixes, forced defaults, or checks of the running system's short name (`machine().system().name`). A change of a DIP or configuration default "doesn't fix anything". An unavoidable patch is grouped in an `#if 1` block, gated by a configuration setting, or done in a Lua script, and labelled as a hack.
- Grep: `rom\[0x[0-9a-f]+\]\s*=` outside a grouped patch block; `system\(\)\.name`; `strcmp\(.*name`; `HOLD_LINE`; PR text saying "workaround" or "fixes it"
- Not when: existing `#if 1` patch groups ("so hacks/patches are clearly grouped and can be toggled with a one-character change").
- Source: #13764, #15086, #15531, #15714, #16239

### C-10 Cross-CPU communication is synchronised · Blocking
Data passed between devices running in different scheduler domains (two CPUs, a CPU and an MCU) becomes visible through `machine().scheduler().synchronize(timer_expired_delegate(FUNC(cls::fn), this), param)`, with the state change inside the callback — calling `synchronize()` and then changing state immediately is a race. `generic_latch_8_device` already synchronises. Per-tick internal events come off the execute loop, not per-cycle `emu_timer`s.
- Grep: `synchronize\(\);` followed by assignments; `adjust\(attotime::zero\)` inside timer callbacks
- Source: #15369, #11872, #11598

### C-11 Behaviour matches the hardware documentation · Blocking
An emulation logic error the change introduces — a register, command, pin, flag or timing that doesn't do what the chip's datasheet, the system's schematics or service manual, or the reference the PR cites says — is Blocking. Examples: a command's parameter byte executed as a new command; an interrupt wired to the wrong pin for this chip variant; erased EEPROM reading 0x00 where the part reads 0xff; a register mapped to another chip's handler. Cite the reference and section in the finding. When no reference is available, say what the code does and why it looks wrong, and make it Tidy.
- Grep: none; this comes from reading each handler against its reference.
- Source: MAME's purpose ("emulating how systems actually work", #15531); #13764

## F. Files, build and placement

### F-1 License header · Blocking
Every source file begins `// license:<SPDX identifier>` then `// copyright-holders:<names>`, with names written as their owners write them (accented letters are fine) and no © or other symbols. Copyright holders are people with a substantial creative contribution, not everyone who edited the file. Layouts and netlists are CC0. Third-party code comes in only under a compatible license. MAME encourages BSD-3-Clause for new contributions; mention it as a Nit when a new file uses anything else.
- Grep: first two lines of each new file; `copyright-holders:` lines changed by small edits
- Source: cxx: Structural organization; MAME `README.md`; 7b513f954e6; #13680, #14988

### F-2 Include guards · Blocking
Headers under `src/devices` and `src/mame` use `#ifndef MAME_<PATH>_<FILE>_H`, where the path is relative to `src/devices` or `src/mame`, uppercased, with `/`, `.` and `-` turned into `_` (`src/devices/bus/a2bus/foo.h` → `MAME_BUS_A2BUS_FOO_H`; `src/mame/apple/mac.h` → `MAME_APPLE_MAC_H`). CI fails on a mismatch there, so a wrong guard is Blocking. Elsewhere the prefix follows the neighbouring headers (`MAME_UTIL_`, `MAME_FORMATS_`, `MAME_OSD_`, `MAME_FRONTEND_`); CI doesn't check those, which makes a stale one Tidy. `#pragma once` right after the `#define` and a closing `#endif // MAME_…_H` are Tidy. A moved header gets a new guard.
- Grep: `scripts/build/check_include_guards.py`; `#endif\s*$` as the last line of a header
- Source: cxx: Naming conventions; 116c0bf3843, 146c0b9191a, 82f9511ca48; #15810

### F-3 Includes · Tidy
Order: `emu.h`; the file's own header immediately after it; then groups separated by a blank line, each sorted byte-wise: local headers from the same directory; all `src/devices` headers (`bus/`, `cpu/`, `imagedev/`, `machine/`, `sound/`, `video/`) as one group; `src/emu` headers (`emupal.h`, `screen.h`, `softlist_dev.h`, `speaker.h`, `tilemap.h`); `src/lib/formats`; `src/lib/util` (`multibyte.h`, `path.h`); OSD; third-party (after MAME, before the standard library); C++ standard; C standard; OS-specific (in `<>`); layouts (`*.lh`); and `logmacro.h` last, after its `#define VERBOSE`. Include the standard header for every standard facility used (`<algorithm>`, `<cmath>`, `<memory>`, `<string_view>`, `<tuple>`). A header includes only what its own declarations need (`speaker.h`, format headers and helpers go in the .cpp), and nothing is re-included that the file's own header already provides.
- Grep: the line after `#include "emu.h"`; `formats/` or `screen.h` above a `cpu/`/`machine/` line; `std::(fill|copy_n|clamp|min|max)` without `<algorithm>`; `speaker.h` in a device header
- Source: cxx: Structural organization; 9626b93a411, a213644a830, 8c7f1a819db, b86767ac50e; #13596, #13712

### F-4 Anonymous namespace; nothing private at global scope · Tidy
A driver .cpp wraps everything except the `GAME`/`SYST`/`CONS`/`COMP` lines in `namespace {` … `} // anonymous namespace`, opened near the top: classes, maps, input ports, ROM definitions, `init_*` bodies, member definitions. A device class declared only in a .cpp goes inside the namespace too, with `DEFINE_DEVICE_TYPE_PRIVATE` after it closes. Tables, enums, structs and helpers that only the implementation uses live in the class (cxx) or in the .cpp's anonymous namespace — never as `static` data in a header, never as a non-static free function at global scope. Headers never contain `namespace {`, and never declare enums or constants at global scope (nest them in the class). `static` inside an anonymous namespace is redundant (a Nit).
- Grep: a .cpp containing `GAME\(|SYST\(|CONS\(|COMP\(|DEFINE_DEVICE_TYPE_PRIVATE` without `namespace {`; `^namespace \{`, `^static const`, `^(typedef )?enum\b` in `.h`
- Not when: a class declared in a header because other files use it.
- Source: cxx: Structural organization, Scoping; a17c87d7e5c, a213644a830, 14e2e3e762b, b86767ac50e; #13418, #13340

### F-5 New files are registered and placed · Blocking
A new device source has a block in `scripts/src/<cpu|sound|video|machine|bus|formats>.lua` headed by its `--@src/devices/<path>.h,<KIND>["<NAME>"] = true` marker, listing each existing file exactly once. A new system's short names go under its `@source:<dir>/<file>.cpp` section of `src/mame/mame.lst`; `mame.lst` carries no comments. A new slot card is added to its bus's card list (`*_cards.cpp`). Missing registration is Blocking. New entries go in sorted position next to related ones, never appended at the end (Tidy): a `scripts/src/*.lua` block sits in the alphabetical order of its `--@…` marker (`SSD1306` between `SNES_PPU` and `ST7626`). Placement (Tidy): `bus/` holds slot cards and bus interfaces, chips go in `machine/`, `sound/` or `video/`; a skeleton leaves `skeleton/` for its maker's directory once the maker is known; a single-system driver keeps its state class, video and machine code in one .cpp, with no separate header unless another translation unit needs it; a new `src/mame/<maker>` directory needs more than one driver's worth of reason.
- Grep: new files in `git status` or the PR file list, checked against `scripts/src/*.lua`, `src/mame/mame.lst` and `*_cards.cpp`
- Source: d01a9db4135, 8ac41b05194, 0f64064d47c, ae25c056895, 367b2d3e742; #15501, #15010, #12704

### F-6 Source format (srcclean) · Tidy
UTF-8 with LF line endings and no BOM; tab indentation with four-column stops; spaces, not tabs, for alignment after the start of a line (including after `//`); no trailing whitespace; exactly one newline at end of file; file mode 0644; printable ASCII outside comments and string or character literals (srcclean turns anything else into `?`, and escapes literal characters beyond U+052F). A whole-file diff usually means line endings changed. Report this once per file ("run srcclean"), not per line.
- Grep: `[ \t]+$`, `\S\t`, `\r$`, `\x{FEFF}`, a `mode 100755` in the diff summary
- Source: cxx: Source file format; ae78e07ff05, 029341aeb2b; #13340, #13966

### F-7 Generated sources · Blocking
A file headed "Generated … edits will be lost" or "do not edit" is never edited by hand: the change edits its generator or input and regenerates it. When a generator input changes (a CPU core's `*.lst`, `*make.py`, `*gen.py`), check whether its output is committed — today that is `m68k_in.lst` → `m68kops.cpp`/`m68kops.h` (`m68kmake.py`), `m68000.lst` → the `m68000-*`, `m68000mcu-*` and `m68008-*` sources and heads (`m68000gen.py`), and the `dsp563xx` tables (`dsp563xx-make.py`). Committed output must be regenerated in the same change. Other inputs (`h8.lst`, `mcs96ops.lst`, the 6502 family) are generated at build time by rules in `scripts/src/cpu.lua`. The C++ rules apply to the C++ fragments inside an input file, not to its macro language.
- Grep: `Generated|do not edit|edits will be lost` in the first lines of touched files; `grep -rl` for the input's name among sibling files
- Source: `src/devices/cpu/m68000/m68kops.cpp` header; `scripts/src/cpu.lua`

## N. Naming

### N-1 Casing · Tidy
Functions, variables, classes and members are snake_case; constants, enumerators, macros and device types SCREAMING_SNAKE_CASE (only for real constants); template parameters LlamaCase (`template <unsigned Which>`); abstract classes end in `_base`, driver classes in `_state`, devices in `_device`, interfaces are `device_…_interface`. The `m_` prefix (optional, used consistently) marks instance members only — never locals, statics or constants.
- Grep: `template\s*<\s*(typename|class|int|unsigned|bool|size_t|u\d+)\s+[a-z_]`; `^\s+[\w:<>]+\s+m_\w+\s*=` inside function bodies; camelCase identifiers in added lines
- Not when: names mirroring a datasheet's register names or a generator's conventions; names the change doesn't introduce; established `_r`/`_w` handler suffixes.
- Source: cxx: Naming conventions; 8c7f1a819db, 9359d54aeee; #11468, #13421

### N-2 Reserved identifiers · Tidy
No `__` anywhere in an identifier, no leading `_` followed by an uppercase letter, no leading `_` at global scope (including include guards and macro parameters), and no new `_t`-suffixed names in the global namespace.
- Grep: `\b_[A-Z]\w*|\w*__\w*`
- Source: cxx: Naming conventions; 116c0bf3843

### N-3 Global names are specific · Tidy
Device types, device classes and device short names share one global namespace, so they name the maker or bus and follow the prefix the rest of that family uses: `VOTRAX_SC01`, `votrax_sc01_device`, `zxbus_nemoide`. A generic name (`base_state`) is only for anonymous namespaces. Device short names are lowercase and don't end in `_device`; device long names are descriptive, capitalising only proper nouns and part numbers ("10-bit signed ADC", "sprite generator"), never a bare "Printer Interface". Renaming existing names for their own sake is churn — and a device short-name change forces ROM renames.
- Grep: `DEFINE_DEVICE_TYPE\([^,]+,[^,]+,\s*"[^"]*_device"`; a `DECLARE_DEVICE_TYPE` losing its maker prefix; `class \w*base_state` outside `namespace {`
- Source: 6e2d6cd2c56, 7f3d4ebb232, 0bdb4f0669a, 550751c0e4f; #13421, #13665, #15331

### N-4 System short names · Tidy
A parent's short name is specific and at most 8 characters where reasonable (the validity limit is 16), leaving room for clones, whose names are the parent's plus a mnemonic suffix (`ccorsario` → `ccorsarioa`). The unsuffixed name is the latest version, and the parent is the most widespread variant. Existing short names aren't renamed unless they're genuinely confusing, ideally before the first release that includes them.
- Grep: `ROM_START\(\s*\w{9,}\s*\)` on new sets; clone short names not prefixed by their parent's; renamed `ROM_START(` names
- Source: 3866c082a09, 8386284db51, 05e8f264415; #14605, #16203, #15063

## D. Devices and drivers

### D-1 Finders, not tag lookups · Tidy
Resources come from object finders: `required_device`, `required_ioport` / `required_ioport_array<N>`, `required_region_ptr<T>`, `required_memory_bank`, `required_shared_ptr`, `memory_share_creator`, `output_finder<…>` (two-dimensional for lamp and LED matrices). Handlers never call `ioport("X")`, `membank("X")`, `memregion("X")` or `subdevice<T>(…)` ("On-the-fly tag lookups are error-prone and hurt performance"), and never switch on tag strings. Pass the finder, not a tag string, to device creation (`I8256(config, m_uart, …)`), callbacks (`.set(m_fdc, FUNC(…))`), configuration setters (`set_screen(m_screen)`) and address maps (`.share(m_vram)`, `.rw(m_uart, FUNC(…))`). A `required_*` finder is never null-checked. `optional_*` is for genuinely optional hardware: variants get derived classes and their own configurations, not optional finders "because of reuse" or `config.device_remove()`. Tags name hardware (`"muart"`), not members (`"m_uart"`). Sub-device ROM regions use relative tags (`"nichisnd:audiorom"`, not `":nichisnd:audiorom"`).
- Grep: `\b(ioport|membank|memregion|memshare)\("`, `subdevice<`, `\.set\("`, `\.share\("`, `device_remove`, `ROM_REGION\w*\([^,]+,\s*":`, `\(\*this, "m_`, `if \(m_\w+\)` on a `required_` member
- Not when: one-time lookups in start or init; `subdevice<>()` inside machine configuration, where finders aren't resolved yet; a share with no finder that another device looks up by tag; a finder that would only be used at configuration time; a tiny block where a memory share is overkill.
- Source: a5889c83221, 9359d54aeee, c43a83bbdcd, fcb4f01dfb4, 14e2e3e762b, c2330affb53; #12659, #10958, #10890

### D-2 `ATTR_COLD` on configuration-time members · Tidy
Declarations of machine-configuration members (`void foo(machine_config &config)`), address maps (`void map(address_map &map)`), `init_*()`, lifecycle overrides (`device_start`, `device_reset`, `device_add_mconfig`, `device_post_load`, `device_rom_region`, `device_input_ports`, `machine_start`, `machine_reset`, `video_start`), palette init, `memory_space_config()` and start-only helpers end with `ATTR_COLD`. This is a norm since late 2024, so old code lacks it.
- Grep: `\((machine_config|address_map) &\w*\)( const)?;`, `void init_\w+\(\);`, `_(start|reset|post_load|add_mconfig)\([^)]*\) override;`, `memory_space_config\(\) const override;`
- Not when: anything that runs during emulation — handlers, timer and input callbacks, `screen_update`, `sound_stream_update`; declarations the change doesn't add or touch.
- Source: 0b4076983d8, 78df9e810bb, a17c87d7e5c, dfa3b923398; #13350, #15501

### D-3 Class layout · Tidy
One section per access level where practical, `protected` before `private`, no empty sections, and each member at the least access it needs: configuration entry points named by `GAME`/`SYST` lines, `init_*`, live-signal inputs and device maps installed by a host are public; overridden virtuals keep the base's access (usually protected); shared `*_common` configuration helpers and constructors of classes used only as bases are protected; internal handlers, internal maps, timer callbacks and data are private. Inside a section, constants and types come first, then data members and member functions as two separate blocks, in either order — never interleaved — with configuration functions before live-signal handlers. Among the data members, object finders (`required_device`, `required_shared_ptr`, `required_ioport`, `output_finder`) sit together, and plain run-time state follows them rather than landing between two finders. `unemulated_features()`/`emulation_flags()` go at the very top, above the constructor. Override groups are labelled `// device_t implementation`, `// device_<x>_interface implementation` — never the copied "device-level overrides".
- Grep: `(device|machine)_(start|reset)\(\) override` or `_map\(address_map &` under `public:`; `// (device|interface)-level overrides`; a data member between two function declarations, or a function declaration between two data members, in the same section
- Source: cxx: Structural organization; bb532d39a12, 9e463d2b89c, 14e2e3e762b, c2330affb53, 272ed7c2767, 367b2d3e742, d4024db0e8e; #16084, #15697, #12648

### D-4 Virtuals · Tidy
Overrides are written `virtual … override` (MAME convention, even though `override` implies `virtual`). Virtual function bodies and virtual destructors are defined in the .cpp, not the header, and a virtual destructor is never `= default` in a header (it makes vtable link errors hard to diagnose). A function nothing overrides isn't virtual.
- Grep: `^\s+(?!virtual)[\w:<>&* ]+\) (const )?override`; `virtual ~\w+\(\)\s*(\{\s*\}|= default)` in `.h`
- Source: d01a9db4135; #14151, #11918, #11835

### D-5 Constructors · Tidy
Members are initialised in the constructor's initialiser list in declaration order, not assigned in its body. A class whose constructor is defined out of line (in the .cpp) has no in-class member initialisers — "too much chance of contradictory initialisation". Configuration members (offsets, bank selection, delay flags) take their defaults in that list; run-time state that `device_start`/`device_reset` sets (C-3) has no initialiser anywhere else. A nested plain struct with no constructor instead carries default member initialisers on its own fields (`uint16_t mcr = 0;`), and the member or array holding it has no trailing `{}`. One comma style per file, indented one level: `) :` then trailing commas, or leading `:` and `,` — never `: base(…),`. Overloaded constructors delegate. A derived class that differs by a value passes it to a protected base constructor. Clocks come from machine configuration: a device that needs a clock has no `clock = 0` default and doesn't pass a literal to its base; a clock-less device does default to 0.
- Grep: `^\s+: [\w:]+\(.*\),\s*$`; constructor bodies made of `m_\w+ = `; `m_\w+\s*(=|\{)[^;]*;` in a `.h` class whose constructor is in the `.cpp`; `uint32_t clock = 0\)` on clocked devices
- Not when: in-class initialisers in a class whose constructor is defined in the class body (typical of driver state classes); a file that already uses the `: base(…),` form throughout.
- Source: cxx: Structural organization; 7f3d4ebb232, a2d27222f86, 9359d54aeee, fe4cd123b98, 272ed7c2767, 367b2d3e742, d4024db0e8e; #15542, #11915, #13591

### D-6 Memory, banking and maps · Tidy
Fixed RAM a CPU sees is `.ram()` in the map or a `memory_share_creator<T>(*this, "tag", bytes, ENDIANNESS_…)` sized to the real part (a device's small private buffers are plain member arrays) — not `make_unique` plus `save_pointer`, a vector, or a `ram_device` (which is for user-selectable sizes). Mappings the software switches at run time use `memory_view` (`select()`/`disable()`) or a configured `memory_bank` (`configure_entries` + `set_entry`), never `address_map_bank_device` or `set_base`; mappings fixed by configuration are installed once in `machine_start`. ROM is never mapped with `bankrw`. Register blocks are installed as device maps with `map.m(…)`; ignored writes are unmapped (`.nopw()`), not stored in dummy buffers. In each map entry the addressing — range, `.mirror()`, `.umask*()` written full width (`0x0000'ffff`), `.cswidth()` — comes before the handlers. Map straight to a device's handler instead of a one-line trampoline. Handlers have the narrowest signature (unused `offset`/`mem_mask` dropped, `offs_t` offsets), and a wide handler whose whole body sits under one `if (ACCESSING_BITS_0_7)` (or `_8_15`) becomes an 8-bit handler (`void eeprom_w(u8 data)`) mapped with `.umask16(0x00ff)` (or `0xff00`) — the byte lane is the map's business; unrelated registers get separate entries instead of one `switch (offset)`; addresses are composed with `+`, not `|`; `FUNC()` names the class that owns the handler; read lambdas declare their return type (`-> u16`). Hot-path access to another address space caches `memory_access<…>::specific` (or `cache`) in start.
- Grep: `make_unique(_clear)?<.*\[\]>` near `save_pointer`; `if \(ACCESSING_BITS_(0_7|8_15|16_23|24_31)\)` as a handler's only test; `address_map_bank_device|bankdev\.h|->set_base\(`; `bankrw`; `\.(r|w|rw|lr\d+|lw\d+)\(.*\)\.(umask|mirror)`; `switch \(offset\)`; `space\(AS_\w+\)\.(read|write)_`; `\((u32|uint32_t) offset[,)]`
- Not when: a full-width `umask` (a no-op, don't add one); a handler that also latches the whole word (`COMBINE_DATA`) or treats the lanes differently, which keeps `mem_mask` and `ACCESSING_BITS_*`; complex stride or width mappings a view can't express; address maps that need a `.m` trampoline map because of finder limitations; an unused `offset` in a handler that uses `mem_mask` — the handler forms are `(offset, data, mem_mask)`, `(offset, data)` and `(data)` for writes and `(offset, mem_mask)`, `(offset)` and `()` for reads, so `mem_mask` can't be kept without `offset` (`src/emu/emumem.h`).
- Source: 116c0bf3843, f4b165a6313, 65b5718e0c0, bf90f2bd7c0, 3f75ea4e1c3, d4024db0e8e; #13202, #13208, #13340, #16135, #15407

### D-7 Signals and callbacks · Tidy
Use devcb's features instead of trampolines: `.set_inputline(cpu, LINE)`, `.set_constant(v)` for a tied level, `.set_nop()` rather than an empty lambda, `.set_ioport(…)`, `.bit(n)`, `devcb_write_line::array<N>` instead of numbered `m_foo1_cb`, `m_foo2_cb`. Leave an optional callback the device checks with `isunset()` unconfigured. Two outputs never drive one input directly: combine them with `input_merger` or track each source. A device deasserts whatever it asserts. Write-line handlers take `int state`. A 0/1 value goes straight to a line (`->clk_write(BIT(data, 5))`), and `ASSERT_LINE`/`CLEAR_LINE` are only for `set_input_line`. An output callback fires when the output changes, not on every update. An interrupting latch uses `data_pending_callback().set_inputline(…)`. A write handler drives device lines directly (`m_eeprom->di_write(BIT(data, 0)); m_eeprom->clk_write(BIT(data, 1)); m_eeprom->cs_write(BIT(data, 2));`): an `IPT_OUTPUT` port of `PORT_WRITE_LINE_DEVICE_MEMBER` bits that a handler writes with `ioport("EEPROMOUT")->write(data, 0xff)` is a tag lookup and a detour through the input system for a plain fan-out ("Don't do unnecessary indirection through I/O port").
- Grep: `\? ASSERT_LINE : CLEAR_LINE`; `IPT_OUTPUT`; `ioport\("\w+"\)->write\(`; `\.set\(\[\]\([^)]*\)\s*\{\s*\}\)`; `m_\w+[0-9]_cb\b`; line handlers taking `bool`/`u8 state`; unconditional `m_\w+_cb\(` at the end of update functions
- Source: 7808b3a7862, 2cabaf1541c, fcb4f01dfb4, 38fd821b643, d4024db0e8e; #14319, #13560, #10926, #14981

### D-8 Reuse, don't reimplement · Tidy
Use what MAME already has: the hopper (`ticket_dispenser_device`, `machine/ticket.h`) for gambling payouts, `generic_latch_8_device`, `input_merger`, `util::sext`, `bitswap`, `get_u16le`/`put_u32be`, `util::little_endian_cast`/`big_endian_cast`, `clocks_to_attotime`, `std::ranges` algorithms. Handlers, tile callbacks or helpers that differ only by an index become one `template <unsigned N>` member bound as `FUNC(cls::fn<N>)`, and a `switch` that picks among numbered tags becomes an indexed finder array. Copy-pasted functions are merged.
- Grep: local hopper motor timers; hand-rolled latch/ack flags; runs of names differing in one digit (`_p1_r`/`_p2_r`, `_0_w` … `_7_w`); `case N:` blocks differing by a constant
- Not when: variants with different logic; deriving from a device just to reuse part of it.
- Source: 511b9de919b, 5f27f5e9c3f, 2cabaf1541c, 0bdb4f0669a; #16084, #15450, #14754

### D-9 Clocks · Tidy
Crystals and resonators use XTAL literals (`18_MHz_XTAL / 6`, `3.579545_MHz_XTAL`); the value must be in the known-crystal list in `src/emu/xtal.cpp`, or validation fails (Blocking). A clock a PLL or divider derives from a crystal is written as that crystal and the ratio (`3.579545_MHz_XTAL * 3 // chroma clock multiplied with a PLL`), not as the product looked up as a crystal of its own (`10.738635_MHz_XTAL`) when no such crystal is on the board. RC/LC and unverified clocks are plain integers with digit separators (`1'500'000`). Units are spelled "MHz"/"kHz".
- Grep: `XTAL\(\d`; `\b\d{5,}\b` clock arguments; `Mhz|KHz`
- Source: 7808b3a7862, 387a494c637, ed77088c171, 272ed7c2767; #14003, #13942

### D-10 Devices are general and assume nothing about the host · Blocking
A device or slot card never uses absolute or parent tags (`":maincpu"`, `"^ram"`), `machine().root_device()`, or a downcast of `owner()` to a driver class; it gets what it needs through finders (defaulting to `finder_base::DUMMY_TAG`), a slot-provided address space, or the slot interface ("A slot device making an assumption about the host system is a regression… a blocker for a release"). `src/devices` never includes headers from `src/mame`. Variant behaviour lives in derived classes or configuration, not type flags checked throughout the device. A widely shared device (SCSI/HDD, CPU cores, CRTCs) doesn't get a hack for one system. Optional hardware is a slot card, or a clone when it's a fixed modification.
- Grep: `"[:^]` tag literals under `src/devices`; `root_device\(\)`; `downcast<\w+_state`; `#include "(\.\./)*mame/` or a maker path in `src/devices`; `m_type ==|switch \(m_variant\)`
- Source: #15512, #12239, #11918, #15531, #14029, #15399

### D-11 Driver structure · Tidy
Variant-specific members, configurations and maps live in derived state classes. `init_*` is only for decrypting or unscrambling ROMs and similar things that can't be done another way — not for setting flags or installing handlers. The legacy `MCFG_MACHINE_START_OVERRIDE`/`MCFG_VIDEO_START_OVERRIDE` machinery and `DECLARE_MACHINE_START`/`DECLARE_VIDEO_START` hooks still exist in `src/emu/driver.h` but aren't used in new code: override `machine_start()`, `machine_reset()` and `video_start()` directly. Device output routing sits next to the device's instantiation.
- Grep: `init_\w+\(\)\s*\{[^}]*(install_|m_\w+ = (true|false|\d))`; `MCFG_|DECLARE_(MACHINE|VIDEO)_(START|RESET)`
- Not when: address-line unscrambling in `machine_start`.
- Source: fcb4f01dfb4, a53816282c3; #12044, #10958, #14029, #11670

### D-12 Video · Tidy
Screens are configured with `set_raw(clock, htotal, hbend, hbstart, vtotal, vbend, vbstart)` from the master clock when the timings are known, not `set_refresh_hz`/`set_vblank_time`. `screen_update` draws only inside `cliprect`, uses it only for clipping (never as the resolution), and changes no state or outputs. An indexed bitmap (`bitmap_ind16`) takes pens (`black_pen()`, palette indices), never `rgb_t` values. Run-time timing changes go through `screen().configure()` (C-3). Video state changes mid-frame call `screen().update_partial()` first. `rectangle` bounds are inclusive and are read through the accessors `left()`, `right()`, `top()`, `bottom()` — not the `min_x`/`max_x`/`min_y`/`max_y` fields — in loops over `cliprect` or `screen.visible_area()` and when building a clip (`rectangle clip(visarea.left(), 255, 192, visarea.bottom())`). Row pointers come from `&bitmap.pix(y)`, not `base + y * width`, and a per-pixel loop takes each row's pointer once (`u16 *const dst = &bitmap.pix(y);`, then `dst[x]`) instead of calling `pix(y, x)` for every pixel ("Don't do bitmap line offset calculation for every pixel"). Work bitmaps are members sized with `allocate()`/`resize()`, not `std::unique_ptr<bitmap_…>`. A tilemap write handler marks the one tile dirty, not the whole map.
- Grep: `set_refresh_hz|set_vblank_time`; `m_\w+\s*=` or output writes inside `screen_update`; `cliprect\.(width|height)\(\)`; `rgb_t` in a function taking `bitmap_ind16`; `unique_ptr<bitmap_`; `\.(min|max)_[xy]\b`; `\.pix\(\w+, \w+\)` inside an inner x loop; `mark_all_dirty` in write handlers
- Source: a5889c83221, 550751c0e4f, c43a83bbdcd, d4024db0e8e, 272ed7c2767, 367b2d3e742; #12044, #11819, #14193

### D-13 CPU cores and disassemblers · Tidy
Integrated peripherals are driven from the core's execute loop and facilities, so their events stay in the core's scheduler domain. `execute_min_cycles()`/`execute_max_cycles()` match how instructions are split. Per-model differences are constructor parameters. A disassembler takes its configuration at construction, decodes by bit pattern rather than one `case` per opcode, and never depends on the CPU core's files (`unidasm` builds it alone). Address spaces that debuggers or scripts need are exposed even if the core never uses them internally. In UML generators: `const uml::code_label`, `UML_TEST` for zero checks, `UML_BFXU` for field extraction, immediates passed directly rather than moved into a scratch register.
- Grep: CPU-device headers included from `*dasm*.cpp`; hundreds of `case 0x` labels in a disassembler; `UML_CMP\(.*, 0\)` before a conditional jump
- Source: b2d602267fd, 8c7f1a819db; #11105, #15082, #15592, #16080, #11878
