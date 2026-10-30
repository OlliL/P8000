#!/usr/bin/env python3
"""cutgram.py - cut the parser module cgram1 (text 0x002a..0x1374, data 0x0002..0x1a64) of /lib/cparse as an
assembler module ../cgram1.s (fallback until the grammar is rebuilt; owner decision, HANDOFF_cparse.md).

Usage: cutgram.py [OUT.s]      (WEGA=root of the original tree)

host/tools/unlink.py does the cut with "immmin"/"idxmin" (the data of cgram1 starts at 0x0002, so small constants
look like data addresses); the names of the other modules come from findings.json (functions) and the table
DATA below (the variables that the parser uses). The EPU instructions are .word lines for unlink.py; their address
word (third word) is made symbolic here when it is a data address of another module.
"""
import json
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..', '..', '..'))
ROOT = os.environ.get('WEGA') or '/mnt/share/cvs/P8000/tmp/wega31_root'

# the functions of cgram1 (exported) and the variables of the other modules that the parser uses
OWN = {'_mkty': 0x002a, '_bdty': 0x004e, '_dstash': 0x00b4, '_savebc': 0x00e2, '_resetbc': 0x0130,
       '_addcase': 0x0174, '_adddef': 0x01e0, '_swstart': 0x0230, '_swend': 0x0250, '_yyparse': 0x0312}
DATA = {'_Fflag': 0x1aa2, '_reached': 0x1aa4, '_lineno': 0x1b0a, '_retlab': 0x1b1c, '_psavbc': 0x1b22,
        '_curftn': 0x489a, '_asavbc': 0x489c, '_instruct': 0x4964, '_ntab': 0x4966, '_brklab': 0x62cc,
        '_stwart': 0x62d2, '_contlab': 0x652c, '_paramno': 0x652e, '_Ffile': 0x6530, '_flostat': 0x6532,
        '_lastcon': 0x6534, '_curdim': 0x6538, '_idname': 0x653a, '_strucoff': 0x653c, '_dcon': 0x6540,
        '_blevel': 0x6548, '_regvar': 0x654a, '_curclass': 0x654c, '_retstat': 0x654e,
        '_swtab': 0x6554, '_swp': 0x6c5c, '_swx': 0x6c5e, '_autooff': 0x6c60, '_lregvar': 0x6c64,
        '_dimtab': 0x6c66}
COMM = {'_yyval': (0x62ce, 4), '_yyv': (0x62d4, 600), '_yylval': (0x6550, 4)}
LIBC = {'_printf': 0xa790, '_fprintf': 0xa7e0, '_sprintf': 0xa830}


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, '..', 'cgram1.s')
    fn = json.load(open(os.path.join(HERE, 'findings.json')))['functions']
    syms = dict(DATA)
    tsyms = dict(LIBC)
    for n, f in fn.items():
        a = int(f['orig'], 16)
        if not 0x002a <= a < 0x1374 and a < 0x9f06 and a >= 0x2a:
            tsyms['_' + n] = a
    spec = ['noown', 'notables', 'immmin 1000', 'idxmin 100']
    spec.append('sym ' + ' '.join('%s=%04x' % (n, a) for n, a in sorted(syms.items())))
    spec.append('tsym ' + ' '.join('%s=%04x' % (n, a) for n, a in sorted(tsyms.items())))
    spec.append('cgram1 old 002a 1374 %d dstart=0002 ' % (0x1a64 - 0x0002) +
                ' '.join('text:%s=%04x' % (n, a) for n, a in sorted(OWN.items())) + ' ' +
                ' '.join('comm:%s=%04x:%d' % (n, a, s) for n, (a, s) in sorted(COMM.items())))
    with tempfile.TemporaryDirectory() as d:
        sp = os.path.join(d, 'spec')
        open(sp, 'w').write('\n'.join(spec) + '\n')
        r = subprocess.run([sys.executable, os.path.join(REPO, 'host', 'tools', 'unlink.py'), sp,
                            os.path.join(ROOT, 'lib', 'cparse'), d,
                            os.path.join(REPO, 'src', 'lib', 'libc', 'crt', 'crt0-codgen.o')], capture_output=True, text=True)
        sys.stderr.write(r.stdout)
        sys.stderr.write(r.stderr)
        text = open(os.path.join(d, 'cgram1', '0_1.s')).read()
    names = {a: n for n, a in syms.items()}
    ext = set()

    def epu(m):
        w = m.group(3)
        v = int(w, 16)
        if v in names and v >= 0x1000:
            ext.add(names[v])
            return '.word\t%s,%s,%s' % (m.group(1), m.group(2), names[v])
        return m.group(0)
    text = re.sub(r'\.word\t(%[0-9A-F]{4}),(%[0-9A-F]{4}),%([0-9A-F]{4})\b', epu, text)
    # immediates into the bss that are not the start of a variable
    for a, sym in ((0x495c, '_asavbc+192'), (0x4960, '_asavbc+196'), (0x62d0, '_yyv-4')):
        if '#%%%04X' % a in text and sym[:3] != '_yy':
            ext.add(sym.split('+')[0])
        text = text.replace('#%%%04X' % a, '#' + sym)
    if ext:
        m = re.search(r'^\t\.extern\t(.*)$', text, re.M)
        old = set(m.group(1).split(', ')) if m else set()
        line = '\t.extern\t' + ', '.join(sorted(old | ext))
        text = text.replace(m.group(0), line) if m else line + '\n' + text
    # cas: a long .extern line overflows its parser stack; 8 names a line
    def split_ext(m):
        ns = m.group(1).split(', ')
        return '\n'.join('\t.extern\t' + ', '.join(ns[i:i + 8]) for i in range(0, len(ns), 8))
    text = re.sub(r'^\t\.extern\t(.*)$', split_ext, text, flags=re.M)
    hdr = ('// cgram1: the parser of /lib/cparse (cgram1.y 2.5) cut from the original by tools/cutgram.py\n'
           '// (fallback until the grammar is rebuilt; do not edit, make it again)\n')
    open(out, 'w').write(hdr + text)
    print('wrote', out)


if __name__ == '__main__':
    main()
