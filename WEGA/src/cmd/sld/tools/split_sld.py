#!/usr/bin/env python3
"""split_sld.py - one-off: split the work file sld.c into the C files of module_ranges.txt and ld.h.

Usage: split_sld.py [--write]     (without --write: report only)

Every block of sld.c ("/* name ..: status */" up to the next such comment) holds the declarations used by one function and the
function. The function goes to the C file of its original address (table own_functions.txt, module_ranges.txt), in address order,
with its comment. Structs, function declarations and variable declarations of all blocks go to ld.h (variables as extern);
ext.c defines the variables (ld.h with DEFINE_VARS defined leaves out the externs). Different declarations of one name are listed.
"""
import collections
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ldsrc  # noqa: E402

LD = os.path.join(HERE, '..')
HDR = re.compile(r'^/\* ([A-Za-z0-9_]+)(?: 0x[0-9a-f]+)?\.\.')
FDEF = re.compile(r'^(?:[A-Za-z_][\w\s\*]*?[\s\*])?([A-Za-z_]\w*)\(([^()]*)\)\s*$')
FDECL = re.compile(r'^([A-Za-z_][\w\s\*]*?[\s\*])?([A-Za-z_]\w*)\(\);$')


def norm(t):
    return re.sub(r'\s+', ' ', t.strip())


def var_name(t):
    t = re.sub(r'\{[^}]*\}', '', t)
    m = re.search(r'([A-Za-z_]\w*)\s*(\[[^\]]*\]\s*)*;$', t)
    return m.group(1) if m else None


def table():
    ent = {}
    for l in open(os.path.join(HERE, 'own_functions.txt')):
        if l[0] == '#' or not l.strip():
            continue
        p = l.split()
        ent[p[1]] = int(p[0], 16)
    return ent


def parse(text):
    s = text.split('\n')
    idx = [i for i, l in enumerate(s) if HDR.match(l)] + [len(s)]
    blocks = []
    for k in range(len(idx) - 1):
        blk = s[idx[k]:idx[k + 1]]
        o = blk.index('{')
        h = max(i for i in range(1, o) if FDEF.match(blk[i]) and not blk[i].rstrip().endswith(';'))
        c = o + blk[o:].index('}')
        pre = blk[1:h]
        name = FDEF.match(blk[h]).group(1)
        blocks.append({'comment': blk[0], 'pre': pre, 'fn': blk[h:c + 1], 'name': name, 'head': blk[h]})
    return blocks


def statements(pre):
    out, i = [], 0
    while i < len(pre):
        t = pre[i].strip()
        if not t:
            i += 1
            continue
        if re.match(r'^struct\s+\w+\s*\{.*\};$', t):
            out.append(('struct', t))
            i += 1
        elif t.startswith('/*') and t.endswith('*/'):
            out.append(('comment', t))
            i += 1
        elif t.startswith('struct') and '{' in t and not t.endswith(';'):
            j = i
            while pre[j].rstrip() != '};' and not pre[j].startswith('}'):
                j += 1
            out.append(('struct', '\n'.join(pre[i:j + 1])))
            i = j + 1
        elif FDECL.match(t):
            out.append(('fdecl', t))
            i += 1
        elif t.endswith(';'):
            out.append(('var', t))
            i += 1
        else:
            out.append(('other', t))
            i += 1
    return out


def main():
    write = '--write' in sys.argv
    text = open(os.path.join(LD, 'sld.c')).read()
    ent = table()
    blocks = parse(text)
    structs, fdecls, vars_, conflicts, other = collections.OrderedDict(), collections.OrderedDict(), collections.OrderedDict(), [], []
    for b in blocks:
        for kind, t in statements(b['pre']):
            if kind == 'struct':
                tag = re.match(r'struct\s+(\w+)', t)
                key = tag.group(1) if tag else t
                if key in structs and norm(structs[key]) != norm(t):
                    conflicts.append(('struct', key, structs[key], t))
                structs.setdefault(key, t)
            elif kind == 'fdecl':
                m = FDECL.match(t)
                key = m.group(2)
                if key in fdecls and norm(fdecls[key]).replace('int ', '') != norm(t).replace('int ', ''):
                    conflicts.append(('fdecl', key, fdecls[key], t))
                fdecls.setdefault(key, t)
            elif kind == 'var':
                key = var_name(t)
                if key in vars_ and norm(vars_[key]) != norm(t):
                    conflicts.append(('var', key, vars_[key], t))
                vars_.setdefault(key, t)
            elif kind == 'comment':
                b.setdefault('comments', []).append(t)
            else:
                other.append((b['name'], t))
    # types of the functions defined in the blocks against the declarations
    for b in blocks:
        m = FDEF.match(b['head'])
        typ = norm(b['head'][:b['head'].index(b['name'] + '(')])
        d = fdecls.get(b['name'])
        if d is None:
            fdecls[b['name']] = '%s%s();' % (typ + ' ' if typ else '', b['name'])
        else:
            dt = norm(d[:d.index(b['name'] + '(')])
            if (dt.replace(' ', '') or 'int') != (typ.replace(' ', '') or 'int'):
                conflicts.append(('deftype', b['name'], d, b['head']))
    print('blocks %d, structs %d, function declarations %d, variables %d' % (len(blocks), len(structs), len(fdecls), len(vars_)))
    print('other statements:', other[:10])
    for c in conflicts:
        print('CONFLICT', c)
    # files
    files = collections.OrderedDict((f, []) for f, _, _ in ldsrc.modules())
    for b in blocks:
        e = ent.get(b['name'])
        f = ldsrc.module_of(e) if e is not None else None
        if f is None:
            print('no file for', b['name'], e)
            continue
        files[f].append((e, b))
    if conflicts or not write:
        return
    hdr = ['/*', ' * ld.h - declarations shared by the C files of sld (structs, functions, variables).', ' * The variables are defined in ext.c',
           ' * (DEFINE_VARS defined there); the other files see them as extern.', ' */', '']
    hdr += list(structs.values()) + ['']
    hdr += list(fdecls.values()) + ['']
    hdr += ['#ifndef DEFINE_VARS']
    for t in vars_.values():
        hdr.append('extern ' + t)
    hdr += ['#endif', '']
    open(os.path.join(LD, 'ld.h'), 'w').write('\n'.join(hdr))
    for f, items in files.items():
        items.sort(key=lambda x: x[0])
        out = ['/*', ' * %s - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.' % f, ' */', '']
        if f == 'ext.c':
            out += ['#define DEFINE_VARS', '#include "ld.h"', '', '/* the global variables of sld */']
            out += list(vars_.values()) + ['']
        else:
            out += ['#include "ld.h"', '']
        for e, b in items:
            out += [b['comment']] + ['\t' + c if False else c for c in b.get('comments', [])] + b['fn'] + ['']
        open(os.path.join(LD, f), 'w').write('\n'.join(out))
    print('written:', ', '.join(files))


if __name__ == '__main__':
    main()
