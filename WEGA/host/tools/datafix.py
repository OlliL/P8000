#!/usr/bin/env python3
"""datafix.py - make pointer words in the data section of a cut module (unlink.py output) symbolic.

Usage: datafix.py MODULE.s DSTART WORD=TARGET ...

  MODULE.s  output of unlink.py; its data section (from the first .data line) is rewritten
  DSTART    address of the module's data in the program (hex), the D labels count from it
  WORD      address of a pointer word in the program (hex), or a range FIRST-LAST (every second byte)
  TARGET    'data': the word points into the module's data, it becomes .addr D<offset> (the label is added);
            any other text is a symbol name (.addr NAME, an external for the module, e.g. _end or a common)

unlink.py writes jump tables of its own code as labels, but other words of the data stay raw bytes; a raw pointer
breaks when the module is linked at another address (sys_errlist, the buffer pointers of _iob, the initial break).
The module must assemble to the same bytes at its original address: check with cas and a comparison afterwards.
"""
import re
import sys


def parse_data(lines):
    """bytes of the data section with labels and .addr items: list of ('label', name) / ('byte', v) / ('addr', sym)"""
    items = []
    for l in lines:
        s = l.strip()
        m = re.match(r'^(\w+)::?$', s)
        if m:
            items.append(('label', m.group(1), l.rstrip('\n')))
            continue
        if s.startswith('.byte'):
            for v in s[5:].split(','):
                items.append(('byte', int(v.strip().lstrip('%'), 16)))
            continue
        if s.startswith('.addr'):
            items.append(('addr', s[5:].strip()))
            continue
        if s.startswith(('.even', '.data', '.psec')) or not s:
            items.append(('raw', l.rstrip('\n')))
            continue
        sys.exit('datafix: cannot read data line: %r' % l)
    return items


def main():
    path, dstart = sys.argv[1], int(sys.argv[2], 16)
    want = {}
    for a in sys.argv[3:]:
        w, t = a.split('=', 1)
        if '-' in w:
            lo, hi = (int(x, 16) for x in w.split('-'))
            for x in range(lo, hi + 1, 2):
                want[x - dstart] = t
        else:
            want[int(w, 16) - dstart] = t
    lines = open(path).read().split('\n')
    i0 = next(i for i, l in enumerate(lines) if l.strip() == '.data')
    items = parse_data(lines[i0 + 1:])
    # offsets of the items
    off, pos = 0, []
    for it in items:
        pos.append(off)
        off += 1 if it[0] == 'byte' else 2 if it[0] == 'addr' else 0
    byoff = {}
    for it, p in zip(items, pos):
        if it[0] == 'byte':
            byoff[p] = it[1]
    labels = {p for it, p in zip(items, pos) if it[0] == 'label'}
    need = set()
    repl = {}
    for o, t in want.items():
        if o not in byoff or o + 1 not in byoff:
            sys.exit('datafix: word at offset %04x is not two raw bytes' % o)
        v = (byoff[o] << 8) | byoff[o + 1]
        if t == 'data':
            tgt = v - dstart
            need.add(tgt)
            repl[o] = 'D%04x' % tgt
        else:
            repl[o] = t
    out, skip = [], 0
    pending = []

    def flush():
        while pending:
            out.append('\t.byte\t' + ','.join('%%%02x' % b for b in pending[:8]))
            del pending[:8]
    for it, p in zip(items, pos):
        if it[0] == 'byte':
            if p in need and p not in labels:
                flush()
                out.append('D%04x:' % p)
                labels.add(p)
            if skip:
                skip -= 1
                continue
            if p in repl:
                flush()
                out.append('\t.addr\t' + repl[p])
                skip = 1
                continue
            pending.append(it[1])
        else:
            flush()
            out.append(it[2] if it[0] == 'label' else ('\t.addr\t' + it[1]) if it[0] == 'addr' else it[1])
    flush()
    missing = need - labels
    if missing:
        sys.exit('datafix: targets inside an .addr item: %s' % sorted(missing))
    externs = sorted(set(t for t in want.values() if t != 'data'))
    head = lines[:i0 + 1]
    ext = ['\t.extern\t' + e for e in externs if not any(re.match(r'^\s*\.comm\s+\d+,\s*%s\s*$' % re.escape(e), l) for l in lines)]
    open(path, 'w').write('\n'.join(ext + head + out) + '\n')
    print('datafix: %d words made symbolic, %d labels added' % (len(repl), len(need)))


if __name__ == '__main__':
    main()
