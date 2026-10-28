#!/usr/bin/env python3
"""assign_files.py - file (SCCS module) of every own function of /bin/ld.

The C compiler stores the SCCS strings in link order and keeps the functions in the order of the sources, so the
text order is the order of the files. Rule:
  - crt0 functions (before CRT0_END) belong to crt0 (generated, not a file of this directory);
  - a function with a string vote (datamap.txt) belongs to the voted file ('vote');
  - any other function belongs to the file of the preceding function ('guess': it lies between two files, the
    earlier one is taken; corrections later).
Votes must be monotone in text order; a vote that goes back is reported.
Output: assigned.txt (address, name, file, reason).
"""
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
CRT0_END = 0x0018
ORDER = ['ld.c', 'copyname.c', 'printname.c', 'symbols.c']


def read_datamap():
    rows = []
    for line in open(os.path.join(HERE, 'datamap.txt')):
        if not re.match(r'[0-9a-f]{4}-', line):
            continue
        rng, name, fil = line.split()[:3]
        a = int(rng.split('-')[0], 16)
        rows.append((a, name, fil if fil not in ('-', 'None') else None))
    return rows


def main():
    rows = read_datamap()
    vote = {a: f for a, n, f in rows if f}
    names = {a: n for a, n, f in rows}
    out = []
    cur = None
    back = []
    for a, n, f in rows:
        if a < CRT0_END:
            out.append('%04x %-8s %-11s %s' % (a, names[a], 'crt0', 'generated start code'))
            continue
        if a in vote:
            if cur and ORDER.index(vote[a]) < ORDER.index(cur):
                back.append((a, vote[a], cur))
            cur = vote[a]
            reason = 'vote'
        else:
            reason = 'guess' if cur else 'guess (first own function)'
            if cur is None:
                cur = 'ld.c'
        out.append('%04x %-8s %-11s %s' % (a, names[a], cur, reason))
    open(os.path.join(HERE, 'assigned.txt'), 'w').write('\n'.join(out) + '\n')
    files = {}
    for line in out:
        files[line.split()[2]] = files.get(line.split()[2], 0) + 1
    print('functions per file:', files)
    print('votes going back in text order:', back or 'none')


if __name__ == '__main__':
    main()
