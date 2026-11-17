#!/usr/bin/env python3
"""textfiles.py: src/TEXTFILES.md, the list of the text and data files of the WEGA root that src/ does not yet produce (increment 130).

The rows come from src/COVERAGE.md (status not exact, kind text or data; binaries and archives are not part of 130). For every file:
  Art      data, text, or script (executable, first byte # or :)
  Regel    C: the WEGA original is copied (System III has nothing, or only the finished file, or a copy rule);
           G: System III makes the file (generator and inputs: taken 1:1 from the System III tree; the result must be byte-identical
              with the WEGA original, else the WEGA original is copied: the check is "offen" until the generator has run)
  SYSIII   the file of that name in the System III tree: "=" same bytes as the WEGA file, "~" differs; source: usr/src/cmd/..., image: the
           installed file of the System III tree
  Ziel     where the file goes in src/: a source directory of System III (usr/src/cmd/X/...) becomes src/cmd/X/...; a script src/cmd/NAME.sh;
           else the path of the root below src/ (as src/etc/inittab)
  Korpus   ja: a corpus is required (owner: every script that lands in usr/bin); nein: no program reads... (a plain file: checkall's
           byte identity is the test); offen: a script outside usr/bin, to be decided
Run: python3 host/tools/textfiles.py [WEGA_ROOT]   (needs the System III tree in tmp/sys3)"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..'))
TOP = os.path.dirname(REPO)
W = sys.argv[1] if len(sys.argv) > 1 else os.path.join(TOP, 'tmp', 'wega31_root')
S = os.path.join(TOP, 'tmp', 'sys3')
SC = 'usr/src/cmd/'

# generators: prefix or name -> (generator files in usr/src/cmd/, what it needs)
GEN = [
    ('usr/lib/macros/', ['text/macros.d/macros.mk', 'text/macros.d/macrunch', 'text/macros.d/ntar.c', 'text/macros.d/%(b)s.src'],
     'macrunch (sed, ntar -g) makes the macro package from NAME.src'),
    ('usr/lib/term/tab', ['text/roff.d/terms.d/terms.mk', 'text/roff.d/terms.d/maketerms.c', 'text/roff.d/terms.d/terms.hd',
                          'text/roff.d/terms.d/%(b)s.c'],
     'maketerms.c (three passes: -DPART1 .. 3) writes the tables; tab300S, tab300S-12 are links to tab300s, tab300s-12'),
    ('usr/lib/suftab', ['text/roff.d/makefile', 'text/roff.d/suftabmake.c', 'text/roff.d/suftab.c'], 'suftabmake.c writes suftab'),
    ('usr/lib/llib-lc.ln', ['lint/lint.mk', 'lint/lint.c', 'lint/llib-lc'], 'lint1 -vx over cpp -C llib-lc'),
    ('usr/lib/llib-port.ln', ['lint/lint.mk', 'lint/lint.c', 'lint/llib-port'], 'lint1 -vxp over cpp -C llib-port'),
    ('usr/dict/hlista', ['spell/spell.mk', 'spell/spellin.c', 'spell/american', 'spell/local', 'spell/list'], 'spellin over american, local'),
    ('usr/dict/hlistb', ['spell/spell.mk', 'spell/spellin.c', 'spell/british', 'spell/local', 'spell/list'], 'spellin over british, local'),
    ('usr/dict/hstop', ['spell/spell.mk', 'spell/spellin.c', 'spell/stop'], 'spellin over stop'),
    ('usr/lib/cref/ctab', ['cref/cref.mk', 'cref/mtab.c', 'cref/ctable'], 'mtab over ctable'),
    ('usr/lib/cref/cign', ['cref/cref.mk', 'cref/mign.c', 'cref/csym'], 'mign over csym'),
    ('usr/lib/cref/eign', ['cref/cref.mk', 'cref/esym'], 'copy of esym'),
]
# source directories of System III for files that it only copies (the WEGA original is taken anyway)
SRCDIR = [('usr/lib/help/term', 'text/shells.d/term'), ('usr/lib/help/text', 'text/shells.d/text'),
          ('usr/lib/help/', 'sccs/help.d/%(b)s'), ('usr/lib/tmac/', 'text/macros.d/%(b)s'), ('usr/lib/yaccpar', 'yacc/yaccpar'),
          ('usr/lib/uucp/', 'uucp/%(b)s'), ('usr/lib/llib-lc', 'lint/llib-lc'), ('usr/lib/llib-port', 'lint/llib-port'),
          ('usr/bin/sccsdiff', 'sccs/src/sccsdiff'), ('usr/lib/spell', 'spell/%(b)s')]
# generators that were run: the result (2026-11-17); a prefix that did not reproduce the WEGA file falls back to C with the reason
CHECKED = {'usr/lib/macros/': ('G', 'macrunch (option -r, WEGA\'s addition): the .src files are the WEGA packages; the .src of System III are another version, '
                                     'so macrunch/ntar of System III do not give the WEGA bytes (checked 2026-11-17)')}
ZIELDIR = []
NOGEN = {'usr/lib/term/tab-12', 'usr/lib/term/tab630wp-12'}	# no System III source for these


def rows():
    r = []
    for l in open(os.path.join(REPO, 'src', 'COVERAGE.md'), errors='replace'):
        m = re.match(r'\| `([^`]+)` \| (\w+)[^|]*\| ([^|]*)\| (\w+) \|', l)
        if m and m.group(4) != 'exact' and m.group(2) in ('text', 'data'):
            r.append(m.group(1))
    return r


def same(a, b):
    try:
        return open(a, 'rb').read() == open(b, 'rb').read()
    except OSError:
        return None


def main():
    allf = subprocess.run(['find', S, '-type', 'f'], capture_output=True, text=True).stdout.split('\n')
    byname = {}
    for p in allf:
        byname.setdefault(os.path.basename(p), []).append(p)
    tab = []
    for f in rows():
        b = os.path.basename(f)
        w = os.path.join(W, f)
        head = open(w, 'rb').read(2)
        script = os.stat(w).st_mode & 0o111 and head[:1] in (b'#', b':')
        art = 'script' if script else ('data' if f.startswith('usr/dict') or '/term/tab' in f or f.endswith('.ln') else 'text')
        rule, gen, note = 'C', [], ''
        for pre, files, n in GEN:
            if f.startswith(pre) and f not in NOGEN:
                bb = b.lower() if b.startswith('tab300S') else b	# the table of tab300S is made as tab300s (a link)
                rule, gen, note = 'G', [x % {'b': bb} for x in files], n
                for cp, (cr, cn) in CHECKED.items():
                    if f.startswith(cp):
                        rule, note = cr, cn
                break
        sys3 = []
        srcrel = None
        for p in byname.get(b, []):
            rel = p[len(S) + 1:]
            kind = 'source' if rel.startswith(SC) else 'image'
            e = same(w, p)
            sys3.append('%s %s %s' % (rel, kind, '=' if e else '~' if e is False else '?'))
            if kind == 'source' and (rel.startswith(SC + 'sccs/') or '/' in rel[len(SC):]) and srcrel is None:
                srcrel = rel[len(SC):]
        for pre, tpl in SRCDIR:
            if f.startswith(pre):
                cand = tpl % {'b': b}
                if os.path.isfile(os.path.join(S, SC, cand)):
                    srcrel = cand
        for zp, zd in ZIELDIR:
            if f.startswith(zp):
                srcrel = zd + b
        if script:
            ziel = 'src/cmd/%s.sh' % b
            korpus = 'ja (Vorgabe: Skript nach usr/bin)' if f.startswith('usr/bin/') else 'offen'
        else:
            ziel = 'src/cmd/' + srcrel if srcrel else 'src/' + f
            korpus = 'nein'
        if rule == 'G':
            ziel = 'made by the build from src/cmd/' + os.path.dirname(gen[-1]) + '/'
        miss = [g for g in gen if not os.path.isfile(os.path.join(S, SC, g))]
        tab.append((f, art, rule, ziel, korpus, sys3, gen, miss, note, os.path.getsize(w)))
    out = ['# Text and data files still to take into src/ (increment 130)', '',
           'Generated by `host/tools/textfiles.py` from `src/COVERAGE.md` (do not edit). Binaries and archives are not part of 130.', '',
           'Rules (owner, 2026-11-17): the WEGA file is copied; System III and Plexus are ideas. Exception **G**: System III makes the file',
           '(generator and inputs 1:1 from the System III tree) - the result must be byte-identical with the WEGA file, else **C**.',
           'A file System III only copies, or has nothing for, is **C**: the WEGA original. A script that lands in usr/bin gets a corpus.', '']
    n = {}
    for t in tab:
        n[(t[2], t[1])] = n.get((t[2], t[1]), 0) + 1
    out.append('| Rule | Art | files |\n|---|---|---|')
    for (r, a), c in sorted(n.items()):
        out.append('| %s | %s | %d |' % (r, a, c))
    out.append('| **all** | | %d |' % len(tab))
    out.append('')
    cur = None
    for t in sorted(tab, key=lambda x: (os.path.dirname(x[0]), x[0])):
        d = os.path.dirname(t[0]) or 'root'
        if d != cur:
            cur = d
            out += ['', '## ' + d, '', '| File | Art | Rule | Bytes | Ziel in src/ | Korpus | System III | Generator (usr/src/cmd/) |', '|---|---|---|---|---|---|---|---|']
        g = ''
        if t[2] == 'G':
            g = t[8] + ': ' + ', '.join(t[6]) + (' (**missing: %s**)' % ', '.join(t[7]) if t[7] else '') + ' - check: offen'
        out.append('| `%s` | %s | %s | %d | `%s` | %s | %s | %s |' % (os.path.basename(t[0]), t[1], t[2], t[9], t[3], t[4],
                                                                         '<br>'.join(t[5]) or '-', g))
    open(os.path.join(REPO, 'src', 'TEXTFILES.md'), 'w').write('\n'.join(out) + '\n')
    print(len(tab), 'files;', {k: v for k, v in n.items()})


main()
