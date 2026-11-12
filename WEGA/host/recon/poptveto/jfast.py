#!/usr/bin/env python3
"""jfast.py - fast check of one function of cas: spopt + host cas on the raw code of one module, the function's code
against the original, without the model link (0.2 s instead of 15 s).

  jfast.py ref FUNC LO HI          write the normalized original (hex range) to compare/cas/FUNC_orig.txt
  jfast.py SPOPT RAW.s [FUNC] [-v] spopt binary (run under z8k-emu with root of SPOPT's .../lib/vc), raw code from
                                   `sccvc -S` (no -O); prints the number of differing lines with jump targets masked
                                   (strictcmp style; negative: same lines, other targets), -v the diff
Normalized: jump targets inside the function as offsets, calls and segment numbers masked, 32-bit immediates masked.
The layout of the code is compared exactly; constants that the link sets are not (use strictcmp for the final check)."""
import difflib, os, re, subprocess, sys, tempfile
W = '/mnt/share/cvs/P8000/WEGA'
sys.path.insert(0, W + '/host/tools'); sys.path.insert(0, W + '/host/recon')
from sout import SOut  # noqa: E402
import unlink_seg as U  # noqa: E402
import z8k  # noqa: E402
EMU = W + '/host/z8k-emu/z8k-emu'; CAS = W + '/host/cas/cas'
TGT = re.compile(r'%([0-9A-Fa-f]{3,8})\b')


def norm(insns, lo, hi):
    out = []
    for pc, i in insns:
        t = z8k.text(i)
        op = t.split()[0]
        if op in ('call', 'calr'):
            out.append(op); continue
        t = re.sub(r'<<\d+>>', '<<S>>', t)
        if op in ('jr', 'jp', 'djnz', 'dbjnz'):
            m = TGT.search(t)
            if m:
                a = int(m.group(1), 16) & 0xffff
                t = t[:m.start()] + ('@%x' % (a - lo) if lo <= a < hi else 'EXT') + t[m.end():]
        t = re.sub(r'#%[0-9A-Fa-f]{8}\b', '#ADDR', t)
        out.append(t)
    return out


def ref(func, lo, hi):
    sys.path.insert(0, W + '/host/recon')
    os.environ.setdefault('RECON_PROG', 'cas')
    import progcfg
    cfg = progcfg.load()
    a = SOut(open(os.path.join(cfg.ROOT, cfg.ORIG_BIN), 'rb').read())
    prog = bytes(a.image[0:a.segs[0]['code']])
    lines = norm(U.decode_range(prog, cfg.SEG, lo, hi), lo, hi)
    open(W + '/compare/cas/%s_orig.txt' % func, 'w').write('\n'.join(lines) + '\n')
    print(len(lines), 'lines')


def ours(spopt, raw, func='jpr', verbose=False):
    root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(spopt))))
    rel = os.path.relpath(os.path.abspath(spopt), root)
    with tempfile.TemporaryDirectory() as d:
        open(d + '/in.s', 'w').write(open(raw).read())
        subprocess.run(['timeout', '-s', 'KILL', '20', EMU, '-r', root, '/' + rel, 'in.s', 'out.s'], cwd=d,
                       capture_output=True)
        if not os.path.exists(d + '/out.s'):
            return None, []
        r = subprocess.run([CAS, '-u', '-o', 'out.o', 'out.s'], cwd=d, capture_output=True)
        if r.returncode or not os.path.exists(d + '/out.o'):
            return None, []
        a = SOut(open(d + '/out.o', 'rb').read())
    code = bytes(a.image[0:a.segs[0]['code']])
    syms = sorted((s['value'], s['name']) for s in a.symidx.values() if s['type'] == 34)
    lo = [v for v, n in syms if n == '_' + func][0]
    hi = min([v for v, n in syms if v > lo] + [len(code)])
    mine = norm(U.decode_range(code, True, lo, hi), lo, hi)
    orig = open(W + '/compare/cas/%s_orig.txt' % func).read().split('\n')[:-1]
    m = lambda L: [re.sub(r'@[0-9a-f]+', 'CODE', x) for x in L]
    diff = [l for l in difflib.unified_diff(m(orig), m(mine), 'orig', 'ours', n=3, lineterm='')]
    n = sum(1 for l in diff if l[:1] in '+-' and not l.startswith(('+++', '---')))
    if n == 0 and mine != orig:
        diff = [l for l in difflib.unified_diff(orig, mine, 'orig', 'ours', n=3, lineterm='')]
        n = -sum(1 for l in diff if l[:1] in '+-' and not l.startswith(('+++', '---')))
    return n, diff


if __name__ == '__main__':
    if sys.argv[1] == 'ref':
        ref(sys.argv[2], int(sys.argv[3], 16), int(sys.argv[4], 16))
    else:
        args = [x for x in sys.argv[1:] if x != '-v']
        n, diff = ours(args[0], args[1], args[2] if len(args) > 2 else 'jpr', '-v' in sys.argv)
        print(n)
        if '-v' in sys.argv:
            print('\n'.join(diff))
