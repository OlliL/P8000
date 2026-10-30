#!/usr/bin/env python3
"""modcheck.py - compile one module of our sources alone and compare each of its functions with the original.

Usage: modcheck.py MODULE[.c] [-v NAME] [-k] [-a] [-f]

  MODULE  a C file of progcfg SRCDIR (or a .y file: progcfg YACC makes the .c, see below); compiled with progcfg CC -O -c
          in the emulator (the .h files and the include files of SRCDIR are copied next to it)
  -v NAME print the diff of NAME (original | object)
  -a      print the diffs of all differing functions
  -k      keep the work directory (RECON_TMP/modcheck_NAME)
  -f      also hide the frame: offsets (r15) and the frame size (dec/inc/sub/add r15); shows whether a function
          differs only in its frame layout (D1); for the classification only, never for "exact"

Every function of the object whose name is in findings.json (functions: name -> orig, size_orig) is compared with its
range of the original: masked (zcode.norm_masked, addresses hidden) but registers and frame offsets kept. This is the
fast loop of step 5; a function counts as exact only with strictcmp.py on the whole program.
"""
import difflib
import json
import os
import shutil
import subprocess
import sys

import progcfg
import zcode

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
from sout import SOut  # noqa: E402


def build(mod):
    base = os.path.splitext(mod)[0]
    work = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'modcheck_' + CFG.NAME)
    shutil.rmtree(work, ignore_errors=True)
    os.makedirs(work)
    for f in os.listdir(CFG.SRCDIR):
        p = os.path.join(CFG.SRCDIR, f)
        if os.path.isfile(p) and not f.endswith(('.o', '.s', '.py')) and f not in ('Makefile',):
            shutil.copy(p, work)
    if mod.endswith('.y'):
        r = subprocess.run([CFG.EMU, '-r', CFG.ROOT, '/usr/bin/yacc', mod], cwd=work, capture_output=True, text=True)
        os.rename(os.path.join(work, 'y.tab.c'), os.path.join(work, base + '.c'))
    r = subprocess.run([CFG.EMU, '-r', CFG.ROOT, CFG.CC, '-O', '-c', base + '.c'], cwd=work, capture_output=True,
                       text=True, timeout=1200)
    obj = os.path.join(work, base + '.o')
    if not os.path.exists(obj):
        sys.exit('does not compile:\n' + (r.stdout + r.stderr)[-3000:])
    if r.stdout.strip() or r.stderr.strip():
        print((r.stdout + r.stderr).strip()[-1500:])
    return work, obj


def main():
    a = sys.argv[1:]
    keep = '-k' in a
    alld = '-a' in a
    frame = '-f' in a
    a = [x for x in a if x not in ('-k', '-a', '-f')]
    verbose = None
    if '-v' in a:
        i = a.index('-v')
        verbose = a[i + 1]
        del a[i:i + 2]
    mod = a[0] if a[0].endswith(('.c', '.y')) else a[0] + '.c'
    work, obj = build(mod)
    o = SOut(open(obj, 'rb').read())
    oc = bytes(o.image)[:o.segs[0]['code']]
    starts = sorted(set(s['value'] for s in o.symbols if s['type'] in (34, 2) and s['value'] < len(oc)))
    g = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    gc = bytes(g.image)[:g.segs[0]['code']]
    fn = json.load(open(CFG.FINDINGS))['functions']
    same = diff = 0
    syms = sorted((s['value'], s['name'][1:]) for s in o.symbols if s['type'] in (34, 2) and s['name'].startswith('_'))
    for v, name in syms:
        f = fn.get(name)
        e = min([t for t in starts if t > v] + [len(oc)])
        if f is None:
            print('%-16s ----  ---- %4d  (not in findings.json)' % (name, e - v))
            continue
        lo = int(f['orig'], 16)
        hi = lo + f['size_orig']
        mine = [zcode.norm_masked(t, CFG.D18) for _, t in zcode.insns(oc, v, e, CFG.SEG)]
        theirs = [zcode.norm_masked(t, CFG.D18) for _, t in zcode.insns(gc, lo, hi, CFG.SEG)]
        if frame:
            mine = [zcode.hide_frame(t) for t in mine]
            theirs = [zcode.hide_frame(t) for t in theirs]
        d = [l for l in difflib.unified_diff(theirs, mine, lineterm='', n=0) if l[:1] in '+-' and l[:3] not in ('---', '+++')]
        same += not d
        diff += bool(d)
        print('%-16s %04x %4d %4d  %s' % (name, lo, hi - lo, e - v, 'SAME' if not d else 'DIFF %d' % len(d)))
        if verbose == name or (alld and d):
            print('\n'.join(difflib.unified_diff(theirs, mine, 'orig', 'ours', lineterm='', n=2)))
    print('%s: same %d, differ %d (masked%s, module alone)' % (mod, same, diff, ', frame hidden' if frame else ''))
    if not keep:
        shutil.rmtree(work)


if __name__ == '__main__':
    main()
