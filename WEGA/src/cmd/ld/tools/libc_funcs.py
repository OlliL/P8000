#!/usr/bin/env python3
"""libc_funcs.py - function-level comparison of the libc part of /bin/sld with the members of slibc.a.

Member functions: the text symbols of each member object (value = start, next symbol = end).
sld functions: each libc piece split at the call targets inside it (libc_rank.txt has the pieces).
For every sld function the three most similar member functions (difflib ratio of the normalised instructions,
relocated words masked as in libcver.py) are printed.
"""
import difflib
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import libcver as L  # noqa: E402
import war  # noqa: E402
import z8k  # noqa: E402
from sout import SOut  # noqa: E402

SLIB = '/mnt/share/cvs/P8000/tmp/wega31_root/lib/slibc.a'
PIECES = [('p01', 0x8686, 0x8922), ('p02', 0x8924, 0x89b8), ('p03', 0x89ba, 0x8cb2), ('p04', 0x8cb4, 0x8f7a),
          ('p05', 0x8f7c, 0x9038), ('p06', 0x903a, 0x946c), ('p07', 0x946e, 0x95a4), ('p08', 0x95b0, 0x96bc),
          ('p09', 0x96be, 0x96f0), ('p10', 0x96fa, 0x97fc)]


def norm_seq(img, seg, lo, hi, relocs=None):
    """normalised instruction texts of img[lo:hi] (linear decode)"""
    buf = z8k.Buf(img[lo:hi], 0)
    out, pc = [], 0
    while pc < hi - lo:
        i = z8k.decode(buf, pc, seg)
        if not i:
            pc += 2
            continue
        out.append(L.norm(z8k.text(i)))
        pc = i.end
    return out


def member_functions(path):
    """(member name, function name, normalised sequence) for every text symbol of every member"""
    res = []
    for m in war.members(open(path, 'rb').read()):
        if m['name'].startswith('__.SYMDEF'):
            continue
        try:
            s = SOut(m['body'])
        except ValueError:
            continue
        seg = (s.magic >> 8) == 0xE6
        tsz = s.segs[0]['code']
        img = bytearray(s.image)
        for a in range(0, len(s.reloc) - 1, 2):
            if s.reloc[a] or s.reloc[a + 1]:
                img[a:a + 2] = b'\xff\xff'
        starts = sorted(set(sy['value'] for sy in s.symbols if sy.get('segt') == 0 and (sy.get('type', 0) & 0x1f) == 0x02
                            and sy['value'] < tsz))
        if not starts:
            continue
        ends = starts[1:] + [tsz]
        names = {}
        for sy in s.symbols:
            if sy.get('segt') == 0 and sy.get('value') in starts:
                names.setdefault(sy['value'], sy['name'])
        for a, b in zip(starts, ends):
            if b <= a:
                continue
            res.append((m['name'], names.get(a, '?'), norm_seq(bytes(img), seg, a, b)))
    return res


def piece_functions(img, lo, hi, seg):
    """split the piece [lo, hi) at call targets inside it; returns [(start, end)] of the sld functions"""
    buf = z8k.Buf(img, 0)
    starts = {lo}
    pc = lo
    while pc < hi:
        i = z8k.decode(buf, pc, seg)
        if not i:
            pc += 2
            continue
        for o in i.ops:
            if i.mn in ('call', 'calr'):
                t = o.a[1] if o.kind == 'da' else o.a
                if isinstance(t, int) and lo <= t < hi:
                    starts.add(t)
        pc = i.end
    st = sorted(starts)
    return list(zip(st, st[1:] + [hi]))


def main():
    s, ins, raw = L.load_program('/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld')
    img = s.image
    codesz = s.segs[0]['code']
    sld_seg = (s.magic >> 8) == 0xE6
    mf = member_functions(SLIB)
    print('member functions', len(mf))
    for pname, lo, hi in PIECES:
        for a, b in piece_functions(img[0:codesz], lo, hi, sld_seg):
            seq = norm_seq(img[0:codesz], sld_seg, a, b)
            if len(seq) < 3:
                continue
            res = []
            for mem, fn, mm in mf:
                if not (0.5 * len(seq) <= len(mm) <= 2 * len(seq) + 4):
                    continue
                r = difflib.SequenceMatcher(None, seq, mm, autojunk=False).ratio()
                res.append((round(r, 3), mem, fn))
            res.sort(reverse=True)
            print('%s %04x-%04x %3d  %s' % (pname, a, b, len(seq), res[:3]))


if __name__ == '__main__':
    main()
