#!/usr/bin/env python3
"""oldslibc_cas.py OLDSLIBC_DIR [VERSION]: the members of the libc part of /bin/cas that differ from /lib/slibc.a, cut as assembler sources.

/bin/cas holds an older slibc than the shipped one. The members equal to the shipped ones (relocated words masked) are taken from /lib/slibc.a by
the Makefile of oldslibc; only the members below are cut: OLDSLIBC_DIR/<object>/<VERSION>.s (default 0_2, an invented revision number), text and data of /bin/cas.
A call into another member is its global name, a reference to the shared data (__iob, _errno, ...) a .extern (bss: a common as in the shipped member);
the other data of a member lies in its range of the data segment and is written in the source; a word in it that is an address becomes .long.
Assembled by sccvc (the assembler that keeps bit 31 of an address constant) each source is verified against the bytes of /bin/cas, relocated words masked.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import unlink_seg as U  # noqa: E402
import z8k  # noqa: E402
from sout import SOut  # noqa: E402

U.CODESEG = 4
WEGA = os.environ.get('WEGA', '/home/olivleh1/git/P8000/tmp/wega31_root')
OLDROOT = os.environ.get('OLDROOT', '/home/olivleh1/git/P8000/tmp/oldroot')
EMU = U.EMU

# (object, text start, text end, data start, data end, {entry: names})
OBJ = [('fseek', 0x96ce, 0x982e, 0, 0, {0x96ce: ['_fseek']}),
       ('rdwr', 0x982e, 0x9938, 0, 0, {0x982e: ['_fread'], 0x98b2: ['_fwrite']}),
       ('printf', 0x995c, 0x9b4a, 0, 0, {0x995c: ['_printf'], 0x99f0: ['_fprintf'], 0x9a74: ['_sprintf']}),
       ('doprnt', 0x9b4a, 0xa61a, 0x8f32, 0x8ff6, {0x9b4a: ['__align'], 0x9bb0: ['__doprnt'], 0xa58a: ['__atoip']}),
       ('filbuf', 0xa68c, 0xa7a0, 0x8ff6, 0x900a, {0xa68c: ['__filbuf']}),
       ('flsbuf', 0xa7a0, 0xa9dc, 0, 0, {0xa7a0: ['__flsbuf'], 0xa8b6: ['_fflush'], 0xa924: ['__cleanup'], 0xa950: ['_fclose']}),
       ('endopen', 0xa9dc, 0xab9e, 0x900a, 0x901c, {0xa9dc: ['__endopen']}),
       ('findiop', 0xab9e, 0xabc8, 0, 0, {0xab9e: ['__findiop']}),
       ('perror', 0xabdc, 0xaca6, 0x9110, 0x9124, {0xabdc: ['_perror']}),
       ('cnvt', 0xaca6, 0xadbe, 0x9124, 0x9136, {0xaca6: ['__cnvt']}),
       ('length', 0xae5c, 0xae8a, 0, 0, {0xae5c: ['__length']}),
       ('malloc', 0xae8a, 0xb276, 0x9136, 0x9154, {0xae8a: ['_calloc'], 0xaee2: ['_cfree'], 0xaef6: ['_malloc'], 0xb168: ['_free'], 0xb198: ['_realloc']}),
       ('mktemp', 0xb29c, 0xb326, 0x94d0, 0x94d2, {0xb29c: ['_mktemp']}),
       ('strcpy', 0xb39a, 0xb3cc, 0, 0, {0xb39a: ['_strcpy']}),
       ('strncpy', 0xb3cc, 0xb422, 0, 0, {0xb3cc: ['_strncpy']}),
       ('strlen', 0xb422, 0xb446, 0, 0, {0xb422: ['_strlen']})]
# the entries of the members that are not cut
EXT = {0xa8b6: '_fflush', 0xb4ce: '_lseek', 0xb504: '_read', 0xaef6: '_malloc', 0xb168: '_free', 0xb276: '_isatty', 0xb490: '_close', 0xb5fe: '_write', 0xb47e: '_chmod',
       0xb4a0: '_creat', 0xb4f2: '_open', 0xb5e6: '_umask', 0xadbe: '__deciml', 0xb516: '_ssgbrk', 0xb446: '_access', 0xb4b0: '_getpid'}
# shared data, by the address in /bin/cas: (start, end, name)
SHARED = [(0x901c, 0x910c, '__iob'), (0x910c, 0x9110, '__lastbuf'), (0x9154, 0x91e0, '_sys_errlist'), (0x91e0, 0x91e2, '_sys_nerr'), (0x91e2, 0x9202, '_dev_errlist'),
          (0x9202, 0x9204, '_dev_nerr'), (0x94d2, 0x95d4, '__ctype'), (0x95d4, 0x95d6, '_errno'), (0x95d6, 0x95d8, '_deverr'), (0xc166, 0xc168, '_rc'), (0xc168, 0xc16c, '__pfile'), (0xc16c, 0xc36c, '__sobuf')]
COMM = {'doprnt': [(4, '__pfile')], 'printf': [(2, '_rc')]}


def shared(a):
    for lo, hi, n in SHARED:
        if lo <= a < hi:
            return n, a - lo
    return None


def main():
    outdir = sys.argv[1]
    ver = sys.argv[2] if len(sys.argv) > 2 else '0_2'
    g = SOut(open(os.path.join(WEGA, 'bin/cas'), 'rb').read())
    t = g.segs[0]['code']
    code = bytes(g.image[:t])
    data = bytes(g.image)[t:t + g.segs[1]['data']]
    ext = {'0x%x' % k: v for k, v in EXT.items()}
    for o in OBJ:
        for a, ns in o[5].items():
            ext['0x%x' % a] = ns[0]
    work = os.path.join(os.environ.get('RECON_TMP', '/home/olivleh1/git/P8000/tmp/tmp'), 'oldslibc_cas')
    os.makedirs(work, exist_ok=True)
    bad = 0
    for name, a, b, da, db, exports in OBJ:
        labels = set()
        for pc, i in U.decode_range(code, True, a, b):
            for m in re.finditer(r'#%[08]4([0-9A-F]{6})', z8k.text(i)):
                labels.add(int(m.group(1)[2:], 16))
        # the words of the data that are addresses of this member
        ptr = {}
        k = da
        while k + 4 <= db:
            v = int.from_bytes(data[k:k + 4], 'big')
            sg, off = (v >> 24) & 0x7f, v & 0xffff
            if (v >> 16) & 0xff == 0 and (v >> 24) in (4, 5, 0x84, 0x85) and ((sg == 4 and a <= off < b) or (sg == 5 and da <= off < db)):
                ptr[k] = (sg, off)
                if sg == 4:
                    labels.add(off)
                k += 4
            else:
                k += 2
        first = exports[a][0]
        src = U.write_source(code, True, a, b, first, work, ext, exports={k: v for k, v in exports.items()}, extra_labels=labels - {a})
        txt = open(src).read()
        dl, ex = set(), []
        for x, y in ptr.values() if ptr else []:
            if x == 5:
                dl.add(y)

        def expr(ad):
            s = shared(ad)
            if s:
                if s[0] not in ex:
                    ex.append(s[0])
                return s[0] + ('+%d' % s[1] if s[1] else '')
            if da <= ad < db:
                dl.add(ad)
                return 'D%x' % ad
            raise SystemExit('%s: data address %x neither shared nor in the range of the member' % (name, ad))
        txt = re.sub(r'<<5>>%([0-9A-F]{4})', lambda m: expr(int(m.group(1), 16)), txt)
        txt = re.sub(r'#%[08]5([0-9A-F]{2})([0-9A-F]{4})', lambda m: '#' + expr(int(m.group(2), 16)), txt)
        txt = re.sub(r'#%[08]4[0-9A-F]{2}([0-9A-F]{4})', lambda m: '#L%x' % int(m.group(1), 16), txt)
        comm = [c for c in COMM.get(name, [])]
        cn = [n for _, n in comm]
        ext2 = ''.join('\t.extern\t%s\n' % e for e in ex if e not in cn) + ''.join('\t.comm\t%d,%s\n' % c for c in comm)
        txt = txt.replace('\t.code\n', ext2 + '\t.psec\n\t.code\n', 1)
        if da != db:
            out = ['\t.psec data', '\t.data']
            k = da
            while k < db:
                if k in dl:
                    out.append('D%x:' % k)
                if k in ptr:
                    sg, off = ptr[k]
                    out.append('\t.long\t%s' % ('L%x' % off if sg == 4 else 'D%x' % off))
                    k += 4
                else:
                    out.append('\t.byte\t%%%02x' % data[k])
                    k += 1
            txt += '\n'.join(out) + '\n'
        open(src, 'w').write(txt)
        r = subprocess.run([EMU, '-r', OLDROOT, '/bin/sccvc', '-c', os.path.basename(src)], cwd=work, capture_output=True, text=True)
        o = SOut(open(os.path.join(work, first.lstrip('_') if False else os.path.basename(src)[:-2] + '.o'), 'rb').read()) if r.returncode == 0 else None
        if o is None:
            print(name, 'ASSEMBLER', (r.stdout + r.stderr)[-300:])
            bad += 1
            continue
        tc = o.segs[0]['code']
        obj = bytes(o.image[:tc])
        diff = [hex(a + k) for k in range(0, min(len(obj), b - a), 2) if not any(o.reloc[k + j] for j in (0, 1)) and obj[k:k + 2] != code[a + k:a + k + 2]]
        dd = bytes(o.image)[tc:tc + sum(s['data'] for s in o.segs)]
        rel = set()
        for k in range(len(dd)):
            if tc + k < len(o.reloc) and o.reloc[tc + k]:
                rel.update(range(k - 1, k + 4))
        ddiff = [hex(da + k) for k in range(min(len(dd), db - da)) if k not in rel and dd[k] != data[da + k]] if da != db else []
        print('%-8s text %d/%d data %d/%d differ: text %s data %s' % (name, len(obj), b - a, len(dd), db - da, diff[:4], ddiff[:6]))
        if diff or ddiff or len(obj) != b - a or len(dd) != db - da:
            bad += 1
        d = os.path.join(outdir, name)
        os.makedirs(d, exist_ok=True)
        with open(os.path.join(d, ver + '.s'), 'w') as f:
            f.write(txt)
    print('problems:', bad)


if __name__ == '__main__':
    main()
