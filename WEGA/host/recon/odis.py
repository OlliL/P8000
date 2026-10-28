#!/usr/bin/env python3
"""odis.py - disassemble the original code of an own function of the program (progcfg.py): odis.py NAME (from own_functions.txt) or LO HI (hex)"""
import os
import re
import sys

import progcfg  # noqa: E402
CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
from sout import SOut  # noqa: E402
import unlink_seg as U  # noqa: E402
import z8k  # noqa: E402


def main():
    a = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    prog = bytes(a.image[0:a.segs[0]['code']])
    if len(sys.argv) == 2:
        m = re.search(r'^(\w+) %s (\w+)' % re.escape(sys.argv[1]), open(os.path.join(CFG.DIR, 'own_functions.txt')).read(), re.M)
        lo, hi = int(m.group(1), 16), int(m.group(2), 16)
    else:
        lo, hi = int(sys.argv[1], 16), int(sys.argv[2], 16)
    for pc, i in U.decode_range(prog, True, lo, hi):
        print('%04x %s' % (pc, z8k.text(i)))


if __name__ == '__main__':
    main()
