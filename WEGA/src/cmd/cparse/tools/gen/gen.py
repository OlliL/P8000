"""gen.py MODULE... : compose WEGA cparse modules from the Plexus PCC functions in the order of /lib/cparse.

Used once to make the base of the sources (2026-10-30); the modules are edited by hand afterwards, so do not run it
again on a module that was changed. PLEXUS_PCC: the pcc.d directory of the Plexus sources."""
import sys, json, re, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import chunks
P = os.environ.get('PLEXUS_PCC', '/mnt/share/cvs/P8000/tmp/plexus/cmd/cc/pcc.d') + '/'
SRC = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')) + '/'
F = json.load(open(SRC + 'tools/findings.json'))['functions']
addr2name = {int(f['orig'], 16): n for n, f in F.items()}
names = sorted(addr2name)
# PCC function texts
pfun = {}
pother = {}
for f in ['mip/scan.c', 'mip/pftn.c', 'mip/trees.c', 'mip/optim.c', 'mip/xdefs.c', 'code.c', 'local.c']:
    its = chunks.items(open(P + f, errors='replace').read())
    pother[f] = ''.join(t for k, n, t in its if k == 'other')
    for k, n, t in its:
        if k == 'func':
            pfun.setdefault(n, (f, t))
def p_pftn(p):
    a = p.index('struct instk {'); b = p.index('*pstk;') + len('*pstk;\n')
    p = p[:a] + p[b:]
    for v in ['int idebug = 0;\n', 'int ibseen = 0;  /* the number of } constructions which have been filled */\n',
              'int iclass;  /* storage class of thing being initialized */\n',
              'int ilocctr = 0;  /* location counter for current initialization */\n']:
        p = p.replace(v, '')
    return p


def p_vinit(v):
    p = open(P + 'mip/pftn.c').read()
    a = p.index('struct instk {'); b = p.index('*pstk;') + len('*pstk;\n')
    return v.replace('# include "mfile1"\n\n', '# include "mfile1"\n\n' + p[a:b] + '\nint idebug = 0;\n\nint ibseen = 0;  /* the number of } constructions which have been filled */\n\nint iclass;  /* storage class of thing being initialized */\n\nint ilocctr = 0;  /* location counter for current initialization */\n\n', 1)


PATCH = {
    'pftn1.c': p_pftn,
    'vinit1.c': p_vinit,
    'sym1.c': lambda s: s.replace('# include "mfile1"\n\n', '# include "mfile1"\n\nextern int ddebug;\nstruct symtab *relook();\n\n', 1),
    'code1.c': lambda c: re.sub(r'^\s*#\s*include\s*<stdio\.h>\s*\n', '', c, flags=re.M).replace('# include <signal.h>\n', '# include <signal.h>\n\nextern int cdebug;\nint eprint();\n', 1),
    'trees1.c': lambda t: t.replace('\tcase FORCE:\n', '\tcase RET:\t/* PCC: FORCE */\n'),
}
MODS = {
    'scan1.c': (0x13b6, 0x2448, ['mip/scan.c']),
    'pftn1.c': (0x2448, 0x397a, ['mip/pftn.c']),
    'trees1.c': (0x397a, 0x560e, ['mip/trees.c']),
    'optim': (0x560e, 0x60d8, ['mip/optim.c']),
    'code1.c': (0x60d8, 0x7574, ['code.c']),
    'local1.c': (0x7574, 0x7b4c, ['local.c']),
    'vinit1.c': (0x8100, 0x8c8e, []),
    'sym1.c': (0x8c8e, 0x9f06, []),
}
for m in sys.argv[1:]:
    lo, hi, base = MODS[m]
    out = []
    if m != 'optim':
        out.append('# include "mfile1"\n\n')
    for b in base:
        o = pother[b]
        o = re.sub(r'^\s*#\s*include\s+"(mfile1|manifest|macdefs)"\s*\n', '', o, flags=re.M)
        out.append(o.strip('\n') + '\n\n')
    for i, a in enumerate(names):
        if not (lo <= a < hi):
            continue
        n = addr2name[a]
        e = a + F[n]['size_orig']
        if n in pfun:
            out.append(pfun[n][1].rstrip('\n') + '\n\n')
        else:
            out.append('/* %s: not written yet (0x%04x..0x%04x of /lib/cparse) */\n%s()\n{\n}\n\n' % (n, a, e, n))
    txt = ''.join(out)
    txt = PATCH.get(m, lambda t: t)(txt)
    open(SRC + m, 'w').write(txt)
    print(m, 'written')
