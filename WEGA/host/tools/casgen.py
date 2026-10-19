#!/usr/bin/env python3
"""casgen.py: write the data modules of cas (the tables) as C source from /bin/cas.

    casgen.py BINARY OUTDIR

OUTDIR gets pstdata.h (the permanent symbol table, included by pst.c), table1.c (the
operand forms of the instructions), table2.c (operand templates, classes, the lengths)
and pstab.c (the hash table of the permanent symbols). The ranges are those of
KNOWLEDGE.md (section cas); the data of a module is compared with the binary by
cmpdata.py.
"""
import sys, os, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sout import SOut

def data5(path):
    s = SOut(open(path, 'rb').read())
    off = sum(g['code'] for g in s.segs)
    for g in s.segs:
        if g['segno'] == 5:
            return s.image[off:off + g['data']]
        off += g['data']

def u16(d, a): return struct.unpack('>H', d[a:a+2])[0]
def u32(d, a): return struct.unpack('>I', d[a:a+4])[0]

def cstr(d, a):
    e = d.index(b'\0', a)
    return d[a:e]

def cescape(b):
    out = ''
    for c in b:
        ch = chr(c)
        if ch == '"': out += '\\"'
        elif ch == '\\': out += '\\\\'
        elif ch == '\t': out += '\\t'
        elif ch == '\n': out += '\\n'
        elif 32 <= c < 127: out += ch
        else: out += '\\%03o' % c
    return out

def sccs(d, a):
    return cstr(d, a).decode('latin-1').replace('\t', '\\t')

T1, T1END = 0x5526, 0x655e       # table1
ROWS = 0x5556
T2, T2END = 0x655e, 0x6b82       # table2
OPCLS, CLSTYPE, INSTLEN = 0x658e, 0x6798, 0x67b6
PST, PSTEND = 0x388e, 0x535a     # pst
PENT = 0x38c2                    # the entries
BUCKETS, NB = 0x7df0, 503

HDR = '/* generated from /bin/cas by host/tools/casgen.py */\n'

def gen_pst(d, out):
    n = 0
    ents = []
    a = PENT
    while True:
        tok, b2, bn = struct.unpack('>hBB', d[a:a+4])
        rows, name = u32(d, a + 4), u32(d, a + 8)
        ents.append((tok, b2, bn, rows & 0xffff if rows else 0, name & 0xffff if name else 0))
        a += 12
        if name == 0:
            break
    with open(os.path.join(out, 'pstdata.h'), 'w') as f:
        f.write(HDR)
        f.write('/* the entries: token, byte 2, number of forms, the forms (itab1), the name */\n')
        f.write('struct pst pst[] = {\n')
        for i, (tok, b2, bn, rows, name) in enumerate(ents):
            if name == 0:
                f.write('\t{ 0 }\t\t/* end */\n')
                continue
            r = '&itab1[%d]' % ((rows - ROWS) // 6) if rows else '0'
            f.write('\t{ %d, %d, %d, %s, "%s" },\n' % (tok, b2, bn, r, cescape(cstr(d, name))))
        f.write('};\n')
    return len(ents)

def gen_table1(d, out):
    nrows = (T1END - ROWS) // 6
    with open(os.path.join(out, 'table1.c'), 'w') as f:
        f.write('static char sccsid[] = "%s";\n\n' % sccs(d, T1))
        f.write('/*\n * table1 - the forms of the instructions: base opcode, offset of the operand\n'
                ' * template in opcls[] (table2.c), index into instlen[], one more byte\n */\n')
        f.write('#include "cas.h"\n\n' + HDR)
        f.write('struct irow itab1[] = {\n')
        for i in range(nrows):
            a = ROWS + 6 * i
            f.write('\t{ 0x%04x, 0x%02x, %d, %d },\n' % (u16(d, a), u16(d, a + 2), d[a + 4], d[a + 5]))
        f.write('};\n')
    return nrows

def gen_table2(d, out):
    with open(os.path.join(out, 'table2.c'), 'w') as f:
        f.write('static char sccsid[] = "%s";\n\n' % sccs(d, T2))
        f.write('/*\n * table2 - operand templates and the lengths of the instructions\n */\n')
        f.write('#include "cas.h"\n\n' + HDR)
        f.write('/* the templates (a form points into this array), each ends with 0x12 or 0x1c */\n')
        f.write('unsigned char opcls[] = {\n')
        for a in range(OPCLS, CLSTYPE, 16):
            f.write('\t' + ''.join('0x%02x,' % b for b in d[a:min(a + 16, CLSTYPE)]) + '\n')
        f.write('};\n\n/* the node type that an operand class needs */\nunsigned char clstype[] = {\n')
        f.write('\t' + ''.join('0x%02x,' % b for b in d[CLSTYPE:INSTLEN]) + '\n};\n\n')
        f.write('/* length in bytes and five bytes more for every form index */\n')
        f.write('struct ilen instlen[] = {\n')
        for a in range(INSTLEN, T2END, 6):
            f.write('\t{ %d, { %s } },\n' % (d[a], ', '.join(str(d[a + 1 + i]) for i in range(5))))
        f.write('};\n')

def gen_pstab(d, out):
    with open(os.path.join(out, 'pstab.c'), 'w') as f:
        f.write('/*\n * pstab - the hash table of the permanent symbols (503 buckets, chained inside the array)\n */\n')
        f.write('#include "cas.h"\n\n' + HDR)
        f.write('extern struct pst pst[];\n\n')
        f.write('struct pbucket pbuckets[%d] = {\n' % NB)
        for i in range(NB):
            a = BUCKETS + 8 * i
            nx, en = u32(d, a) & 0xffff, u32(d, a + 4) & 0xffff
            nxs = '&pbuckets[%d]' % ((nx - BUCKETS) // 8) if nx else '0'
            ens = '&pst[%d]' % ((en - PENT) // 12) if en else '0'
            f.write('\t{ %s, %s },\n' % (nxs, ens))
        f.write('};\n')

def main(binary, out):
    d = data5(binary)
    os.makedirs(out, exist_ok=True)
    print('pst entries:', gen_pst(d, out))
    print('itab1 rows:', gen_table1(d, out))
    gen_table2(d, out)
    gen_pstab(d, out)

if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
