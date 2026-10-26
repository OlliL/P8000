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
import bisect
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

# static functions of our build (no symbol in the binary), named by their source (checked by the callers and the body)
# static functions of our build, by their caller: the names of the static functions that a function calls, in the
# order of their first call (identified from the source: the call sequence and the body; see FUNCTIONS.md).
# None = not identified. The address is found from the caller, so the table survives rebuilds.
CALLBACKS = {0x7592: 'delstrip', 0x88d8: 'free1'}   # fwalk callbacks (reader3.c delstrip, comm3.c free1), no call instruction

STATIC_NAMES = {
    'allocreg': ['rcookie'],                                              # allo3.c
    'genargs': ['argscan', 'argstrip', 'argsort', 'argplace'],            # order3.c
    'argstrip': ['fixstarg'],                                             # order3.c
    'nxwork': ['rnassign', 'rnnullw', 'rnnulll', 'rnnullp', 'rnnullall', 'rnnullcall'],  # regfile3.c, call order
    'rnnullw': ['rnnull'],                                                # regfile3.c
    'expand': ['prlong'],                                                 # out3.c
    'gswitch': ['newreg'],                                                # lcl3.c
}

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
    'gswitch':   (0xa7e0, None, 'the newreg blocks are written out inline (non-SEG path, lcl3.c); the rest: a memory compare `cp @r2,#0x7c` (l->op) and a tail jp instead of jr (layout)'),
    'eread':     (0x2480, None, 'd=22 after ty as register variable: getchar byte load and masking (D8), lval store form, D7'),
    'main':      (0x23c,  None, 'not compared yet'),
    'zzzcode':   (0x2edc, None, 'd=37 after char c and register locals: switch dispatch (D6), D3, D7; the old name table was data'),
    'start':     (None,   'crt0', 'entry of crt0, not codgen code'),
    'rcookie':   (0x89a2, None, 'identical after the null test `if (p && ...)` was written (allo3.c fix)'),
    'fixstarg':  (0x5c1c, None, 'identical up to the ret (order3.c; identified by the body: STASG, resc[0].type = 8, type 0x18)'),
    'rnnullw':   (0x988c, None, 'identical up to the ret (regfile3.c rnnullw)'),
    'rnnulll':   (0x986a, None, 'identical up to the ret (regfile3.c rnnulll)'),
    'rnnullp':   (0x97cc, None, 'identical up to the ret (regfile3.c rnnullp)'),
    'rnnullcall': (0x9816, None, 'identical up to the ret (regfile3.c rnnullcall)'),
    'argscan':   (0x5a4c, None, 'd=14 after switch(argsize) as if chain (order3.c); left: the argsize value in a stack slot (original: register), jump layout (D7)'),
    'argplace':  (0x5d2c, None, 'D5: the local array cnt is addressed with lda (original: direct); 13 instructions, else identical'),
    'argsort':   (0x5bcc, None, 'identical up to the ret after argl became a tagged struct and the loops use pointers (order3.c)'),
    'rnnullall': (0x983c, None, 'identical up to the ret after register int i (regfile3.c)'),
    'static_8696': (None, 'own', 'clears the 400-byte work area before printf (cgprintf wrapper, comm3.c); the original calls printf directly'),
    'delstrip': (0x9d2, None, 'delstrip (reader3.c), the callback of fwalk in delay2; the struct copy is inline in the original'),
    'newreg':    (None,   'inline', 'the non-SEG gswitch has the blocks inline (as the original); newreg stays only for the SEG path (scodgen)'),
    # automatic pairings from the function-boundary work (not yet checked one by one)
    'allocinit': (0x8bea, None, 'auto, verified: identical'),
    'argsize': (0x88f6, None, 'auto, verified: identical'),
    'bcon': (0xb0a0, None, 'auto-paired, identical, two ret in our function: check'),
    'branch': (0x89e8, None, 'comm3.c: printf("\\tjpr\\tL%d\\n", n); same calls as the original'),
    'bsize': (0x8878, None, 'auto, verified: identical'),
    'canon': (0x1884, None, 'auto, verified: identical'),
    'canon2': (0x46fe, None, 'auto, verified: identical'),
    'cbgen': (None, None, 'auto-paired orig 0x460e is indope; wrong pairing, removed'),
    'cgprintf': (None, None, 'our wrapper of printf: no counterpart in the original'),
    'codgen': (0x0aaa, None, 'auto, verified: identical'),
    'constore': (0x11da, None, 'auto, verified: identical'),
    'convelim': (0xba7c, None, 'verified: D8 byte compare form'),
    'dopeinit': (0x8844, None, 'auto, verified: identical'),
    'efcode': (0x28f6, None, 'NOT verified: saved register set and an addl difference: check'),
    'endline': (0x8c22, None, 'auto, verified: identical'),
    'endrun': (0x4868, None, 'auto, verified: identical'),
    'errexit': (0x89ce, None, 'auto, verified: identical'),
    'findtemp': (0xa7ac, None, 'auto, verified: identical'),
    'fixname': (0xa0c6, None, 'auto, verified: identical'),
    'freeparm': (0x6792, None, 'verified: D2 scratch register (ld rS,X)'),
    'fwalk': (0x87ec, None, 'auto, verified: identical'),
    'genargs': (0x59f4, None, 'auto, verified: identical'),
    'gencall': (0x4534, None, 'auto, verified: identical'),
    'geninc': (0x2e4c, None, 'auto, verified: identical'),
    'genscall': (0x4520, None, 'auto, verified: identical'),
    'getlab': (0x59d4, None, 'auto, verified: identical'),
    'getlr': (0x6636, None, 'verified: D8 byte load (ldb rl,rl)'),
    'hdrout': (0x8c0c, None, 'auto, verified: identical'),
    'hopcode': (0x2de0, None, 'auto, verified: identical'),
    'init': (0x019e, None, 'auto, verified: identical'),
    'prlong': (0x3fc4, None, 'out3.c, the printf("%ld") of the M and N templates; the original calls it as a function'),
    'free1': (0x8758, None, 'comm3.c: fwalk callback of tfree (if p == 0 cerror("freeing blank tree!") else p->op = FREE)'),
    'insput': (0x4088, None, 'called by expand (case I); the original spills its argument and calls cerror("insput")'),
    'ispow2': (0x8af6, None, 'verified: old cc (clr/ZERO, inc form)'),
    'lastchance': (0x46ae, None, 'auto, verified: identical'),
    'lineout': (0x2880, None, 'efcode3.c: printf("\\t.line\\t%d\\t%s\\t\\n", ...); called from main (case .) with ifline and ifname'),
    'ltypeok': (0x3e1c, None, 'auto, verified: identical'),
    'makeblock': (0xaece, None, 'D4: the original homes all four parameters (ldm of r4-r7), the rebuild one'),
    'mapn': (0x2038, None, 'auto, verified: identical'),
    'mapsym': (0x206a, None, 'auto, verified: identical'),
    'memname': (0xa734, None, 'auto, verified: identical'),
    'mkhash': (0x2406, None, 'verified: known reload quirk (mkhash, D3)'),
    'mkrall': (0x51c0, None, 'identical after the register declarations of p and r (order3.c)'),
    'ncopy': (0x8a02, None, 'auto, verified: identical'),
    'niceuty': (0x547a, None, 'auto, verified: identical'),
    'nlcheck': (0x244a, None, 'auto, verified: identical'),
    'notoff': (0x49c0, None, 'differs by one ldm: the original homes only off (t is unused); register r is a register variable (fixed)'),
    'offstar': (0x53fa, None, 'auto, verified: identical'),
    'oregfix': (0xa12a, None, 'auto, verified: identical'),
    'oregput': (0x409a, None, 'auto, verified: identical'),
    'pairmate': (0x73f0, None, 'auto, verified: identical'),
    'plainicon': (0x8b44, None, 'auto, verified: identical'),
    'prcook': (0x0af6, None, 'auto, verified: identical'),
    'psuffix': (0x2780, None, 'auto, verified: identical'),
    'rallkind': (0x89a2, None, 'auto, verified: identical'),
    'rbusy': (0x66bc, None, 'auto, verified: identical'),
    'rcount': (0x15ba, None, 'auto, verified: identical'),
    'rcount_reset': (0x778e, None, 'auto, verified: identical'),
    'rdin': (0x269a, None, 'rdin(base) = rdin2(base, stdin): same calls and spill, the original keeps a 2-byte frame (ours 6)'),
    'rdin2': (0x26ae, None, 'auto, verified: identical'),
    'readmems': (0x1d0e, None, 'auto, verified: identical'),
    'readsyms': (None, None, 'auto-paired distance 7 (unclear), removed'),
    'readtab': (0x1c30, None, 'auto, verified: identical'),
    'regforsym': (0xa03a, None, 'auto, verified: identical'),
    'regreset': (0xa252, None, 'auto, verified: identical'),
    'regtabinit': (0x910e, None, 'auto, verified: identical'),
    'rfree': (0x6f76, None, 'auto, verified: identical'),
    'rfreeall': (0x9696, None, 'auto, verified: identical'),
    'rfreenode': (0x6efa, None, 'auto, verified: identical'),
    'rmove': (0x3cf2, None, 'verified: D7 layout'),
    'rsbuild': (0x7458, None, 'auto, verified: identical'),
    'rsdump': (0x750c, None, 'auto, verified: identical'),
    'rsmatch': (0x83ec, None, 'auto, verified: identical'),
    'rspush': (0x81c4, None, 'auto, verified: identical'),
    'rsrelall': (0x84b6, None, 'auto, verified: identical'),
    'rsrelease': (0x847a, None, 'auto, verified: identical'),
    'setasg': (0x56c6, None, 'auto, verified: identical'),
    'setasop': (0x57b0, None, 'verified: one load of a field (ld rS,%000E(rS)) more in our build: check'),
    'setrew': (0x60e2, None, 'verified: D3 memory operand, D7 layout'),
    'shincr': (0x3ed6, None, 'auto, verified: identical'),
    'shtemp': (0x3e8e, None, 'verified: D7 layout, tcc form'),
    'shumul': (0x3f4e, None, 'auto, verified: identical'),
    'stoarg': (0x126a, None, 'auto, verified: identical'),
    'stoasg': (0x487e, None, 'auto, verified: identical'),
    'store': (0x1084, None, 'auto, verified: identical'),
    'structretcode': (0x8f7e, None, 'auto, verified: identical'),
    'symreg': (0x9fdc, None, 'D2: a leaf function; the original saves three registers (r10-r12), the shipped cc keeps n in a scratch register'),
    'symuse': (0x964c, None, 'auto, verified: identical'),
    'szty': (0x88c4, None, 'auto, verified: identical'),
    'tabclear': (0x9152, None, 'auto, verified: identical'),
    'tabfind': (0x9116, None, 'auto, verified: identical'),
    'tagfind': (0x1f82, None, 'auto, verified: identical'),
    'talloc': (0x869c, None, 'auto, verified: identical'),
    'tbytes': (0x8970, None, 'auto, verified: identical'),
    'tfree': (0x873c, None, 'auto, verified: identical'),
    'tinit': (0x866c, None, 'auto, verified: identical'),
    'tmpdump': (0x93f4, None, 'auto, verified: identical'),
    'tsuffix': (0x2796, None, 'auto, verified: identical'),
    'ttype': (0x5ff6, None, 'auto, verified: identical'),
    'uerror': (0x8570, None, 'auto, verified: identical'),
    'upput': (0x40d4, None, 'verified: call target masked (printf wrapper)'),
    'werror': (0x862e, None, 'auto, verified: identical'),
}


OWN_OBJ = re.compile(r'^(crt0|allo3|local3|match3|order3|out3|regallo3|regfile3|tmp3|zzz3|reader3|gen3|optim3|'
                     r'comm3|rdr3|main3|efcode3|lcl3|tr3|regs3|table3_c|ops_codg|hop_codg)')


def own_limit(s):
    """End of our program part: the first object file after the codgen objects (the C library follows)."""
    marks = sorted((x['value'], x['name']) for x in s.symbols if x['type'] == 31 and x['segt'] == 0)
    for v, n in marks:
        if not OWN_OBJ.match(n):
            return v
    return LIM



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


def disasm(path, lim=LIM):
    out = subprocess.run([sys.executable, os.path.join(TOOLS, 'sdis.py'), '-l', path],
                         capture_output=True, text=True, check=True).stdout
    rows = []
    for l in out.splitlines():
        if l.startswith('// segment') and 'data,' in l:
            break
        m = re.match(r'^([0-9a-f]{4}) [0-9a-f ]+?\t(.*)$', l)
        if m:
            a = int(m.group(1), 16)
            if a < lim:
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


def strict_bag(rows_):
    """The same comparison but register-to-register moves and the frame arithmetic are kept (n3 drops them)."""
    out = []
    for r in rows_:
        if re.match(r'(dec|inc|sub|add)\s+r15,', r):
            continue
        r = re.sub(r'L0t_[0-9a-f]+', 'L', r)
        r = re.sub(r'%[0-9A-F]{4}\(r15\)', 'F', r)
        r = re.sub(r'@r15', 'F', r)
        r = re.sub(r'^(call|calr)\s+.*', r'\1 T', r)
        r = re.sub(r'\b(r[lh]?|rr|rq)\d+\b', r'\1S', r)
        r = re.sub(r'%[0-9A-F]{4}', 'X', r)
        out.append(r)
    return collections.Counter(out)



def orig_candidates(orig_rows):
    """Possible starts of functions in the original: call targets (strong) and the instruction after a ret (weak);
    a prologue after a ret is strong. A function ends at the next strong start."""
    tg = set()
    for a, raw in orig_rows:
        m = re.match(r'^(call|calr)\s+(?:%([0-9A-F]{4})|L0t_([0-9a-f]+))', raw)
        if m:
            tg.add(int(m.group(2) or m.group(3), 16))
    strong = set(tg)
    weak = set()
    prev = None
    for a, raw in orig_rows:
        if prev == 'ret':
            weak.add(a)
            if re.match(r'(dec|sub)\s+r15,', raw):
                strong.add(a)
        prev = raw
    return sorted(strong), sorted(strong | weak)


def auto_pairs(mine_syms, mine_rows, orig_rows, mine_lim):
    """Greedy one-to-one pairing of our functions with candidate starts of the original (bag distance).
    Returns {our start: (orig start, distance)}. A proposal, not the table: the table in ORIG decides."""
    import bisect
    strong, cand = orig_candidates(orig_rows)
    orig_idx = {a: k for k, (a, _) in enumerate(orig_rows)}
    starts = [v for v, n in mine_syms]
    extents = {}
    for i, st in enumerate(starts):
        end = starts[i + 1] if i + 1 < len(starts) else mine_lim
        extents[st] = [r for a, r in mine_rows if st <= a < end]
    bags = {}
    for c in cand:
        j = bisect.bisect_right(strong, c)
        e = strong[j] if j < len(strong) else LIM
        bags[c] = collections.Counter(n5([x for x in map(n3, [r for a, r in orig_rows if c <= a < e]) if x]))
    scored = []
    for st in starts:
        bm = collections.Counter(n5([x for x in map(n3, extents[st]) if x]))
        size = sum(bm.values()) + 1
        for c in cand:
            bo = bags[c]
            d = sum(((bm - bo) + (bo - bm)).values())
            scored.append((d / (size + sum(bo.values())), d, st, c))
    scored.sort()
    used_m, used_c, out = set(), set(), {}
    for r, d, st, c in scored:
        if st in used_m or c in used_c or r > 0.3:
            continue
        used_m.add(st)
        used_c.add(c)
        out[st] = (c, d)
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--mine', default='codgen', help='our binary (codgen or scodgen)')
    ap.add_argument('--orig', help='the original binary (default: $WEGA/lib/<name>)')
    ap.add_argument('--out', default=os.path.join(HERE, 'FUNCTIONS.md'))
    ap.add_argument('--name', default='codgen', help='codgen or scodgen')
    a = ap.parse_args()
    orig = a.orig or os.path.join(os.environ.get('WEGA', ''), 'lib', a.name)
    s = SOut(open(a.mine, 'rb').read())
    mine_lim = own_limit(s)
    mine_rows = disasm(a.mine, mine_lim)
    code = {addr: raw for addr, raw in mine_rows}
    # symbols of type 34 also name data (tables); a function starts with an instruction, not with .word/.byte
    syms = {(x['value'], x['name'].lstrip('_')) for x in s.symbols
            if x['segt'] == 0 and x['type'] == 34 and x['value'] < mine_lim
            and not code.get(x['value'], '.').startswith('.')}
    # static functions have no symbol in our binary: they show up only as call targets (calr to a label)
    symv = {x['value'] for x in s.symbols if x['segt'] == 0 and x['type'] in (34, 35, 36)}
    named = sorted((x['value'], x['name'].lstrip('_')) for x in s.symbols
                   if x['segt'] == 0 and x['type'] == 34 and x['value'] < mine_lim)
    static_calls = []
    for addr, raw in mine_rows:
        m = re.match(r'^(call|calr)\s+(?:%([0-9A-F]{4})|L0t_([0-9a-f]+))', raw)
        if m:
            t = int(m.group(2) or m.group(3), 16)
            if t < mine_lim and t not in symv and not code.get(t, '.').startswith('.'):
                static_calls.append((addr, t))
    # static functions have no symbol: their name comes from the caller (STATIC_NAMES), resolved in rounds
    resolved = {}
    for _ in range(4):
        starts_now = sorted(named + [(v, n) for v, n in resolved.items()])

        def owner_of(a):
            o = None
            for v, n in starts_now:
                if v <= a:
                    o = n
            return o
        seqs = {}
        for addr, t in static_calls:
            seq = seqs.setdefault(owner_of(addr), [])
            if t not in seq:
                seq.append(t)
        for caller, seq in seqs.items():
            names = STATIC_NAMES.get(caller, [])
            todo = [t for t in seq if t not in resolved]   # the names are given for the unresolved callees in order
            for t, nm in zip(todo, names):
                if nm:
                    resolved[t] = nm
    for addr, t in static_calls:
        pass
    all_static = {t for addr, t in static_calls}
    for t in sorted(all_static):
        syms.add((t, resolved.get(t, 'static_%04x' % t)))
    # static functions that are only passed as a pointer (fwalk callbacks) have no call: named here, in our layout
    for t, nm in CALLBACKS.items():
        syms.add((t, nm))
    syms = sorted(syms)
    starts = [v for v, n in syms]
    orig_rows = disasm(orig)
    auto = auto_pairs(syms, mine_rows, orig_rows, mine_lim)
    strong, cand = orig_candidates(orig_rows)

    # the original start of each of our functions: the table ORIG, else the automatic proposal (marked)
    rows = []
    for i, (st, short) in enumerate(syms):
        end = starts[i + 1] if i + 1 < len(starts) else mine_lim
        o_man, verdict, note = ORIG.get(short, (None, None, ''))
        if o_man is not None or verdict is not None:
            rows.append((short, st, end, o_man, 'table', verdict, note))
        elif st in auto:
            rows.append((short, st, end, auto[st][0], 'auto', None, 'automatic proposal, distance %d, not confirmed' % auto[st][1]))
        else:
            rows.append((short, st, end, None, None, None, ''))
    assigned = {r[3]: r[0] for r in rows if r[3] is not None}

    lines = []
    lines.append('# %s: functions and their places in the binaries' % a.name)
    lines.append('')
    lines.append('Generated by `funcs.py` (do not edit by hand; change the table `ORIG` in `funcs.py`).')
    lines.append('')
    lines.append('- Our binary: `%s`: %d functions from the symbol table (start and end are exact).' % (a.mine, len(syms)))
    lines.append('- Original: `%s` (stripped). Its starts come from the table `ORIG` (`table`) or from the automatic '
                 'pairing (`auto`, not confirmed).' % os.path.basename(orig))
    lines.append('- Comparison: instruction multiset from the start up to the first `ret`, old-cc effects masked '
                 '(normalisation of `host/tools/cgcmp.py`). `identical` = same multiset; `identical*` = only '
                 'register-to-register moves differ; `differs (n)` = n instructions differ.')
    lines.append('')
    lines.append('| Function | Our start | Our end | Original start | Source | Comparison | Note |')
    lines.append('|---|---|---|---|---|---|---|')
    nsame = 0
    ncmp = 0
    for short, st, end, o, src, verdict, note in rows:
        if verdict == 'crt0':
            lines.append('| %s | 0x%04x | 0x%04x | - | table | not codgen code | %s |' % (short, st, end, note))
            continue
        if o is None:
            lines.append('| %s | 0x%04x | 0x%04x | - | %s | %s | %s |' % (short, st, end, src or '-', verdict or 'not paired', note))
            continue
        ob = bag(body_to_ret(orig_rows, o))
        mb = bag(body_to_ret(mine_rows, st))
        d = sum(((mb - ob) + (ob - mb)).values())
        ds = sum(((strict_bag(body_to_ret(mine_rows, st)) - strict_bag(body_to_ret(orig_rows, o)))
                  + (strict_bag(body_to_ret(orig_rows, o)) - strict_bag(body_to_ret(mine_rows, st)))).values())
        if verdict is not None:
            cmp_txt = verdict
        else:
            ncmp += 1
            if d == 0 and ds == 0:
                cmp_txt = 'identical'
            elif d == 0:
                cmp_txt = 'identical*'
            else:
                cmp_txt = 'differs (%d)' % d
            if d == 0:
                nsame += 1
        lines.append('| %s | 0x%04x | 0x%04x | 0x%04x | %s | %s | %s |' % (short, st, end, o, src, cmp_txt, note))
    lines.append('')
    lines.append('Summary: %d of %d functions with a table entry identical up to the first `ret`; %d with an automatic '
                 'proposal only.' % (nsame, ncmp, sum(1 for r in rows if r[4] == 'auto')))
    lines.append('')
    lines.append('## Open: our functions without an original')
    lines.append('')
    for short, st, end, o, src, verdict, note in rows:
        if o is None:
            lines.append('- `%s` at 0x%04x' % (short, st))
    lines.append('')
    lines.append('## Open: original call targets without one of our functions')
    lines.append('')
    lines.append('The nearest function of ours (bag distance over the same extent) is given only as a hint.')
    lines.append('')
    ext = {}
    for i, (st, short) in enumerate(syms):
        end = starts[i + 1] if i + 1 < len(starts) else mine_lim
        ext[short] = bag(body_to_ret(mine_rows, st))
    for c in strong:
        if c >= LIM or c in assigned:
            continue
        j = bisect.bisect_right(strong, c)
        e = strong[j] if j < len(strong) else LIM
        ob = bag([r for a2, r in orig_rows if c <= a2 < e])
        best = min(((sum(((ext[n] - ob) + (ob - ext[n])).values()), n) for n in ext), default=(0, '-'))
        lines.append('- 0x%04x (%d instructions up to the next call target): nearest `%s` (distance %d)'
                     % (c, sum(ob.values()), best[1], best[0]))
    lines.append('')
    with open(a.out, 'w') as f:
        f.write('\n'.join(lines) + '\n')
    print('wrote', a.out, 'identical', nsame, 'of', ncmp, 'auto-only', sum(1 for r in rows if r[4] == 'auto'),
          'open ours', sum(1 for r in rows if r[3] is None), 'open orig', sum(1 for c in strong if c < LIM and c not in assigned))


if __name__ == '__main__':
    main()
