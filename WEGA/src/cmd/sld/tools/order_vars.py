#!/usr/bin/env python3
"""order_vars.py - write the block of global (bss) definitions of copyname.c in the order of /bin/sld.

The old sld allocates the commons in the order of the global symbol table, which is the order of the first mention in the objects
in link order; the program was built with all commons in one place in the order of their address in the original, segment 2
before segment 1 (the segment with the data gets the end of the commons). The block between "/* the global variables of sld */"
and the next empty line is sorted by (segment 2, 1; lowest referenced address of the variable from var_addr.txt). SPARE are
variables that no function of /bin/sld uses (gaps between the addresses).
Usage: order_vars.py [--check]
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
EXT = os.path.join(HERE, '..', 'copyname.c')
# name: (segment, address); the unused variables fill the gaps of the original
SPARE = {'spare0': (2, 0x0000), 'spare4b8': (2, 0x4b8 + 0xe000), 'spare4c4': (2, 0xe4c4), 'spare4d0': (2, 0xe4cc), 'sparef314': (2, 0xf314)}
SPARE_DEF = {'spare0': 'long spare0;', 'spare4b8': 'int spare4b8;', 'spare4c4': 'long spare4c4;', 'spare4d0': 'int spare4d0;', 'sparef314': 'char sparef314[12];'}
EXTRA = {'cmdbuf': (1, 0x6988), 'page': (2, 0x0004), 'fpage': (2, 0x0410), 'archdr': (2, 0x0438), 'cursym': (2, 0x046a), 'filhdr': (2, 0x0452), 'text': (2, 0x0414), 'reloc': (2, 0x0424)}
HEAD = '/* the global variables of sld */'


def base_addresses():
    addr = dict(EXTRA)
    for l in open(os.path.join(HERE, 'var_addr.txt')):
        refs = re.findall(r'(\d):([0-9a-f]+)\((\d+)\)', l)
        if refs:
            addr[l.split()[0]] = min((int(s), int(a, 16)) for s, a, v in refs)
    return addr


def name_of(line):
    return re.search(r'(\w+)(\[[^;]*)?;$', line).group(1)


def main():
    lines = open(EXT).read().split('\n')
    i0 = lines.index(HEAD) + 1
    i1 = next(i for i in range(i0, len(lines)) if lines[i] == '')
    blk = [l for l in lines[i0:i1] if name_of(l) not in SPARE]

    addr = base_addresses()
    addr.update(SPARE)
    blk += [SPARE_DEF[n] for n in SPARE]

    def key(l):
        s, a = addr.get(name_of(l), (9, 0))
        return (0 if s == 2 else 1 if s == 1 else 2, a)
    blk.sort(key=key)
    lines[i0:i1] = blk
    new = '\n'.join(lines)
    if '--check' in sys.argv:
        print('ordered' if new == open(EXT).read() else 'NOT ordered')
        return
    open(EXT, 'w').write(new)


if __name__ == '__main__':
    main()
