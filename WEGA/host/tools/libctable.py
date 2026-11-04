#!/usr/bin/env python3
"""libctable.py [-o OUT] [SOURCE ...]: which C source and which compiler give which libc object.

Every SOURCE (default: the C files of src/lib/libc and of the System III libc, SYS3LIBC) is compiled with every compiler
(shipped cc, ccv1, ccv0, ccv0p; with and without -O; with the headers of the guest and with the stdio.h of cparse). The
object is compared with the members of the shipped /lib/libc.a (cmp) and with the versions in src/lib/oldlibc and
src/lib/libc/oldcc (instructions with addresses masked, size of data; a module of oldlibc is cut from a program and has
no relocation, so this is a fit, and the bytes of code and data that differ are printed: 0 0 is the best); the external
names must be the same.
One line per fit: SOURCE COMPILER FLAGS HEADERS KIND MODULE BYTES-CODE BYTES-DATA. The proof is the cmp of a program that
is linked with the object, make -C host checkall.

Environment: WEGA, OLDROOT (with bin/ccvN), JOBS, LIBC_TMP (scratch, default /tmp/wega_libc).
"""
import concurrent.futures
import glob
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', 'recon'))
import zcode  # noqa: E402
from sout import SOut  # noqa: E402

REPO = os.path.normpath(os.path.join(HERE, '..', '..'))
WEGA = os.environ.get('WEGA', os.path.normpath(os.path.join(REPO, '..', 'tmp', 'wega31_root')))
OLDROOT = os.environ.get('OLDROOT', os.path.normpath(os.path.join(REPO, '..', 'tmp', 'oldroot')))
SYS3 = os.environ.get('SYS3LIBC', '/mnt/users/olivleh1/BACKUPs/Operating_Systems/SYSIII/sys3/usr/src/lib/libc')
TMP = os.environ.get('LIBC_TMP', '/tmp/wega_libc')
EMU = os.path.join(REPO, 'host', 'z8k-emu', 'z8k-emu')
COMPILERS = [('cc', WEGA, '/bin/cc'), ('ccv1', OLDROOT, '/bin/ccv1'), ('ccv0', OLDROOT, '/bin/ccv0'), ('ccv0p', OLDROOT, '/bin/ccv0p')]


def norm(t):
    return re.sub(r'%[0-9A-Fa-f]{1,8}\b', 'A', t)


def parts(path):
    o = SOut(open(path, 'rb').read())
    s = o.segs[0]
    img = bytes(o.image)
    return img[:s['code']], img[s['code']:s['code'] + s['data']]


def names(path):
    """the external names of an object: what it defines and what it needs. __fltuse: cc names it in every object that uses
    double; a module cut from a program cannot show it"""
    return frozenset(x['name'] for x in SOut(open(path, 'rb').read()).symbols if x['type'] in (32, 34) and x['name'] != '__fltuse')


def key(code):
    return tuple(norm(t) for _, t in zcode.insns(code, 0, len(code), False))


def targets():
    """masked code -> [(kind, name, code, data)]; also the objects of the shipped members by name"""
    work = os.path.join(TMP, 'orig')
    os.makedirs(work, exist_ok=True)
    subprocess.run([EMU, '-r', WEGA, '/bin/ar', 'x', '/lib/libc.a'], cwd=work, capture_output=True)
    res, shipped = {}, {}
    for f in sorted(os.listdir(work)):
        if f.endswith('.o'):
            shipped[f[:-2]] = open(os.path.join(work, f), 'rb').read()
            c, d = parts(os.path.join(work, f))
            if c:
                res.setdefault(key(c), []).append(('shipped', f[:-2], c, d, names(os.path.join(work, f))))
    mods = os.path.join(TMP, 'mods')
    os.makedirs(mods, exist_ok=True)
    for top, kind in ((os.path.join(REPO, 'src/lib/oldlibc'), 'old'), (os.path.join(REPO, 'src/lib/libc/oldcc'), 'oldcc')):
        for s in sorted(glob.glob(top + '/*/*.s')):
            n = s.split('/')[-2] + '/' + os.path.basename(s)[:-2]
            o = os.path.join(mods, kind + '_' + n.replace('/', '_') + '.o')
            if not os.path.exists(o):
                shutil.copy(s, os.path.join(mods, 'x.s'))
                subprocess.run([EMU, '-r', WEGA, '/bin/cas', '-u', '-o', os.path.basename(o), 'x.s'], cwd=mods, capture_output=True)
            if os.path.exists(o):
                c, d = parts(o)
                if c:
                    res.setdefault(key(c), []).append((kind, n, c, d, names(o)))
    return res, shipped


def compile_one(src, tag, comp, flags, headers, T, shipped):
    name, root, cc = comp
    base = os.path.basename(src)
    obj = base[:-2] + '.o'
    work = os.path.join(TMP, 'w', '%s_%s_%s_%s_%s' % (tag, base[:-2], name, flags.strip('-') or 'n', headers))
    os.makedirs(work, exist_ok=True)
    for f in glob.glob(os.path.dirname(src) + '/*.h'):
        shutil.copy(f, work)
    shutil.copy(src, work)
    if headers == 'cparse':
        shutil.copy(os.path.join(REPO, 'src/cmd/cparse/stdio.h'), work)
    argv = [EMU, '-r', root, cc] + ([flags] if flags else []) + ['-I.', '-c', base]
    out = []
    try:
        subprocess.run(argv, cwd=work, capture_output=True, timeout=120)
        p = os.path.join(work, obj)
        if os.path.exists(p):
            raw = open(p, 'rb').read()
            c, d = parts(p)
            if base[:-2] in shipped and raw == shipped[base[:-2]]:
                out.append(('shipped=', base[:-2], 0, 0))
            nm = names(p)
            for kind, n, c2, d2, nm2 in T.get(key(c), []) if c else []:
                if len(d) == len(d2) and nm == nm2:
                    out.append((kind, n, sum(x != y for x, y in zip(c, c2)), sum(x != y for x, y in zip(d, d2))))
    except Exception as e:
        out.append(('error', str(e)[:40], 0, 0))
    shutil.rmtree(work, ignore_errors=True)
    return [(src, name, flags or '-', headers, *x) for x in out]


def main():
    a = sys.argv[1:]
    out = sys.stdout
    if '-o' in a:
        i = a.index('-o')
        out = open(a[i + 1], 'w', buffering=1)
        del a[i:i + 2]
    srcs = a
    if not srcs:
        for top in (os.path.join(REPO, 'src/lib/libc'), SYS3 + '/pdp11', SYS3 + '/vax'):
            for d in ('gen', 'stdio', 'sys'):
                srcs += sorted(glob.glob(os.path.join(top, d, '*.c')))
        srcs += sorted(glob.glob(os.path.join(REPO, 'src/lib/oldlibc/*/*.c')))
    T, shipped = targets()
    jobs = int(os.environ.get('JOBS') or max(1, (os.cpu_count() or 2) - 1))
    with concurrent.futures.ThreadPoolExecutor(jobs) as ex:
        futs = []
        for k, s in enumerate(srcs):
            has = 'stdio.h' in open(s, errors='replace').read()
            for comp in COMPILERS:
                for fl in ('-O', ''):
                    for h in (('guest', 'cparse') if has and comp[0] != 'cc' else ('guest',)):
                        futs.append(ex.submit(compile_one, s, str(k), comp, fl, h, T, shipped))
        for f in concurrent.futures.as_completed(futs):
            for r in f.result():
                print('\t'.join(str(x) for x in r), file=out)
    print('compiled', len(futs), file=sys.stderr)


if __name__ == '__main__':
    main()
