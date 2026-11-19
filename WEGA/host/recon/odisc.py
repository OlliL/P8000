#!/usr/bin/env python3
"""odisc.py - the original of a function in a compact form (context economy; progcfg CODESEGS, SEG:OFFSET addresses).

Usage: RECON_PROG=NAME odisc.py [-o] FUNC           compact listing: no segment noise (<<63>>, | |), no segment prefix, strings and
                                                    function names resolved (the call to 4:402e shows as cerror)
       RECON_PROG=NAME odisc.py -s FUNC             outline only: the calls (names), the strings read, the switch tables, the size
       -o   write the listing to $RECON_TMP/odisc/FUNC.txt and print only the outline (read the file in parts with sed -n)
FUNC: a key of findings.json; the range includes the case blocks behind it (see fnc.py).
"""
import json
import os
import re
import sys

import progcfg

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import unlink_seg as U  # noqa: E402
import z8k  # noqa: E402
from sout import SOut  # noqa: E402
import fnc  # noqa: E402

FN = fnc.FN
BYADDR = dict(getattr(CFG, 'LIBNAMES', {}))
for k, f in FN.items():
    if not re.match(r'f\d_', k):
        sg, off = f['orig'].split(':')
        BYADDR[(int(sg), int(off, 16))] = k


def main():
    a = sys.argv[1:]
    out_file = '-o' in a
    outline = '-s' in a
    a = [x for x in a if x not in ('-o', '-s')]
    name = a[0]
    k0, k1 = fnc.span(name)
    sg, lo = FN[k0]['orig'].split(':')
    sg, lo = int(sg), int(lo, 16)
    hi = int(FN[k1]['orig'].split(':')[1], 16) + FN[k1]['size_orig']
    s = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    img = bytes(s.image)
    off, n = CFG.CODESEGS[sg]
    data = img[sum(x[1] for x in CFG.CODESEGS.values()):]

    def cstr(o):
        e = data.find(b'\0', o)
        return data[o:e if e >= 0 else o + 40][:50].decode('latin-1').replace('\n', '\\n').replace('\t', '\\t')
    lines, calls, strings = [], [], []
    for pc, i in U.decode_range(img[off:off + n], sg, lo, hi):
        t = z8k.text(i).replace('<<63>>', '').replace('|', '')
        m = re.match(r'(call|calr)\t(?:<<(\d)>>)?%([0-9A-F]+)$', t)
        if m:
            sgm = int(m.group(2) or sg)
            nm = BYADDR.get((sgm, int(m.group(3), 16)))
            t = '%s\t%s' % (m.group(1), nm or '%d:%s' % (sgm, m.group(3).lower()))
            calls.append(nm or '%d:%s' % (sgm, m.group(3).lower()))
        for mm in re.finditer(r'#%87[0-9A-F]{2}([0-9A-F]{4})', t):
            v = int(mm.group(1), 16)
            strings.append('%x:%s' % (v, cstr(v)))
            t += '\t; ' + cstr(v)
        lines.append('%04x %s' % (pc, t))
    summ = '%s: %s %04x..%04x %d bytes, %d instructions; calls: %s' % (name, FN[k0]['orig'], lo, hi, hi - lo, len(lines),
                                                                      ' '.join(sorted(set(calls))) or '-')
    if outline or out_file:
        print(summ)
        for x in dict.fromkeys(strings):
            print('  str', x)
    if out_file:
        d = os.path.join(fnc.TMP, '..', 'odisc')
        os.makedirs(d, exist_ok=True)
        p = os.path.join(d, name + '.txt')
        open(p, 'w').write('\n'.join(lines) + '\n')
        print('listing:', os.path.normpath(p))
    elif not outline:
        print('\n'.join(lines))


if __name__ == '__main__':
    main()
