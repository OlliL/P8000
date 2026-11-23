#!/usr/bin/env python3
"""paramreload.py BINARY [-v]: in each function of a nonsegmented WEGA binary, the reads of a parameter's frame home while its
register still holds the parameter (R: reload) and the reads of the register itself (K: kept).

A function starts behind a ret (or at 0); its entry stores (ld/ldl/ldm of r2..r7 to frame slots in the first
instructions) map registers to slots. Forward must-analysis per instruction: the set of parameter registers that still
hold their parameter (no write, no call on any path) and the slots that still hold it (no store into the slot).
Prints one line per R event (with -v also K) with the predecessors of the instruction and the parameters assigned
on some path before it. Used to find the rule of OLD_D3K2 (host/OLDCC_LOG.md, OLDCC_PARAMREGS.md)."""
import sys, re
import os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'tools'))
import libcver as L

REGW = {}  # register name -> set of word registers it covers
for i in range(16):
    REGW['r%d' % i] = {i}
for i in range(8):
    REGW['rh%d' % i] = {i}
    REGW['rl%d' % i] = {i}
for i in range(0, 16, 2):
    REGW['rr%d' % i] = {i, i + 1}
for i in range(0, 16, 4):
    REGW['rq%d' % i] = {i, i + 1, i + 2, i + 3}

NOWRITE = {'cp', 'cpb', 'cpl', 'test', 'testb', 'testl', 'bit', 'bitb', 'jp', 'jr', 'call', 'calr', 'ret', 'push', 'pushl'}


def regs_of(op):
    op = op.strip()
    return REGW.get(op, set())


def slot_of(op):
    """frame slot (offset, size hint) of an operand, or None"""
    op = op.strip()
    if op == '@r15':
        return 0
    m = re.match(r'%([0-9A-F]{4})\(r15\)$', op)
    if m:
        return int(m.group(1), 16)
    return None


def parse(t):
    t = t.strip()
    p = t.split(None, 1)
    mn = p[0]
    ops = [o.strip() for o in p[1].split(',')] if len(p) > 1 else []
    return mn, ops


def target(ops):
    for o in ops:
        m = re.match(r'%([0-9A-F]{4})$', o)
        if m:
            return int(m.group(1), 16)
    return None


def functions(ins):
    """split at ret"""
    f, cur = [], []
    for p, t in ins:
        cur.append((p, t))
        if parse(t)[0] in ('ret',):
            f.append(cur)
            cur = []
    if cur:
        f.append(cur)
    return f


def analyse(fn, out, prog_name):
    if not fn:
        return
    mn0, ops0 = parse(fn[0][1])
    if mn0 not in ('dec', 'sub') or ops0[:1] != ['r15']:
        return
    # entry stores within the first 4 instructions
    slots = {}  # word reg -> slot offset
    k = 1
    while k < len(fn) and k <= 4:
        mn, ops = parse(fn[k][1])
        if mn == 'ldm' and slot_of(ops[0]) is not None:
            base = slot_of(ops[0])
            r0 = int(ops[1][1:])
            n = int(ops[2][1:])
            for j in range(n):
                if 2 <= r0 + j <= 7:
                    slots[r0 + j] = base + 2 * j
        elif mn == 'ld' and slot_of(ops[0]) is not None and ops[1] in REGW and len(REGW[ops[1]]) == 1:
            r = list(REGW[ops[1]])[0]
            if 2 <= r <= 7:
                slots[r] = slot_of(ops[0])
        elif mn == 'ldl' and slot_of(ops[0]) is not None and ops[1].startswith('rr'):
            r = int(ops[1][2:])
            if 2 <= r <= 6:
                slots[r] = slot_of(ops[0])
                slots[r + 1] = slot_of(ops[0]) + 2
        k += 1
    if not slots:
        return
    entry_end = k
    addr = [p for p, t in fn]
    idx = {p: i for i, p in enumerate(addr)}
    succ = [[] for _ in fn]
    for i, (p, t) in enumerate(fn):
        mn, ops = parse(t)
        tg = target(ops) if mn in ('jr', 'jp', 'djnz', 'dbjnz') else None
        cond = mn in ('jr', 'jp') and len(ops) == 2 or mn in ('djnz', 'dbjnz')
        if tg is not None and tg in idx:
            succ[i].append(idx[tg])
        if mn == 'ret' or (mn in ('jr', 'jp') and not cond):
            continue
        if i + 1 < len(fn):
            succ[i].append(i + 1)
    pred = [[] for _ in fn]
    for i, s in enumerate(succ):
        for j in s:
            pred[j].append(i)
    allp = set(slots)
    # state: (regs intact, slots intact, params assigned on some path)
    INF = None
    inr = [INF] * len(fn)
    ins_ = [INF] * len(fn)
    asg = [set() for _ in fn]
    inr[entry_end - 1] = set(allp)
    ins_[entry_end - 1] = set(allp)
    work = [entry_end - 1]
    outs = {}
    while work:
        i = work.pop()
        R, Sl, A = set(inr[i]), set(ins_[i]), set(asg[i])
        mn, ops = parse(fn[i][1])
        if i >= entry_end:
            if mn in ('call', 'calr', 'sc'):
                R -= {2, 3, 4, 5, 6, 7}
            elif mn not in NOWRITE and ops:
                d = ops[0]
                R -= regs_of(d)
                so = slot_of(d)
                if so is not None:
                    for r, s in slots.items():
                        if s == so or s == so - 1 or (mn.endswith('l') and s == so + 2):
                            Sl.discard(r)
                            A.add(r)
                if mn in ('ldir', 'ldirb', 'lddr', 'cpir', 'cpirb', 'ex', 'exb'):
                    for o in ops:
                        R -= regs_of(o.lstrip('@'))
                if mn in ('mult', 'div', 'multl', 'divl') and d in REGW:
                    pass
        outs[i] = (R, Sl, A)
        for j in succ[i]:
            if inr[j] is INF:
                inr[j], ins_[j], asg[j] = set(R), set(Sl), set(A)
                work.append(j)
            else:
                nr, ns, na = inr[j] & R, ins_[j] & Sl, asg[j] | A
                if (nr, ns, na) != (inr[j], ins_[j], asg[j]):
                    inr[j], ins_[j], asg[j] = nr, ns, na
                    work.append(j)
    for i in range(entry_end, len(fn)):
        if inr[i] is INF:
            continue
        mn, ops = parse(fn[i][1])
        srcs = ops[1:] if (mn not in NOWRITE and ops) else ops
        if mn in ('inc', 'dec', 'add', 'sub', 'and', 'or', 'xor', 'neg', 'com') and ops and slot_of(ops[0]) is not None:
            srcs = ops
        for o in srcs:
            so = slot_of(o)
            if so is not None:
                for r, s in slots.items():
                    if (s == so or (s + 1 == so and mn.endswith('b'))) and r in inr[i] and r in ins_[i]:
                        out.append(('R', prog_name, fn[0][0], fn[i][0], r, len(pred[i]), fn[i][1], ' '.join(map(str, sorted(asg[i]))), [fn[q][1] for q in pred[i]], blockstart(fn, pred, i)))
            for r in regs_of(o.lstrip('@')):
                if r in slots and r in inr[i] and r in ins_[i] and o.lstrip('@') in ('r%d' % r, 'rl%d' % r):
                    out.append(('K', prog_name, fn[0][0], fn[i][0], r, len(pred[i]), fn[i][1], ' '.join(map(str, sorted(asg[i]))), [fn[q][1] for q in pred[i]], blockstart(fn, pred, i)))


def blockstart(fn, pred, i):
    """walk back to the start of the basic block: (index of the block start, its predecessor count)"""
    j = i
    while j > 0 and len(pred[j]) == 1 and pred[j][0] == j - 1 and parse(fn[j - 1][1])[0] not in ('jr', 'jp', 'djnz'):
        j -= 1
    return (i - j, len(pred[j]), [fn[q][1] for q in pred[j]], any(q >= j for q in pred[j]))


def run(path):
    s, prog, raw = L.load_program(path)
    ins = [(p, r) for (p, t), r in zip(prog, raw)]
    out = []
    for fn in functions(ins):
        analyse(fn, out, path.split('/')[-1])
    return out


if __name__ == '__main__':
    for path in sys.argv[1:]:
        if path.startswith('-'):
            continue
        for e in run(path):
            if e[0] == 'R' or '-v' in sys.argv:
                print('%s %-8s f%04x %04x r%d preds=%d asg=[%s]  %s' % (e[0], e[1], e[2], e[3], e[4], e[5], e[7], e[6]))
