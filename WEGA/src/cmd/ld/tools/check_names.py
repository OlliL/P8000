#!/usr/bin/env python3
"""check_names.py - names of ../sld.c that the old compiler cannot tell apart.

Usage: check_names.py

The old cc keeps only the first 8 characters of an identifier (an external name is '_' + 7 characters in the
object): two identifiers that are equal in their first 8 characters are the same symbol. Prints the groups of
identifiers equal in the first 8 characters (an error) and the groups equal in the first 7 characters (a risk for
external names), and the number of identifiers (the compiler's symbol table is small, about 560 file-scope names).
"""
import collections
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))


def main():
    t = open(os.path.join(HERE, '..', 'sld.c')).read()
    t = re.sub(r'/\*.*?\*/', '', t, flags=re.S)
    t = re.sub(r'"(\\.|[^"\\])*"', '""', t)
    ids = set(re.findall(r'\b[A-Za-z_][A-Za-z_0-9]*\b', t))
    for n, what in ((8, 'error'), (7, 'risk for external names')):
        g = collections.defaultdict(set)
        for i in ids:
            if len(i) >= n:
                g[i[:n]].add(i)
        bad = {k: v for k, v in g.items() if len(v) > 1}
        print('first %d characters equal (%s): %d' % (n, what, len(bad)))
        for k, v in sorted(bad.items()):
            print('  %s: %s' % (k, ' '.join(sorted(v))))
    print('identifiers in the file: %d' % len(ids))


if __name__ == '__main__':
    main()
