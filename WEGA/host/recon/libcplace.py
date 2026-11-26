#!/usr/bin/env python3
"""libcplace.py [-u vN=PROG,PROG ...] PROG [START]: which libc-vN can take a nonsegmented program (manual libc assignment,
when the walk of host/tools/whichlibc.py leaves gaps).

PROG and the users are paths below the WEGA root (bin/more) or names looked up in bin, usr/bin, etc, usr/lib, lib.
START (hex) is the start of the libc part; default: behind the last run of 16 instructions that no member fits (the own
code), the first member of at least 32 bytes with the members right in front of it (as HOWTO_reconstruct.md).

1. The members the program needs: its libc part walked with every module version of src/lib/oldlibc (NAME/0_n.o) and the
   objects of src/lib/libc/build/nonseg (version "libc"); a member is the module whose masked instructions fit, with all
   versions that fit (relocated addresses differ, so the bytes are not compared). make -C src/lib/oldlibc first.
2. Member collisions per library: a module the library has in another version. The libraries are the LIBvN lists of
   src/lib/oldlibc/Makefile (in their order) and libc-v12, v14, v15 and the shipped libc.a (the archive order of
   src/lib/libc/build/nonseg/libc.a with the replaced members).
3. Order, for the libraries without member collisions: the users of the library (the programs whose Makefile in src/cmd
   links it: $(LIBC_VN) or libc-vN.a; -u replaces the list) are walked the same way; every user fixes a before b for each
   two members it holds in that order. A pair the program needs the other way round while the users force it
   (transitively) is a collision. A pair the Makefile list has the other way round but no user forces is free (the list
   can be changed).
4. The SCCS stamps (host/OLDCC_DATES.md: crt0, the newest source stamps) of the users and of the program.
Output: markdown tables.
"""
import glob
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(TOP, 'host', 'tools'))
import zcode  # noqa: E402
from sout import SOut  # noqa: E402
import war  # noqa: E402

WEGA = os.environ.get('WEGA', os.path.normpath(os.path.join(TOP, '..', 'tmp', 'wega31_root')))
OLDLIBC = os.path.join(TOP, 'src', 'lib', 'oldlibc')
LIBCOBJ = os.path.join(TOP, 'src', 'lib', 'libc', 'build', 'nonseg')
SKIP = ('fakcu',)
GAPRUN = 16


def norm(t):
    return re.sub(r'%[0-9A-Fa-f]{1,8}\b', 'A', t)


def load(path):
    o = SOut(open(path, 'rb').read())
    c = o.segs[0]['code']
    return bytes(o.image)[:c], c


def objects():
    out = []
    for p in sorted(glob.glob(OLDLIBC + '/*/0_*.o')):
        out.append((p.split('/')[-2], os.path.basename(p)[:-2], p))
    for p in sorted(glob.glob(LIBCOBJ + '/*.o')):
        if 'crt0' not in os.path.basename(p):
            out.append((os.path.basename(p)[:-2], 'libc', p))
    M = []
    for mod, ver, p in out:
        try:
            code, c = load(p)
        except Exception:
            continue
        if c:
            M.append((mod, ver, code, [norm(t) for _, t in zcode.insns(code, 0, c, False)]))
    return M


def walk(path, M, start=None):
    """[(addr, module, {versions}, size)] from start (None: the libc start as in HOWTO_reconstruct.md)."""
    gc, cg = load(path)
    A = list(zcode.insns(gc, 0, cg, False))
    addr = [a for a, _ in A]
    NA = [norm(t) for _, t in A]
    i = 0 if start is None else next(k for k, a in enumerate(addr) if a >= start)
    rows = []
    while i < len(A):
        # the best fit: the size, then the equal bytes (the number of a syscall is masked with the addresses)
        hits = [((len(code), sum(x == y for x, y in zip(gc[addr[i]:addr[i] + len(code)], code))), mod, ver, len(N))
                for mod, ver, code, N in M if N and NA[i:i + len(N)] == N]
        if not hits:
            rows.append(None)
            i += 1
            continue
        key = max(h[0] for h in hits)
        top = [h for h in hits if h[0] == key]
        size = key[0]
        mod = top[0][1]
        rows.append((addr[i], mod, {h[2] for h in hits if h[1] == mod and h[0][0] == size}, size))
        i += top[0][3]
    if start is None:	# behind the last run of GAPRUN instructions no member fits (the own code): the first member of
        k0, run = 0, 0	# 32 bytes or more there, with the members right in front of it
        for k, r in enumerate(rows):
            run = run + 1 if r is None else 0
            if run >= GAPRUN:
                k0 = k + 1
        k = next((k for k in range(k0, len(rows)) if rows[k] and rows[k][3] >= 32), len(rows))
        while k > k0 and rows[k - 1]:
            k -= 1
        rows = rows[k:]
    return [r for r in rows if r and r[1] not in SKIP]


def order(rows):
    out = []
    for r in rows:
        if r[1] not in out:
            out.append(r[1])
    return out


def libraries():
    mk = open(os.path.join(OLDLIBC, 'Makefile')).read().replace('\\\n', ' ')
    libs = {}
    for m in re.finditer(r'^LIBv(\d+)\s*=(.*)$', mk, re.M):
        L = []
        for t in m.group(2).split():
            if t.startswith('../libc/build/nonseg/'):
                L.append((os.path.basename(t)[:-2], 'libc'))
            elif re.match(r'\w+/0_\d+\.o$', t):
                mod, v = t.split('/')
                L.append((mod, v[:-2]))
        libs['v' + m.group(1)] = L
    arch = os.path.join(LIBCOBJ, 'libc.a')
    if os.path.exists(arch):
        names = [m['name'].strip()[:-2] for m in war.members(open(arch, 'rb').read()) if m['name'].strip().endswith('.o')]
        for n, rep in (('v12', 'printf doprnt'), ('v14', 'data filbuf doscan printf doprnt malloc'), ('v15', 'doprnt'), ('libc.a', '')):
            rep = rep.split()
            libs[n] = [(x, n if x in rep else 'libc') for x in names]	# a replaced member: a version of its own
    return libs


def users(lib):
    """Programs of src/cmd whose rule or Makefile links libc-vN."""
    n = lib[1:]
    pat = re.compile(r'\$\(LIBC_V%s\)|libc-v%s\.a' % (n, n))
    out = set()
    for mf in glob.glob(os.path.join(TOP, 'src', 'cmd', '**', 'Makefile'), recursive=True):
        d = os.path.dirname(mf)
        txt = open(mf, errors='replace').read().replace('\\\n', ' ')
        if os.path.normpath(d) == os.path.join(TOP, 'src', 'cmd'):
            for line in txt.split('\n'):
                m = re.match(r'^([\w.-]+)\s*:[^=]*$', line)
                if m and pat.search(line):
                    out.add(m.group(1))
        elif re.search(r'^\s*LIBC\s*=\s*\S*libc-v%s\.a\s*$' % n, txt, re.M):
            out.add(os.path.basename(d))
    return sorted(out)


def find(name):
    if '/' in name:
        return os.path.join(WEGA, name)
    for d in ('bin', 'usr/bin', 'etc', 'usr/lib', 'lib'):
        p = os.path.join(WEGA, d, name)
        if os.path.isfile(p):
            return p
    return None


def stamps(name):
    rel = os.path.relpath(find(name) or name, WEGA)
    for line in open(os.path.join(TOP, 'host', 'OLDCC_DATES.md')):
        c = [x.strip() for x in line.split('|')]
        if len(c) > 4 and c[1] == rel:
            return c[3] or '-', c[4] or '-'
    return '?', '?'


def magic(path):
    return open(path, 'rb').read(2).hex()


def main():
    a = sys.argv[1:]
    override = {}
    while a and a[0] == '-u':
        k, v = a[1].split('=')
        override[k] = v.split(',')
        a = a[2:]
    prog = a[0]
    start = int(a[1], 16) if len(a) > 1 else None
    path = find(prog)
    M = objects()
    need = walk(path, M, start)
    names = order(need)
    vers = {}
    for r in need:
        vers.setdefault(r[1], set()).update(r[2])
    print('## %s: the members it needs (from %04x)\n' % (prog, need[0][0]))
    print(' '.join('%s %s' % (m, '/'.join(sorted(vers[m]))) for m in names) + '\n')
    libs = libraries()
    print('## Members\n\n| libc | collisions | missing |\n|---|---|---|')
    clean = []
    for n in sorted(libs, key=lambda x: (not x[1:].isdigit(), int(x[1:]) if x[1:].isdigit() else 0)):
        have = {}
        for mod, v in libs[n]:
            have.setdefault(mod, []).append(v)
        coll = ['%s %s (needs %s)' % (m, '/'.join(have[m]), '/'.join(sorted(vers[m])))
                for m in names if m in have and any(v not in vers[m] for v in have[m])]
        miss = [m for m in names if m not in have]
        print('| %s | %d%s | %d%s |' % (n, len(coll), ': ' + ', '.join(coll) if coll else '', len(miss), ': ' + ', '.join(miss) if miss else ''))
        if not coll:
            clean.append(n)
    print('\n## Order (libraries without member collisions)\n')
    print('| libc | users (walked) | forced by the users | free (only the list) |\n|---|---|---|---|')
    keep = []
    for n in clean:
        us = override.get(n) or users(n)
        adj, walked = {}, []
        for u in us:
            p = find(u)
            if not p or magic(p) not in ('e707', 'e711'):
                continue
            s = order(walk(p, M))
            walked.append(u)
            for i, x in enumerate(s):
                adj.setdefault(x, set()).update(s[i + 1:])

        def before(x, y):
            seen, st = set(), [x]
            while st:
                z = st.pop()
                for w in adj.get(z, ()):
                    if w == y:
                        return True
                    if w not in seen:
                        seen.add(w)
                        st.append(w)
            return False
        pos = {}
        for k, (mod, v) in enumerate(libs[n]):
            pos.setdefault(mod, k)
        forced, free = [], []
        for i, x in enumerate(names):
            for y in names[i + 1:]:
                if before(y, x) and not before(x, y):
                    forced.append('%s<%s' % (x, y))
                elif x in pos and y in pos and pos[x] > pos[y]:
                    free.append('%s<%s' % (x, y))
        print('| %s | %s | %s | %s |' % (n, ' '.join(walked) or '-', ' '.join(forced) or 'none', ' '.join(free) or 'none'))
        if not forced:
            keep.append((n, walked))
    print('\n## SCCS stamps (OLDCC_DATES.md)\n\n| libc | program | crt0 | sources (newest) |\n|---|---|---|---|')
    c, s = stamps(prog)
    print('| - | **%s** | %s | %s |' % (prog, c, s))
    for n, walked in keep:
        for u in walked:
            c, s = stamps(u)
            print('| %s | %s | %s | %s |' % (n, u, c, s))


if __name__ == '__main__':
    main()
