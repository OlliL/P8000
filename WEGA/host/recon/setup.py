#!/usr/bin/env python3
"""setup.py NAME [ORIG] - the mechanical start of the reconstruction of a nonsegmented program (host/HOWTO_reconstruct.md, step 1).

ORIG is the path below the WEGA root (default bin/NAME; WEGA=dir names the root). In order:
  1. the SCCS lines of the original (the own source files, their versions and dates);
  2. the System III source (the path of src/COVERAGE.md, else tmp/sys3/usr/src/cmd/NAME.c or cmd/NAME/), copied unchanged in the
     V7 layout: cmd/NAME.c to src/cmd/NAME.c, a file of cmd/DIR/ as the directory src/cmd/DIR/; an existing target is kept;
  3. the dating (the line of host/OLDCC_DATES.md) and the compiler it points to (OLDCC.md, section 2): D1 tight = v0, slots = v1
     (neighbours v1r, v1q), the crt0 stamp 3.2 11/16/83 or 11/18/83 = vm (ccvm), loose with the libc of 09/83 = the
     shipped cc; the main session decides between them;
  4. stripped or not (the header);
  5. compare/NAME/progcfg.py (kept when it exists), the inventory (inventory.py), the end of the own code OWN_LIM and the
     library (the walk of whichlibc.py from the start of the text, per library: the libc starts at its first member of at
     least 32 bytes and the members right before it; the best library covers the most bytes from there; its gaps are modules
     of another version or of another library, for the main session), compare/NAME/findings.json (own_table.py) with the candidate compiler.
Prints a report with the keys of the former agent recon-setup. Writes only src/cmd/NAME.c (or src/cmd/DIR/), compare/NAME/ and
tmp/tmp/NAME/inv.json. Exit status 1 when a step fails (the report says which).
"""
import glob
import json
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
W = os.path.normpath(os.path.join(HERE, '..', '..'))
REPO = os.path.dirname(W)
sys.path.insert(0, os.path.join(W, 'host', 'tools'))
import whichlibc  # noqa: E402
import zcode  # noqa: E402
from sout import SOut  # noqa: E402

ROOT = os.environ.get('WEGA', os.path.join(REPO, 'tmp', 'wega31_root'))
SYS3 = os.path.join(REPO, 'tmp', 'sys3', 'usr', 'src')
LIBDIR = os.path.join(W, 'src', 'lib')


def sccs(path):
    data = open(path, 'rb').read()
    out = []
    for m in re.finditer(rb'@\[\$\]([^\0\n]*)', data):
        t = m.group(1).decode('latin-1')
        f = t.split()
        if f and not f[0].startswith('crt0'):
            lib = any(glob.glob(os.path.join(W, 'src', 'lib', d, '**', f[0]), recursive=True) for d in ('libc', 'oldlibc'))
            out.append(' '.join(f[:3]) + (' (libc)' if lib else ''))
    return out


def sys3_source(name, orig):
    """the System III path (relative to SYS3) and the target in src/cmd, or (None, None)"""
    cov = os.path.join(W, 'src', 'COVERAGE.md')
    cand = []
    if os.path.exists(cov):
        for line in open(cov):
            if line.startswith('| `%s`' % orig):
                m = re.search(r'`tmp/sys3/usr/src/([^`]+)`', line)
                if m:
                    cand.append(m.group(1))
    cand += ['cmd/%s.c' % name, 'cmd/%s' % name]
    for c in cand:
        p = os.path.join(SYS3, c)
        if not os.path.exists(p):
            continue
        parts = c.split('/')
        if parts[0] != 'cmd':
            return c, None              # not below cmd: the main session decides where it goes
        if len(parts) == 2 and os.path.isfile(p):
            return c, 'src/cmd/' + parts[1]
        return c, 'src/cmd/' + parts[1] + '/'
    return None, None


def copy_source(rel, target):
    src = os.path.join(SYS3, rel)
    dst = os.path.join(W, target.rstrip('/'))
    if os.path.exists(dst):
        return 'kept'
    if target.endswith('/'):
        d = src if os.path.isdir(src) else os.path.dirname(src)
        shutil.copytree(d, dst)
    else:
        shutil.copy2(src, dst)
    return 'copied'


def sys3_id(rel):
    p = os.path.join(SYS3, rel)
    files = [p] if os.path.isfile(p) else sorted(glob.glob(os.path.join(p, '*')))
    for f in files:
        if os.path.isfile(f):
            m = re.search(rb'@\(#\)([^"\n]*)', open(f, 'rb').read())
            if m:
                return m.group(1).decode('latin-1').strip()
    return 'none'


def dating(orig):
    for line in open(os.path.join(W, 'host', 'OLDCC_DATES.md')):
        if line.startswith('| %s |' % orig):
            c = [x.strip() for x in line.strip().strip('|').split('|')]
            # Program, Magic, crt0 stamp, own sources, libc 09/83, D14, D12 packed/padded, D12, D1 counts, D1
            return {'crt0': c[2], 'libc83': c[4] == 'x', 'd12': c[7], 'd1': c[9]}
    return None


def compiler(d):
    if d is None:
        return None, []
    if d['d1'] == 'tight':
        return 'ccv0', ['ccv1r']
    if d['d1'] == 'slots':
        return 'ccv1', ['ccv1r', 'ccv1q']
    if re.match(r'3\.2 11/1[68]/83', d['crt0']):
        return 'ccvm', ['cc']       # the build of 11/16 .. 11/18/83 (OLDCC.md, vm)
    if d['d1'] == 'loose' and d['libc83']:
        return 'cc', ['ccv1']
    return None, ['ccv0', 'ccv1', 'cc']


def libc_end(path, starts):
    """(key, OWN_LIM, best library, covered, total, gaps, first member, equally good libraries) by the walk of whichlibc.py"""
    gc, cg = whichlibc.load(path)
    while cg > 1 and gc[cg - 2:cg] == b'\0\0':                   # E711: the code is padded with zeros to 256 bytes
        cg -= 2
    A = list(zcode.insns(gc, 0, cg, False))
    addr = [a for a, _ in A]
    NA = [whichlibc.norm(t) for _, t in A]
    libs = sorted(glob.glob(LIBDIR + '/libc-v*.a')) + [os.path.join(ROOT, 'lib', 'libc.a')]   # the shipped libc (src/lib/libc builds it identical)
    # the termlib (src/lib/libtermlib*.a) is linked before the libc (ex): its members belong to the library part too
    term = [m for t in sorted(glob.glob(LIBDIR + '/libtermlib*.a')) for m in whichlibc.members(t)]
    mem = {lib: whichlibc.members(lib) + term for lib in libs}
    first = next(k for k, a in enumerate(addr) if a >= 0x10)      # behind the jump of crt0
    res = []
    for lib in libs:
        _, rows = whichlibc.walk(gc, A, NA, addr, mem[lib], first)
        walk = []                                   # (addr, size, member or None for a gap)
        for r in rows:
            m = re.match(r'# gap ([0-9a-f]+) \.\. (end|[0-9a-f]+)', r)
            if m:
                lo = int(m.group(1), 16)
                hi = cg if m.group(2) == 'end' else int(m.group(2), 16)
                walk.append((lo, hi - lo, None))
            else:
                m = re.match(r'(\S+)\t# ([0-9a-f]+), (\d+) bytes', r)
                walk.append((int(m.group(2), 16), int(m.group(3)), m.group(1)))
        # the libc starts at the first member of at least 32 bytes (smaller fits inside the own code are stray: a ret, a
        # stub), together with the members right before it without a gap between (not a member of 2 bytes: a lone ret, the end
        # of the own function before, as fakcu)
        k = next((n for n, w in enumerate(walk) if w[2] and w[1] >= 32), None)
        if k is None:
            continue
        while k > 0 and walk[k - 1][2] and walk[k - 1][1] > 2 and walk[k - 1][0] + walk[k - 1][1] == walk[k][0]:
            k -= 1
        lim = walk[k][0]
        cov = sum(w[1] for w in walk[k:] if w[2])
        gaps = ['%x..%x (%d bytes)' % (w[0], w[0] + w[1], w[1]) for w in walk[k:] if not w[2]]
        key = (cov, -len(gaps))
        res.append((key, lim, os.path.basename(lib), cov, cg - lim, gaps, walk[k][2]))
    if not res:
        return None
    res.sort(key=lambda r: r[0])
    best = res[-1]
    ties = [r[2] for r in res[:-1] if r[0] == best[0]]
    return best + (ties,)


PROGCFG = '''"""Configuration of the recon toolkit (host/recon/progcfg.py) for %(name)s (nonsegmented); written by host/recon/setup.py."""
NAME = '%(name)s'
ORIG = '%(orig)s'
OWN_LIM = 0x%(lim)04x    # the own code: crt0 and the program; the first libc function (%(first)s) starts here
%(origlim)sCC = '/bin/%(cc)s'%(ccpad)s# the candidate of the dating (D1 %(d1)s); old compilers under WEGA_ROOT=$OLDROOT
'''


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    name = sys.argv[1]
    orig = sys.argv[2] if len(sys.argv) > 2 else 'bin/' + name
    opath = os.path.join(ROOT, orig)
    rep = {}
    ok = True
    files = []

    rep['SCCS'] = '; '.join(sccs(opath)) or '-'

    rel, target = sys3_source(name, orig)
    if rel is None:
        rep['SOURCE'] = 'none'
    elif target is None:
        rep['SOURCE'] = 'tmp/sys3/usr/src/%s (not below cmd: not copied); sys3 id: %s' % (rel, sys3_id(rel))
    else:
        how = copy_source(rel, target)
        rep['SOURCE'] = 'tmp/sys3/usr/src/%s -> %s (%s); sys3 id: %s' % (rel, target, how, sys3_id(rel))
        files.append(target)

    d = dating(orig)
    cc, nb = compiler(d)
    rep['DATE'] = 'crt0 %(crt0)s, D12 %(d12)s, D1 %(d1)s, libc 09/83 %(libc83)s' % d if d else 'not in OLDCC_DATES.md'
    rep['COMPILER'] = 'candidate %s, neighbours %s' % (cc or '?', ', '.join(nb))

    o = SOut(open(opath, 'rb').read())
    hdr = open(opath, 'rb').read(2)
    if hdr not in (b'\xe7\x07', b'\xe7\x11'):     # E711: split I/D, the code alone from address 0
        rep['RESULT'] = 'stopped: magic %s, not a nonsegmented E707 program' % hdr.hex()
        report(rep)
        sys.exit(1)
    stripped = bool(o.flag & 1)                                  # the STRIP flag of the header

    cdir = os.path.join(W, 'compare', name)
    os.makedirs(cdir, exist_ok=True)
    tdir = os.path.join(REPO, 'tmp', 'tmp', name)
    os.makedirs(tdir, exist_ok=True)
    env = dict(os.environ, RECON_PROG=name, WEGA_ROOT=ROOT)
    pc = os.path.join(cdir, 'progcfg.py')
    pc_new = not os.path.exists(pc)
    if pc_new:
        open(pc, 'w').write("NAME = %r\nORIG = %r\nOWN_LIM = 0x10000\nORIG_LIM = 0x10000\nCC = '/bin/%s'\n" % (name, orig, cc or 'cc'))
    inv = os.path.join(tdir, 'inv.json')
    r = subprocess.run([sys.executable, os.path.join(HERE, 'inventory.py')], env=env, capture_output=True, text=True)
    if r.returncode:
        rep['RESULT'] = 'stopped: inventory.py: ' + (r.stderr.strip().splitlines() or ['?'])[-1]
        report(rep)
        sys.exit(1)
    open(inv, 'w').write(r.stdout)
    iv = json.loads(r.stdout)
    starts = sorted({int(a, 16) for a in iv['proven_starts'] + iv['candidate_starts_after_ret_or_jump']})

    best = libc_end(opath, starts)
    if best is None:
        rep['RESULT'] = 'stopped: no library walk ends the own code'
        report(rep)
        sys.exit(1)
    _, lim, lib, cov, tot, gaps, first, ties = best
    own = [s for s in starts if s < lim]   # proven and candidate starts
    rep['OWN_LIM'] = '0x%04x (own functions: %d incl. crt0; first libc member: %s); stripped: %s' % (
        lim, len(own), first, 'yes' if stripped else 'no')
    rep['LIBC'] = 'best %s covers %d of %d bytes; gaps: %s; as good: %s' % (lib, cov, tot, ', '.join(gaps) or 'none',
                                                                         ', '.join(ties) or 'none')
    if gaps:
        ok = False

    if pc_new:
        open(pc, 'w').write(PROGCFG % {'name': name, 'orig': orig, 'lim': lim, 'first': first,
                                       'origlim': 'ORIG_LIM = 0x10000  # the original is stripped\n' if stripped else '',
                                       'cc': cc or 'cc', 'ccpad': ' ' * max(1, 8 - len(cc or 'cc')),
                                       'd1': d['d1'] if d else '?'})
        files.append('compare/%s/progcfg.py' % name)
    else:
        files.append('compare/%s/progcfg.py (kept)' % name)
    r = subprocess.run([sys.executable, os.path.join(HERE, 'own_table.py'), inv], env=env, capture_output=True, text=True)
    if r.returncode:
        rep['RESULT'] = 'stopped: own_table.py: ' + (r.stderr.strip().splitlines() or ['?'])[-1]
        report(rep)
        sys.exit(1)
    fj = os.path.join(cdir, 'findings.json')
    j = json.load(open(fj))
    if j.get('compiler', '?') == '?' and cc:
        j['compiler'] = cc
        json.dump(j, open(fj, 'w'), indent=1)
        open(fj, 'a').write('\n')
    files.append('compare/%s/findings.json' % name)
    rep['FILES'] = ', '.join(files)
    rep['RESULT'] = 'ok' if ok else 'ok, with gaps in the library (the main session decides)'
    report(rep)


def report(rep):
    for k in ('RESULT', 'SCCS', 'SOURCE', 'DATE', 'COMPILER', 'OWN_LIM', 'LIBC', 'FILES'):
        print('%-9s %s' % (k + ':', rep.get(k, '-')))


if __name__ == '__main__':
    main()
