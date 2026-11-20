#!/usr/bin/env python3
"""oldslibc_gopt.py: the libc part of /lib/gopt (segments 4 and 5, data 7, bss 8) cut into assembler sources, as oldslibc_cas.py does for /bin/cas.

gopt has no symbol table. The names come from the members that are known (their C sources built by sccvc -O -DSEG, or the member of /lib/slibc.a):
the relocations of such an object say which symbol an address word is, the linked word at the same place says where the symbol lies in gopt.
`names` prints that table; `cut NAME` writes oldslibc/NAME/VERSION.s for one member of MEMBERS (the ones no C source form reaches).
Usage: oldslibc_gopt.py names | cut OLDSLIBC_DIR [NAME...]    (env OLDSLIBC_VERSION, default 0_3)
"""
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', 'recon'))
import unlink_seg as U  # noqa: E402
import zcode  # noqa: E402
import war  # noqa: E402
import z8k  # noqa: E402
from sout import SOut  # noqa: E402

WEGA = os.environ.get('WEGA', '/home/olivleh1/git/P8000/tmp/wega31_root')
OLDROOT = os.environ.get('OLDROOT', '/home/olivleh1/git/P8000/tmp/oldroot')
EMU = U.EMU
TMP = os.environ.get('RECON_TMP', '/home/olivleh1/git/P8000/tmp/tmp')
VERSION = '0_3'    # invented revision number of the gopt members (0_1 sld, 0_2 cas)
SRC = os.path.join(HERE, '..', '..', 'src', 'lib')
CODE = {4: (0, 65534), 5: (65534, 50432)}    # segment -> (image offset, size)
DATA7 = 0x1c4fe                              # image offset of the data of segment 7 (the string of chklpdefs is at 7:6bf1)

# members that a C source reaches: (name, source, segment, address); built by sccvc -O -DSEG with the old stdio.h
CMEM = [('cnvt', 'libc/common/gen/cnvt.c', 5, 0xbc3e), ('filbuf', 'libc/common/stdio/filbuf.c', 5, 0xb712),
        ('findiop', 'libc/common/stdio/findiop.c', 5, 0xbc00), ('fopen', 'libc/common/stdio/fopen.c', 4, 0xff54),
        ('isatty', 'libc/common/gen/isatty.c', 5, 0xc1b4), ('length', 'libc/common/gen/length.c', 5, 0xbdf6),
        ('rdwr', 'libc/common/stdio/rdwr.c', 4, 0xfe50), ('scanf', 'libc/common/stdio/scanf.c', 5, 0xa170),
        ('strcat', 'libc/common/gen/strcat.c', 5, 0xc218), ('strcpy', 'libc/common/gen/strcpy.c', 5, 0xc252),
        ('stty', 'libc/common/gen/stty.c', 5, 0xc1dc), ('tolower', 'libc/common/gen/tolower.c', 5, 0xc282),
        ('ungetc', 'libc/common/stdio/ungetc.c', 5, 0xb2ba), ('getw', 'libc/common/stdio/getw.c', 4, 0xff78),
        ('flsbuf', 'oldslibc/flsbuf/0_3.c', 5, 0xb81e)]
# members equal to the shipped ones: (name of the member of slibc.a, segment, address)
SMEM = [('atof', 5, 0xb326), ('pow10', 5, 0xb69c), ('decimal', 5, 0xbd58), ('cuexit', 5, 0xbc32),
        ('chmod', 5, 0xc2be), ('close', 5, 0xc2d0), ('creat', 5, 0xc2f6), ('ioctl', 5, 0xc306), ('lseek', 5, 0xc31e), ('mkseg', 5, 0xc332),
        ('open', 5, 0xc358), ('read', 5, 0xc380), ('sgbrk', 5, 0xc392), ('ssgbrk', 5, 0xc3a2), ('umask', 5, 0xc3b8), ('write', 5, 0xc3d6),
        ('chkseg', 5, 0xc3e8), ('cerror', 5, 0xc400)]


def load():
    g = SOut(open(os.path.join(WEGA, 'lib/gopt'), 'rb').read())
    return g, bytes(g.image)


def find_src(rel):
    for base in (SRC, os.path.join(SRC, 'libc'), os.path.join(HERE, '..', '..', 'src')):
        p = os.path.join(base, rel)
        if os.path.exists(p):
            return p
    return os.path.join(SRC, rel)


def compile_c(rel):
    src = find_src(rel)
    w = tempfile.mkdtemp(dir=TMP)
    shutil.copy(src, w)
    shutil.copy(os.path.join(HERE, '..', '..', 'src', 'head', 'oldv7', 'stdio.h'), w)
    base = os.path.splitext(os.path.basename(src))[0]
    r = subprocess.run([EMU, '-r', OLDROOT, '/bin/sccvc', '-O', '-DSEG', '-I.', '-c', base + '.c'], cwd=w, capture_output=True, text=True, timeout=600)
    obj = os.path.join(w, base + '.o')
    if not os.path.exists(obj):
        raise SystemExit('%s: %s' % (rel, (r.stdout + r.stderr)[-200:]))
    o = SOut(open(obj, 'rb').read())
    shutil.rmtree(w, ignore_errors=True)
    return o


def shipped(name):
    for m in war.members(open(os.path.join(WEGA, 'lib/slibc.a'), 'rb').read()):
        if m['name'] == name + '.o':
            return SOut(m['data'] if 'data' in m else m['body'])
    raise SystemExit('no member ' + name)


def harvest(g, img):
    """(code names {(seg, off): name}, data names {(seg, off): name}) from the known members"""
    cn, dn = {}, {}
    objs = [(n, compile_c(s), sg, a) for n, s, sg, a in CMEM] + [(n, shipped(n), sg, a) for n, sg, a in SMEM]
    for name, o, sg, a in objs:
        tc = o.segs[0]['code']
        code = bytes(o.image[:tc])
        lin = img[CODE[sg][0] + a:CODE[sg][0] + a + tc]
        for y in o.symbols:       # defined text symbols: the name of the entry in gopt
            if y['segt'] == 0 and (y['type'] & 0x0f) == 2 and y['type'] & 0x60 and y['name']:
                cn.setdefault((sg, a + y['value']), y['name'])
        for k in range(0, tc - 2, 2):
            rw = struct.unpack('>H', o.reloc[k:k + 2])[0]
            if rw & 8 and (rw & 7) == 1:    # ext seg, then the off16 word
                y = o.symidx.get(rw >> 4)
                rw2 = struct.unpack('>H', o.reloc[k + 2:k + 4])[0]
                if not y or not (rw2 & 8) or (rw2 & 7) != 0:
                    continue
                add = struct.unpack('>H', code[k + 2:k + 4])[0]
                sw = struct.unpack('>H', lin[k:k + 2])[0]
                off = (struct.unpack('>H', lin[k + 2:k + 4])[0] - add) & 0xffff
                seg = (sw >> 8) & 0x7f
                tab = cn if seg in (4, 5) else dn
                tab.setdefault((seg, off), y['name'])
    return cn, dn, objs


# data of gopt: shared symbols (start, end, name) and the data range of the members that are cut
SHARED7 = [(0x779e, 0x788e, '__iob'), (0x788e, 0x7892, '__lastbuf'), (0x78b0, 0x79b2, '__ctype'), (0x79b2, 0x79b4, '_errno'), (0x79b4, 0x79b6, '_deverr')]
DATA = {'fltioc': (0x7500, 0x7516), 'doscan': (0x7516, 0x75b4), 'doprnt': (0x75b4, 0x7628), 'ecvt': (0x76fc, 0x772a), 'malloc': (0x7892, 0x78b0)}
# names of the entries of the members that are cut (by the order of the symbols of the shipped member and the function starts of findings.json)
ENTRY = {0x9bbe: '__fltuse', 0x9bc0: '__inflt', 0x9efe: '_gcvt', 0x9f2c: '_fegcvt', 0x9f5e: '__fegcvt',
         0xa264: '__doscan', 0xa49a: '__innum', 0xa78c: '__instr', 0xa8de: '__getccl',
         0xa94e: '_printf', 0xaa8a: '__strout', 0xa9b0: '_fprintf', 0xaa06: '_sprintf', 0xaafc: '__align', 0xab06: '__doprnt',
         0xb49e: '_ecvt', 0xb4b4: '_fcvt', 0xb4cc: '__cvt', 0xba54: '__endopen', 0xbe20: '_malloc', 0xc0a4: '_free', 0xc0d4: '_realloc', 0xc2a8: '_chmod', 0xc2e0: '_creat', 0xc342: '_open', 0xc36a: '_read', 0xc3c0: '_write'}
# the members that are cut: (name, segment, start, end, {entry: [names]}); the entries not named here are named by `exports()`
CUT = [('fseek', 4, 0xfc3a, 0xfd9a), ('ftell', 4, 0xfd9a, 0xfe50), ('fltioc', 5, 0x9bbe, 0xa170), ('doscan', 5, 0xa264, 0xa94e),
       ('printf', 5, 0xa94e, 0xaa8a), ('doprnt', 5, 0xaa8a, 0xb2ba), ('ecvt', 5, 0xb49e, 0xb69c), ('endopen', 5, 0xba54, 0xbc00),
       ('malloc', 5, 0xbe20, 0xc1b4), ('chmod', 5, 0xc2a8, 0xc2d0), ('creat', 5, 0xc2e0, 0xc306), ('open', 5, 0xc342, 0xc36a),
       ('read', 5, 0xc36a, 0xc392), ('write', 5, 0xc3c0, 0xc3e8)]


def segimg(img, sg):
    return img[CODE[sg][0]:CODE[sg][0] + CODE[sg][1]]


def call_targets(img):
    """{(seg, off): [callers]} of every call/jp/lda to an absolute code address"""
    out = {}
    for sg in (4, 5):
        code = segimg(img, sg)
        for pc, t in zcode.insns(code, 0, CODE[sg][1], True):
            for m in re.finditer(r'<<([45])>>%([0-9A-F]{4})', t):
                out.setdefault((int(m.group(1)), int(m.group(2), 16)), []).append((sg, pc))
    return out


def exports(name, sg, a, b, targets, cn):
    ents = sorted(o for (s_, o), cl in targets.items() if s_ == sg and a <= o < b and any(not (c == sg and a <= p < b) for c, p in cl))
    ents = sorted(set(ents) | {e for e in ENTRY if a <= e < b and sg == 5} | {a})
    names = {}
    try:
        o = shipped(name)
        sy = sorted((y['value'], y['name']) for y in o.symbols if y['segt'] == 0 and (y['type'] & 0x0f) == 1 and False)
    except SystemExit:
        o = None
    for e in ents:
        names[e] = ENTRY.get(e) or cn.get((sg, e)) or ('x%d_%x' % (sg, e))
    return names


def gen(img, data, name, sg, a, b, cn, dn, targets, outdir):
    code = segimg(img, sg)
    insns = U.decode_range(code, True, a, b)
    internal = {pc for pc, _ in insns}
    ex = exports(name, sg, a, b, targets, cn)
    ex[a] = ex[a] if not ex[a].startswith('x') else '_' + name
    labels = set()
    for pc, i in insns:
        for o in i.ops:
            if o.kind == 'rel' and o.a in internal:
                labels.add(o.a)
    for t in re.findall(r'<<([45])>>%([0-9A-F]{4})', '\n'.join(z8k.text(i) for _, i in insns)):
        if int(t[0]) == sg and int(t[1], 16) in internal:
            labels.add(int(t[1], 16))
    ext, unresolved, body = [], set(), []

    def cname(s_, off):
        if s_ == sg and a <= off < b:
            labels.add(off)
            return 'L%x' % off
        n = ENTRY.get(off) if s_ == 5 else None
        n = n or cn.get((s_, off))
        if n is None:
            unresolved.add(('code', s_, off))
            n = 'x%d_%x' % (s_, off)
        if n not in ext:
            ext.append(n)
        return n

    dl = set()
    rng = DATA.get(name)

    def dname(s_, off):
        if s_ == 63:
            if '_stkseg' not in ext:
                ext.append('_stkseg')
            return '_stkseg+%d' % off
        if s_ == 7:
            for lo, hi, n in SHARED7:
                if lo <= off < hi:
                    if n not in ext:
                        ext.append(n)
                    return n + ('+%d' % (off - lo) if off > lo else '')
            if rng and rng[0] <= off < rng[1]:
                dl.add(off)
                return 'D%x' % off
        for k in range(0, 5):
            n = dn.get((s_, off - k))
            if n:
                if n not in ext:
                    ext.append(n)
                return n + ('+%d' % k if k else '')
        unresolved.add(('data', s_, off))
        return 'd%d_%x' % (s_, off)
    for pc, i in insns:
        t = z8k.text(i)
        t = re.sub(r'<<(\d+)>>%([0-9A-F]{4})', lambda m: cname(int(m.group(1)), int(m.group(2), 16)) if int(m.group(1)) in (4, 5) else dname(int(m.group(1)), int(m.group(2), 16)), t)
        t = re.sub(r'#%([8-9A-F][0-9A-F])00([0-9A-F]{4})', lambda m: m.group(0) if (int(m.group(1), 16) & 0x7f) not in (4, 5, 7, 8) else '#' + (cname(int(m.group(1), 16) & 0x7f, int(m.group(2), 16)) if (int(m.group(1), 16) & 0x7f) in (4, 5) else dname(int(m.group(1), 16) & 0x7f, int(m.group(2), 16))), t)
        if i.mn in ('calr', 'jr', 'jpr', 'djnz', 'dbjnz'):
            tg = [o.a for o in i.ops if o.kind == 'rel']
            if tg and tg[0] in internal:
                t = re.sub(r'%[0-9A-F]{4}', 'L%x' % tg[0], t, count=1)
                labels.add(tg[0])
        body.append((pc, t))
    ptr = {}
    if rng:
        k = rng[0]
        while k < rng[1]:
            v = int.from_bytes(img[DATA7 + k:DATA7 + k + 4], 'big')
            sg2, off2, mid = (v >> 24) & 0x7f, v & 0xffff, (v >> 16) & 0xff
            if k + 4 <= rng[1] and v >> 31 and mid == 0 and (sg2 in (4, 5) or (sg2 == 7 and rng[0] <= off2 < rng[1])):
                ptr[k] = (sg2, off2)
                if sg2 in (4, 5):
                    labels.add(off2)
                else:
                    dl.add(off2)
                k += 4
            else:
                k += 2
    out = ['\t.seg'] + ['\t.extern\t%s' % e for e in ext] + ['\t.code', '']
    for pc, t in body:
        if pc in labels:
            out.append('L%x:' % pc)
        if pc in ex:
            out.append('%s::' % ex[pc])
        out.append('\t' + t)
    if rng:
        out += ['', '\t.psec data', '\t.data']
        k = rng[0]
        while k < rng[1]:
            if k in dl:
                out.append('D%x:' % k)
            if k in ptr:
                sg2, off2 = ptr[k]
                out.append('\t.addr\t%s' % ('L%x' % off2 if sg2 in (4, 5) else 'D%x' % off2))
                k += 4
            else:
                out.append('\t.byte\t%%%02x' % img[DATA7 + k])
                k += 1
    src = os.path.join(outdir, name + '.s')
    open(src, 'w').write('\n'.join(out) + '\n')
    return src, unresolved, ex


def verify(img, name, sg, a, b, src, work):
    r = subprocess.run([EMU, '-r', OLDROOT, '/bin/sccvc', '-c', os.path.basename(src)], cwd=work, capture_output=True, text=True)
    objf = src[:-2] + '.o'
    if r.returncode != 0 or not os.path.exists(objf):
        print('  ASSEMBLER', (r.stdout + r.stderr)[-300:])
        return
    o = SOut(open(objf, 'rb').read())
    tc = o.segs[0]['code']
    obj = bytes(o.image[:tc])
    code = segimg(img, sg)[a:b]
    diff = [hex(a + k) for k in range(0, min(len(obj), len(code)), 2) if not any(o.reloc[k + j] for j in (0, 1)) and obj[k:k + 2] != code[k:k + 2]]
    dd = bytes(o.image)[tc:tc + sum(s_['data'] for s_ in o.segs)]
    rng = DATA.get(name)
    ddiff = []
    if rng:
        ref = img[DATA7 + rng[0]:DATA7 + rng[1]]
        rel = set()
        for k in range(len(dd)):
            if tc + k < len(o.reloc) and o.reloc[tc + k]:
                rel.update(range(k - 1, k + 4))
        ddiff = [hex(rng[0] + k) for k in range(min(len(dd), len(ref))) if k not in rel and dd[k] != ref[k]]
    print('  text %d/%d differ %s  data %d/%d differ %s' % (len(obj), len(code), diff[:6], len(dd), (rng[1] - rng[0]) if rng else 0, ddiff[:6]))


def main():
    a = sys.argv[1:]
    g, img = load()
    cn, dn, objs = harvest(g, img)
    if a[0] == 'names':
        for k, v in sorted(cn.items()):
            print('code %d:%04x %s' % (k[0], k[1], v))
        for k, v in sorted(dn.items()):
            print('data %d:%04x %s' % (k[0], k[1], v))
        return
    if a[0] == 'cut':
        outdir = a[1]
        global VERSION
        VERSION = os.environ.get('OLDSLIBC_VERSION', '0_3')
        targets = call_targets(img)
        work = os.path.join(TMP, 'oldslibc_gopt')
        os.makedirs(work, exist_ok=True)
        for name, sg, lo, hi in CUT:
            if len(a) > 2 and name not in a[2:]:
                continue
            src, unres, ex = gen(img, None, name, sg, lo, hi, cn, dn, targets, work)
            print(name, 'entries', {hex(k): v for k, v in ex.items()}, 'unresolved', sorted(unres)[:12])
            verify(img, name, sg, lo, hi, src, work)
            d = os.path.join(outdir, name)
            os.makedirs(d, exist_ok=True)
            shutil.copy(src, os.path.join(d, VERSION + '.s'))
        return


if __name__ == '__main__':
    main()
