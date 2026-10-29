#!/usr/bin/env python3
"""accept_ld.py - accept a checked draft into the C file of its module and update the progress files.

Usage: accept_ld.py DRAFT.c ENTRY END NAME [--finding]

  DRAFT.c  the draft (one function, K&R); it is checked with ldcheck.py first (exactly, without D classes)
  ENTRY    entry address (hex), END its end (hex), NAME the function name the draft defines (f<addr> until named)

On OK: the function goes into the C file of its module (assigned.txt), in address order; findings.json gets status
done for NAME (file, level exact), own_functions.txt the status done. Nothing is written on DIFF.
"""
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
LD = os.path.join(HERE, '..')


def module_of(entry):
    fil = None
    for l in open(os.path.join(HERE, 'assigned.txt')):
        f = l.split()
        if f and int(f[0], 16) <= entry:
            fil = f[2]
    return fil


def main():
    draft, entry, end, name = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]
    r = subprocess.run([sys.executable, os.path.join(HERE, 'ldcheck.py'), draft, entry, end], capture_output=True, text=True)
    print(r.stdout.strip())
    finding = '--finding' in sys.argv
    if not r.stdout.startswith('OK') and not finding:
        sys.exit(1)
    fil = module_of(int(entry, 16))
    if fil in (None, 'crt0'):
        sys.exit('not a module of the own code: %s' % fil)
    path = os.path.join(LD, fil)
    lines = open(draft).read().rstrip().split('\n')
    # declarations (top-level lines ending with ';') go in only once; the function is the rest
    # globals: everything before the function header (a struct definition is one block); K&R parameter
    # declarations stay in the function
    head = next(i for i, l in enumerate(lines) if '(' in l and not l.endswith(';') and not l.endswith('{'))
    glob = '\n'.join(lines[:head]).strip('\n')
    # one block per top-level declaration (separated by an empty line)
    decl = [b.strip('\n') for b in glob.split('\n\n') if b.strip()]
    body = '\n'.join(lines[head:]).strip('\n') + '\n\n'
    text = body
    src = open(path).read() if os.path.exists(path) else ''
    for d_ in decl:
        # a struct definition that is already in the file (by its tag) is not written again
        tag = re.match(r'(struct \w+) \{', d_)
        if d_ in src or (tag and (tag.group(1) + ' {') in src):
            continue
        src = d_ + '\n\n' + src
    # the function replaces an existing definition of the same name, otherwise it goes in address order
    pat = re.compile(r'^%s\(.*?(?=^\S|\Z)' % re.escape(name), re.M | re.S)
    if pat.search(src):
        src = pat.sub(lambda m: text, src, count=1)
    else:
        src = src + text
    open(path, 'w').write(src)
    d = json.load(open(os.path.join(HERE, 'findings.json')))
    d['functions'][name]['status'] = 'findings' if finding else 'done'
    d['functions'][name]['level'] = 'findings' if finding else 'exact'
    d['functions'][name]['pairing'] = 'confirmed'
    d['functions'][name]['ours'] = '%s-%s' % (entry, end)
    d['functions'][name]['file'] = fil
    json.dump(d, open(os.path.join(HERE, 'findings.json'), 'w'), indent=1)
    lines = open(os.path.join(HERE, 'own_functions.txt')).read().split('\n')
    out = []
    for l in lines:
        f = l.split()
        if f and f[0] == entry.lower().zfill(4):
            l = l.rsplit(' ', 1)[0] + (' findings' if finding else ' done')
        out.append(l)
    open(os.path.join(HERE, 'own_functions.txt'), 'w').write('\n'.join(out))
    print('accepted %s into %s' % (name, fil))


if __name__ == '__main__':
    main()
