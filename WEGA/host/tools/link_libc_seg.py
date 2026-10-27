#!/usr/bin/env python3
"""link_libc_seg.py - cut all segmented libc modules of /bin/sld with resolved cross references.

Modules are the verified ranges (src/lib/oldslibc/README). Every call into another module becomes a symbol
(_name); the entry points that other modules call get the export label _name:: in their module.
getpid (0x95aa) and umask (0x96f2) are shipped members of slibc.a (not cut); ffltioc (0x8680) lies before the cut.
Names: the known function names of libc_bounds.txt, otherwise f<addr>.
Usage: link_libc_seg.py OUTDIR
"""
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import unlink_seg as U  # noqa: E402
from sout import SOut  # noqa: E402

PROG = '/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld'
MODULES = [('filbuf', 0x8686, 0x881e), ('flsbuf', 0x881e, 0x8a50), ('cnvt', 0x8a50, 0x8be8),
           ('decimal', 0x8be8, 0x8cb4), ('malloc', 0x8cb4, 0x91e8), ('isatty', 0x91e8, 0x9224),
           ('mktemp', 0x9224, 0x9296), ('stty', 0x9296, 0x932c), ('swab', 0x932c, 0x93f6),
           ('strcmp', 0x93f6, 0x943c), ('strcpy', 0x943c, 0x946e), ('swap', 0x946e, 0x95aa),
           ('signal', 0x95b0, 0x96f2), ('stubs', 0x96fa, 0x97fc)]
KNOWN = {0x8686: 'ungetc', 0x86dc: 'filbuf', 0x881e: 'flsbuf', 0x8924: 'fflush', 0x89ba: 'fclose',
         0x8a50: 'cnvt', 0x8be8: 'decimal', 0x8cb4: 'malloc', 0x9170: 'calloc', 0x91d8: 'cfree',
         0x91e8: 'isatty', 0x9224: 'mktemp', 0x9296: 'stty', 0x92e6: 'strrchr', 0x932c: 'swab',
         0x9398: 'strcat', 0x93d6: 'strncmp', 0x93f6: 'strcmp', 0x943c: 'strcpy', 0x946e: 'swap',
         0x95aa: 'getpid', 0x95b0: 'signal', 0x96f2: 'umask'}


def owner(t):
    for name, a, b in MODULES:
        if a <= t < b:
            return name
    return None


def main():
    out = sys.argv[1]
    os.makedirs(out, exist_ok=True)
    s = SOut(open(PROG, 'rb').read())
    codesz = s.segs[0]['code']
    prog = bytes(s.image[0:codesz])
    seg = (s.magic >> 8) == 0xE6
    # 1. every call target inside the libc part and its owner module
    targets = {a for _, a, _ in MODULES}
    for name, a, b in MODULES:
        for pc, i in U.decode_range(prog, seg, a, b):
            k, t = U.target_of(i)
            if i.mn in ('call', 'calr', 'jp') and k in ('abs', 'rel') and t is not None and 0x8680 <= t < 0x9800:
                targets.add(t)
    names = {t: KNOWN.get(t, 'f%x' % t) for t in targets}
    # a module start carries the module name as its label (header of the .s file), callers must use the same
    names.update({a: name for name, a, _ in MODULES})
    exports = {}
    for name, a, b in MODULES:
        for pc, i in U.decode_range(prog, seg, a, b):
            k, t = U.target_of(i)
            if i.mn in ('call', 'calr', 'jp') and k in ('abs', 'rel') and t is not None and 0x8680 <= t < 0x9800:
                o = owner(t)
                if o is not None and o != name:
                    exports.setdefault(o, {})[t] = names[t]
    extnames = {'0x%x' % t: n for t, n in names.items()}
    for name, a, b in MODULES:
        exp = exports.get(name, {})
        src = U.write_source(prog, seg, a, b, name, out, extnames, exports=exp)
        diff, info = U.verify(prog, seg, a, b, src, out)
        print('%-8s %04x-%04x exports %d  verify %s %s' % (name, a, b, len(exp), diff, info))


if __name__ == '__main__':
    main()
