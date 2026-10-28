#!/usr/bin/env python3
"""accept.py - accept a checked draft function into the C files of sld, update own_functions.txt and findings.json.

Usage: accept.py DRAFT.c ENTRY END [CLASSES RESULT]

  DRAFT.c  the draft (K&R, one function, plus its declarations; it may #include "ld.h"); lines after a line '/* test stubs */'
           are test stubs of callees and are not accepted
  ENTRY    entry address of the function (hex, e.g. 0aba), END its end (hex, e.g. 0b04)
  CLASSES  optional difference classes of the old compiler (D1 .. D17, comma separated) that remain
  RESULT   optional text: what differs (required with CLASSES)

The draft is checked with fncheck.py. Only on OK (or with CLASSES) the function goes to the C file of its address
(module_ranges.txt), in address order; a function that is in the file already is replaced. Structs, function declarations and
variables of the draft that ld.h does not have yet are added to ld.h (variables as extern, defined in copyname.c). The status in
own_functions.txt is set: 'done' without CLASSES, 'findings:CLASSES' with CLASSES. With CLASSES the D classes go to the audit of
the function in findings.json; findings_pending is only for items without a D class.
Then every C file is compiled alone (ldsrc.compile_all); a compile error or a lost function name (empty label) restores all files.
"""
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ldsrc  # noqa: E402
import split_sld as S  # noqa: E402

LD = os.path.join(HERE, '..')
TABLE = os.path.join(HERE, 'own_functions.txt')
FINDINGS = os.path.join(HERE, 'findings.json')
HDRC = re.compile(r'^/\* ([A-Za-z0-9_]+)(?: 0x[0-9a-f]+)?\.\.')


def check_compile():
    outs, errs = ldsrc.compile_all('-O', asm=True)
    if errs:
        print(errs[:600])
        return False
    for p in outs.values():
        if '/*~~:*/' in open(p).read():
            print('a function of %s got no name (rename a function; see KNOWLEDGE, sld compiler limits)' % os.path.basename(p))
            return False
    return True


def record_evidence(entry, result):
    """the manual reading of a difference goes to pairing_evidence (codgen style); the audit is classify_sld.py"""
    name = [line.split()[1] for line in open(TABLE) if line.startswith(entry + ' ')][0]
    data = json.load(open(FINDINGS))
    data['functions'][name]['pairing_evidence'] = result
    json.dump(data, open(FINDINGS, 'w'), indent=1)
    open(FINDINGS, 'a').write('\n')
    return name


def table_entries():
    ent = {}
    for l in open(TABLE):
        if l[0] == '#' or not l.strip():
            continue
        p = l.split()
        ent[p[1]] = int(p[0], 16)
    return ent


def split_blocks(lines):
    """module file -> (prefix lines, [(name, lines)])"""
    idx = [i for i, l in enumerate(lines) if HDRC.match(l)]
    pre = lines[:idx[0]] if idx else lines
    idx.append(len(lines))
    return pre, [(HDRC.match(lines[idx[k]]).group(1), lines[idx[k]:idx[k + 1]]) for k in range(len(idx) - 1)]


VARFILE = 'copyname.c'    # the file with the definitions of the uninitialized variables (the order is the order of the allocation)


def add_declarations(statements, hdr_text, ext_text):
    """new structs, function declarations and variables of the draft go to ld.h / copyname.c (VARFILE)"""
    hl = hdr_text.split('\n')
    el = ext_text.split('\n')
    have = {'struct': {}, 'fdecl': {}, 'var': {}}
    for t in hdr_text.split('\n'):
        m = S.FDECL.match(t.strip())
        if m:
            have['fdecl'][m.group(2)] = t
        if t.startswith('extern '):
            have['var'][S.var_name(t[7:])] = t
    for m in re.finditer(r'struct\s+(\w+)\s*\{', hdr_text):
        have['struct'][m.group(1)] = True
    for kind, t in statements:
        if kind == 'struct':
            tag = re.match(r'struct\s+(\w+)', t)
            if tag and tag.group(1) in have['struct']:
                continue
            hl.insert(hl.index('/* functions */'), t)
        elif kind == 'fdecl':
            m = S.FDECL.match(t)
            if m.group(2) in have['fdecl']:
                continue
            hl.insert(hl.index('/* variables */'), t)
        elif kind == 'var':
            name = S.var_name(t)
            if name in have['var']:
                continue
            hl.insert(hl.index('#endif', hl.index('#ifndef DEFINE_VARS')), 'extern ' + t)
            k = el.index('/* the global variables of sld */')
            while k + 1 < len(el) and el[k + 1].strip():
                k += 1
            el.insert(k + 1, t)
    return '\n'.join(hl), '\n'.join(el)


def main():
    args = sys.argv[1:]
    if len(args) not in (3, 5):
        print(__doc__)
        return 2
    draft, entry, end = args[0], args[1].zfill(4), args[2].zfill(4)
    cls = args[3] if len(args) == 5 else None
    result = args[4] if len(args) == 5 else ''
    r = subprocess.run([sys.executable, os.path.join(HERE, 'fncheck.py'), draft, entry, end], capture_output=True, text=True)
    print(r.stdout + r.stderr, end='')
    if r.returncode != 0 and not cls:
        print('not accepted')
        return 1
    ent = table_entries()
    tname = [n for n, e in ent.items() if e == int(entry, 16)]
    if len(tname) != 1:
        print('no function at %s in own_functions.txt' % entry)
        return 1
    tname = tname[0]
    body = open(draft).read().split('\n')
    if '/* test stubs */' in body:
        body = body[:body.index('/* test stubs */')]
    body = [l for l in body if not l.startswith('#include')]
    blk = S.parse('\n'.join(['/* x..y: z */'] + body))[0]
    statements = S.statements(blk['pre'])
    fn = blk['fn']
    dname = blk['name']
    if dname not in (tname, 'f' + entry):
        print('the draft function is %s, the table name is %s' % (dname, tname))
        return 1
    if dname != tname:
        fn = [re.sub(r'\b%s\b' % dname, tname, fn[0])] + fn[1:]
    status = 'done' if not cls else 'findings:' + cls
    comment = '/* %s 0x%s..0x%s: %s */' % (tname, entry, end, status)
    comments = [t for k, t in statements if k == 'comment']
    mod = ldsrc.module_of(entry)
    path = os.path.join(LD, mod)
    saved = {p: open(p).read() for p in (path, os.path.join(LD, 'ld.h'), os.path.join(LD, VARFILE))}
    hdr, ext = add_declarations(statements, saved[os.path.join(LD, 'ld.h')],
                                saved[os.path.join(LD, VARFILE)] if mod != VARFILE else saved[path])
    lines = saved[path].split('\n')
    if mod == VARFILE:
        lines = ext.split('\n')
    pre, blocks = split_blocks(lines)
    new = [comment] + comments + fn + ['']
    out, placed = list(pre), False
    for name, b in blocks:
        if name == tname:
            out += new
            placed = True
            continue
        if not placed and ent.get(name, 0) > int(entry, 16):
            out += new
            placed = True
        out += b
    if not placed:
        out += new
    texts = {path: '\n'.join(out), os.path.join(LD, 'ld.h'): hdr}
    if mod != VARFILE:
        texts[os.path.join(LD, VARFILE)] = ext
    for p, t in texts.items():
        open(p, 'w').write(t)
    if not check_compile():
        for p, t in saved.items():
            open(p, 'w').write(t)
        print('the sources do not compile with this function; restored, not accepted')
        return 1
    res = []
    for line in open(TABLE).read().split('\n'):
        if line.startswith(entry + ' '):
            line = line[:-len(line.split()[-1])] + status
        res.append(line)
    open(TABLE, 'w').write('\n'.join(res))
    subprocess.run([sys.executable, os.path.join(HERE, 'sync_findings.py')], check=True)
    subprocess.run([sys.executable, os.path.join(HERE, 'findings_md.py')], check=True, capture_output=True)
    if cls:
        name = record_evidence(entry, result or 'see fncheck output')
        print('pairing_evidence of %s: %s' % (name, cls))
    subprocess.run([sys.executable, os.path.join(HERE, 'classify_sld.py')], check=True, capture_output=True)
    subprocess.run([sys.executable, os.path.join(HERE, 'findings_md.py')], check=True, capture_output=True)
    print('accepted %s (%s): %s' % (tname, mod, status))
    return 0


if __name__ == '__main__':
    sys.exit(main())
