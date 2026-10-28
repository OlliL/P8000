#!/usr/bin/env python3
"""inner_names.py - names of the inner entries of the matched libc modules of /bin/ld.

A module of /bin/ld that is byte-identical with a version in src/lib/oldlibc (oldlibc_match.py) carries the symbols that
the spec of another program gives for the same version: the spec line of a program listed for that version in VERSIONS,
with the same name and the same size. Symbol addresses are taken relative to the start of the module.
Output: address, name, module, source (program and spec line).
"""
import glob
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
sys.path.insert(0, HERE)
import libcver  # noqa: E402
import oldlibc_match as om  # noqa: E402

OLD = os.path.join(HERE, '..', '..', '..', 'lib', 'oldlibc')
PROG = '/mnt/share/cvs/P8000/tmp/wega31_root/bin/ld'
ASM = '/mnt/share/cvs/P8000/tmp/tmp/oldlibc_asm'


def versions():
    """(module, version) -> list of programs, from VERSIONS"""
    out = {}
    for line in open(os.path.join(OLD, 'VERSIONS')):
        if line.startswith('#') or not line.strip():
            continue
        f = line.split()
        out[(f[0], f[1])] = f[2:]
    return out


def spec_lines():
    rows = []
    for path in glob.glob(os.path.join(HERE, '..', '..', '*', '*.spec')):
        prog = os.path.basename(path)[:-5]
        for line in open(path):
            f = line.split('#')[0].split()
            if len(f) < 4 or f[1] != 'old':
                continue
            start, end = int(f[2], 16), int(f[3], 16)
            syms = {}
            for s in f[5:]:
                if '=' in s and not s.startswith('comm:'):
                    n, a = s.split('=')
                    syms[n] = int(a, 16) - start
            rows.append((f[0], end - start, start, syms, prog))
    return rows


def main():
    _, pins, _ = libcver.load_program(PROG)
    texts = [t for p, t in pins]
    vers = versions()
    specs = spec_lines()
    seen = []
    for f in sorted(glob.glob(os.path.join(ASM, '*.o'))):
        name = os.path.basename(f)[:-2]
        mod, ver = name.rsplit('_', 2)[0], name[len(name.rsplit('_', 2)[0]) + 1:]
        seq = om.module_seq(f)
        if len(seq) < 3:
            continue
        hit = None
        for i in range(len(texts) - len(seq) + 1):
            if texts[i:i + len(seq)] == seq:
                hit = i
                break
        if hit is None:
            continue
        addr = pins[hit][0]
        nbytes = pins[hit + len(seq)][0] - addr if hit + len(seq) < len(pins) else None
        progs = vers.get((mod, ver), [])
        cand = [(n, sy, pr) for (n, size, st, sy, pr) in specs if n == mod and pr in progs and (nbytes is None or size == nbytes)]
        if not cand:
            print('%04x %-10s %s: no spec line with a symbol set (programs %s)' % (addr, mod, ver, ' '.join(progs)))
            continue
        _, syms, pr = cand[0]
        seen.append(addr)
        for n, off in sorted(syms.items(), key=lambda kv: kv[1]):
            print('%04x %-14s %-10s %s (from %s)' % (addr + off, n, mod, ver, pr))


if __name__ == '__main__':
    main()
