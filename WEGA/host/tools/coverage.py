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
          'lib/libc.a': 'lib/libc', 'lib/crt0.o': 'lib/libc/nonseg/csu', 'lib/slibc.a': 'lib/libc', 'lib/scrt0.o': 'lib/libc/seg/csu', 'etc/dump': 'cmd/dump.c', 'etc/dumpdir': 'cmd/dumpdir.c',
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


_INSTALLS = None


def installed_by(rel):
    """the directory below src/ whose Makefile installs the root file rel ($(PREFIX)/rel, also through a variable that
    names a directory below $(PREFIX)), or None"""
    global _INSTALLS
    if _INSTALLS is None:
        _INSTALLS = []
        for dp, dn, fn in os.walk(SRC):
            if 'Makefile' not in fn:
                continue
            try:
                text = open(os.path.join(dp, 'Makefile'), errors='replace').read()
            except OSError:
                continue
            if '$(PREFIX)' not in text:
                continue
            for var, val in re.findall(r'^(\w+)\s*=\s*(\$\(PREFIX\)\S*)\s*$', text, re.M):
                text = text.replace('$(%s)' % var, val)
            _INSTALLS.append((os.path.relpath(dp, SRC), text))
    name = os.path.basename(rel)
    d = os.path.dirname(rel)
    for src, text in _INSTALLS:
        if re.search(r'\$\(PREFIX\)/+%s(\s|$)' % re.escape(rel), text):
            return src
        if d and re.search(r'^\s*-?cp\b[^\n]*\b%s\b[^\n]*\$\(PREFIX\)/+%s/?\s*$' % (re.escape(name), re.escape(d)), text, re.M):
            return src
        # for f in $(LIST); do cp $$f $(PREFIX)/d/$$f; done
        for var, line in re.findall(r'for\s+\w+\s+in\s+\$\((\w+)\)(.*)$', text, re.M):
            if not re.search(r'\$\(PREFIX\)/+%s/\$\$' % re.escape(d), line):
                continue
            m = re.search(r'^%s\s*=((?:.*\\\n)*.*)$' % var, text, re.M)
            if m and name in m.group(1).replace('\\\n', ' ').split():
                return src
    return None


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
        for l in open(path, errors='replace').read().replace('\\\n', ' ').split('\n'):	# continuation lines joined
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
    if src.startswith('lib/') and name.endswith('.o') and src.endswith('/csu'):
        return 'as', 'n/a (object)'		# crt0.o, scrt0.o: $(AS) in src/lib/libc/Makefile
    if src.startswith('lib/') and name.endswith('.a') and os.path.exists(mk):
        # a library: the compiler variables of its Makefile (CC, CCV1, CCV1C ...; the segmented one: SCC, SCCV1 ...);
        # an archive is not linked, so it has no libc
        v = makefile_vars(mk)
        ccs = [k for k in v if re.match(r'^CC(V\w*)?$', k)]
        sccs = [k for k in v if re.match(r'^SCC(V\w*)?$', k)]
        pick = sccs if name.startswith('slib') else ccs
        return (os.path.basename(v[pick[0]]) if len(pick) == 1 else '-'), 'n/a (archive)'
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


TOOLVARS = re.compile(r'^(CC|SCC|CCV\w*|SCCV\w*|AS|CAS)$')


def mkrule(text, t):
    m = re.search(r'^%s\s*:([^\n]*)\n((?:\t[^\n]*\n)*)' % re.escape(t), text, re.M)
    return (m.group(1), m.group(2)) if m else None


def find_rule(d, name):
    """(text, vars, rule) of the Makefile below d that makes name (a versioned subdirectory first, then any)"""
    mks = []
    top = os.path.join(d, 'Makefile')
    v = makefile_vars(top)
    for key, val in v.items():		# COPYDISK_VERSION = 1.5
        if key.endswith('_VERSION') and os.path.exists(os.path.join(d, val, 'Makefile')):
            mks.append(os.path.join(d, val, 'Makefile'))
    mks.append(top)
    for root, dirs, fs in sorted(os.walk(d)):
        if 'Makefile' in fs and root.count(os.sep) - d.count(os.sep) <= 2:
            mks.append(os.path.join(root, 'Makefile'))
    first = None
    for mk in mks:
        try:
            text = open(mk, errors='replace').read()
        except OSError:
            continue
        v = makefile_vars(mk)
        if first is None:
            first = (text, v, None, mk)
        names = [name] + ['$(%s)' % k for k, val in v.items() if val == name]
        for n in names:
            r = mkrule(text, n)
            if r:
                return text, v, r, mk
    return first if first else ('', {}, None, top)


def complete(rel, k, src, cc, lib):
    """compiler and libc of an exact binary or archive from the rules of its Makefile (the rule of the file, the rules of
    its objects, the .c.o rule, the link line): an archive or an object is not linked; a program linked by cc or scc gets
    their libc"""
    d = os.path.join(SRC, src)
    if not os.path.isdir(d):
        return cc, lib
    name = os.path.basename(rel)
    text, v, r, mkpath = find_rule(d, name)

    def tools_of(cmds):
        t = []
        for x in re.findall(r'\$\((\w+)\)', cmds):
            if TOOLVARS.match(x) and x in v and os.path.basename(v[x].split()[0]) not in t:
                t.append(os.path.basename(v[x].split()[0]))
        return t
    suffix = mkrule(text, '.c.o')

    def obj_tools(o):
        orule = mkrule(text, o)
        t = tools_of(orule[1]) if orule else []
        if not t and suffix:
            t = tools_of(suffix[1])
        if not t and 'CC' in v and os.path.exists(os.path.join(os.path.dirname(mkpath), o[:-2] + '.c')):
            t = [os.path.basename(v['CC'].split()[0])]	# make's default rule: $(CC)
        if not t and 'AS' in v and os.path.exists(os.path.join(os.path.dirname(mkpath), o[:-2] + '.s')):
            t = [os.path.basename(v['AS'].split()[0])]
        return t

    def join(ts):
        u = []
        for t in ts:
            for x in t:
                if x not in u:
                    u.append(x)
        return ', '.join(u) if u else '-'
    if name.endswith('.o'):
        t = obj_tools(name)
        if name.startswith('s') and not mkrule(text, name) and mkrule(text, name[1:]) and 'SCC' in v:
            # a segmented object made in build/seg under the name without s, by $(SCC) (src/lib: scnvt.o, sdoprnt.o)
            t = [os.path.basename(v['SCC'].split()[0]) if x == os.path.basename(v.get('CC', '').split()[0] if v.get('CC') else '') else x
                 for x in obj_tools(name[1:])]
        return join([t]), 'n/a (object)'
    if k == 'archive':
        if cc == '-' or src.startswith('uts/'):
            objs = re.findall(r'^([\w./]+\.o)\s*:', text, re.M)
            cc = join([obj_tools(o) for o in objs] + ([tools_of(suffix[1])] if suffix else []))
        return cc, 'n/a (archive)'
    if r:
        words = (r[0] + ' ' + r[1]).split()
        if re.search(r'\$\((LD|SLD|GLD)\)', r[1]):	# linked by ld: the tools of its objects
            deps = []
            for w in words:
                m = re.match(r'^\$\((\w+)\)$', w)
                for x in (v.get(m.group(1), '').split() if m else [w]):
                    if x.endswith('.o') and '/' not in x:
                        deps.append(x)
            if deps:
                cc = join([obj_tools(o) for o in deps])
        if lib == '-':
            libs = []
            for w in words:
                m = re.match(r'^\$\((\w+)\)$', w)
                for x in (v.get(m.group(1), '').split() if m else [w]):
                    b = os.path.basename(x)
                    if (b.endswith('.a') or re.match(r'^(LIB\w*|BOOTLIB)$', b)) and b not in libs:
                        libs.append(b)
            if libs:
                lib = ', '.join(libs)
            else:
                drv = [x for x in re.findall(r'\$\((\w+)\)', r[1]) if x in ('CC', 'SCC')]
                if drv:
                    lib = 'slibc.a' if os.path.basename(v[drv[0]]).startswith('s') else 'libc.a'
                elif re.search(r'\$\((LD|SLD|GLD)\)', r[1]):
                    lib = 'none'
    elif lib == '-' and 'CC' in v:
        lib = 'libc.a'	# a program made by the default rule of cc
    return cc, lib


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
                src = installed_by(rel) or '?'
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
        if status == 'exact' and (k.startswith('binary') or k == 'archive') and (cc == '-' or lib == '-') and src:
            cc, lib = complete(rel, k, src, cc, lib)
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
         'of `host/OLDCC_DATES.md` (D12: data layout, D1: frame layout); an archive or an object is not linked (libc n/a).', '',
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
