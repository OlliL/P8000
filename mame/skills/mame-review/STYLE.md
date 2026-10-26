# MAME review rules: types, logging and layout

The second half of the C++ rule catalog, read together with RULES.md for every C++ change. The legend, tiers and source notation are described at the top of [RULES.md](RULES.md); the calibration in [SKILL.md](SKILL.md) governs every rule.

## T. Types, expressions and constants

### T-1 Literals · Tidy
Hex digits and the `0x` prefix are lowercase, in code, address maps and comments. Long literals use digit separators (`1'751'938`, `0xfff8'1fff`). 64-bit literals carry a suffix, and long suffixes are uppercase (`1LL`, `1ULL`, never `1ll`). No octal. A literal doesn't exceed the width of what stores it.
- Grep: `0[xX][0-9a-fA-F]*[A-F]`, `\b0X`, `\d(u?ll|u?l)\b`, `\b\d{7,}\b`
- Not when: `%02X`-style format specifiers; disassembler output matching the manufacturer's syntax; text quoted from a datasheet; bare addresses in memory-layout tables.
- Source: cxx: Variables and literals; 0bdb4f0669a, 1887dfd233f, fa2deb3e353; #15193, #13038

### T-2 Casts and conversions · Tidy
Simple-type conversions use functional casts (`u32(x)`, `bool(x)`, `double(a)`), not C-style casts or `static_cast`; named casts are for pointers. `std::min<int>(a, b)` replaces casting one argument. A `bool` doesn't take part in arithmetic implicitly (`flag ? 0x20 : 0x00`, not `flag << 5`), and a `char` or small value is cast before shifting or promotion.
- Grep: `\((u?int(8|16|32|64)_t|[us](8|16|32|64)|int|unsigned|char|float|double)\s*\*?\)\s*[\w(]`; `static_cast<(u?int\d+_t|[us]\d+|bool|unsigned|int)>`
- Not when: casts that document intent; arguments to logging calls, which aren't cast.
- Source: 1887dfd233f, 2cabaf1541c, 9359d54aeee; #14151, #15051, #12948, #14911

### T-3 Declarations · Tidy
Variables are declared at first use, initialised, as narrowly scoped as possible, and `const` when never reassigned (pointers too: `u8 const *const`); loop counters are declared in the `for`. An object is constructed with its values (`rectangle clip(l, r, t, b)`), not default-constructed and then `set()`. A local that only aliases a member (`uint8_t *spriteram = m_spriteram;`) goes; the member is used directly. `const` on by-value parameters and return types does nothing and goes. Member functions that use no instance state are `static`. Two-state values are `bool` with `true`/`false`. `auto` only where it avoids repeating the type (device creation, iterators, lambdas), never where it deduces `int` from a literal. Type aliases use `using`, not `typedef`. C-isms go: `f()` not `f(void)`, no elaborated `struct X &` parameters, empty bodies are `{ }` or `= default`, and there is no `;` after a function body or `DEFINE_DEVICE_TYPE(…)`.
- Grep: runs of bare declarations at the top of a function; `\(const (u?int\d+_t|[us]\d+|int|bool) \w+[,)]`; `^\s*const \w+ \w+\(`; `auto \w+ = \d`; `\btypedef\b`; `\(void\)`; `override\s*\{\s*\}\s*;`
- Not when: locals declared early on purpose because they're reused.
- Source: cxx: Scoping, Const Correctness; 1887dfd233f, 2cabaf1541c, 164dbc5d025, d2d85d39519, 367b2d3e742; #12711, #11053, #10933

### T-4 Constants and macros · Tidy
"Please don't use preprocessor macros where a `constexpr` will work." Constants are `constexpr`, never `const` or `static const`, whenever the initialiser is a constant expression (`constexpr uint8_t hw_sprite[16] = {…}` inside a function, `constexpr uint16_t width = TERMINAL_WIDTH * 8;`) — local when one function uses them, otherwise in the class (`static constexpr` with a sized type) or the .cpp's anonymous namespace — and new constant arrays are SCREAMING_CASE. Enums replace groups of `#define`d values. Function-like macros, and macros that touch members, become `constexpr` or inline member functions, or a lambda capturing `this` local to the one function that uses them (`auto const dummy_byte_check = [this] (unsigned pos, uint8_t expected) { … };`); a macro that must stay a statement is wrapped in `do { … } while (false)`. Compile-time switches are `if (CONSTANT)`, not `#if`.
- Grep: `^\s*(static )?const \w+ \w+\[.*\]\s*=`; `^\s*const \w+ \w+ = ` with a constant initialiser; `^#define \w+\s+[\w(]`; `#define \w+\(`; `\bm_\w+` inside a `#define` or its continuation lines
- Not when: `LOG_*` channel masks, `VERBOSE` and include guards; existing constants the change doesn't touch.
- Source: cxx: Const Correctness, Structural organization; 3e87ac03a86, cd971009fd4, c43a83bbdcd, 272ed7c2767, 367b2d3e742; #14491, #13328, #13560

### T-5 MAME helpers · Tidy
`BIT(x, n)`, `BIT(x, n, width)` and `BIT(~x, n)` replace shift-and-mask — either shift and mask plainly or use `BIT`, never a hybrid like `(x & 0x08) >> 3`. A one-bit test with a computed mask (`x & (1 << n)`) is `BIT(x, n)`. The complement goes inside: `BIT(~x, n, w)`, never `~BIT(x, n, w)`, which carries set bits above the field. `bitswap<N>(x, …)` gathers scattered bits, sized to what it returns. `util::sext(v, bits)` sign-extends. `util::make_bitmask<T>(n)` replaces `(1 << n) - 1`. `get_u16le`/`put_u32be` and friends (`multibyte.h`) replace hand-assembled bytes. Bit arithmetic is simplified (`~x & 7`, not `7 - (x & 7)`; `~y & 1`, not `(y ^ 1) & 1`).
- Grep: `>>\s*\d+\)\s*&\s*1\b`, `&\s*0x[0-9a-f]+\)\s*>>`, `&\s*\(1\s*<<\s*\w+\)`, `\(1\s*<<\s*\w+\)\s*-\s*1`, `\bbitswap\(`, `\[\w+\s*\+\s*1\]\s*<<\s*8`, `7 - \(\w+ & 7\)`, `~BIT\(`, `\^ 1\) & 1`
- Not when: multi-bit masks that read naturally as masks.
- Source: cxx: MAME-Specific Helpers; 38fd821b643, 587598e6187, 116c0bf3843, 42391bbb6be, a5889c83221, 272ed7c2767, 367b2d3e742, d4024db0e8e; #16216, #14846

### T-6 Standard library over C · Tidy
`std::fill`, `std::fill_n`, `std::copy_n` and `std::ranges` algorithms replace `memset`/`memcpy`/`memmove` (required for non-trivial types). `std::size` replaces `sizeof(a) / sizeof(a[0])` and literal sizes. A fixed-size buffer allocated once in start is `std::unique_ptr<T []>` from `make_unique_clear`, not a resizable `std::vector` or `new[]`. `std::clamp` and `std::exchange` replace hand-rolled versions.
- Grep: `\bmem(set|cpy|move)\(`, `sizeof\s*\(?\w+\)?\s*/\s*sizeof`, `\bnew \w+\[`, `delete\s*\[\]`, `std::vector<\w+> m_\w+;` resized in start
- Not when: raw byte buffers at a C API boundary.
- Source: 24154bc1f00, 587598e6187, fe4cd123b98, 3e87ac03a86; #15531, #10888

### T-7 Hot paths · Nit
Per-access, per-pixel and per-sample code avoids `std::map`/`unordered_map`, virtual calls, repeated I/O port reads and allocation; fixed lookups use sorted `constexpr` arrays with `std::lower_bound`; a per-pixel loop hoists the row pointer out of the x loop (D-12). A callable parameter that isn't stored is a template parameter (`T &&func`), not `std::function`. `inline` goes on the out-of-class definition, not the in-class declaration. Performance never blocks a correct change on its own ("first make it work, then you can worry about making it fast").
- Grep: `std::(unordered_)?map<` members; `std::function<` in parameter lists; the same `->read()` twice in a function; `^\s+(static )?inline \w` inside class bodies in headers
- Source: 79cea2cbf54, 0b4076983d8, d4024db0e8e; #14151, #15462, #14934

## L. Logging, strings and comments

### L-1 Logging · Tidy
Debug logging uses `logmacro.h`, included after every other header and after `#define VERBOSE`. The submitted change has `VERBOSE` at `0` or commented out and any `LOG_OUTPUT_FUNC` commented out — leaving either on is Blocking. Channels are `#define LOG_FOO (1U << 1)` in ascending bit order (bit 0 is `LOG_GENERAL`), with helpers `#define LOGFOO(...) LOGMASKED(LOG_FOO, __VA_ARGS__)`. Register-access logs start with `machine().describe_context()` and show offsets in hex; `logerror` already includes the device tag. Logging is never commented out, wrapped in `#if`, or guarded by `if (VERBOSE)` — "code the compiler never sees rots"; a message worth keeping gets a channel. Rare unimplemented or unhandled conditions stay unconditional `logerror`. No home-grown `LOG((…))`, `verboselog` or `vsprintf` wrappers; `LOG(fmt, args…)`, never `LOG("%s", string_format(…).c_str())`. No `printf`, `fprintf(stderr, …)` or `std::cout` in `src/devices` or `src/mame`: `logerror` for developers, `osd_printf_*` for user diagnostics, and a commented-out `LOG_OUTPUT_FUNC` names `osd_printf_info`. No debug `popmessage` left enabled.
- Grep: `^#define VERBOSE\s+\(?[1-9A-Z]`, `^#define LOG_OUTPUT_FUNC`, `\b(f?printf|std::cout)\b`, `//\s*(LOG|logerror|printf)`, `if \(VERBOSE\)`, `LOG\w*\(\(`, `verboselog`, `string_format\(.*\)\.c_str\(\)`, `logerror\(.*tag\(\)`, `if \((1|true)\)` before `popmessage`
- Not when: one-line commented-out debug hooks the change doesn't add.
- Source: cxx: Logging; 57ec3ee4a16, 3f75ea4e1c3, 78df9e810bb, 27fc2006786; #15519, #14491, #11250

### L-2 Strings · Tidy
No `sprintf`, `snprintf`, `vsprintf`, `strcpy`, `strcat` or static `char` buffers for formatting ("a buffer overflow waiting to happen"). Use `util::string_format`, or for a string built in pieces `std::ostringstream` with `util::stream_format(os, …)` and `std::move(os).str()` instead of `+=` on `string_format` temporaries; unformatted text uses `<<` or plain concatenation. Format arguments go straight to `logerror`, `popmessage`, `osd_printf_*` and `emu_fatalerror`, without formatting into a temporary first and without `.c_str()`. Text parameters are `std::string_view` when ownership isn't needed; paths are joined with `util::path_concat`. No `std::string` in static objects. User-visible text isn't assembled in a way that can't be translated.
- Grep: `\b(v?s?n?printf|strcpy|strcat)\(` outside logging calls; `static char \w+\[`; `\+=\s*(util::)?string_format\(`; `\.c_str\(\)` inside format-function calls
- Not when: plain concatenation off hot paths.
- Source: b2d602267fd, 8c7f1a819db, 57ec3ee4a16, 3f75ea4e1c3; #10892, #13421, #15192

### L-3 Dead and disabled code · Tidy
Commented-out code, unused members, functions and parameters, impossible tests, one-line trampolines, IDE boilerplate ("Created on:") and duplicate includes come out before the PR ("Commented code always rots"). Code that must stay is guarded by `if (false)` or a `constexpr` condition so the compiler keeps checking it (cxx); new `#if 0` blocks and commented-out blocks are findings. A one-line commented-out debug hook that stays is indented with the code around it (`\t//popmessage(…)`), not parked at column 0 (Nit). A member only written and read within one call becomes a local.
- Grep: three or more consecutive `^\s*//.*[;{}]\s*$` lines; `^#if 0`; `/\*` blocks containing `;`; `Created on:`
- Not when: existing `#if 0` blocks the change doesn't touch; `#if 1` groups of protection patches (C-9).
- Source: cxx: Comments; 4c56746809d, a17c87d7e5c, 2cabaf1541c, 367b2d3e742; #13596, #13018, #13350

### L-4 Comments describe the code, not its history · Tidy
A comment explains the hardware or the code as it now stands, and stays in sync with it (an address range in a comment matches the map). Changelogs don't go in source files ("That's what VCS history is for"), finished items leave TODO lists, and narration of the patch ("changed X to fix Y", "this is correct because…", "no change needed here") comes out, as do explanations written for a reviewer or produced by an AI assistant. No comments explaining basic MAME behaviour, and no speculation presented as fact. A known problem left for later is a one-line `// FIXME: …`.
- Grep: `\b(now|changed|fixed|no longer|is correct|as requested)\b` in added comments; dated change lists; `(fixed)` in TODO lists; comments longer than the code they annotate
- Not when: explanatory comments where behaviour is surprising; terminology from original documentation.
- Source: 2eec6e9cbbd, 55fa36e1629, 0bdb4f0669a, 14e2e3e762b; #13106, #14491, #13340

### L-5 Comment text · Nit
Comments are English (cxx), with acronyms and terms written properly: ROM, RAM, PROM, CPU, IRQ, BIOS, MIDI, MAME, Z80, "DIP switch", MHz/kHz, and byte units with a capital M (Mbyte, MiB). Markers are `TODO:` and `FIXME:`; MAME Testers bugs are cited as `MT07516`. A header comment names its own file.
- Grep: `\b(irq|bios|cpu|rom|ram|proms?|Mame|Midi|z80|dip switch|Mhz|KHz|mbyte)\b` in comments; `To ?[Dd]o:`; `\btodo\b`; `MT ?#?\d+`
- Not when: text quoted from hardware, manuals or the screen.
- Source: cxx: Comments; 387a494c637, fa2deb3e353, 7b513f954e6, 146c0b9191a, 367b2d3e742, d4024db0e8e; #16239

## X. Layout beyond srcclean

### X-1 Bracing and indentation · Tidy
Braces follow the file's K&R or Allman style (cxx) — "not random bits of K&R in between" — consistently within one `if`/`else` chain: if either branch needs braces, both get them. A body spanning more than one line — a nested loop or `if` — is braced. A single-statement body goes on its own line (`if (card)` / `card->set_bus(…);`), never `if (x) y;` on one line. `else if` is written on one line. `switch` braces sit at the `switch`'s level. Namespace contents aren't indented; `ROM_START` bodies and constructor initialiser lists are indented one level; continuation lines two (cxx).
- Grep: `^\s*(if|for|while)\s*\(.*\)\s*[^{;\s][^;]*;\s*$`; `^\s*else\s*$` followed by `if`; `\) \{$` or `\} else \{` in an Allman file; indented lines directly inside `namespace {`
- Not when: neither branch of an `if`/`else` is longer than one statement (braces optional).
- Source: cxx: Bracing and indentation; 4c56746809d, fe4cd123b98, 0bdb4f0669a, d4024db0e8e; #13665, #13021, #12659

### X-2 Spacing, line breaks and blank lines · Nit
A space follows `if`, `for`, `while` and `switch`; binary operators and commas get single spaces; parentheses have no inner padding (`f(a, b)`, not `f( a, b )`) (cxx). Member declarations aren't column-aligned or partially tabulated ("It's grating") — DIP settings are the exception. A long call or declaration breaks after `(`, with continuation lines two tabs deeper rather than aligned under the parenthesis, and the closing `);` stays on the last argument's line. A multi-line argument list breaks after a comma, never before one (`a,` ending a line, not `,b` starting the next). Array-of-unknown-bound types take a space before the brackets — `std::unique_ptr<u8 []>`, `std::make_unique<u8 []>(n)` — a Nit, since most of the tree omits it. `template <…>` sits on its own line in out-of-line definitions. One blank line separates functions, classes and sections; there are no blank lines between the blocks of an `if`/`else` chain (cxx), between an initialiser list and its body, or at the end of a function. Lambdas list their captures explicitly (`[this]`, never `[&]`/`[=]`) and are written `[this] (args)`. Fallthrough is `[[fallthrough]];`, not a comment.
- Grep: `\b(if|for|while|switch)\(`; `,\S`; `\(\s+\S|\S\s+\)`; `^\s*\);\s*$`; `\}\n\n\s*else`; `\[[=&][,\]]`; `\]\(`; `//\s*fall ?through`
- Source: cxx: Spacing, Bracing and indentation; 7b513f954e6, 9359d54aeee, 79cea2cbf54, 660670fd9bd, 367b2d3e742, d4024db0e8e; #14029, #15531, #15517

### X-3 Parentheses · Nit
A ternary's compound condition and compound operands are parenthesised (`(i & 1) ? 7 : 0`, `(a || b) ? x : y`), as are shifts mixed with `|` or `+`. Parentheses around a whole right-hand side or a single name go — unless they document intent ("They're for the developer, not the compiler").
- Grep: `[&|<>=+-]\s*\w+\s*\?` without a `(` before the condition; `=\s*\(\w+\s*&\s*0x[0-9a-f]+\);`
- Source: 79cea2cbf54, d2d85d39519, fa2deb3e353, 367b2d3e742, d4024db0e8e; #16135, #14911
