#!/usr/bin/env python3
"""datemap.py - the build dates of the original binaries, to find the times of the compiler versions.

Usage: datemap.py [-m] [WEGA]      (WEGA: the original tree, default tmp/wega31_root; -m: a markdown table)

For every s.out program of the tree (magic E7xx nonsegmented, E6xx segmented; objects and archives are left out) it
prints: the stamp of the start-up file (`crt0.s Rev : 3.x date`, made at the build), the SCCS ids of the own sources
(`@[$]name.c version date`), the libc marks and the D14 mark:
  libc 09/83   the string `malloc.c 3.3 09/12/83` of the shipped libc is in the binary
  D14          the printf family spills the EPU registers inline (`.word 4FFC,8303`): an older printf (it is in
               the old libc of Jan 1983, but cc and scc of 11/83 have it too)
  D12          string literals in the data: "old" = an odd-length string is followed at once by the next string
               (packed, the old cparse), "new" = it is padded to an even address (the shipped cparse); the
               counts packed/padded over the first 16 strings that decide (the own modules lie in front of the libc)
  D1           the frames of the functions that save register variables (nonsegmented programs): "slots" = the
               frame has at least as many words that no instruction uses (below the top word, which may be the
               temporary of a return) as saved registers (v1: every register variable has its place in the frame),
               "tight" = no unused word at all, "loose" = between (the shipped cc); counts functions/slots/tight
SCCS ids of libc files (the names of src/lib/libc and src/lib/oldlibc) are not counted as own sources.
"""
import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'tools'))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

HERE = os.path.dirname(os.path.abspath(__file__))
SCCS = re.compile(rb'@\[\$\]\s*([A-Za-z0-9_./-]+)\s+([0-9.]+)\s+(\d\d/\d\d/\d\d)')
FIRST = 16
CRT0 = re.compile(rb'crt0\.s\s+Rev : ([0-9.]+)\s+(\d\d/\d\d/\d\d)')


def d1frames(data):
    """(functions, with unused words >= saved registers, with no unused word) over the functions that save register
    variables and take no frame address (lda): see the module comment"""
    import zcode
    from sout import SOut
    o=SOut(data); img=bytes(o.image); seg=(data[0]==0xE6)
    res=[]
    for si,kind,off,size in o.section_ranges():
        if kind!='text': continue
        ins=zcode.insns(img[off:off+size],0,size,seg)
        n=len(ins); k=0
        while k<n:
            t=ins[k][1]
            m=re.match(r'^(dec\tr15,#(\d+)|sub\tr15,#%([0-9A-F]{4}))$',t)
            if not m: k+=1; continue
            fr=int(m.group(2)) if m.group(2) else int(m.group(3),16)
            # body until ret
            e=k+1
            while e<n and ins[e][1]!='ret': e+=1
            body=[x for _,x in ins[k+1:e+1]]
            k=e+1
            nregs=0; save=fr
            for t in body[:3]:
                mm=re.match(r'^ldm\t%([0-9A-F]{4})\(r15\),r(\d+),#(\d+)$',t)
                if mm and int(mm.group(2))>=8: nregs=int(mm.group(3)); save=int(mm.group(1),16); break
                mm=re.match(r'^ld\t%([0-9A-F]{4})\(r15\),r(1[0-4]|[89])$',t)
                if mm: nregs=1; save=int(mm.group(1),16); break
            if nregs==0 or fr>0x200: continue
            if any(re.search(r'lda\tr\d+,%[0-9A-F]{4}\(r15\)',t) or re.search(r'lda\tr\d+,@r15',t) for t in body): continue
            used=set()
            for t in body:
                for mm in re.finditer(r'%([0-9A-F]{4})\(r15\)',t): used.add(int(mm.group(1),16)&~1)
                if '@r15' in t: used.add(0)
                mm=re.match(r'^ldm\t(@r15|%([0-9A-F]{4})\(r15\)),r\d+,#(\d+)$',t) or re.match(r'^ldm\tr\d+,(@r15|%([0-9A-F]{4})\(r15\)),#(\d+)$',t)
                if mm:
                    b=int(mm.group(2),16) if mm.group(2) else 0
                    for w in range(int(mm.group(3))): used.add(b+2*w)
                mm=re.search(r'ldl\t(?:rr\d+,)?%([0-9A-F]{4})\(r15\)|ldl\t%([0-9A-F]{4})\(r15\),rr\d+|ldl\t@r15|ldl\trr\d+,@r15',t)
                if mm:
                    b=int(mm.group(1) or mm.group(2) or '0',16); used.add(b+2)
            unused=[w for w in range(0,save,2) if w not in used]
            # the top word below the save area may be the return temporary of both compilers
            top = (save-2) in unused
            un=len(unused)-(1 if top else 0)
            res.append((nregs,un))
    return len(res), sum(1 for nr,un in res if un>=nr), sum(1 for nr,un in res if un==0)


def libc_names():
    names = set()
    for d in ('src/lib/libc', 'src/lib/oldlibc'):
        for root, dirs, files in os.walk(os.path.join(HERE, '..', '..', d)):
            names.update(f for f in files if f.endswith(('.c', '.s')))
            names.update(x + '.c' for x in dirs)
    return names


def key(date):
    m, d, y = date.split('/')
    return y + m + d


def scan(path, libc):
    data = open(path, 'rb').read()
    if len(data) < 16 or data[0] not in (0xE6, 0xE7) or data[1] not in (0x01, 0x05, 0x07, 0x08, 0x09, 0x11):
        return None
    c = CRT0.search(data)
    own, lib = [], []
    for m in SCCS.finditer(data):
        name = os.path.basename(m.group(1).decode())
        if name.startswith('crt0') or name.startswith('scrt0'):
            continue
        (lib if name in libc else own).append((name, m.group(2).decode(), m.group(3).decode()))
    # D12: the first FIRST decisive strings of the data (the own modules come before the libc in the link)
    packed = padded = 0
    try:
        sys.path.insert(0, os.path.join(HERE, '..', 'tools'))
        from sout import SOut
        o = SOut(data)
        img = bytes(o.image)
        for si, kind, off, size in o.section_ranges():
            if kind != 'data':
                continue
            seg = o.segs[si]
            # address of the data: behind the code in a combined (E707) program or a segment with code, else 0
            base = seg['code'] if (data[1] == 0x07 or (data[0] == 0xE6 and seg['code'])) else 0
            dd = img[off:off + size]
            i = 0
            while i < len(dd) and packed + padded < FIRST:
                j = i
                while j < len(dd) and (32 <= dd[j] < 127 or dd[j] in (9, 10)):
                    j += 1
                if j - i >= 3 and j < len(dd) and dd[j] == 0:
                    end = j + 1
                    if (base + end) & 1 and end + 1 < len(dd):
                        if 32 < dd[end] < 127 and dd[end + 1] != 0:
                            packed += 1
                        elif dd[end] == 0 and 32 < dd[end + 1] < 127:
                            padded += 1
                    i = end
                else:
                    i = j + 1
    except Exception:
        pass
    try:
        d1 = d1frames(data)
    except Exception:
        d1 = (0, 0, 0)
    return {
        'd1': d1,
        'packed': packed, 'padded': padded,
        'magic': '%02X%02X' % (data[0], data[1]),
        'crt0': (c.group(1).decode(), c.group(2).decode()) if c else None,
        'own': own,
        'lib': lib,
        'libc83': b'malloc.c\t3.3  09/12/83' in data or b'malloc.c 3.3' in data,
        'd14': b'\x4f\xfc\x83\x03' in data,
    }


def verdict(r):
    if r['packed'] >= 12:
        return 'old'
    if r['padded'] >= 12:
        return 'new'
    if r['packed'] + r['padded'] >= 4:
        return 'old?' if r['packed'] > r['padded'] else 'new?' if r['padded'] > r['packed'] else 'mixed'
    return '-'


def d1verdict(r):
    n, sl, ti = r['d1']
    if n < 5:
        return '-'
    if sl * 2 >= n:
        return 'slots'
    if ti * 10 >= n * 9:
        return 'tight'
    return 'loose'


def main():
    a = sys.argv[1:]
    md = '-m' in a
    a = [x for x in a if x != '-m']
    root = os.path.abspath(a[0] if a else os.path.join(HERE, '..', '..', '..', 'tmp', 'wega31_root'))
    libc = libc_names()
    rows = []
    for d, _, files in os.walk(root):
        if '/tmp' in d[len(root):]:
            continue
        for f in files:
            p = os.path.join(d, f)
            if os.path.islink(p) or not os.path.isfile(p) or f.endswith('.o'):
                continue
            r = scan(p, libc)
            if r:
                rows.append((os.path.relpath(p, root), r))

    def sortkey(x):
        r = x[1]
        if r['crt0']:
            return key(r['crt0'][1])
        dates = [key(o[2]) for o in r['own']]
        return (max(dates) if dates else '999999') + 'z'
    rows.sort(key=lambda x: (sortkey(x), x[0]))
    if md:
        print('| Program | Magic | crt0 stamp | own sources (newest two) | libc 09/83 | D14 | D12 packed/padded | D12 | D1 functions/slots/tight | D1 |')
        print('|---|---|---|---|---|---|---|---|---|---|')
    for p, r in rows:
        crt = '%s %s' % r['crt0'] if r['crt0'] else '-'
        own = sorted(r['own'], key=lambda o: key(o[2]))
        ownt = ', '.join('%s %s %s' % o for o in own[-2:]) if own else '-'
        if md:
            print('| %s | %s | %s | %s | %s | %s | %d/%d | %s | %d/%d/%d | %s |' % (
                p, r['magic'], crt, ownt, 'x' if r['libc83'] else '', 'x' if r['d14'] else '', r['packed'], r['padded'],
                verdict(r), r['d1'][0], r['d1'][1], r['d1'][2], d1verdict(r)))
        else:
            print('%-28s %s  crt0 %-14s libc83 %s d14 %s d12 %3d/%-3d %-5s d1 %3d/%-3d/%-3d %-5s own: %s' % (
                p, r['magic'], crt, 'x' if r['libc83'] else '-', 'x' if r['d14'] else '-', r['packed'], r['padded'],
                verdict(r), r['d1'][0], r['d1'][1], r['d1'][2], d1verdict(r), ownt))


if __name__ == '__main__':
    main()
