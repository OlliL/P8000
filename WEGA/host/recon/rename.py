#!/usr/bin/env python3
"""rename.py - rename an identifier of a program everywhere (sources, tables, notes, documents; configuration: progcfg.py).

Usage: rename.py OLD NEW [OLD NEW ...] [-n]      (-n: dry run, print the counts only)

OLD is a function (key of findings.json or its address id f<entry>) or any other identifier of the sources. Replaced as a
whole word in:
  - the sources: SRCDIR/*.c, SRCDIR/*.h (SRCDIR/*.y too)
  - the tables: own_functions.txt and progcfg TABLES (files of the tools directory)
  - findings.json (keys of functions and manual_classes, and the text of the notes)
  - every .md of the tools directory, progcfg DOCS, src/TODO.md, host/KNOWLEDGE.md, host/HANDOFF_wega_disasm.md
then FINDINGS.md is generated again (findings_md.py). A text in KNOWLEDGE.md that belongs to another program with the
same identifier is changed too: check the diff of the documents (-n first).

Unlike src/cmd/sld/tools/rename.py the documents are changed too. Prose like "NEW (OLD)" or "OLD (NEW)", which the
replacement turns into "NEW (NEW)", is collapsed to "NEW". External names are significant for 7 characters: NEW must
not share its first 7 characters with another external name of the sources (checked only when OLD is in the sources;
an address id f<entry> of a function that has its name already may map onto that name).
"""
import json
import os
import re
import subprocess
import sys

import progcfg

CFG = progcfg.load()
HERE = CFG.DIR
LD = CFG.SRCDIR
WEGA = CFG.REPO
SOURCES = [os.path.join(LD, f) for f in sorted(os.listdir(LD)) if f.endswith(('.c', '.h', '.y'))]
TABLES = [os.path.join(HERE, f) for f in ['own_functions.txt'] + list(CFG.TABLES)]
DOCS = [os.path.join(HERE, f) for f in sorted(os.listdir(HERE)) if f.endswith('.md') and f != 'FINDINGS.md']
DOCS += [os.path.join(HERE, f) for f in CFG.DOCS]
DOCS += [os.path.join(WEGA, 'src', 'TODO.md'), os.path.join(WEGA, 'host', 'KNOWLEDGE.md'),
         os.path.join(WEGA, 'host', 'HANDOFF_wega_disasm.md')]
FINDINGS = CFG.FINDINGS


def externals():
    """names defined at top level in the sources (functions and globals), without static ones"""
    names = set()
    for p in SOURCES:
        for l in open(p):
            if l.startswith(('static', '#', ' ', '\t', '/*', '*', '}', '{')) or not l.strip():
                continue
            names.update(re.findall(r'\b([A-Za-z_]\w*)\s*[\(\[=;,]', l))
    return names


def sub(text, old, new):
    t, n = re.subn(r'\b%s\b' % re.escape(old), new, text)
    if n:
        t = re.sub(r'\b%s \(%s\)' % (re.escape(new), re.escape(new)), new, t)
    return t, n


def main():
    a = sys.argv[1:]
    dry = '-n' in a
    a = [x for x in a if x != '-n']
    if len(a) % 2:
        sys.exit(__doc__)
    pairs = list(zip(a[0::2], a[1::2]))
    ext = externals()
    for old, new in pairs:
        if old not in ext:
            continue    # an address id (f<entry>) of an already named function: only the documents and tables change
        clash = [e for e in ext if e != old and e[:7] == new[:7]]
        if clash:
            sys.exit('%s: the first 7 characters clash with %s' % (new, ', '.join(clash)))
        if new in ext:
            sys.exit('%s exists already' % new)
    total = {}
    for p in SOURCES + TABLES + DOCS:
        if not os.path.exists(p):
            continue
        t = open(p, encoding='latin-1').read()
        n0 = 0
        for old, new in pairs:
            t, n = sub(t, old, new)
            n0 += n
        if n0:
            total[os.path.relpath(p, WEGA)] = n0
            if not dry:
                open(p, 'w', encoding='latin-1').write(t)
    j = json.load(open(FINDINGS))
    n0 = 0
    for old, new in pairs:
        for sect in ('functions', 'manual_classes'):
            if old in j[sect]:
                j[sect] = {(new if k == old else k): v for k, v in j[sect].items()}
                n0 += 1
    s = json.dumps(j, indent=1)
    for old, new in pairs:
        s, n = sub(s, old, new)
        n0 += n
    if n0:
        total[os.path.relpath(FINDINGS, WEGA)] = n0
        if not dry:
            open(FINDINGS, 'w').write(s)
    for p, n in sorted(total.items()):
        print('%5d  %s' % (n, p))
    if not dry:
        subprocess.run([sys.executable, os.path.join(os.path.dirname(os.path.abspath(__file__)), 'findings_md.py')],
                       cwd=HERE, check=True, stdout=subprocess.DEVNULL, env=dict(os.environ, RECON_PROG=HERE))


if __name__ == '__main__':
    main()
