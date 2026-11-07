#!/usr/bin/env python3
"""findings.py PROG: brings src/cmd/PROG/tools/findings.json up to the current model and writes FINDINGS.md from it.

The status is measured only for the programs with a model of the old compiler v0p (switchmatrix.py PROGS; oldcc.sh model
PROG first); for the others the json is taken as it is (byte-identical programs: every function exact). The status comes from strictcmp and is never
edited by hand: exact (strictcmp -c finds no difference), masked (strictly equal, but call against calr differs: the old cas,
OLDCC.md D17), open (strictly different; lines = differing lines). Written by hand: only note, for an open difference, an odd
source form, a guessed name, dead code. Which function needs which switch: host/OLDCC_SWITCHES.md.
A program built by a compiler we do not have (cas: "measure": "check.sh" in its json) is measured by its tools/check.sh
(host/tools/cmpmod.py, a normal form): same (equal in the normal form), close (same calls, constants and variables),
open (lines = differing instructions); per module the differing data bytes.
"""
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..'))


def measure(tools, opts):
    r = subprocess.run([sys.executable, os.path.join(HERE, 'strictcmp.py')] + opts, cwd=os.path.dirname(tools),
                       env=dict(os.environ, RECON_PROG=tools), capture_output=True, text=True)
    res = {}
    for line in r.stdout.split('\n'):
        f = line.split()
        if len(f) > 2 and f[-1] == 'SAME':
            res[f[0]] = 0
        elif len(f) > 2 and f[-2] == 'DIFF':
            res[f[0]] = int(f[-1])
    if not res:
        sys.exit('no result from strictcmp: run oldcc.sh model first\n' + r.stderr[-400:])
    return res


def main():
    prog = sys.argv[1]
    tools = os.path.join(REPO, 'src', 'cmd', prog, 'tools')
    path = os.path.join(tools, 'findings.json')
    d = json.load(open(path))
    if d.get('measure') == 'check.sh':
        measure_check(d, tools)
        json.dump(d, open(path, 'w'), indent=1)
        open(path, 'a').write('\n')
        write_md(d, os.path.join(tools, 'FINDINGS.md'), prog)
        return
    import switchmatrix
    if prog not in switchmatrix.PROGS:	# no model: the program is byte-identical (checkall), only FINDINGS.md is written
        write_md(d, os.path.join(tools, 'FINDINGS.md'), prog)
        return
    strict = measure(tools, [])
    forms = measure(tools, ['-c'])
    for name, f in d['functions'].items():
        if f.get('status') == 'crt0':
            continue
        f.pop('lines', None)
        if name not in strict:
            f['status'] = 'open'
            f['lines'] = -1
        elif strict[name]:
            f['status'] = 'open'
            f['lines'] = strict[name]
        elif forms.get(name):
            f['status'] = 'masked'
            f['lines'] = forms[name]
        else:
            f['status'] = 'exact'
    d['measured_at'] = subprocess.run(['git', '-C', REPO, 'log', '-1', '--format=%h'], capture_output=True,
                                      text=True).stdout.strip()
    json.dump(d, open(path, 'w'), indent=1)
    open(path, 'a').write('\n')
    write_md(d, os.path.join(tools, 'FINDINGS.md'), prog)


def measure_check(d, tools):
    """the functions and the data of every module from tools/check.sh"""
    wega = os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or '/mnt/share/cvs/P8000/tmp/wega31_root'
    r = subprocess.run(['sh', os.path.join(tools, 'check.sh'), 'WEGA=' + wega], capture_output=True, text=True)
    old = d.get('functions', {})
    fn, data, mod = {}, {}, None
    for line in r.stdout.split('\n'):
        f = line.split()
        if line.startswith('== '):
            mod = f[1]
        elif len(f) > 4 and f[4] == 'instr' and mod:
            name = f[0].lstrip('_') if f[0] != '-' else '(%s %s)' % (mod, f[1])
            e = dict(old.get(name, {}))
            e.update({'file': mod + ('.y' if mod == 'parser' else '.c'), 'orig': '0x' + f[1],
                      'instr': '%s/%s' % (f[2].rstrip('/'), f[3])})
            e.pop('lines', None)
            if f[5] == 'identical':
                e['status'] = 'same'
            elif f[5] == 'same':
                e['status'] = 'close'
            else:
                e['status'] = 'open'
                e['lines'] = int(f[5])
            fn[name] = e
        elif 'differing bytes in the first' in line and mod:
            data[mod] = int(f[0])
    if not fn:
        sys.exit('no result from check.sh\n' + r.stderr[-400:])
    d['functions'] = fn
    d['data'] = data
    d['measured_at'] = subprocess.run(['git', '-C', REPO, 'log', '-1', '--format=%h'], capture_output=True,
                                      text=True).stdout.strip()


def write_md(d, path, prog):
    if d.get('measure') == 'check.sh':
        return write_md_check(d, path, prog)
    fn = {k: f for k, f in d['functions'].items() if f.get('status') != 'crt0'}
    st = lambda s: sorted((k for k, f in fn.items() if f['status'] == s), key=lambda k: fn[k]['orig'])
    with open(path, 'w') as o:
        o.write('# %s: findings\n\nGenerated by `host/recon/findings.py %s` from `findings.json`, do not edit.\n\n' % (prog, prog))
        o.write('Against %s, built with %s%s. %d functions: %d exact, %d masked, %d open.\n\n' % (
            d['binary'], d['compiler'], ', commit ' + d['measured_at'] if 'measured_at' in d else '', len(fn),
            len(st('exact')), len(st('masked')), len(st('open'))))
        if 'measured_at' in d:
            o.write('exact: no difference in `strictcmp -c`, jump and call targets included. masked: strictly equal, `call` against `calr` differs (the old cas, `OLDCC.md` '
                    'D17). open: strictly different. Which switch a function needs: `host/OLDCC_SWITCHES.md`.\n\n')
        else:
            o.write('exact: the program is byte-identical with the original (`host/tools/checkall.sh`).\n\n')
        for s, title in (('open', 'Open'), ('masked', 'Masked')):
            o.write('## %s\n\n' % title)
            if not st(s):
                o.write('None.\n\n')
                continue
            o.write('| Function | Address | Lines | Note |\n|---|---|---|---|\n')
            for k in st(s):
                o.write('| %s | %s | %s | %s |\n' % (k, fn[k]['orig'], fn[k].get('lines', ''), fn[k].get('note', '')))
            o.write('\n')
        notes = [k for k in st('exact') if fn[k].get('note')]
        o.write('## Exact, with a note\n\nOdd source forms, guessed names, dead code: the bytes fit, what was really written is not known.\n\n')
        o.write('| Function | Address | Note |\n|---|---|---|\n')
        for k in notes:
            o.write('| %s | %s | %s |\n' % (k, fn[k]['orig'], fn[k]['note']))



def write_md_check(d, path, prog):
    fn = d['functions']
    st = lambda s: sorted((k for k, f in fn.items() if f['status'] == s), key=lambda k: fn[k]['orig'])
    with open(path, 'w') as o:
        o.write('# %s: findings\n\nGenerated by `host/recon/findings.py %s` from `findings.json`, do not edit.\n\n' % (prog, prog))
        o.write('Against %s, built with %s, commit %s. %d functions: %d same, %d close, %d open.\n\n' % (
            d['binary'], d['compiler'], d['measured_at'], len(fn), len(st('same')), len(st('close')), len(st('open'))))
        o.write(d['about'] + '\n\n')
        o.write('same: equal in the normal form of `host/tools/cmpmod.py` (addresses, labels, frame sizes, scratch registers '
                'masked). close: the same calls, constants and variables. open: lines = differing instructions. '
                'Instr: ours/original.\n\n')
        for s, title in (('open', 'Open'), ('close', 'Close'), ('same', 'Same')):
            o.write('## %s\n\n' % title)
            if not st(s):
                o.write('None.\n\n')
                continue
            o.write('| Function | File | Address | Instr | Lines | Note |\n|---|---|---|---|---|---|\n')
            for k in st(s):
                f = fn[k]
                o.write('| %s | %s | %s | %s | %s | %s |\n' % (k, f['file'], f['orig'], f['instr'], f.get('lines', ''),
                                                           f.get('note', '')))
            o.write('\n')
        o.write('## Data\n\nDiffering bytes of the data of each module (relocated words skipped; `tools/DATA`):\n\n'
                '| Module | Bytes |\n|---|---|\n')
        for m, n in d['data'].items():
            o.write('| %s | %d |\n' % (m, n))


if __name__ == '__main__':
    main()
