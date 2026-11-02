#!/usr/bin/env python3
"""switchmatrix.py - the switches of an old compiler version and the functions that need them (host/OLDCC_SWITCHES.md).

Usage (through src/cmd/cparse/tools/oldcc.sh):
  switchmatrix.py run [SWITCH ...]   build the passes in a scratch copy, once without each switch (default: all, else only the
                                     named rows are replaced), measure every program of PROGS, write the table
  switchmatrix.py check              no measurement: are the switches and programs of the sources those of the table?

The source tree and the installed compilers (OLDROOT) stay untouched: the passes are built in RECON_TMP/matrix and installed
into a copy of OLDROOT. "Lost": exact with all switches, not exact without this one. "Gained": the other way round, a sign
that the switch hides something. The v0 bench is not run here.
"""
import os
import re
import shutil
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..'))
OLDCC = os.path.join(REPO, 'src', 'cmd', 'cparse', 'tools', 'oldcc.sh')
OUT = os.path.join(REPO, 'host', 'OLDCC_SWITCHES.md')
VER = 'v0p'
PASSES = ['codgen', 'cparse', 'popt']
# the programs measured for this version; add a new program here when it has a model (HOWTO_reconstruct.md, checklist)
PROGS = ['cparse', 'popt']
# programs with a tools/progcfg.py that are not measured, and why
NOTPROGS = {'codgen': 'v0p, no model yet', 'ld': 'v1', 'sld': 'no old compiler work yet'}
ROOT = os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or '/mnt/share/cvs/P8000/tmp/wega31_root'
OLDROOT = os.environ.get('OLDROOT', '/mnt/share/cvs/P8000/tmp/oldroot')
TMP = os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp')
EMU = os.path.join(REPO, 'host', 'z8k-emu', 'z8k-emu')
TIMEOUT = 900


def switches():
    """{switch: pass} of the version, from the sources"""
    sw = {}
    txt = open(os.path.join(REPO, 'src', 'cmd', 'codgen', 'oldcc.h')).read()
    for name in ('OLD_V0P', 'OLD_V0'):
        m = re.search(r'^#ifdef %s\n(.*?)^#endif' % name, txt, re.M | re.S)
        for s in re.findall(r'^#define (OLD_D\w+)', m.group(1), re.M):
            sw[s] = 'codgen'
    for p in ('cparse', 'popt'):
        mk = open(os.path.join(REPO, 'src', 'cmd', p, VER, 'Makefile')).read()
        m = re.search(r'^SWITCHES = (.*)$', mk, re.M)
        for s in re.findall(r'-D(OLD_\w+)', m.group(1) if m else ''):
            sw[p + ':' + s] = p
    return sw


def commit():
    return subprocess.run(['git', '-C', REPO, 'log', '-1', '--format=%h'], capture_output=True, text=True).stdout.strip()


def build(work, name, pas):
    """build pass pas without the switch in the scratch copy and install it; returns an error text or None"""
    src = os.path.join(work, 'src', 'cmd', pas)
    plain = name.split(':')[-1]
    if pas == 'codgen':
        p = os.path.join(src, 'oldcc.h')
        txt = open(p + '.all').read()
        new = re.sub(r'^#define %s\b.*\n' % plain, '', txt, count=1, flags=re.M)
        assert new != txt, name
        open(p, 'w').write(new)
    else:
        p = os.path.join(src, VER, 'Makefile')
        txt = open(p + '.all').read()
        new = re.sub(r'(^SWITCHES = .*?) ?-D%s\b' % plain, r'\1', txt, count=1, flags=re.M)
        assert new != txt, name
        open(p, 'w').write(new)
    return make(work, pas)


def make(work, pas):
    d = os.path.join(work, 'src', 'cmd', pas, VER)
    for f in os.listdir(d):
        if f.endswith('.o') or f == pas:
            os.remove(os.path.join(d, f))
    w = EMU + ' -r ' + ROOT
    cmd = ['make', 'CC=' + w + ' /bin/cc', 'CAS=' + w + ' /bin/cas', 'AR=' + w + ' /bin/ar', 'LD=' + w + ' /bin/ld',
           'LDBIN=' + w + ' /bin/ld']
    r = subprocess.run(cmd, cwd=d, capture_output=True, text=True, timeout=TIMEOUT)
    if r.returncode or not os.path.exists(os.path.join(d, pas)):
        return (r.stdout + r.stderr)[-300:]
    shutil.copy(os.path.join(d, pas), os.path.join(work, 'root', 'lib', VER, pas))
    return None


def measure(work, prog):
    """{function: lines of its difference}, or None when the model fails"""
    env = dict(os.environ, OLDROOT=os.path.join(work, 'root'), RECON_TMP=os.path.join(work, 'tmp'))
    res = os.path.join(work, 'tmp', 'oldcc_model_%s.txt' % prog)
    if os.path.exists(res):
        os.remove(res)
    try:
        subprocess.run(['sh', OLDCC, 'model', prog], env=env, capture_output=True, text=True, timeout=TIMEOUT)
    except subprocess.TimeoutExpired:
        return None
    if not os.path.exists(res):
        return None
    d, seen = {}, False
    for line in open(res):
        f = line.split()
        if len(f) > 2 and f[-1] == 'SAME':
            seen = True
        if len(f) > 2 and f[-2] == 'DIFF':
            d[f[0]] = int(f[-1])
            seen = True
    return d if seen else None


def setup():
    work = os.path.join(TMP, 'matrix')
    shutil.rmtree(work, ignore_errors=True)
    os.makedirs(os.path.join(work, 'tmp'))
    os.makedirs(os.path.join(work, 'src', 'cmd'))
    os.symlink(os.path.join(REPO, 'src', 'lib'), os.path.join(work, 'src', 'lib'))
    subprocess.run(['cp', '-a', OLDROOT, os.path.join(work, 'root')], check=True)
    for p in PASSES:
        subprocess.run(['cp', '-a', os.path.join(REPO, 'src', 'cmd', p), os.path.join(work, 'src', 'cmd', p)], check=True)
    for f in ('codgen/oldcc.h', 'cparse/%s/Makefile' % VER, 'popt/%s/Makefile' % VER):
        p = os.path.join(work, 'src', 'cmd', f)
        shutil.copy(p, p + '.all')
    return work


def restore(work, pas):
    src = os.path.join(work, 'src', 'cmd', pas)
    f = 'oldcc.h' if pas == 'codgen' else os.path.join(VER, 'Makefile')
    shutil.copy(os.path.join(src, f + '.all'), os.path.join(src, f))
    shutil.copy(os.path.join(work, 'base', pas), os.path.join(work, 'root', 'lib', VER, pas))


def read_rows():
    rows = {}
    if os.path.exists(OUT):
        for line in open(OUT):
            m = re.match(r'\| (\S*OLD_\w+) \|', line)
            if m:
                rows[m.group(1)] = line.rstrip('\n')
    return rows


def write(rows, base, stamp):
    with open(OUT, 'w') as f:
        f.write('# The switches of %s and the functions that need them\n\n' % VER)
        f.write('Generated by `oldcc.sh matrix` (host/recon/switchmatrix.py), do not edit. What a switch does: `OLDCC.md`.\n\n')
        f.write('Programs: %s. Baseline at commit %s: %s.\n\n' % (', '.join(PROGS), stamp, base))
        f.write('Lost: exact with all switches, not without this one. Gained: exact only without it. Lines: differing lines '
                'without the switch minus those with all switches, over all programs. A pass name in front of a switch: not '
                'codgen. `no build`, `no model`: the switch cannot be taken out alone.\n\n')
        f.write('| Switch | Lost | Gained | Lines | Measured |\n|---|---|---|---|---|\n')
        for k in sorted(rows):
            f.write(rows[k] + '\n')


def run(names):
    sw = switches()
    if names:
        bad = [n for n in names if n not in sw]
        if bad:
            sys.exit('unknown switch: ' + ' '.join(bad))
    todo = names or sorted(sw)
    work = setup()
    os.makedirs(os.path.join(work, 'base'))
    for p in PASSES:
        e = make(work, p)
        if e:
            sys.exit('baseline build of %s failed: %s' % (p, e))
        shutil.copy(os.path.join(work, 'src', 'cmd', p, VER, p), os.path.join(work, 'base', p))
    base = {p: measure(work, p) for p in PROGS}
    if None in base.values():
        sys.exit('baseline model failed')
    stamp = commit()
    btxt = ', '.join('%s %d differing functions, %d lines' % (p, len(base[p]), sum(base[p].values())) for p in PROGS)
    print('baseline', btxt, flush=True)
    rows = {} if not names else read_rows()
    for i, name in enumerate(todo):
        t = time.time()
        pas = sw[name]
        err = build(work, name, pas)
        if err:
            row = '| %s | no build | | | %s |' % (name, stamp)
        else:
            res = {p: measure(work, p) for p in PROGS}
            if None in res.values():
                row = '| %s | no model (%s) | | | %s |' % (name, ' '.join(p for p in PROGS if res[p] is None), stamp)
            else:
                lost, gained, lines = [], [], 0
                for p in PROGS:
                    lost += ['%s %s' % (p, f) for f in sorted(res[p]) if f not in base[p]]
                    gained += ['%s %s' % (p, f) for f in sorted(base[p]) if f not in res[p]]
                    lines += sum(res[p].values()) - sum(base[p].values())
                row = '| %s | %s | %s | %+d | %s |' % (name, ', '.join(lost) or '-', ', '.join(gained) or '-', lines, stamp)
        restore(work, pas)
        rows[name] = row
        write(rows, btxt, stamp)
        print('%d/%d %.0fs %s' % (i + 1, len(todo), time.time() - t, row), flush=True)
    shutil.rmtree(work, ignore_errors=True)


def check():
    bad = 0
    sw = switches()
    rows = read_rows()
    for s in sorted(set(sw) - set(rows)):
        print('not in the table (new switch: oldcc.sh matrix %s): %s' % (s, s))
        bad = 1
    for s in sorted(set(rows) - set(sw)):
        print('in the table but gone from the sources (oldcc.sh matrix): %s' % s)
        bad = 1
    cmd = os.path.join(REPO, 'src', 'cmd')
    for d in sorted(os.listdir(cmd)):
        if os.path.exists(os.path.join(cmd, d, 'tools', 'progcfg.py')) and d not in PROGS and d not in NOTPROGS:
            print('program without a decision (add it to PROGS or NOTPROGS in host/recon/switchmatrix.py): %s' % d)
            bad = 1
    stamps = set(re.findall(r'\| (\w+) \|$', '\n'.join(rows.values()), re.M))
    for st in sorted(stamps):
        r = subprocess.run(['git', '-C', REPO, 'diff', '--name-only', st, 'HEAD', '--'] +
                           [os.path.join('src', 'cmd', p) for p in PASSES], capture_output=True, text=True)
        n = len([x for x in r.stdout.split('\n') if x and 'findings' not in x])
        print('measured at %s: %d files of the passes changed since' % (st, n))
    print('check: ' + ('differences, see above' if bad else 'switches and programs as in the table'))
    return bad


if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == 'check':
        sys.exit(check())
    if len(sys.argv) > 1 and sys.argv[1] == 'run':
        run(sys.argv[2:])
    else:
        sys.exit(__doc__)
