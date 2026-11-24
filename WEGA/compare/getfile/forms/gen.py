#!/usr/bin/env python3
# gen.py DIR: writes the bodies for try.sh into DIR (one file per form, name = form) and DIR/list. A body replaces
# "\t\tack = ACK;\n\tbad:" .. "\tfail:" of getfile.c. Blocks: H the retry test (bad:), W werr/next, O ok, N nak, C can,
# T test/drn; every order with W < O < N < C (the order of the strings in the original); H first falls in from
# "ack = ACK", else "goto bad" in front. A block in front of T is tried with and without its "goto test".
# Run all:  cat DIR/list | xargs -P 6 -I{} sh try.sh DIR/{} > res.txt; sort -t' ' -k4 -n res.txt | head
import itertools, os, sys
d = sys.argv[1]; os.makedirs(d, exist_ok=True)
G = '\t\tgoto test;\n'
W = ('\twerr:\n\t\tprintf("%c", CAN);\n\t\tprintf("disk write error\\n\\r");\n\tnext:\n\t\tanswer = getchar();\n'
     '\t\tif (answer == CAN || answer == ESC || answer == EOT)\n\t\t\tgoto done;\n\t\tack = NAK;\n\t\tretry = 0;\n' + G)
T = ('\ttest:\n\t\tif (retry >= 10 || ack == ACK)\n\t\t\tgoto next;\n\t\tif (answer == STX)\n\t\t\tgoto record;\n'
     '\tdrn:\n\t\tdrain();\n\t\tgoto bad;\n')
B = {'O': '\tok:\n\t\tprintf("%c", ack);\n', 'C': '\tcan:\n\t\tprintf("%c", CAN);\n',
     'N': '\tnak:\n\t\tdo {\n\t\t\tprintf("%c", ack);\n\t\t\tgetc1();\n\t\t} while (tflag);\n'}
# the retry tests: (text, the block it must fall into or None)
H = {'a': ('if (ack == ACK)\n\t\t\tgoto ok;\n\t\tif (retry >= 10)\n\t\t\tgoto can;\n\t\tgoto nak;\n', None),
     'b': ('if (ack == ACK)\n\t\t\tgoto ok;\n\t\tif (retry < 10)\n\t\t\tgoto nak;\n\t\tgoto can;\n', None),
     'c': ('if (ack != ACK) {\n\t\t\tif (retry < 10)\n\t\t\t\tgoto nak;\n\t\t\tgoto can;\n\t\t}\n\t\tgoto ok;\n', None),
     'd': ('if (ack != ACK) {\n\t\t\tif (retry >= 10)\n\t\t\t\tgoto can;\n\t\t\tgoto nak;\n\t\t}\n\t\tgoto ok;\n', None),
     'j': ('if (ack == ACK)\n\t\t\tgoto ok;\n\t\tif (retry >= 10)\n\t\t\tgoto can;\n', 'N'),
     'k': ('if (ack == ACK)\n\t\t\tgoto ok;\n\t\tif (retry < 10)\n\t\t\tgoto nak;\n', 'C'),
     'l': ('if (ack != ACK) {\n\t\t\tif (retry >= 10)\n\t\t\t\tgoto can;\n\t\t\tgoto nak;\n\t\t}\n', 'O')}
names = []
for h, (hs, fol) in H.items():
    HB = '\tbad:\n\t\tretry++;\n\t\t' + hs
    for o in itertools.permutations('HWONCT'):
        if not (o.index('W') < o.index('O') < o.index('N') < o.index('C')):
            continue
        i = o.index('H')
        if fol and (i == 5 or o[i + 1] != fol):
            continue
        falls = [k for k, x in enumerate(o[:-1]) if x in 'WONC' and o[k + 1] == 'T']
        for fv in ((0, 1) if falls else (0,)):
            parts = [] if o[0] == 'H' else ['\t\tgoto bad;\n']
            for k, x in enumerate(o):
                drop = fv and k in falls
                if x == 'H': parts.append(HB)
                elif x == 'T': parts.append(T)
                elif x == 'W': parts.append(W.replace(G, '') if drop else W)
                else: parts.append(B[x] + ('' if drop else G))
            n = '%s_%s_%d' % (h, ''.join(o), fv)
            open(os.path.join(d, n), 'w').write('\t\tack = ACK;\n' + ''.join(parts)); names.append(n)
open(os.path.join(d, 'list'), 'w').write('\n'.join(names) + '\n')
print(len(names), 'forms')
