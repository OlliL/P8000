#!/usr/bin/env python3
"""funcs.py - writes FUNCTIONS.md: the functions of codgen/scodgen, their place in our binary and in the original.

  python3 funcs.py [--mine ./codgen] [--orig $WEGA/lib/codgen] [--out FUNCTIONS.md] [--name codgen]
  python3 funcs.py --mine ./scodgen --orig $WEGA/lib/scodgen --name scodgen

Our start and end of each function come from the symbol table of our binary (exact). The original binary is
stripped: its function starts are KNOWN below (ORIG, from the comparison work in host/KNOWLEDGE.md "codgen
fidelity"); the comparison is the instruction multiset of the code from the start up to the first `ret`
(host/tools/cgcmp.py normalisation, old-cc effects masked). Keep ORIG up to date when a function is compared again.

The tools sdis.py and sout.py are read from ../../../host/tools (the repository layout).
"""
import argparse
import collections
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.normpath(os.path.join(HERE, '..', '..', '..', 'host', 'tools'))
sys.path.insert(0, TOOLS)
from sout import SOut  # noqa: E402

LIM = 0xbbc4   # the program part of the original ends here (the C library follows)

# name (as in the symbol table of our binary): (start in the original, verdict or None, note)
# verdict None: the comparison decides; otherwise the verdict is written as it is (for functions whose first `ret`
# does not end the function, or whose difference is known from the analysis).
ORIG = {
    'getreg':    (0x7c10, None, 'D2/D6 (switch layout), D7'),
    'rsreturn':  (0x7f50, None, 'D1 frame, D7 layout'),
    'nullsyms':  (0x811e, None, 'mode test first (quirk), old cc memory operand'),
    'rsfreeall': (0x815e, None, ''),
    'rsrestart': (0x8502, None, 'identical up to the first ret'),
    'rsinit':    (0x740c, None, 'lda/inc of a pointer (old cc)'),
    'rsinsert':  (0x7650, None, ''),
    'regjam':    (0x7916, None, ''),
    'roverlap':  (0x7790, None, 'early returns are the case values (quirk)'),
    'newnode':   (0x75e0, None, 'switch layout (D7), shared tail'),
    'freereg':   (0x6d5a, None, 'rewritten; D1 frame, D7 layout'),
    'usable':    (0x6e78, None, ''),
    'reclaim':   (0x7058, None, 'D7 layout'),
    'allo':      (0x67e8, None, 'D1 frame, D10 shifts (sdal), D7'),
    'allocreg':  (None, None, 'not located yet'),
    'rfind':     (None, None, 'not located yet'),
    'rclass':    (None, None, 'not located yet'),
    'ltypeop':   (0x3e66, None, 'D3: p->op loaded once in the original'),
    'argprep':   (0x18be, None, 'frame 6 bytes (D1), extra ld r,r at early return'),
    'deflab':    (0x59de, None, 'identical up to the ret (call target masked)'),
    'funcend':   (0x289a, None, 'identical up to the ret'),
    'nextcook':  (0x4674, None, 'identical'),
    'icright':   (0x46d0, None, 'D3'),
    'mycanon':   (0x4804, None, 'identical (switch with break cases)'),
    'alloc':     (0x8b6e, None, 'identical after register n, n++/n&=~1, if/else'),
    'fcmd':      (0xa1ba, None, 'identical after spare register and removed s = 0'),
    'delay':     (0x838,  None, 'D3 end of function'),
    'fwalkd':    (0x8774, None, 'D11 tail merge'),
    'fbegin':    (0x8fda, None, 'structsz /16 (fixed); D1'),
    'cbranch':   (0x12bc, None, 'D6 (cpir dispatch), D3, D7'),
    'markcall':  (0x1212, None, 'D3 (ty kept in a register in the original)'),
    'stmt':      (0x75a,  None, 'identical (if chain, two-argument fwalkd)'),
    'mkadrs':    (0x48a0, None, 'D3 scratch registers'),
    'oreg2':     (0x175e, None, 'D1 frame, stw layout'),
    'ffld':      (0x15d0, None, 'D4 parameter spill'),
    'fixtemp':   (0x918e, None, 'D3, D6'),
    'cerror':    (0x85ae, None, 'ldm displacement form (old cc)'),
    'nxwork':    (0x98e4, None, 'D6 table starts at 0 (old cc)'),
    'rdsym':     (0x1a86, None, 'D1 frame (no frame in the original first part)'),
    'mapregs':   (0x9512, None, 'D3, D6 partly; if chain now'),
    'gswitch':   (0xa7e0, None, 'not compared yet'),
    'eread':     (0x2480, None, 'not compared yet'),
    'main':      (0x23c,  None, 'not compared yet'),
    'table':     (0x2edc, None, 'not compared yet'),
    'start':     (None,   'crt0', 'entry of crt0, not codgen code'),
    # automatic pairings from the function-boundary work (not yet checked one by one)
    'allocinit': (0x8bea, None, 'auto-paired (bag distance 0), check'),
    'argsize': (0x88f6, None, 'auto-paired (bag distance 0), check'),
    'bcon': (0xb0a0, None, 'auto-paired (bag distance 1), check'),
    'branch': (0x2880, None, 'auto-paired (bag distance 0), check'),
    'bsize': (0x8878, None, 'auto-paired (bag distance 0), check'),
    'canon': (0x1884, None, 'auto-paired (bag distance 0), check'),
    'canon2': (0x46fe, None, 'auto-paired (bag distance 0), check'),
    'cbgen': (0x460e, None, 'auto-paired (bag distance 4), check'),
    'cgprintf': (0x854c, None, 'auto-paired (bag distance 2), check'),
    'codgen': (0x0aaa, None, 'auto-paired (bag distance 0), check'),
    'constore': (0x11da, None, 'auto-paired (bag distance 0), check'),
    'convelim': (0xba7c, None, 'auto-paired (bag distance 4), check'),
    'dopeinit': (0x8844, None, 'auto-paired (bag distance 0), check'),
    'efcode': (0x28f6, None, 'auto-paired (bag distance 3), check'),
    'endline': (0x8c22, None, 'auto-paired (bag distance 0), check'),
    'endrun': (0x4868, None, 'auto-paired (bag distance 0), check'),
    'errexit': (0x89ce, None, 'auto-paired (bag distance 0), check'),
    'findtemp': (0xa7ac, None, 'auto-paired (bag distance 0), check'),
    'fixname': (0xa0c6, None, 'auto-paired (bag distance 0), check'),
    'freeparm': (0x6792, None, 'auto-paired (bag distance 1), check'),
    'fwalk': (0x87ec, None, 'auto-paired (bag distance 0), check'),
    'genargs': (0x59f4, None, 'auto-paired (bag distance 0), check'),
    'gencall': (0x4534, None, 'auto-paired (bag distance 0), check'),
    'geninc': (0x2e4c, None, 'auto-paired (bag distance 0), check'),
    'genscall': (0x4520, None, 'auto-paired (bag distance 0), check'),
    'getlab': (0x59d4, None, 'auto-paired (bag distance 0), check'),
    'getlr': (0x6636, None, 'auto-paired (bag distance 2), check'),
    'hdrout': (0x8c0c, None, 'auto-paired (bag distance 0), check'),
    'hopcode': (0x2de0, None, 'auto-paired (bag distance 0), check'),
    'init': (0x019e, None, 'auto-paired (bag distance 0), check'),
    'insput': (0x269a, None, 'auto-paired (bag distance 0), check'),
    'ispow2': (0x8af6, None, 'auto-paired (bag distance 5), check'),
    'lastchance': (0x46ae, None, 'auto-paired (bag distance 0), check'),
    'lineout': (0x4088, None, 'auto-paired (bag distance 0), check'),
    'ltypeok': (0x3e1c, None, 'auto-paired (bag distance 0), check'),
    'makeblock': (0xaece, None, 'auto-paired (bag distance 0), check'),
    'mapn': (0x2038, None, 'auto-paired (bag distance 0), check'),
    'mapsym': (0x206a, None, 'auto-paired (bag distance 0), check'),
    'memname': (0xa734, None, 'auto-paired (bag distance 0), check'),
    'mkhash': (0x2406, None, 'auto-paired (bag distance 3), check'),
    'mkrall': (0x51c0, None, 'auto-paired (bag distance 0), check'),
    'ncopy': (0x8a02, None, 'auto-paired (bag distance 0), check'),
    'niceuty': (0x547a, None, 'auto-paired (bag distance 0), check'),
    'nlcheck': (0x244a, None, 'auto-paired (bag distance 0), check'),
    'notoff': (0x49c0, None, 'auto-paired (bag distance 0), check'),
    'offstar': (0x53fa, None, 'auto-paired (bag distance 0), check'),
    'oregfix': (0xa12a, None, 'auto-paired (bag distance 0), check'),
    'oregput': (0x409a, None, 'auto-paired (bag distance 0), check'),
    'pairmate': (0x73f0, None, 'auto-paired (bag distance 0), check'),
    'plainicon': (0x8b44, None, 'auto-paired (bag distance 0), check'),
    'prcook': (0x0af6, None, 'auto-paired (bag distance 0), check'),
    'psuffix': (0x2780, None, 'auto-paired (bag distance 0), check'),
    'rallkind': (0x89a2, None, 'auto-paired (bag distance 0), check'),
    'rbusy': (0x66bc, None, 'auto-paired (bag distance 0), check'),
    'rcount': (0x15ba, None, 'auto-paired (bag distance 0), check'),
    'rcount_reset': (0x778e, None, 'auto-paired (bag distance 0), check'),
    'rdin': (0x3fc4, None, 'auto-paired (bag distance 0), check'),
    'rdin2': (0x26ae, None, 'auto-paired (bag distance 0), check'),
    'readmems': (0x1d0e, None, 'auto-paired (bag distance 0), check'),
    'readsyms': (0x1b64, None, 'auto-paired (bag distance 5), check'),
    'readtab': (0x1c30, None, 'auto-paired (bag distance 0), check'),
    'regforsym': (0xa03a, None, 'auto-paired (bag distance 0), check'),
    'regreset': (0xa252, None, 'auto-paired (bag distance 0), check'),
    'regtabinit': (0x910e, None, 'auto-paired (bag distance 0), check'),
    'rfree': (0x6f76, None, 'auto-paired (bag distance 0), check'),
    'rfreeall': (0x9696, None, 'auto-paired (bag distance 0), check'),
    'rfreenode': (0x6efa, None, 'auto-paired (bag distance 0), check'),
    'rmove': (0x3cf2, None, 'auto-paired (bag distance 4), check'),
    'rsbuild': (0x7458, None, 'auto-paired (bag distance 0), check'),
    'rsdump': (0x750c, None, 'auto-paired (bag distance 0), check'),
    'rsmatch': (0x83ec, None, 'auto-paired (bag distance 0), check'),
    'rspush': (0x81c4, None, 'auto-paired (bag distance 0), check'),
    'rsrelall': (0x84b6, None, 'auto-paired (bag distance 0), check'),
    'rsrelease': (0x847a, None, 'auto-paired (bag distance 0), check'),
    'setasg': (0x56c6, None, 'auto-paired (bag distance 0), check'),
    'setasop': (0x57b0, None, 'auto-paired (bag distance 1), check'),
    'setrew': (0x60e2, None, 'auto-paired (bag distance 3), check'),
    'shincr': (0x3ed6, None, 'auto-paired (bag distance 0), check'),
    'shtemp': (0x3e8e, None, 'auto-paired (bag distance 4), check'),
    'shumul': (0x3f4e, None, 'auto-paired (bag distance 0), check'),
    'stoarg': (0x126a, None, 'auto-paired (bag distance 0), check'),
    'stoasg': (0x487e, None, 'auto-paired (bag distance 0), check'),
    'store': (0x1084, None, 'auto-paired (bag distance 0), check'),
    'structretcode': (0x8f7e, None, 'auto-paired (bag distance 0), check'),
    'symreg': (0x9fdc, None, 'auto-paired (bag distance 1), check'),
    'symuse': (0x964c, None, 'auto-paired (bag distance 0), check'),
    'szty': (0x88c4, None, 'auto-paired (bag distance 0), check'),
    'tabclear': (0x9152, None, 'auto-paired (bag distance 0), check'),
    'tabfind': (0x9116, None, 'auto-paired (bag distance 0), check'),
    'tagfind': (0x1f82, None, 'auto-paired (bag distance 0), check'),
    'talloc': (0x869c, None, 'auto-paired (bag distance 0), check'),
    'tbytes': (0x8970, None, 'auto-paired (bag distance 0), check'),
    'tfree': (0x873c, None, 'auto-paired (bag distance 0), check'),
    'tinit': (0x866c, None, 'auto-paired (bag distance 0), check'),
    'tmpdump': (0x93f4, None, 'auto-paired (bag distance 0), check'),
    'tsuffix': (0x2796, None, 'auto-paired (bag distance 0), check'),
    'ttype': (0x5ff6, None, 'auto-paired (bag distance 0), check'),
    'uerror': (0x8570, None, 'auto-paired (bag distance 0), check'),
    'upput': (0x40d4, None, 'auto-paired (bag distance 1), check'),
    'werror': (0x862e, None, 'auto-paired (bag distance 0), check'),
}


def n3(raw):
    m = raw.split()[0]
    if m in ('dec', 'inc', 'add', 'sub') and re.match(r'\w+\s+r15,', raw):
        return None
    if m == 'ld' and re.match(r'ld\s+r\d+,r\d+$', raw):
        return None
    if m == 'ldl' and re.match(r'ldl\s+rr\d+,rr\d+$', raw):
        return None
    s = re.sub(r'L0t_[0-9a-f]+', 'L', raw)
    s = re.sub(r'%[0-9A-F]{4}\(r15\)', 'F', s)
    s = re.sub(r'@r15', 'F', s)
    s = re.sub(r'^(call|calr)\s+.*', r'\1 T', s)
    s = re.sub(r'#%([0-9A-F]{4})', lambda x: '#%' + x.group(1) if int(x.group(1), 16) < 0x100 else '#X', s)
    s = re.sub(r'(?<!#)%([0-9A-F]{4})(?!\()', 'X', s)
    s = re.sub(r'%([0-9A-F]{4})\((r\d+)\)', lambda x: 'X(rS)' if int(x.group(1), 16) >= 0x100 else x.group(0), s)
    s = re.sub(r'^(ld\s+r[567]),#%([0-9A-F]{4})', lambda x: x.group(1) + ',#X' if int(x.group(2), 16) >= 0x20 else x.group(0), s)
    s = re.sub(r'\b(r[lh]?|rr|rq)\d+\b', r'\1S', s)
    return s


def n5(seq):
    o = []
    for x in seq:
        if re.match(r'^ldl?\t(F,rrS|rrS,F|F,rS|rS,F)$', x):
            continue
        if x in ('neg\trS', 'add\trS,rS', 'extsb\trS', 'subb\trhS,rhS'):
            continue
        x = x.replace('calr', 'call')
        x = re.sub(r'^(jr|jp)\t[a-z]+,(L|X)$', r'\1 \2', x)
        if x in ('test\trS', 'testl\trrS', 'cp\trS,#%0000', 'cp\tF,#%0000', 'test\tF'):
            continue
        if re.match(r'^ld\trS,(X|%00[0-9A-F]{2})\(rS\)$', x) and len(o) >= 2 and o[-1].startswith('jr') \
                and o[-2].startswith('cpir'):
            continue
        if x in ('sub\trS,rS', 'ldk\trS,#0'):
            x = 'ZERO'
        if x.split('\t')[0] in ('sdal', 'sdll', 'sda', 'sdl', 'sla', 'sll', 'sra', 'srl', 'slal', 'sral', 'slll',
                                'srll', 'sdab', 'sdlb'):
            x = 'SHIFT'
        x = re.sub(r'^ld\trS,#.*', 'ldimm', x)
        o.append(x)
    return o


def disasm(path):
    out = subprocess.run([sys.executable, os.path.join(TOOLS, 'sdis.py'), '-l', path],
                         capture_output=True, text=True, check=True).stdout
    rows = []
    for l in out.splitlines():
        if l.startswith('// segment') and 'data,' in l:
            break
        m = re.match(r'^([0-9a-f]{4}) [0-9a-f ]+?\t(.*)$', l)
        if m:
            a = int(m.group(1), 16)
            if a < LIM:
                rows.append((a, m.group(2).strip()))
    return rows


def body_to_ret(rows, start):
    out = []
    on = False
    for a, raw in rows:
        if a == start:
            on = True
        if on:
            out.append(raw)
            if raw == 'ret':
                break
    return out


def bag(rows_):
    return collections.Counter(n5([x for x in map(n3, rows_) if x]))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--mine', default='codgen', help='our binary (codgen or scodgen)')
    ap.add_argument('--orig', help='the original binary (default: $WEGA/lib/<name>)')
    ap.add_argument('--out', default=os.path.join(HERE, 'FUNCTIONS.md'))
    ap.add_argument('--name', default='codgen', help='codgen or scodgen')
    a = ap.parse_args()
    orig = a.orig or os.path.join(os.environ.get('WEGA', ''), 'lib', a.name)
    s = SOut(open(a.mine, 'rb').read())
    syms = sorted({(x['value'], x['name']) for x in s.symbols
                   if x['segt'] == 0 and x['type'] == 34 and x['value'] < LIM})
    starts = [v for v, n in syms]
    mine_rows = disasm(a.mine)
    orig_rows = disasm(orig)
    omap = {addr: raw for addr, raw in orig_rows}
    lines = []
    lines.append('# %s: functions and their places in the binaries' % a.name)
    lines.append('')
    lines.append('Generated by `funcs.py` (do not edit by hand; change the table `ORIG` in `funcs.py`).')
    lines.append('')
    lines.append('- Our binary: `%s` (%d functions from the symbol table; start and end are exact).' % (a.mine, len(syms)))
    lines.append('- Original: `%s` (stripped; its starts are in `ORIG` in `funcs.py`).' % os.path.basename(orig))
    lines.append('- Comparison: instruction multiset from the start up to the first `ret`, old-cc effects masked '
                 '(the normalisation of `host/tools/cgcmp.py`). `gleich` = same multiset; the notes give the old-cc '
                 'classes (D1-D14 of `src/TODO.md`) or the open point.')
    lines.append('')
    lines.append('| Function | Our start | Our end | Original start | Comparison | Note |')
    lines.append('|---|---|---|---|---|---|')
    nsame = 0
    ncmp = 0
    for i, (st, name) in enumerate(syms):
        end = starts[i + 1] if i + 1 < len(starts) else LIM
        short = name.lstrip('_')
        o_start, verdict, note = ORIG.get(short, (None, None, ''))
        if verdict == 'crt0':
            cmp_txt = 'not codgen code'
            o_txt = '-'
        elif o_start is None:
            cmp_txt = 'not paired'
            o_txt = '-'
        else:
            o_txt = '0x%04x' % o_start
            mine_b = bag([r for r in body_to_ret(mine_rows, st)])
            orig_b = bag(body_to_ret(orig_rows, o_start))
            if verdict is not None:
                cmp_txt = verdict
            else:
                d = sum(((mine_b - orig_b) + (orig_b - mine_b)).values())
                ncmp += 1
                if d == 0:
                    cmp_txt = 'identical'
                    nsame += 1
                else:
                    cmp_txt = 'differs (%d)' % d
        lines.append('| %s | 0x%04x | 0x%04x | %s | %s | %s |' % (short, st, end, o_txt, cmp_txt, note))
    lines.append('')
    lines.append('Summary: %d of %d paired functions identical up to the first `ret`.' % (nsame, ncmp))
    lines.append('')
    with open(a.out, 'w') as f:
        f.write('\n'.join(lines) + '\n')
    print('wrote', a.out, 'identical', nsame, 'of', ncmp)


if __name__ == '__main__':
    main()
