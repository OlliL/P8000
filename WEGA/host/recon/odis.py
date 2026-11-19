#!/usr/bin/env python3
"""odis.py - disassemble the original code of an own function of the program (progcfg.py): odis.py NAME (orig and size_orig in findings.json) or LO HI (hex)"""
import json
import os
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
    cs = getattr(CFG, 'CODESEGS', None)
    if cs:    # several code segments (gopt): addresses SEG:OFFSET, the decoder gets the segment number
        if len(sys.argv) == 2:
            f = json.load(open(CFG.FINDINGS))['functions'][sys.argv[1]]
            sg, lo = f['orig'].split(':')
            lo, hi, sg = int(lo, 16), int(lo, 16) + f['size_orig'], int(sg)
        else:
            sg, lo = sys.argv[1].split(':')
            sg, lo, hi = int(sg), int(lo, 16), int(sys.argv[2], 16)
        off, n = cs[sg]
        for pc, i in U.decode_range(bytes(a.image[off:off + n]), sg, lo, hi):
            print('%d:%04x %s' % (sg, pc, z8k.text(i)))
        return
    if len(sys.argv) == 2:
        f = json.load(open(CFG.FINDINGS))['functions'][sys.argv[1]]
        lo = int(f['orig'], 16)
        hi = lo + f['size_orig']
    else:
        lo, hi = int(sys.argv[1], 16), int(sys.argv[2], 16)
    for pc, i in U.decode_range(prog, CFG.SEG, lo, hi):
        print('%04x %s' % (pc, z8k.text(i)))


if __name__ == '__main__':
    main()
