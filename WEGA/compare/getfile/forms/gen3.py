#!/usr/bin/env python3
# gen3.py DIR [GETFILE.C]: the wide search: whole getfile.c files for try.sh (one per form) and DIR/list. The blocks of
# the retry part (H the retry test at bad:, W werr/next, O ok, N nak, C can, T test/drn) may stand in zone A (between
# "goto check;" of done and "record:") or in zone B (behind record); R is record, ending in "ack = ACK;" (falls into H
# when H follows, else "goto bad;"). Every order of the 7 with W < O < N < C (the order of the strings); a block whose
# jump goes to the block behind it is tried with and without that jump (all subsets); the block in front of R and the
# last one keep their jump. Retry tests a, b (two jumps), j (falls into N), k (falls into C), c, d (nested, goto ok),
# l (nested, falls into O); a third argument picks the retry tests (e.g. cdl).
import itertools, os, sys
d = sys.argv[1]; os.makedirs(d, exist_ok=True)
src = open(sys.argv[2] if len(sys.argv) > 2 else
           os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../../src/cmd/getfile.c')).read()
za = src.index('\t\tgoto check;\n\trecord:\n') + len('\t\tgoto check;\n')
rb = src.index('\trecord:\n'); re_ = src.index('\t\tack = ACK;\n\tbad:\n'); fl = src.index('\tfail:\n')
PRE, REC, POST = src[:za], src[rb:re_], src[fl:]
GT, GB = '\t\tgoto test;\n', '\t\tgoto bad;\n'
BL = {
 'W': ('\twerr:\n\t\tprintf("%c", CAN);\n\t\tprintf("disk write error\\n\\r");\n\tnext:\n\t\tanswer = getchar();\n'
       '\t\tif (answer == CAN || answer == ESC || answer == EOT)\n\t\t\tgoto done;\n\t\tack = NAK;\n\t\tretry = 0;\n', GT, 'T'),
 'O': ('\tok:\n\t\tprintf("%c", ack);\n', GT, 'T'),
 'N': ('\tnak:\n\t\tdo {\n\t\t\tprintf("%c", ack);\n\t\t\tgetc1();\n\t\t} while (tflag);\n', GT, 'T'),
 'C': ('\tcan:\n\t\tprintf("%c", CAN);\n', GT, 'T'),
 'T': ('\ttest:\n\t\tif (retry >= 10 || ack == ACK)\n\t\t\tgoto next;\n\t\tif (answer == STX)\n\t\t\tgoto record;\n'
       '\tdrn:\n\t\tdrain();\n', GB, 'H'),
}
HF = {'a': ('if (ack == ACK)\n\t\t\tgoto ok;\n\t\tif (retry >= 10)\n\t\t\tgoto can;\n\t\tgoto nak;\n', None),
      'b': ('if (ack == ACK)\n\t\t\tgoto ok;\n\t\tif (retry < 10)\n\t\t\tgoto nak;\n\t\tgoto can;\n', None),
      'j': ('if (ack == ACK)\n\t\t\tgoto ok;\n\t\tif (retry >= 10)\n\t\t\tgoto can;\n', 'N'),
      'k': ('if (ack == ACK)\n\t\t\tgoto ok;\n\t\tif (retry < 10)\n\t\t\tgoto nak;\n', 'C'),
      'c': ('if (ack != ACK) {\n\t\t\tif (retry < 10)\n\t\t\t\tgoto nak;\n\t\t\tgoto can;\n\t\t}\n\t\tgoto ok;\n', None),
      'd': ('if (ack != ACK) {\n\t\t\tif (retry >= 10)\n\t\t\t\tgoto can;\n\t\t\tgoto nak;\n\t\t}\n\t\tgoto ok;\n', None),
      'l': ('if (ack != ACK) {\n\t\t\tif (retry >= 10)\n\t\t\t\tgoto can;\n\t\t\tgoto nak;\n\t\t}\n', 'O')}
if len(sys.argv) > 3:	# only these retry tests
    HF = {k: v for k, v in HF.items() if k in sys.argv[3]}
names = []
for h, (hs, fol) in HF.items():
    for o in itertools.permutations('RHWONCT'):
        if not (o.index('W') < o.index('O') < o.index('N') < o.index('C')):
            continue
        i = o.index('H')
        if fol and (i == 6 or o[i + 1] != fol):
            continue
        # the blocks that may drop their jump: target behind them, not in front of R, not last
        opt = [k for k, x in enumerate(o[:-1]) if x in BL and o[k + 1] == BL[x][2] and o[k + 1] != 'R']
        for mask in range(1 << len(opt)):
            drop = {opt[b] for b in range(len(opt)) if mask >> b & 1}
            parts = []
            for k, x in enumerate(o):
                if x == 'R':
                    parts.append(REC + '\t\tack = ACK;\n' + ('' if k < 6 and o[k + 1] == 'H' else GB))
                elif x == 'H':
                    parts.append('\tbad:\n\t\tretry++;\n\t\t' + hs)
                else:
                    t, g, _ = BL[x]
                    parts.append(t + ('' if k in drop else g))
            n = '%s_%s_%d' % (h, ''.join(o), mask)
            open(os.path.join(d, n), 'w').write(PRE + ''.join(parts) + POST); names.append(n)
open(os.path.join(d, 'list'), 'w').write('\n'.join(names) + '\n')
print(len(names), 'forms')
