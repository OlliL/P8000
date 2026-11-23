#!/usr/bin/env python3
"""libsetup.py NAME - the mechanical start of the reconstruction of a library of /lib or /usr/lib (host/HOWTO_reconstruct.md).

NAME is the library without the s of the segmented one, e.g. libm: it takes lib/libm.a and lib/slibm.a (or usr/lib/...), as
many of them as the WEGA root has (WEGA=dir names the root; OLDROOT the old compilers, default tmp/oldroot). In order:
  1. the members of each archive (war.py), and Library.SC: the SCCS lines of the sources, with their dates;
  2. the source of each member, from System III (tmp/sys3/usr/src/lib/NAME), else 2.11BSD (usr.lib/NAME), else 4.4BSD-Lite
     (usr/src/lib/NAME; BSD=dir names the directory of both trees): STEM.c or .s (STEM: the member without .o, and without
     the .s of a segmented member such as main.s.o); a member without one is named (it needs a source by hand, or is skipped);
  3. per member every compiler of its kind (nonsegmented: cc, ccv1, ccv1c, ccv1q, ccv1r, ccv0, ccv0a, ccv0p, ccvl; segmented: scc, sccv1, sccv1c,
     sccvk, sccvl, sccvc), with and without -O, with the guest's headers and with the stdio.h of src/head/oldv7 and
     src/head/oldcptr: the object against the member (EXACT, or the bytes that differ); the best one is reported;
  4. src/lib/NAME/ after the pattern of src/lib/libl: the sources and the headers of their tree (.h, .ext; copied unchanged; an
     existing file is kept), mkar.sh, for
     each archive arhdr_ARCHIVE.s (the magic and the member headers of the original) and Library.SC (taken from the
     original; Library_ARCHIVE.SC when the archives have different ones).
The Makefile is not written (the pattern: src/lib/libl/Makefile). Writes only src/lib/NAME/ and tmp/tmp/NAME/.
"""
import concurrent.futures
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
W = os.path.normpath(os.path.join(HERE, '..', '..'))
REPO = os.path.dirname(W)
sys.path.insert(0, os.path.join(W, 'host', 'tools'))
import war  # noqa: E402

ROOT = os.environ.get('WEGA', os.path.join(REPO, 'tmp', 'wega31_root'))
OLD = os.environ.get('OLDROOT', os.path.join(REPO, 'tmp', 'oldroot'))
SYS3 = os.path.join(REPO, 'tmp', 'sys3', 'usr', 'src', 'lib')
BSD = os.environ.get('BSD', '/mnt/users/olivleh1/BACKUPs/Operating_Systems')
ALIAS = {'libtermlib': ['libtermlib', 'libterm', 'libtermcap']}	# the same library under another name in BSD


def roots(name):
    """the directories that may hold the sources of library NAME, in this order: System III, 2.11BSD, 4.4BSD-Lite (owner 2026-11-18)"""
    out = []
    for n in ALIAS.get(name, [name]):
        for d in (os.path.join(SYS3, n), os.path.join(BSD, '2.11BSD', 'usr.lib', n), os.path.join(BSD, '2.11BSD', 'lib', n),
                  os.path.join(BSD, '4.4BSD-Lite', 'usr', 'src', 'lib', n)):
            if os.path.isdir(d) and d not in out:
                out.append(d)
    own = os.path.join(W, 'src', 'lib', name)		# no tree has one: the sources written from the objects (libscreen)
    if not out and os.path.isdir(own) and any(f.endswith('.c') for f in os.listdir(own)):
        out.append(own)
    return out
EMU = os.path.join(W, 'host', 'z8k-emu', 'z8k-emu')
NONSEG = [('cc', ROOT), ('ccv1', OLD), ('ccv1c', OLD), ('ccv1q', OLD), ('ccv1r', OLD), ('ccv0', OLD), ('ccv0a', OLD), ('ccv0p', OLD), ('ccvl', OLD)]
SEG = [('scc', ROOT), ('sccv1', OLD), ('sccv1c', OLD), ('sccvk', OLD), ('sccvl', OLD), ('sccvc', OLD)]
INCLUDES = ('.h', '.ext')		# the files a source of the library includes from its own directory
HEADERS = [('guest', None), ('oldv7', 'src/head/oldv7'), ('oldcptr', 'src/head/oldcptr')]


def archives(name):
    out = []
    for d in ('lib', 'usr/lib'):
        for a in (name, 's' + name):
            p = os.path.join(ROOT, d, a + '.a')
            if os.path.exists(p):
                out.append((d + '/' + a + '.a', a == 's' + name))
    return out


def members(path):
    return list(war.members(open(path, 'rb').read()))


def skip(dp):
    """machine code of other systems"""
    return any(x in dp for x in ('/pdp11', '/vax', '/hp300', '/i386', '/sparc', '/mips', '/luna68k', '/news3400', '/tahoe'))


def find_source(name, stem, bases=None):
    for base in bases or roots(name):
        for ext in ('.c', '.s'):
            p = os.path.join(base, stem + ext)
            if os.path.exists(p):
                return p
        for dp, dn, fn in os.walk(base):
            if skip(dp):
                continue
            for ext in ('.c', '.s'):
                if stem + ext in fn:
                    return os.path.join(dp, stem + ext)
    return None


def short(p):
    for pre, tag in ((SYS3 + '/', 'sys3:'), (os.path.join(BSD, '2.11BSD') + '/', '2.11BSD:'), (os.path.join(BSD, '4.4BSD-Lite') + '/', '4.4BSD:')):
        if p.startswith(pre):
            return tag + p[len(pre):]
    return p


def try_one(src, body, comp, root, opt, hdr, tmp):
    w = tempfile.mkdtemp(dir=tmp)
    try:
        for f in os.listdir(os.path.dirname(src)):
            if f.endswith(INCLUDES):
                shutil.copy(os.path.join(os.path.dirname(src), f), w)
        shutil.copy(src, w)
        if hdr[1]:
            for f in os.listdir(os.path.join(W, hdr[1])):
                if f.endswith('.h'):
                    shutil.copy(os.path.join(W, hdr[1], f), w)
        b = os.path.basename(src)
        args = [EMU, '-r', root, '/bin/' + comp] + (['-O'] if opt else []) + ['-I.', '-c', b]
        try:
            subprocess.run(args, cwd=w, capture_output=True, timeout=300)
        except subprocess.TimeoutExpired:		# scc -O on libu8000's disasm.c runs on and on
            return None
        o = os.path.join(w, b[:-2] + '.o')
        if not os.path.exists(o):
            return None
        mine = open(o, 'rb').read()
        if mine == body:
            return 0
        n = min(len(mine), len(body))
        return sum(1 for i in range(n) if mine[i] != body[i]) + abs(len(mine) - len(body))
    finally:
        shutil.rmtree(w, ignore_errors=True)


def best(src, body, seg, tmp):
    comps = SEG if seg else NONSEG
    hdrs = HEADERS if 'stdio.h' in open(src, errors='replace').read() else HEADERS[:1]
    jobs = [(c, r, o, h) for c, r in comps for o in (True, False) for h in hdrs]
    res = []
    with concurrent.futures.ThreadPoolExecutor(8) as ex:
        futs = {ex.submit(try_one, src, body, c, r, o, h, tmp): (c, o, h[0]) for c, r, o, h in jobs}
        for f in concurrent.futures.as_completed(futs):
            d = f.result()
            if d is not None:
                res.append((d,) + futs[f])
    res.sort()
    return res


def sources(name, bases=None):
    for base in bases or roots(name):
        for dp, dn, fn in os.walk(base):
            if skip(dp):
                continue
            for f in sorted(fn):
                if f.endswith('.c'):
                    yield os.path.join(dp, f)


def by_content(name, body, seg, tmp, used, bases=None):
    """the free source whose object (the first compiler of its kind, -O) is nearest to the member: at most half of it differs"""
    comp, root = (SEG if seg else NONSEG)[0]
    res = []
    for src in sources(name, bases):
        if src in used:
            continue
        d = try_one(src, body, comp, root, True, HEADERS[0], tmp)
        if d is not None:
            res.append((d, src))
    res.sort()
    return res[0][1] if res and res[0][0] <= len(body) // 2 else None


def arhdr(arname, mems, pad=None):
    lines = ['// arhdr_%s.s: the magic and the member headers of the shipped %s, in the order of the archive (ar(5): name 14,'
             % (arname, arname), '// date 4, uid, gid, mode 2, size 4 bytes); mkar.sh joins them with the members. Written by'
             ' host/recon/libsetup.py.', '\t.psec', '\t.data', '_arhdr::', '\t.byte\t%ff,%65']
    for m in mems:
        raw = m['raw'] if 'raw' in m else None
        h = raw if raw else None
        if h is None:
            return None
        lines.append('// ' + m['name'])
        lines.append('\t.byte\t' + ','.join('%%%02x' % x for x in h[:13]))
        lines.append('\t.byte\t' + ','.join('%%%02x' % x for x in h[13:26]))
    if pad is not None:		# an odd last member: the padding byte of the original (what was in ar's buffer), mkar.sh appends it
        lines.append('// padding')
        lines.append('\t.byte\t%%%02x' % pad)
    return '\n'.join(lines) + '\n'


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    name = sys.argv[1]
    tmp = os.path.join(REPO, 'tmp', 'tmp', name)
    os.makedirs(tmp, exist_ok=True)
    dst = os.path.join(W, 'src', 'lib', name)
    ars = archives(name)
    if not ars:
        sys.exit('no archive %s.a in lib or usr/lib of %s' % (name, ROOT))
    if not roots(name):
        print('RESULT:   skipped: no source directory for %s (System III, 2.11BSD, 4.4BSD-Lite, src/lib/%s)' % (name, name))
        return
    print('SOURCES:  ' + ', '.join(roots(name)))
    os.makedirs(dst, exist_ok=True)
    if not os.path.exists(os.path.join(dst, 'mkar.sh')):
        shutil.copy(os.path.join(W, 'src', 'lib', 'libl', 'mkar.sh'), dst)
    scs = {}
    nosrc = set()
    for path, seg in ars:
        data = open(os.path.join(ROOT, path), 'rb').read()
        mems = list(war.members(data))
        arname = os.path.basename(path)[:-2]
        print('== %s (%s, %d members)' % (path, 'segmented' if seg else 'nonsegmented', len(mems)))
        for m in mems:
            m['raw'] = data[m['off'] - 26:m['off']]
        last = mems[-1]
        pad = data[last['off'] + last['size']] if last['size'] % 2 else None
        libl_mkar = open(os.path.join(W, 'src', 'lib', 'libl', 'mkar.sh')).read()
        if pad is not None and open(os.path.join(dst, 'mkar.sh')).read() == libl_mkar:
            shutil.copy(os.path.join(W, 'src', 'lib', 'libc', 'mkar.sh'), dst)		# the one that appends the padding byte
        open(os.path.join(dst, 'arhdr_%s.s' % arname), 'w').write(arhdr(arname, mems, pad))
        tally = {}
        used = set()
        for m in mems:
            if m['name'] == 'Library.SC':
                scs[arname] = m['body']
                continue
            stem = m['name'][:-2]
            if stem.endswith('.s'):
                stem = stem[:-2]
            # one source tree after the other (System III first): by name, else by content; WEGA renamed some files
            # (liby: libmai.c is main.c), which 4.4BSD has under the name of the member
            src, how = None, ''
            for base in roots(name):
                src = find_source(name, stem, [base])
                if not src:
                    src = by_content(name, m['body'], seg, tmp, used, [base])
                    how = ' (by content)' if src else ''
                if src:
                    break
            if not src:
                nosrc.add(stem)
                print('  %-14s no System III source' % m['name'])
                continue
            used.add(src)
            for f in os.listdir(os.path.dirname(src)):		# the headers of the source tree go with it (curses.h, curses.ext)
                if f.endswith(INCLUDES) and not os.path.exists(os.path.join(dst, f)):
                    shutil.copyfile(os.path.join(os.path.dirname(src), f), os.path.join(dst, f))
                    os.chmod(os.path.join(dst, f), 0o644)
            # the source under WEGA's name (the member's: Library.SC names them so), and compiled under it (the object holds it)
            local = os.path.join(dst, stem + src[-2:])
            if not os.path.exists(local):
                shutil.copyfile(src, local)		# not the mode: the BSD trees are read-only
                os.chmod(local, 0o644)
            res = best(local, m['body'], seg, tmp)
            if not res:
                print('  %-14s %s: no compiler made an object' % (m['name'], short(src)))
                continue
            d, c, o, h = res[0]
            ties = [r for r in res if r[0] == d]
            tally[(c, o, h)] = tally.get((c, o, h), 0) + (1 if d == 0 else 0)
            print('  %-14s %-28s %s %s%s%s' % (m['name'], short(src) + how,
                  'EXACT' if d == 0 else '%d bytes differ' % d, c, ' -O' if o else '', '' if h == 'guest' else ' ' + h)
                  + ('  (as good: %d more)' % (len(ties) - 1) if len(ties) > 1 else ''))
        if tally:
            top = sorted(tally.items(), key=lambda kv: -kv[1])[:3]
            print('  exact per compiler: ' + '; '.join('%s%s%s %d' % (c, ' -O' if o else '', '' if h == 'guest' else ' ' + h, n)
                                                    for (c, o, h), n in top))
    if scs:
        vals = list(scs.values())
        if all(v == vals[0] for v in vals):
            open(os.path.join(dst, 'Library.SC'), 'wb').write(vals[0])
        else:
            for a, v in scs.items():
                open(os.path.join(dst, 'Library_%s.SC' % a), 'wb').write(v)
        print('Library.SC:')
        for line in vals[0].decode('latin-1').splitlines()[:40]:
            print('  ' + line.strip())
    print('NO SOURCE: ' + (', '.join(sorted(nosrc)) or '-'))
    print('FILES:    src/lib/%s/ (sources, mkar.sh, arhdr_*.s, Library.SC); the Makefile after src/lib/libl/Makefile' % name)


if __name__ == '__main__':
    main()
