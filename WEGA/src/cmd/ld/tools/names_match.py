#!/usr/bin/env python3
"""names_match.py - name hints for the proven functions of /bin/sld from the Plexus ld source (private reference).

Only the messages are compared: a Plexus function and a WEGA function that print the same message text are a
candidate pair (the message is the shared fact). Nothing of the Plexus text is copied; the output is names and
the message they share. Usage: names_match.py [PLEXUS_LD_C] [BINARY] [INVENTORY]
"""
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import modules as M  # noqa: E402
import z8k  # noqa: E402
from sout import SOut  # noqa: E402

PLEXUS = '/mnt/share/cvs/P8000/tmp/plexus/cmd/ld/ld.z8000.c'


def plexus_functions(path):
    """message -> set of Plexus function names that use it"""
    cur = None
    res = {}
    for line in open(path, errors='replace'):
        m = re.match(r'^([A-Za-z_][\w]*)\s*\(', line)
        if m and not line.startswith(('if', 'while', 'for', 'switch')):
            cur = m.group(1)
        for msg in re.findall(r'"([^"]{4,})"', line):
            if cur:
                res.setdefault(msg, set()).add(cur)
    return res


def main():
    plex = plexus_functions(sys.argv[1] if len(sys.argv) > 1 else PLEXUS)
    binp = sys.argv[2] if len(sys.argv) > 2 else M.DEFAULT
    inv = sys.argv[3] if len(sys.argv) > 3 else '/mnt/share/cvs/P8000/tmp/tmp/sld_inv.json'
    s = SOut(open(binp, 'rb').read())
    codesz = s.segs[0]['code']
    img = s.image
    data = img[codesz:codesz + s.segs[1]['data']]
    # WEGA: message text found in the data -> the proven functions that reference it
    msgs = {}
    for m in re.finditer(rb'[\x20-\x7e]{4,}', data):
        msgs.setdefault(m.group().decode(), m.start())
    b = z8k.Buf(img[0:codesz])
    refs = {}
    for start_s in json.load(open(inv))['proven_starts']:
        start = int(start_s, 16)
        if start >= codesz:
            continue
        todo, seen = [start], set()
        while todo and len(seen) < 400:
            pc = todo.pop()
            if pc in seen or not (0 <= pc < codesz):
                continue
            seen.add(pc)
            ins = z8k.decode(b, pc, 0)
            if ins is None:
                continue
            for o in ins.ops:
                off = None
                if o.kind == 'da' and o.a[0] == 1:
                    off = o.a[1]
                elif o.kind == 'da' and o.a[0] == 0 and o.a[1] >= 0x8000:
                    off = o.a[1] & 0x7fff
                elif o.kind == 'imm' and isinstance(o.a, int) and (o.a >> 16) == 1:
                    off = o.a & 0xffff
                if off is not None:
                    refs.setdefault(off, set()).add(start_s)
                    # the cell may hold a pointer to the text (table of message pointers)
                    if off + 2 <= len(data):
                        p = (data[off] << 8) | data[off + 1]
                        refs.setdefault(p, set()).add(start_s)
            if ins.mn == 'ret':
                continue
            if ins.mn in ('jr', 'jp') and not any(o.kind == 'cc' for o in ins.ops):
                todo += [o.a for o in ins.ops if o.kind == 'rel']
                continue
            todo.append(ins.end)
    out = []
    for msg, plexfns in sorted(plex.items()):
        # the WEGA text may be longer (e.g. "- Multiply defined"): find the Plexus text inside the data
        pos = data.find(msg.encode())
        if pos < 0:
            continue
        wega = sorted(refs.get(pos, set()))
        if wega:
            out.append({'message': msg, 'data_offset': hex(pos), 'plexus': sorted(plexfns), 'wega_functions': wega})
    json.dump(out, sys.stdout, indent=1)
    print()


if __name__ == '__main__':
    main()
