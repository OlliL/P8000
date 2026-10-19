#!/usr/bin/env python3
"""cmpyacc.py: compare the tables yacc generated (y.tab.c / parser.c) with those of /bin/cas.

    cmpyacc.py PARSER.C BINARY

The tables yyexca, yyact, yypact, yypgo, yyr1, yyr2, yychk, yydef are read from the C
source and from the data segment of the binary (addresses in casyacc.py).  They are
the same only if the grammar (rules and their order, the numbers of the tokens, the order
in which tokens and nonterminals are declared) is the same.
"""
import re, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casyacc

NAMES = dict(exca='yyexca', act='yyact', pact='yypact', pgo='yypgo', r1='yyr1', r2='yyr2',
             chk='yychk', defl='yydef')

def table(src, name):
    m = re.search(r'\b' + name + r'\[\]\s*=\s*\{(.*?)\};', src, re.S)
    if not m:
        return None
    # yacc writes a minus sign and the number apart: "-  40"
    return [int(x.replace(' ', '')) for x in re.findall(r'-?\s*\d+', m.group(1))]

def main(argv):
    src = open(argv[0]).read()
    orig = casyacc.load(argv[1])
    bad = 0
    for k, name in NAMES.items():
        a, b = table(src, name), orig[k]
        same = a == b
        bad += not same
        print('%-7s %s (%s entries)' % (name, 'same' if same else 'DIFFERENT', len(b)))
    return 1 if bad else 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
