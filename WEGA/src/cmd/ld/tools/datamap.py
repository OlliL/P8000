#!/usr/bin/env python3
"""datamap.py - which SCCS file (module) owns each own function of /bin/ld, from the data and bss references.

E711 = separate I and D space: data addresses start at 0 (code refers to the string at data offset 0x0156 as
"ld r7,#%0156", see ld.objdu). Data is 2748 bytes; bss is assumed to follow the data (base 2748, 27082 bytes, hypothesis).
The SCCS strings (data offsets, from host/tools/sstrings.py) give the data range of each file: a file's data starts
at its SCCS string and ends at the next one (link order = order of the strings; crt0.s is generated, not a file here).
Each own function (own_functions.txt) votes with the data offsets it references; the file with most votes is its file.
Variables (bss) are listed per function, so the file of a variable is the vote of the functions that use it.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import libcver  # noqa: E402

PROG = os.environ.get('LDBIN', '/mnt/share/cvs/P8000/tmp/wega31_root/bin/ld')
DATA0, DSIZE, BSS0, BSIZE = 0, 2748, 2748, 27082
# (file, data offset of its SCCS string); a file's data runs to the next entry
FILES = [('ld.c', 0x2a), ('copyname.c', 0x5a4), ('printname.c', 0x5d8), ('symbols.c', 0x646)]


def file_of_data(off):
    best = None
    if off < FILES[0][1]:
        return None      # crt0 tag (generated start code), not a file
    for name, start in FILES:
        if off >= start:
            best = name
    return best


def string_starts():
    """start offsets of the NUL-terminated strings in the data (min 3 printable characters, as sstrings.py)"""
    import re
    from sout import SOut
    s = SOut(open(PROG, 'rb').read())
    blob = s.image[s.segs[0]['code']:s.segs[0]['code'] + DSIZE]
    return set(m.start() for m in re.finditer(rb"[\x20-\x7e\t\n\r\x08\x0c]{3,}\x00", blob))


def load_funcs():
    out = []
    for line in open(os.path.join(HERE, 'own_functions.txt')):
        if line.startswith('#') or not line.strip():
            continue
        f = line.split()
        out.append((int(f[0], 16), int(f[2], 16), f[1]))
    return out


def main():
    s, prog, raw = libcver.load_program(PROG)
    funcs = load_funcs()
    starts = string_starts()
    rows = []
    for a, e, name in funcs:
        votes = {}
        bss = set()
        datarefs = set()
        for (p, _), r in zip(prog, raw):
            if not (a <= p < e):
                continue
            for m in re.finditer(r'%([0-9A-F]{4})', r):
                v = int(m.group(1), 16)
                if DATA0 <= v < DATA0 + DSIZE and v in starts:
                    datarefs.add(v - DATA0)
                    f = file_of_data(v - DATA0)
                    if f:
                        votes[f] = votes.get(f, 0) + 1
                elif BSS0 <= v < BSS0 + BSIZE:
                    bss.add(v)
        fn = max(votes, key=votes.get) if votes else '-'
        rows.append((a, e, name, fn, sorted(datarefs), sorted(bss), votes))
    for a, e, name, fn, dr, bs, v in rows:
        print('%04x-%04x %-8s %-11s data=%s bss=%s' % (a, e, name, fn,
              ' '.join('%04x' % d for d in dr[:6]) + ('...' if len(dr) > 6 else ''),
              ' '.join('%04x' % b for b in bs[:6]) + ('...' if len(bs) > 6 else '')))
    print('--- summary')
    cnt = {}
    for r in rows:
        cnt[r[3]] = cnt.get(r[3], 0) + 1
    print(cnt)


if __name__ == '__main__':
    main()
