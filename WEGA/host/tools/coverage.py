#!/usr/bin/env python3
"""coverage.py [-o FILE] - what is missing in src/ for a complete WEGA installation (writes src/COVERAGE.md).

One table per directory of the WEGA root. Status of a file:
  exact    a source exists and what it builds (make install) or its copy is byte-identical with the original
  partial  a source exists but its product is not identical, or nothing is built from it yet
  sys3     no source yet, a file of the same name is in the System III tree (tmp/sys3): a start
  todo     no source, nothing to start from
The built files are taken from an install tree (INSTALL, default CHECK_TMP/install of checkall.sh: run `checkall.sh -b` first).
Compiler and libc of a built file are read from the Makefiles; for a file without source the dating of OLDCC_DATES.md
(D12: data layout of the compiler, D1: frame layout) is shown. Environment: WEGA (the WEGA root), SYS3, INSTALL.
"""
import filecmp
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..'))
SRC = os.path.join(REPO, 'src')
WEGA = os.environ.get('WEGA') or os.path.join(REPO, '..', 'tmp', 'wega31_root')
SYS3 = os.environ.get('SYS3') or os.path.join(REPO, '..', 'tmp', 'sys3')
INSTALL = os.environ.get('INSTALL') or os.path.join(os.environ.get('CHECK_TMP', '/tmp/wega_check'), 'install')
DATES = os.path.join(REPO, 'host', 'OLDCC_DATES.md')

# the sections, in order; a file goes to the first whose prefix it has
SECTIONS = [('root', ''), ('bin', 'bin/'), ('etc', 'etc/'), ('lib', 'lib/'), ('usr/bin', 'usr/bin/'), ('usr/lib', 'usr/lib/'),
            ('usr/sys', 'usr/sys/'), ('usr/other', 'usr/'), ('usr/include', 'usr/include/'), ('usr/man', 'usr/man/')]
LAST = ['usr/include', 'usr/man']
SKIP = ('tmp/', 'dev/', 'z/', '.wegafs-manifest.tsv')

# a built file whose source is not found by its name: root path -> the source (below src/)
SOURCE = {'bin/cc': 'cmd/cc', 'bin/scc': 'cmd/cc', 'bin/cas': 'cmd/cas', 'bin/ld': 'cmd/ld', 'bin/sld': 'cmd/sld',
          'bin/cp': 'cmd/mv.c', 'bin/ln': 'cmd/mv.c', 'bin/mv': 'cmd/mv.c', 'bin/pburn': 'cmd/pburn',
          'usr/bin/strip': 'cmd/strip.c', 'usr/bin/ranlib': 'cmd/ranlib.c', 'bin/size': 'cmd/size.c',
          'lib/cpp': 'cmd/cpp', 'lib/cparse': 'cmd/cparse', 'lib/scparse': 'cmd/cparse', 'lib/codgen': 'cmd/codgen',
          'lib/scodgen': 'cmd/codgen', 'lib/popt': 'cmd/popt', 'lib/spopt': 'cmd/popt', 'pb.image': 'cmd/BOOT0',
          'lib/libc.a': 'lib/libc', 'lib/crt0.o': 'lib/libc/csu', 'etc/dump': 'cmd/dump.c', 'etc/dumpdir': 'cmd/dumpdir.c',
          'etc/restor': 'cmd/restor.c', 'etc/umfs': 'cmd/mfs.sh', 'etc/mlf': 'cmd/makelost+found.sh'}
# Makefile variable -> name of the compiler
CCNAME = {'CC': 'cc', 'SCC': 'scc', 'CCV0': 'ccv0', 'CCV1': 'ccv1', 'CCV1R': 'ccv1r', 'CCV1Q': 'ccv1q', 'CCV0P': 'ccv0p'}


def files(root):
    out = []
    for dp, dn, fn in os.walk(root):
        for f in fn:
            p = os.path.join(dp, f)
            if os.path.isfile(p):
                out.append(os.path.relpath(p, root))
    return sorted(out)


def kind(rel):
    if rel.startswith('usr/man/'):
        return 'man'
    if rel.startswith('usr/include/'):
        return 'header'
    try:
        d = open(os.path.join(WEGA, rel), 'rb').read(512)
    except OSError:
        return '?'
    if d[:2] in (b'\xe7\x07', b'\xe7\x11', b'\xe6\x07', b'\xe7\x01', b'\xe7\x05', b'\xe7\x09', b'\xe7\x0b'):
        return 'binary %s' % d[:2].hex().upper()
    if d[:1] in (b'\xe6', b'\xe7') and len(d) > 1:
        return 'binary %s' % d[:2].hex().upper()
    if d[:2] == b'\xff\x65':
        return 'archive'
    if d and sum(1 for c in d if 32 <= c < 127 or c in (9, 10, 13)) >= 0.95 * len(d):
        return 'text'
    return 'data'


def sys3_index():
    idx = {}
    if not os.path.isdir(SYS3):
        return idx
    for rel in files(SYS3):
        idx.setdefault(os.path.basename(rel), []).append(rel)
    return idx


def sys3_candidate(idx, rel, k):
    """a file of the System III tree that can be the start: for a binary the source of the same name (not a document)"""
    name = os.path.basename(rel)
    if k.startswith('binary') or k == 'archive':
        names = [name + e for e in ('.c', '.s', '.y', '.l', '.sh', '.f')]
    else:
        names = [name]
    cands = []
    for n in names:
        cands += [c for c in idx.get(n, []) if '/docs/' not in c and not c.startswith('usr/src/man/docs')]
    cands.sort(key=lambda p: (0 if p.startswith('usr/src') else 1, len(p)))
    return cands[0] if cands else None


def find_source(rel):
    """the path below src/ of the source of a root file, or None"""
    if rel in SOURCE:
        return SOURCE[rel]
    if rel.startswith('usr/include/'):
        p = 'head/' + rel[len('usr/include/'):]
        return p if os.path.exists(os.path.join(SRC, p)) else None
    if rel.startswith('usr/man/'):
        p = 'man/' + rel[len('usr/man/'):]
        return p if os.path.exists(os.path.join(SRC, p)) else None
    name = os.path.basename(rel)
    if rel.startswith('etc/') and os.path.exists(os.path.join(SRC, rel)):
        return rel
    if os.path.exists(os.path.join(SRC, 'cmd', name + '.sh')):
        return 'cmd/%s.sh' % name
    if name.startswith('dos') and os.path.isdir(os.path.join(SRC, 'cmd', 'doscopy')) and rel.startswith('usr/bin/'):
        return 'cmd/doscopy'
    if name.startswith('sa.') and rel.count('/') == 0:
        return 'cmd/standalone'
    for c in ('cmd/%s.c' % name, 'cmd/%s' % name):
        if os.path.exists(os.path.join(SRC, c)):
            return c
    return None


def makefile_vars(path):
    v = {}
    try:
        for l in open(path, errors='replace'):
            m = re.match(r'^([A-Z0-9_]+)\s*=\s*(.*?)\s*$', l)
            if m and m.group(1) not in v:
                v[m.group(1)] = m.group(2)
    except OSError:
        pass
    return v


def tools(rel, src):
    """(compiler, libc) of a built file, from the Makefiles"""
    if not src:
        return '-', '-'
    name = os.path.basename(rel)
    top = os.path.join(SRC, 'cmd', 'Makefile')
    text = open(top, errors='replace').read()
    tv = makefile_vars(top)
    if src.startswith('cmd/') and src.count('/') == 1 and src.endswith('.c'):
        base = os.path.basename(src)[:-2]
        m = re.search(r'^%s\.o:.*\n\t\$\((\w+)\)' % re.escape(base), text, re.M)
        cc = CCNAME.get(m.group(1), m.group(1)) if m else CCNAME['CC']
        m = re.search(r'^%s:.*\$\((LIBC\w*)\)' % re.escape(base), text, re.M)
        lib = os.path.basename(tv.get(m.group(1), '')) if m else ''
        if not lib:
            lib = os.path.basename(tv.get('LIBC', 'libc.a'))
        return cc, lib
    d = os.path.join(SRC, src)
    mk = os.path.join(d, 'Makefile')
    if os.path.isdir(d) and os.path.exists(mk):
        v = makefile_vars(mk)
        if 'CC' not in v and 'SCC' not in v:
            for sub in sorted(os.listdir(d)):
                smk = os.path.join(d, sub, 'Makefile')
                if os.path.exists(smk):
                    v = makefile_vars(smk)
                    break
        seg = name.startswith('s') and 'SCC' in v and name != 'size'
        cc = v.get('SCC' if seg or ('CC' not in v and 'SCC' in v) else 'CC', '-')
        if name in ('scc', 'scparse', 'scodgen', 'spopt', 'sld') and 'SCC' in v:
            cc = v['SCC']
        lib = v.get('LIBC', v.get('SLIBC', '-'))
        if name.startswith('s') and 'SLIBC' in v:
            lib = v['SLIBC']
        return os.path.basename(cc), os.path.basename(lib) if lib != '-' else '-'
    if src.startswith('lib/'):
        return '-', '-'
    return '-', '-'


def dating():
    """OLDCC_DATES.md: path -> 'D12 old, D1 tight'"""
    d = {}
    try:
        for l in open(DATES, errors='replace'):
            c = [x.strip() for x in l.strip().strip('|').split('|')]
            if len(c) >= 10 and re.match(r'^[a-z]', c[0]) and '/' in c[0] or (len(c) >= 10 and c[0].startswith('bin/')):
                d[c[0]] = 'D12 %s, D1 %s' % (c[7], c[9])
    except OSError:
        pass
    return d


def main():
    out = SRC + '/COVERAGE.md'
    a = sys.argv[1:]
    if a[:1] == ['-o']:
        out = a[1]
    if not os.path.isdir(INSTALL):
        print('coverage: no install tree %s (run checkall.sh -b first, or set INSTALL)' % INSTALL, file=sys.stderr)
        return 1
    idx = sys3_index()
    dates = dating()
    rows = {s[0]: [] for s in SECTIONS}
    for rel in files(WEGA):
        if rel.startswith(SKIP) or rel in SKIP:
            continue
        sec = None
        for s, pre in sorted(SECTIONS, key=lambda x: -len(x[1])):
            if pre and rel.startswith(pre):
                sec = s
                break
        if sec is None:
            sec = 'root' if '/' not in rel else None
        if sec is None:
            continue
        k = kind(rel)
        src = find_source(rel)
        orig = os.path.join(WEGA, rel)
        built = os.path.join(INSTALL, rel)
        cand = ''
        if os.path.exists(built):
            status = 'exact' if filecmp.cmp(orig, built, shallow=False) else 'partial'
            if not src:
                src = '?'
        elif rel.startswith(('usr/include/', 'usr/man/')) and src:
            status = 'exact' if filecmp.cmp(orig, os.path.join(SRC, src), shallow=False) else 'partial'
        elif src:
            status = 'partial'
        else:
            c = sys3_candidate(idx, rel, k)
            if c:
                status, cand = 'sys3', 'tmp/sys3/' + c
            else:
                status = 'todo'
        cc, lib = tools(rel, src if status in ('exact', 'partial') else None)
        if status in ('sys3', 'todo') and k.startswith('binary'):
            cc, lib = dating().get(rel, '-') if False else dates.get(rel, '-'), '-'
        rows[sec].append((rel, k, ('src/' + src) if src and status in ('exact', 'partial') else (cand or '-'), status, cc, lib))
    order = [s[0] for s in SECTIONS if s[0] not in LAST] + LAST
    anchor = lambda s: s.replace('/', '-')
    L = ['# WEGA root and its sources', '',
         'Generated by `host/tools/coverage.py` (do not edit; run `checkall.sh -b` first for the install tree): what `src/` has for every',
         'file of the WEGA 3.1 root and what is still to do. **exact**: a source exists and its product (or copy) is byte-identical;',
         '**partial**: a source exists, the product differs or is not built; **sys3**: no source, a file of the same name is in',
         'the System III tree (a start); **todo**: nothing yet. Compiler and libc: of the build, for a file without source the dating',
         'of `host/OLDCC_DATES.md` (D12: data layout, D1: frame layout).', '',
         '| Directory | files | exact | partial | sys3 | todo |', '|---|---|---|---|---|---|']
    tot = [0] * 5
    for s in order:
        r = rows[s]
        c = [len(r)] + [sum(1 for x in r if x[3] == st) for st in ('exact', 'partial', 'sys3', 'todo')]
        tot = [t + x for t, x in zip(tot, c)]
        L.append('| [%s](#%s) | %d | %d | %d | %d | %d |' % ((s,) + (anchor(s),) + tuple(c)))
    L.append('| **all** | %d | %d | %d | %d | %d |' % tuple(tot))
    L.append('')
    for s in order:
        r = rows[s]
        L += ['<a id="%s"></a>' % anchor(s), '## %s' % s, '']
        if s in LAST:
            L += ['| File | Source | Status |', '|---|---|---|']
            for rel, k, src, st, cc, lib in r:
                L.append('| `%s` | %s | %s |' % (rel, '`%s`' % src if src != '-' else '-', st))
        else:
            L += ['| File | Kind | Source | Status | Compiler | libc |', '|---|---|---|---|---|---|']
            for rel, k, src, st, cc, lib in r:
                L.append('| `%s` | %s | %s | %s | %s | %s |' % (rel, k, '`%s`' % src if src != '-' else '-', st, cc, lib))
        L.append('')
    open(out, 'w').write('\n'.join(L))
    print('%s: %d files, exact %d, partial %d, sys3 %d, todo %d' % ((out,) + tuple(tot)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
