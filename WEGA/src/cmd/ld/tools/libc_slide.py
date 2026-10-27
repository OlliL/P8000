#!/usr/bin/env python3
"""libc_slide.py - for every open libc region of /bin/sld: the functions of slibc.a that occur in it.

Every function of every member (libc_funcs.member_functions) is slid over the region's instruction boundaries; a
cheap prefilter (shared normalised instructions) picks the candidates, difflib ratio decides. Output: region, function,
window start and ratio for ratios >= 0.5.
"""
import difflib
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import libcver as L  # noqa: E402
import z8k  # noqa: E402
import libc_funcs as F  # noqa: E402

REGIONS = [('open1', 0x8686, 0x8922), ('open2', 0x8f7c, 0x9038), ('open3', 0x903a, 0x92ae), ('open4', 0x96be, 0x96f0),
           ('open5', 0x96fa, 0x97fc)]


def main():
    s, ins, raw = L.load_program('/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld')
    img = s.image[0:38912]
    seg = (s.magic >> 8) == 0xE6
    mf = F.member_functions(F.SLIB)
    buf = z8k.Buf(img, 0)
    for name, lo, hi in REGIONS:
        bounds, pc = [], lo
        while pc < hi:
            bounds.append(pc)
            i = z8k.decode(buf, pc, seg)
            pc = i.end if i else pc + 2
        # normalised instruction at each boundary, for the prefilter
        norm_at = {}
        for b0 in bounds:
            i = z8k.decode(buf, b0, seg)
            norm_at[b0] = L.norm(z8k.text(i)) if i else None
        hits = []
        for mem, fn, mm in mf:
            if len(mm) < 3:
                continue
            want = set(mm)
            best = (0.0, None)
            for b0 in bounds:
                # prefilter: the first normalised instruction of the function occurs at b0
                if norm_at.get(b0) not in want:
                    continue
                seq, p = [], b0
                for _ in range(len(mm) + 4):
                    i = z8k.decode(buf, p, seg)
                    if not i or p >= hi:
                        break
                    seq.append(L.norm(z8k.text(i)))
                    p = i.end
                r = difflib.SequenceMatcher(None, mm, seq, autojunk=False).ratio()
                if r > best[0]:
                    best = (r, b0)
            if best[0] >= 0.5:
                hits.append((round(best[0], 3), mem, fn, hex(best[1])))
        hits.sort(reverse=True)
        print(name, hex(lo), hex(hi), hits[:6], flush=True)


if __name__ == '__main__':
    main()
