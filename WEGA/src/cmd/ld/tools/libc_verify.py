#!/usr/bin/env python3
"""libc_verify.py - check of the libc part of /bin/ld, module by module, against the original bytes.

For every line of ld.link the module is taken (old: the cut version in OLDDIR/NAME/0_n.s, assembled with cas -u;
shipped: the member of libc.a), its text is compared with the text of /bin/ld at the start given in ld.spec, and its
data with the data of /bin/ld at dstart. Relocated words (the object has a relocation there) are not compared.
Output: per module the number of differing words, and the total.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import war  # noqa: E402
from sout import SOut  # noqa: E402

ROOT = '/mnt/share/cvs/P8000/tmp/wega31_root'
EMU = ['/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu', '-r', ROOT]
OLD = os.environ.get('OLDDIR', '/mnt/share/cvs/P8000/WEGA/src/lib/oldlibc')
WORK = os.environ.get('WORKDIR', '/mnt/share/cvs/P8000/tmp/tmp/verify')


def spec():
    out = {}
    for line in open(os.path.join(HERE, 'ld.spec')):
        f = line.split('#')[0].split()
        if len(f) < 5 or f[1] not in ('old', 'shipped'):
            continue
        dstart = None
        for x in f[5:]:
            if x.startswith('dstart='):
                dstart = int(x[7:], 16)
        start = None if f[2] == '-' else int(f[2], 16)
        end = None if f[3] == '-' else int(f[3], 16)
        out[f[0]] = dict(kind=f[1], start=start, end=end, data=int(f[4]), dstart=dstart)
    return out


def link_list():
    rows = []
    for line in open(os.path.join(HERE, 'ld.link')):
        if line.startswith('#') or not line.strip():
            continue
        f = line.split()
        if f[0] == 'shipped':
            rows.append((f[1], 'shipped', None))
        else:
            rows.append((f[1], 'old', f[2]))
    return rows


def object_of(kind, name, ver):
    if kind == 'shipped':
        for m in war.members(open(os.path.join(ROOT, 'lib/libc.a'), 'rb').read()):
            if m['name'] == name + '.o':
                return SOut(m['body'])
        raise KeyError(name)
    os.makedirs(WORK, exist_ok=True)
    src = os.path.join(OLD, name, ver + '.s')
    base = os.path.join(WORK, name + '_' + ver)
    tmp = os.path.join(WORK, 'a.out')
    if os.path.exists(tmp):
        os.remove(tmp)
    import shutil
    shutil.copy(src, os.path.join(WORK, name + '_' + ver + '.s'))
    subprocess.run(EMU + ['/bin/cas', '-u', name + '_' + ver + '.s'], cwd=WORK, check=True,
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    os.replace(tmp, base + '.o')
    return SOut(open(base + '.o', 'rb').read())


def compare(obj, img, base, length, off=0, mask_relocs=True):
    """differing words between obj.image[off:off+length] and img[base:base+length]; relocated words skipped.
    The relocation table of the object covers the whole image (text, then data), so the offset is off."""
    bad = 0
    relocs = set()
    if mask_relocs and obj.reloc:
        for a in range(0, min(len(obj.reloc), off + length) - 1, 2):
            if obj.reloc[a] or obj.reloc[a + 1]:
                if off <= a < off + length:
                    relocs.add(a - off)
    for a in range(0, length - 1, 2):
        if a in relocs:
            continue
        if obj.image[off + a:off + a + 2] != img[base + a:base + a + 2]:
            bad += 1
    return bad, len(relocs)


def main():
    sp = spec()
    img = SOut(open(os.path.join(ROOT, 'bin/ld'), 'rb').read())
    code = img.segs[0]['code']
    data = img.image[code:]
    total = 0
    for name, kind, ver in link_list():
        m = sp[name]
        obj = object_of(kind, name, ver or '')
        if m['start'] is None:
            tlen, tbad, trel, size_ok = 0, 0, 0, obj.segs[0]['code'] == 0
        else:
            tlen = m['end'] - m['start']
            tbad, trel = compare(obj, img.image, m['start'], min(tlen, obj.segs[0]['code']), 0)
            size_ok = obj.segs[0]['code'] == tlen
        dbad = drel = 0
        if m['data']:
            # the data of the module: its .data section in the object, at dstart in the data of the program
            dbad, drel = compare(obj, data, m['dstart'], m['data'], obj.segs[0]['code'])
        total += tbad + dbad
        print('%-10s %-7s %-4s text %4d bytes %s differing %d (reloc %d) | data %3d differing %d (reloc %d)' % (
            name, kind, ver or '', tlen, 'ok' if size_ok else 'SIZE %d' % obj.segs[0]['code'],
            tbad, trel, m['data'], dbad, drel))
    print('total differing words (text and data, relocated words excluded):', total)


if __name__ == '__main__':
    main()
