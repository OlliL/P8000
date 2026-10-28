#!/usr/bin/env python3
"""inventory.py [BINARY] - function starts and call graph of the original sld (segmented, stripped).

Recursive descent over the code segment (segment 0) from the entry points: the start code (the `jr` at 0 jumps
over the copyright string), every branch target and every call target. A function starts at its entry point
(`call` target, `calr` target, or the start address). The end of a function is the end of the last block that is
reached from its start without entering another function (estimate: the start of the next function).
Output: JSON on stdout (functions, call edges, data ranges), used by the sld findings.
"""
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import z8k  # noqa: E402
from sout import SOut  # noqa: E402

DEFAULT = '/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld'


def target_of(ins, pc):
    """(kind, address) of the target operand of a call or branch.
    A conditional jump has a 'cc' operand before its target, so the target is searched by kind."""
    for o in ins.ops:
        if o.kind == 'rel':
            return ('rel', o.a)
        if o.kind == 'da':
            seg, off, form = o.a
            return ('call', off) if seg == 0 else ('other', (seg, off))
    return (None, None)


def unconditional(ins):
    """A jump without condition code (cc operand) or a return ends the block."""
    if ins.mn == 'ret':
        return True
    if ins.mn in ('jr', 'jp', 'jpr'):
        return not any(o.kind == 'cc' for o in ins.ops)
    return False


def inv_unconditional(ins):
    return unconditional(ins)


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else DEFAULT
    s = SOut(open(path, 'rb').read())
    codesz = s.segs[0]['code']
    code = s.image[0:codesz]
    buf = z8k.Buf(code)
    funcs = {}           # start -> set of called starts (segment 0 only)
    fails = []
    work = [0x18]        # the start code: the jr at 0 jumps over the copyright string to 0x18
    funcs_seen = set()
    while work:
        start = work.pop()
        if start in funcs_seen or not (0 <= start < codesz):
            continue
        funcs_seen.add(start)
        funcs.setdefault(start, set())
        todo = [start]
        visited = set()
        while todo:
            pc = todo.pop()
            while pc not in visited and 0 <= pc < codesz:
                visited.add(pc)
                ins = z8k.decode(buf, pc, 0)
                if ins is None:
                    fails.append(pc)
                    break
                kind, tgt = target_of(ins, pc)
                if ins.mn in ('call', 'calr') and kind in ('call', 'rel') and tgt is not None:
                    funcs[start].add(tgt)
                    if tgt not in funcs_seen:
                        work.append(tgt)
                elif ins.mn in ('jr', 'jp', 'jpr', 'djnz', 'dbjnz') and kind in ('call', 'rel') and tgt is not None:
                    todo.append(tgt)
                if unconditional(ins):
                    break
                pc = ins.end
    # linear sweep: every instruction start, undecodable words as data; a start after ret or an unconditional
    # return is a candidate function start (not proven: the call targets above are the proven ones)
    lin = []
    calltargets = set()
    for a, v in funcs.items():
        calltargets |= v
    pc = 0x18
    cand = []
    prev_end = True
    while pc < codesz:
        ins = z8k.decode(buf, pc, 0)
        if ins is None:
            pc += 2
            prev_end = True
            continue
        kind, tgt = target_of(ins, pc)
        if ins.mn in ('call', 'calr') and kind in ('call', 'rel') and tgt is not None and 0 <= tgt < codesz:
            calltargets.add(tgt)
        if prev_end and pc not in calltargets:
            cand.append(pc)
        prev_end = ins.mn == 'ret'
        lin.append(pc)
        pc = ins.end
    cand = [c for c in cand if c not in calltargets]
    proven_all = sorted(calltargets | {0x18})
    undecodable = 0
    out = {'binary': path, 'codesz': codesz, 'proven_starts': ['0x%04x' % a for a in proven_all],
           'candidate_starts_after_ret_or_jump': ['0x%04x' % a for a in cand],
           'linear_instructions': len(lin),
           'decode_failures': ['0x%04x' % x for x in sorted(set(fails))],
           'functions': [{'start': '0x%04x' % a, 'calls': ['0x%04x' % c for c in sorted(v)]}
                         for a, v in sorted(funcs.items())]}
    json.dump(out, sys.stdout, indent=1)
    print()


if __name__ == '__main__':
    main()
