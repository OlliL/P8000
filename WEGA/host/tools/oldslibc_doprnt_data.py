#!/usr/bin/env python3
"""oldslibc_doprnt_data.py - complete the data of the old doprnt and printf sources (src/lib/oldslibc/{doprnt,printf}/0_1.s).

oldslibc_cut.py and libc_data_seg.py leave the data of doprnt as three placeholder words and the references to it as absolute
addresses. This tool writes the whole data area of the old doprnt (196 bytes at 0x186a..0x192e of /bin/sld):

    _d186A   "(null)" (even)
    _d1872   the conversion letters of the two scans (22 words, the second list starts at _d187E)
    _d189E   36 longs: the jump tables of the conversion switches (_d18EA and _d192A are entries 19 and 35)

The long entries are code addresses of doprnt: they become labels (L<addr>, added before the instruction at that address). The
immediates of doprnt are written as _dXXXX and the stdout address of printf (0x194e = &_iob[1]) as __iob+12.
Usage: oldslibc_doprnt_data.py OLDSLIBC_DIR      (idempotent: reads the cut source, writes the same file)
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from sout import SOut  # noqa: E402
import unlink_seg as U  # noqa: E402

PROG = '/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld'
A, B = 0x7bbc, 0x8682
DATA = 0x186a
TABLE = 0x189e
NENT = 36
# the entries of the tables that are named in the code
NAMED = {0x189e: '_d189E', 0x18ea: '_d18EA', 0x192a: '_d192A'}


def main():
    d = sys.argv[1]
    s = SOut(open(PROG, 'rb').read())
    prog = bytes(s.image[0:s.segs[0]['code']])
    data = bytes(s.image[s.segs[0]['code']:])
    seg = (s.magic >> 8) == 0xE6
    insns = U.decode_range(prog, seg, A, B)
    path = os.path.join(d, 'doprnt', '0_1.s')
    lines = open(path).read().split('\n')
    cut = lines.index('\t.data') if '\t.data' in lines else len(lines)
    body = lines[:cut]
    while body and body[-1] == '':
        body.pop()
    cd = body.index('\t.code')
    idx = [i for i in range(cd + 1, len(body)) if body[i].startswith('\t') and not body[i].startswith('\t.extern')]
    assert len(idx) == len(insns), (len(idx), len(insns))
    at = {pc: i for (pc, _), i in zip(insns, idx)}
    ents = [int.from_bytes(data[TABLE + 4 * k + 2:TABLE + 4 * k + 4], 'big') for k in range(NENT)]
    for k in range(NENT):
        assert int.from_bytes(data[TABLE + 4 * k:TABLE + 4 * k + 2], 'big') == 0
    have = {l[:-1] for l in body if re.match(r'L[0-9a-f]+:$', l)}
    ins = {}
    for t in sorted(set(ents)):
        if 'L%x' % t not in have:
            ins[at[t]] = 'L%x:' % t
    out = []
    for i, l in enumerate(body):
        if i in ins:
            out.append(ins[i])
        for a, n in (('0100186A', '_d186A'), ('01001872', '_d1872'), ('0100187E', '_d187E')):
            l = l.replace('#%' + a, '#' + n)
        out.append(l)
    out += ['', '\t.data', '', '_d186A::', '\t.byte\t' + ','.join('%d' % c for c in data[DATA:DATA + 8]), '', '_d1872::']
    for k in range(22):
        o = 0x1872 + 2 * k
        if o == 0x187e:
            out.append('_d187E::')
        out.append('\t.word\t%%%04X' % int.from_bytes(data[o:o + 2], 'big'))
    out.append('')
    for k, t in enumerate(ents):
        o = TABLE + 4 * k
        if o in NAMED:
            out.append('%s::' % NAMED[o])
        out.append('\t.long\tL%x' % t)
    open(path, 'w').write('\n'.join(out) + '\n')
    p = os.path.join(d, 'printf', '0_1.s')
    t = open(p).read().replace('#%0100194E', '#__iob+12')
    if '.extern\t__iob' not in t:
        t = t.replace('\t.extern\t', '\t.extern\t__iob\n\t.extern\t', 1)
    open(p, 'w').write(t)


if __name__ == '__main__':
    main()
