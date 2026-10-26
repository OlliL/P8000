# MAME review rules: software lists, layouts and translations

Read when the change touches software lists (`hash/*.xml`), layouts (`*.lay`) or translations (`language/`). Sources: [Guidelines for Software Lists](https://docs.mamedev.org/contributing/softlist.html) (`softlist.rst`), plus commit and pull-request references in the notation described at the top of [RULES.md](RULES.md). Everything here is Tidy unless marked; the calibration in [SKILL.md](SKILL.md) applies.

## Software lists

The XML structure is defined by `hash/softwarelist.dtd`; CI runs `xmllint --noout --valid hash/<list>.xml` on every list.

An item represents the **original media**, not a particular dump: prefer one file per ROM chip over a combined image, and disk images over archives of extracted files. Converted, regenerated, cracked, defaced, watermarked or reconstructed images are not "best available" — they are marked bad dumps or left out (Blocking in `*_orig.xml` lists, which hold only original images). Paid software less than three years old isn't added.
- Source: softlist.rst; 79994c3fee3; #14516, #15266, #14759

### Items and parts

- **SL-1 Item `name`**: lowercase ASCII letters, digits and `_` only; at most 16 characters; unique in the list. Keep it terse so clone names can add a suffix. Source: softlist.rst; 3866c082a09
- **SL-2 `cloneof`**: the parent is in the same list; cross-list parent/clone is unsupported. Regional versions, revisions and "(alt)" dumps of one title are clones of one parent. Source: softlist.rst; c1816c83ff5
- **SL-3 `supported` reflects testing** (Blocking when overstated): `yes` (the default when absent) only when the item runs from the list; `partial` when it runs with limitations; `no` when it doesn't. A dump missing required data is a bad dump with `supported="no"`. Don't promote items on systems nobody has confidence in. Lists for skeleton or not-working systems needn't set it. Source: softlist.rst; db04c5c3b0f; #14043, #14091, #11664
- **SL-4 Parts**: one `part` per mountable medium (each floppy; each tape side). Multiple ROM chips of one cartridge are files in one part, not separate parts. `part name` follows SL-1's character rules; `interface` is required and must match the target system's media device. A part's `part_id` feature carries the physical label text. Source: softlist.rst; #14551

### Metadata

- **SL-5 Required elements**: `description`, `year`, `publisher` on every item. Source: softlist.rst
- **SL-6 `description`**: the title as printed on the media or box, transliterated into English Latin script where needed, keeping any stylised capitalisation the publisher uses; unique in the list. Disambiguating text is lowercase except proper nouns, initialisms and verbatim quotes — `Title (Japan, rev 1)`, `(alt)`, not `(Rev 1)` or `(alt.)`. Titles in the original script go in `<info name="alt_title" .../>`. Source: softlist.rst; 9aba7a8bf02; #12986
- **SL-7 `year`**: year of release or copyright; if unknown, an estimate with `?` (`198?`, `1991?`). Source: softlist.rst
- **SL-8 `publisher`**: the publisher as a proper noun, spelled as the company spelled it (keep "Co." when it's part of the name). An unknown publisher is the markup `&lt;unknown&gt;`, never a company called "Unknown". Source: 6a314d9b3a7; #14551
- **SL-9 `info` elements**: one value per element; repeat the element for several values (several `language` entries, several regional `alt_title`s) rather than joining them. Recognised names: `alt_title`, `author`, `barcode`, `developer`, `distributor`, `install`, `isbn`, `language`, `oem`, `original_publisher`, `partno`, `pcb`, `programmer`, `release`, `serial`, `usage`, `version`. Add a value only when it can be sourced (no unsourced `region`); `programmer` lists programmers, not artists or writers — the list isn't a full credits database ("you're trying to recreate MobyGames"). A one-line usage hint is `<info name="usage" value="…"/>`, not a `<notes>` block. Source: softlist.rst; 8e54efdc35a; #14146, #15410
- **SL-10 `release`**: `YYYYMMDD`, no punctuation; unknown day is `xx` (`199103xx`) — an unambiguous ISO date, never a locale-formatted one. Source: softlist.rst; 4315fd921eb
- **SL-11 Compatibility values**: `compatibility` features and the matching `set_filter()` strings are uppercase, like the other consoles (`EXP`, `!EXP`). Source: bb532d39a12

### File

- **SL-12 Header**: `<?xml version="1.0" encoding="UTF-8"?>` with the explicit encoding, then `<!DOCTYPE softwarelist SYSTEM "softwarelist.dtd">`, then a `<!--` comment whose first line is `license:CC0-1.0` (every list uses this license). Source: 954def46685
- **SL-13 Formatting**: tab indentation matching the rest of the list, kept intact; srcclean applies to `hash/*.xml` too. Source: f124f2ddffa; #15410

## Layouts and translations

### O-1 Layouts (`*.lay`) · Tidy
Layouts are written for humans: indented to show structure, SVG with sane coordinates and `viewBox`, no nested `<svg>`, transforms or CSS `style=`. Shapes are drawn in white and coloured per instance; repeated elements use `<repeat>`; element and quad counts stay low ("MAME still thinks immediate mode is cool"). Only controls that physically exist are drawn, with no brand logos, and a device's layout is attached to the device. Each layout has a CC0 license comment. Text is written in the characters themselves (`£1`), not numeric character references (`&#xa3;1`), for anything up to U+052F — the same cutoff as C++ literals (I-3); characters beyond it (card suits, `&#x2660;`) may stay as references, and `<`, `>` and `&` keep their XML escapes.
- Grep: `<svg[^>]*>[\s\S]*<svg`; `style=`; `transform=`; long high-precision `d="` paths; runs of near-identical `<element ref=` lines; `&#x0*(?!3[cCeE];)[0-9a-fA-F]{2,3};`; missing `license:CC0`
- Source: 272ed7c2767; #14781, #14049, #15697, #11747

### O-2 Translations and UI text · Tidy
`language/*/*.po` changes use correct terminology in the target language, not literal translations, and don't reflow the file. UI behaviour and colours stay consistent across menus and don't assume a dark theme.
- Grep: whole-file `.po` diffs
- Source: #10916, #11983, #15283
