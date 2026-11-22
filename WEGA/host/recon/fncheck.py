#!/usr/bin/env python3
"""fncheck.py - compile a C draft alone with the original cc and compare its code with a range of the original.

Usage: fncheck.py DRAFT.c LO HI [NAME]    (LO as SEG:OFFSET for a program with several code segments, progcfg CODESEGS)

  DRAFT.c  one or more K&R functions (no prototypes), compiled alone with progcfg CC -O -c in the emulator (the .h
           files of SRCDIR are copied next to it)
  LO HI    code range of the original (hex); the function starts at LO
  NAME     the function in the object (default: the first function of the object)

The draft alone has other data addresses and resolves calls to other functions as call instead of calr, so the
comparison is masked (zcode.norm_masked) and only a first look: a function counts as exact only with strictcmp.py on
the whole program. Prints OK or the differing instructions (original | object).
"""
import difflib
import os
import re
import shutil
import subprocess
import sys
import tempfile

import progcfg
import zcode

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
from sout import SOut  # noqa: E402


def compile_obj(src):
    tmp = tempfile.mkdtemp(prefix='fncheck', dir=os.environ.get('RECON_TMP', '/home/olivleh1/git/P8000/tmp/tmp'))
    base = os.path.splitext(os.path.basename(src))[0]
    shutil.copy(src, os.path.join(tmp, base + '.c'))
    for d in [os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'src', 'head', 'oldv7')] * bool(os.environ.get('FNC_OLDV7')) + [CFG.SRCDIR]:
        for f in os.listdir(d):
            if f.endswith('.h') or f in os.environ.get('FNC_EXTRA', '').split():    # FNC_EXTRA: files the draft includes
                shutil.copy(os.path.join(d, f), tmp)
    # FNC_ROOT / FNC_CC / FNC_FLAGS: another compiler root and compiler (the old compilers: OLDROOT, /bin/sccv1), more switches
    r = subprocess.run([CFG.EMU, '-r', os.environ.get('FNC_ROOT', CFG.ROOT), os.environ.get('FNC_CC', CFG.CC), '-O']
                       + os.environ.get('FNC_FLAGS', '').split() + ['-c', base + '.c'], cwd=tmp,
                       capture_output=True, text=True, timeout=900)
    obj = os.path.join(tmp, base + '.o')
    if r.returncode != 0 or not os.path.exists(obj):
        return None, (r.stdout + r.stderr).strip()
    return obj, ''


def main():
    src, hi = sys.argv[1], int(sys.argv[3], 16)
    osg = None
    if ':' in sys.argv[2]:
        osg, lo = sys.argv[2].split(':')
        osg, lo = int(osg), int(lo, 16)
    else:
        lo = int(sys.argv[2], 16)
    name = sys.argv[4] if len(sys.argv) > 4 else None
    path, err = compile_obj(src)
    if path is None:
        sys.exit('does not compile:\n' + err)
    o = SOut(open(path, 'rb').read())
    oc = bytes(o.image)[:o.segs[0]['code']]
    starts = sorted(s['value'] for s in o.symbols if s['type'] == 34)
    s = 0
    if name:
        s = [x['value'] for x in o.symbols if x['name'] == '_' + name][0]
    e = min([t for t in starts if t > s] + [len(oc)])
    g = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    nm = (lambda t: zcode.norm_masked(re.sub(r'<<\d+>>', '', t))) if getattr(CFG, 'CODESEGS', None) else zcode.norm_masked   # segment numbers differ in an object
    raw = [t for _, t in zcode.insns(oc, s, e, CFG.SEG)]
    if os.environ.get('FNC_DROPRAW'):    # FNC_DROPRAW=REGEX: the same on the unmasked lines (the value of an immediate counts)
        raw = [t for t in raw if not re.search(os.environ['FNC_DROPRAW'], t)]
    a = [nm(t) for t in raw]
    if os.environ.get('FNC_DROP'):    # FNC_DROP=REGEX: the lines of the object that match are dropped (a known difference, e.g. a redundant instruction)
        a = [t for t in a if not re.search(os.environ['FNC_DROP'], t)]
    gcode = bytes(g.image)[:g.segs[0]['code']]
    if osg is not None:
        off, n = CFG.CODESEGS[osg]
        gcode = bytes(g.image)[off:off + n]
    b = [nm(t) for _, t in zcode.insns(gcode, lo, hi, CFG.SEG)]
    if a == b:
        print('OK (masked): %d instructions equal (%04x..%04x)' % (len(b), lo, hi))
        return
    print('DIFF (masked): object %d instructions, original %d' % (len(a), len(b)))
    for line in difflib.unified_diff(b, a, 'original', 'object', lineterm='', n=int(os.environ.get('FNC_CTX', '2'))):
        print(line)


if __name__ == '__main__':
    main()
