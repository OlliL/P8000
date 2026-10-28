#!/usr/bin/env python3
"""rename.py - rename an own function of sld consistently.

Usage: rename.py OLD NEW

own_functions.txt is the source of the names (findings.json is generated from it, see sync_findings.py). This tool
changes, in one step: the name column of own_functions.txt, the identifier in the C files and ld.h (whole words), the keys of
findings.json (functions, manual_classes, the 'function' field of findings_pending), then runs sync_findings.py,
classify_sld.py and findings_md.py. Names in prose files (TODO.md, KNOWLEDGE.md) are not changed.
"""
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))


def main():
    if len(sys.argv) != 3:
        print(__doc__)
        return 2
    old, new = sys.argv[1], sys.argv[2]
    table = os.path.join(HERE, 'own_functions.txt')
    lines = open(table).read().split('\n')
    hit = [l for l in lines if l.split()[1:2] == [old]]
    if len(hit) != 1:
        print('%s is not a name in own_functions.txt' % old)
        return 1
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
