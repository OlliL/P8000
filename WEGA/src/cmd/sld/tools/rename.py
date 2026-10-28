#!/usr/bin/env python3
"""rename.py - rename an own function of sld consistently.

Usage: rename.py OLD NEW   (OLD a function of own_functions.txt, or any other identifier of the sources: variable, struct, label)

own_functions.txt is the source of the names (findings.json is generated from it, see sync_findings.py). This tool
changes, in one step: the name column of own_functions.txt, the identifier in the C files and ld.h (whole words), the keys of
findings.json (functions, manual_classes, the 'function' field of findings_pending), then runs sync_findings.py,
classify_sld.py and findings_md.py. Names in prose files (TODO.md, KNOWLEDGE.md) are not changed. A variable is renamed in the sources, var_addr.txt, var_fn_addr.txt and order_vars.py.
"""
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))


def rename_variable(old, new):
    """OLD is no function: a global variable, a struct, a label ... Renamed in the C files and ld.h (whole words); the name must be free"""
    sys.path.insert(0, HERE)
    import ldsrc
    texts = ldsrc.read_all()
    if not any(re.search(r'\b%s\b' % re.escape(old), t) for t in texts.values()):
        print('%s is neither in own_functions.txt nor in the sources' % old)
        return 1
    if any(re.search(r'\b%s\b' % re.escape(new), t) for t in texts.values()):
        print('%s exists already (to merge two variables of the same address edit the sources by hand)' % new)
        return 1
    for path, t in texts.items():
        open(path, 'w').write(re.sub(r'\b%s\b' % re.escape(old), new, t))
    # the tables of the variable addresses and the layout tool keep the names too (quoted or first word)
    for name in ('var_addr.txt', 'var_fn_addr.txt', 'order_vars.py'):
        p = os.path.join(HERE, name)
        if os.path.exists(p):
            t = open(p).read()
            n = re.sub(r"(?m)^%s\b" % re.escape(old), new, t) if name.endswith('.txt') else re.sub(r"'%s'" % re.escape(old), "'%s'" % new, t)
            if n != t:
                open(p, 'w').write(n)
    print('renamed %s -> %s (not a function: sources only)' % (old, new))
    return 0


def main():
    if len(sys.argv) != 3:
        print(__doc__)
        return 2
    old, new = sys.argv[1], sys.argv[2]
    table = os.path.join(HERE, 'own_functions.txt')
    lines = open(table).read().split('\n')
    hit = [l for l in lines if l.split()[1:2] == [old]]
    if len(hit) != 1:
        return rename_variable(old, new)
    if any(l.split()[1:2] == [new] for l in lines):
        print('%s exists already' % new)
        return 1
    lines = [l.replace(' ' + old + ' ', ' ' + new + ' ', 1) if l.split()[1:2] == [old] else l for l in lines]
    open(table, 'w').write('\n'.join(lines))
    import ldsrc
    changed = 0
    for path in ldsrc.sources():
        text = open(path).read()
        new_text = re.sub(r'\b%s\b' % re.escape(old), new, text)
        if new_text != text:
            open(path, 'w').write(new_text)
            changed += 1
    print('%d source files changed' % changed)
    fj = os.path.join(HERE, 'findings.json')
    d = json.load(open(fj))
    if old in d['functions']:
        d['functions'][new] = d['functions'].pop(old)
    if old in d.get('manual_classes', {}):
        d['manual_classes'][new] = d['manual_classes'].pop(old)
    for x in d['findings_pending']:
        if x.get('function') == old:
            x['function'] = new
    json.dump(d, open(fj, 'w'), indent=1)
    open(fj, 'a').write('\n')
    subprocess.run([sys.executable, os.path.join(HERE, 'sync_findings.py')], check=True)
    subprocess.run([sys.executable, os.path.join(HERE, 'classify_sld.py')], check=True, capture_output=True)
    subprocess.run([sys.executable, os.path.join(HERE, 'findings_md.py')], check=True, capture_output=True)
    print('renamed %s -> %s' % (old, new))
    return 0


if __name__ == '__main__':
    sys.exit(main())
