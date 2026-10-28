#!/usr/bin/env python3
"""modules.py [BINARY] [INVENTORY_JSON] - module of each proven function of the original sld, from its data references.

The data segment of /bin/sld (segment 1, 7090 bytes) follows the code in the image. Each module starts with its SCCS
identification ("@[$]name.c" followed by version and date). A function refers to its module's strings and tables by
segment-1 addresses (0x0100xxxx: segment 1, offset xxxx) or by nonsegmented addresses with the 0x8000 bit
(offset without the bit). The module of a function is the module with the most such
references among the function's own instructions (proven starts only).
"""
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import z8k  # noqa: E402
from sout import SOut  # noqa: E402

DEFAULT = '/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld'


def module_ranges(data):
    marks = [(m.start(), m.group(1).decode()) for m in re.finditer(rb'@\[\$\]([\w.]+)', data)]
    ranges = []
    for i, (off, name) in enumerate(marks):
        end = marks[i + 1][0] if i + 1 < len(marks) else len(data)
        ranges.append((off, end, name))
    return ranges


def is_text(data, off):
    """a reference counts when it points to a printable string (messages and names are module specific)."""
    run = 0
    while off + run < len(data) and 0x20 <= data[off + run] < 0x7f:
        run += 1
    return run >= 4


def module_of(off, ranges):
    for a, b, name in ranges:
        if a <= off < b:
            return name
    return None


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else DEFAULT
    inv_path = sys.argv[2] if len(sys.argv) > 2 else '/mnt/share/cvs/P8000/tmp/tmp/sld_inv.json'
    s = SOut(open(path, 'rb').read())
    codesz = s.segs[0]['code']
    img = s.image
    data = img[codesz:codesz + s.segs[1]['data']]
    ranges = module_ranges(data)
    b = z8k.Buf(img[0:codesz])
    inv = json.load(open(inv_path))
    result = {}
    for start_s in inv['proven_starts']:
        start = int(start_s, 16)
        if start >= codesz:
            continue
        refs = {}
        todo = [start]
        seen = set()
        while todo and len(seen) < 400:
            pc = todo.pop()
            if pc in seen or not (0 <= pc < codesz):
                continue
            seen.add(pc)
            ins = z8k.decode(b, pc, 0)
            if ins is None:
                continue
            for o in ins.ops:
                val = None
                if o.kind == 'da' and o.a[0] == 1:
                    val = o.a[1]
                elif o.kind == 'da' and o.a[0] == 0 and o.a[1] >= 0x8000:
                    # nonsegmented address with the 0x8000 bit: the data follows the code (offset without the bit)
                    val = o.a[1] & 0x7fff
                elif o.kind == 'imm' and isinstance(o.a, int) and (o.a >> 16) == 1:
                    val = o.a & 0xffff
                if val is not None and 0 <= val < len(data) and is_text(data, val):
                    m = module_of(val, ranges)
                    if m:
                        refs[m] = refs.get(m, 0) + 1
            if ins.mn in ('ret',):
                continue
            if ins.mn in ('jr', 'jp') and not any(o.kind == 'cc' for o in ins.ops):
                for o in ins.ops:
                    if o.kind == 'rel':
                        todo.append(o.a)
                continue
            todo.append(ins.end)
        result[start_s] = refs
    out = {'modules': [r[2] for r in ranges], 'ranges': [[hex(a), hex(b), n] for a, b, n in ranges],
           'function_refs': result}
    json.dump(out, sys.stdout, indent=1)
    print()


if __name__ == '__main__':
    main()
