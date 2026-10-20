#!/usr/bin/env python3
"""swcmp.py: compare the switch statements of two segmented programs (the original and a rebuilt one).
For every switch that is compiled to a jump table (cpir list + table, or a range table) the case value
-> body is taken, and the body (calls and constants up to the first unconditional branch) is compared.
usage: swcmp.py ORIG NEW ADDRESSES
ORIG, NEW  the original program and the rebuilt one, linked WITHOUT -s (the function names of NEW come from its symbol table)
ADDRESSES  src/cmd/cas/ADDRESSES (the names of the functions of ORIG)
Simplified: a case body ends at the first unconditional branch (one jump is followed when no call was seen), so shared
tails of the other code generator give many false reports (about 150 for cas); read the list, do not trust the count.
Chains of compares (up to 3 cases in the old scc) are included."""
import sys, re, os, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import z8k
from sout import SOut

def load(path):
    s = SOut(open(path, 'rb').read())
    code = sum(g['code'] for g in s.segs if g['segno'] == 4)
    off = sum(g['code'] for g in s.segs)
    data = None
    for g in s.segs:
        if g['segno'] == 5: data = s.image[off:off + g['data']]; break
        off += g['data']
    return s, s.image[:code], data

def funcs_orig(addr_file):
    d = {}
    for l in open(addr_file):
        if l.startswith('# data') or l.startswith('# bss'): continue
        for n, a in re.findall(r'\b([A-Za-z_]\w*) ([0-9a-f]{4})\b', l):
            d[int(a, 16)] = n
    return d

def funcs_new(s):
    return {y['value'] & 0xFFFF: y['name'].lstrip('_') for y in s.symbols if (y['type'] & 0x1F) == 2}

def sweep(text):
    buf = z8k.Buf(text); pc = 0; out = []
    while pc < len(text) - 1:
        ins = z8k.decode(buf, pc, True)
        if ins is None: pc += 2; continue
        out.append(ins); pc = ins.end
    return out

def resolve(text, names, t):
    # a function compiled without -O is entered at its prologue, which jumps to the body
    buf = z8k.Buf(text); pc = t; target = t
    for _ in range(8):
        ins = z8k.decode(buf, pc, True)
        if ins is None: break
        if ins.mn in ('jp', 'jr') and not [o for o in ins.ops if o.kind == 'cc' and o.a != 8]:
            for o in ins.ops:
                if o.kind in ('da', 'rel'): target = o.a if o.kind == 'rel' else o.a[1]
            break
        pc = ins.end
    r = None
    for f in sorted(names):
        if f <= target: r = f
        else: break
    return names.get(r, 'LIB') if r is not None and target < 0x9694 + 0 else 'LIB'

def body(text, names, start, limit=120):
    buf = z8k.Buf(text); pc = start; fp = []; seen = {start}; hops = 0
    for _ in range(limit):
        ins = z8k.decode(buf, pc, True)
        if ins is None: fp.append(('bad',)); break
        mn = ins.mn
        if mn in ('call', 'calr'):
            tgt = None
            for o in ins.ops:
                if o.kind == 'da': tgt = o.a[1]
                if o.kind == 'rel': tgt = o.a
            fp.append(('call', resolve(text, names, tgt)))
        else:
            for o in ins.ops:
                if ins.ops[0].kind == 'reg' and ins.ops[0].b == 15: continue      # frame size
                if o.kind == 'imm' and mn not in ('sub', 'add') or (o.kind == 'imm' and o.b != 32):
                    if o.b == 32 and (o.a >> 24) == 0x85: continue      # address of data
                    if mn == 'ldl' and o.b == 32: continue
                    if o.a != 0: fp.append(('imm', o.a & 0xFFFFFFFF if o.b == 32 else o.a))
        cond = [o for o in ins.ops if o.kind == 'cc']
        if mn in ('jr', 'jp') and (not cond or cond[0].a == 8):
            t = [o.a if o.kind == 'rel' else o.a[1] for o in ins.ops if o.kind in ('rel', 'da')]
            if hops < 1 and not [x for x in fp if x[0] == 'call'] and t and t[0] not in seen:
                hops += 1; pc = t[0]; seen.add(pc); continue      # shared code of the other generator
            break
        if mn == 'ret' and (not cond or cond[0].a == 8): break
        pc = ins.end
    return tuple(fp)

def sites(path, names):
    s, text, data = load(path)
    ins = sweep(text)
    fstarts = sorted(names)
    def fn(a):
        r = None
        for f in fstarts:
            if f <= a: r = f
            else: break
        return names.get(r, '?')
    res = collections.defaultdict(list)
    for i, x in enumerate(ins):
        if x.mn != 'jp' or not x.ops or x.ops[0].kind != 'ir': continue
        prev = ins[i - 1]
        if prev.mn != 'ldl' or len(prev.ops) < 2 or prev.ops[1].kind != 'x': continue
        seg, tbl, form = prev.ops[1].a
        if seg != 5: continue
        idx = prev.ops[1].b
        if i < 6: continue
        shl = ins[i - 2]
        entries = None
        # type A: cpir list
        j = i - 2
        window = ins[max(0, i - 14):i]
        cp = [w for w in window if w.mn == 'cpir']
        if cp:
            k = ins.index(cp[0])
            n = None; vaddr = None
            for w in ins[max(0, k - 6):k]:
                if w.mn == 'ld' and w.ops[0].kind == 'reg' and w.ops[1].kind == 'imm' and w.ops[0].b == cp[0].ops[2].b: n = w.ops[1].a
                if w.mn == 'ldl' and w.ops[1].kind == 'imm' and w.ops[0].b == cp[0].ops[1].a: vaddr = w.ops[1].a & 0xFFFF
            if n is None or vaddr is None: print('?? cpir without count/list at', hex(x.pc)); continue
            vals = [int.from_bytes(data[vaddr + 2 * q:vaddr + 2 * q + 2], 'big') for q in range(n)]
            if any(w.mn == 'neg' for w in window):       # the other code generator: table indexed downwards
                entries = {vals[q]: int.from_bytes(data[tbl - 4 * (n - 1 - q) + 2:tbl - 4 * (n - 1 - q) + 4], 'big') for q in range(n)}
            else:
                entries = {vals[q]: int.from_bytes(data[tbl + 4 * q + 2:tbl + 4 * q + 4], 'big') for q in range(n)}
            # a case list value is a 16-bit pattern; the default follows jr ne after the cpir
            dflt = None
            for w in ins[k + 1:k + 3]:
                if w.mn == 'jr':
                    for o in w.ops:
                        if o.kind == 'rel': dflt = o.a
        else:
            lo = 0; n = None
            regs = {idx}
            for q in range(len(window) - 1):
                if window[q].mn == 'cp' and window[q].ops[1].kind == 'imm' and window[q + 1].mn == 'jr' and window[q + 1].ops[0].kind == 'cc' and window[q + 1].ops[0].a == 11:
                    n = window[q].ops[1].a + 1; regs.add(window[q].ops[0].b)
            for w in window:
                if not (w.ops and w.ops[0].kind == 'reg' and w.ops[0].b in regs): continue
                if w.mn == 'sub' and w.ops[1].kind == 'imm': lo = w.ops[1].a
                if w.mn == 'dec' and w.ops[1].kind in ('imm', 'cnt'): lo = w.ops[1].a
                if w.mn == 'inc' and w.ops[1].kind in ('imm', 'cnt'): lo = -w.ops[1].a
            if n is None: print('?? table switch without range at', hex(x.pc)); continue
            entries = {lo + q: int.from_bytes(data[tbl + 4 * q + 2:tbl + 4 * q + 4], 'big') for q in range(n)}
            dflt = None
        res[fn(x.pc)].append(entries)
    # chains of compares: cp reg,#v ; jr eq,T   (the last one may be jr ne)
    k = 0
    while k < len(ins) - 1:
        run = {}
        j = k
        while j < len(ins) - 1:
            a, b = ins[j], ins[j + 1]
            if a.mn in ('cp', 'cpb') and len(a.ops) == 2 and a.ops[1].kind == 'imm' and b.mn == 'jr' and b.ops and b.ops[0].kind == 'cc' and b.ops[0].a in (6, 14):
                tgt = [o.a for o in b.ops if o.kind == 'rel'][0]
                run[a.ops[1].a & (0xFF if a.mn == 'cpb' else 0xFFFF)] = tgt if b.ops[0].a == 6 else b.end
                j += 2
            else:
                break
        if len(run) >= 2:
            res[fn(ins[k].pc)].append(run)
        k = max(j, k + 1)
    return res, text

def main():
    orig, new, addr = sys.argv[1:4]
    so, _, _ = load(orig); sn, _, _ = load(new)
    no = funcs_orig(addr); nn = funcs_new(sn)
    ro, to = sites(orig, no); rn, tn = sites(new, nn)
    # the names of the callees
    bad = 0
    names_ok = set(no.values())
    for f in sorted(ro):
        if f not in rn: print('function', f, 'has switch tables in the original only'); bad += 1; continue
        for k, a in enumerate(ro[f]):
            same_o = [x for x in ro[f] if set(x) == set(a)]
            same_n = [b for b in rn[f] if set(b) == set(a)]
            cand = [same_n[same_o.index(a)]] if len(same_n) > same_o.index(a) else same_n
            if not cand:
                best = max(rn[f], key=lambda b: len(set(a) & set(b)), default={})
                print(f, 'switch', k, 'values', sorted(a)[:10], '... not found in the new program; nearest differs by', sorted(set(a) ^ set(best))[:10]); bad += 1; continue
            b = cand[0]
            for v in sorted(a):
                fo = body(to, no, a[v]); fb = body(tn, nn, b[v])
                # the other generator may duplicate the exit code; only the calls and constants count
                if fo != fb:
                    bad += 1; print(f, 'switch', k, 'case', v, '\n    orig', fo, '\n    new ', fb)
    for f in sorted(rn):
        if f in ro and f in no.values():
            for b in rn[f]:
                if not [a for a in ro[f] if set(a) == set(b)]: print(f, 'switch with values', sorted(b)[:10], 'in the new program only')
    print('functions with table switches (orig):', len(ro), ' cases checked:', sum(len(e) for l in ro.values() for e in l), ' differences:', bad)
main()
