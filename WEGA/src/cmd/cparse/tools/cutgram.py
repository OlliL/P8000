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
# the bss of /lib/cparse in its order (tools/bsspair.py; the two gap words are not referenced anywhere). ld gives the
# commons their places in the order of the first mention in the objects; cgram1.o is the first object, so its .comm
# lines fix the layout of the whole bss (the libc commons follow from cplibc.o)
BSS = [('_curftn', 2), ('_asavbc', 200), ('_instruct', 2), ('_ntab', 6500), ('_bssgap1', 2), ('_brklab', 2),
       ('_yyval', 4), ('_stwart', 2), ('_yyv', 600), ('_contlab', 2), ('_paramno', 2), ('_Ffile', 2), ('_flostat', 2),
       ('_lastcon', 4), ('_curdim', 2), ('_idname', 2), ('_strucoff', 4), ('_dcon', 8), ('_blevel', 2), ('_regvar', 2),
       ('_curclass', 2), ('_retstat', 2), ('_yylval', 4), ('_swtab', 1800), ('_swp', 2), ('_swx', 2), ('_autooff', 4),
       ('_lregvar', 2), ('_dimtab', 4000), ('_dfile', 2), ('_node', 5600), ('_dpath', 2), ('_sfile', 2), ('_outfile', 2),
       ('_minrvar', 2), ('_ftnpos', 4), ('_dfname', 50), ('_fminrvar', 2), ('_inoff', 4), ('_ifname', 50), ('_ipath', 2),
       ('_fregvar', 2), ('_sfname', 50), ('_spath', 2), ('_strflg', 2), ('_maxoff', 4), ('_lxmask', 258), ('_lxmatch', 2),
       ('_yytext', 126), ('_lxgcp', 2), ('_lxcp', 258), ('_pregvar', 2), ('_paramstk', 300), ('_dpos', 4),
       ('_pfregvar', 2), ('_dope', 258), ('_afull', 2), ('_opst', 258), ('_dbuf', 512), ('_ibuf', 512), ('_sbuf', 512),
       ('_lastfree', 2), ('_instack', 220), ('_iclass', 2), ('_pstk', 2), ('_hashtab', 128), ('_bssgap2', 2),
       ('_shdrpos', 4), ('_reftab', 3306), ('_commflag', 2), ('_shdr', 14), ('_refsym', 1102), ('_taglist', 1000)]
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
    for n, (a, sz) in COMM.items():
        syms[n] = a
    spec = ['noown', 'notables', 'immmin 1000', 'idxmin 100']
    spec.append('sym ' + ' '.join('%s=%04x' % (n, a) for n, a in sorted(syms.items())))
    spec.append('tsym ' + ' '.join('%s=%04x' % (n, a) for n, a in sorted(tsyms.items())))
    spec.append('cgram1 old 002a 1374 %d dstart=0002 ' % (0x1a64 - 0x0002) +
                ' '.join('text:%s=%04x' % (n, a) for n, a in sorted(OWN.items())))
    for n, (a, sz) in COMM.items():
        syms[n] = a
    with tempfile.TemporaryDirectory() as d:
        sp = os.path.join(d, 'spec')
        open(sp, 'w').write('\n'.join(spec) + '\n')
        r = subprocess.run([sys.executable, os.path.join(REPO, 'host', 'tools', 'unlink.py'), sp,
                            os.path.join(ROOT, 'lib', 'cparse'), d,
                            os.path.join(HERE, '..', 'crt0.o')], capture_output=True, text=True)
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
    bssnames = set(n for n, sz in BSS)

    def split_ext(m):
        ns = [n for n in m.group(1).split(', ') if n not in bssnames]
        return '\n'.join('\t.extern\t' + ', '.join(ns[i:i + 8]) for i in range(0, len(ns), 8)) or '//'
    text = re.sub(r'^\t\.extern\t(.*)$', split_ext, text, flags=re.M)
    # the bss in the order of the original
    text = '\n'.join(l for l in text.split('\n') if not l.startswith('\t.comm\t'))
    text = ''.join('\t.comm\t%d, %s\n' % (sz, n) for n, sz in BSS) + text
    hdr = ('// cgram1: the parser of /lib/cparse (cgram1.y 2.5) cut from the original by tools/cutgram.py\n'
           '// (fallback until the grammar is rebuilt; do not edit, make it again)\n')
    open(out, 'w').write(hdr + text)
    print('wrote', out)


if __name__ == '__main__':
    main()
