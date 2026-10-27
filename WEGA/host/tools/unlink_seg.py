#!/usr/bin/env python3
"""unlink_seg.py - cut a module out of a segmented WEGA program (E611, e.g. /bin/sld) as cas assembler source.

Usage: unlink_seg.py PROGRAM START END NAME OUTDIR [EXTNAMES.json]

  PROGRAM   the segmented program (its segment 0 is the code)
  START END the code range of the module (hex), one module = one range; calr/jr/djnz inside the range stay local
  NAME      module name; the source is OUTDIR/NAME.s
  EXTNAMES  optional JSON {"0xaddr": "symbol"} for call targets outside the module (default: x<addr>)

Calls to addresses outside the range become symbols (call _sym, the linker fixes them); relative branches inside the
range become local labels L<addr>. Other operands are written as they are (absolute addresses stay numbers).
verify(): assembles with cas -u (emulator) and compares the code bytes with the program, masking the words that
the assembler relocates (symbols). Returns the number of differing non-masked bytes.
"""
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import z8k  # noqa: E402
from sout import SOut  # noqa: E402

EMU = '/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu'
WEGA = '/mnt/share/cvs/P8000/tmp/wega31_root'


def decode_range(code, seg, a, b):
    buf = z8k.Buf(code, 0)
    out, pc = [], a
    while pc < b:
        i = z8k.decode(buf, pc, seg)
        if i is None:
            raise ValueError('undecodable word at %x' % pc)
        out.append((pc, i))
        pc = i.end
    if pc != b:
        raise ValueError('range end %x is not an instruction boundary (ends at %x)' % (b, pc))
    return out


def target_of(i):
    for o in i.ops:
        if o.kind == 'rel':
            return ('rel', o.a)
        if o.kind in ('da', 'x') and o.a[0] == 0:
            return ('abs', o.a[1])
    return (None, None)


def write_source(prog, seg, a, b, name, outdir, extnames, exports=None):
    code = prog[:]
    insns = decode_range(code, seg, a, b)
    internal = {pc for pc, _ in insns}
    exports = exports or {}
    labels = set()
    for pc, i in insns:
        k, t = target_of(i)
        # a jump or call to a fixed address inside the module is a label, too
        # lda loads a code address as a constant (a table of entries)
        if (k == 'rel' and i.mn in ('calr', 'jr', 'jpr', 'djnz', 'dbjnz')) or (k == 'abs' and i.mn in ('call', 'jp', 'lda')):
            if t in internal:
                labels.add(t)
    externs = []
    body = []
    for pc, i in insns:
        if pc in labels:
            body.append('L%x:' % pc)
        # the module start already carries the label _name:: in the header
        if pc in exports and pc != a:
            body.append('_%s::' % exports[pc].lstrip('_'))
        text = z8k.text(i)
        k, t = target_of(i)
        if i.mn in ('call', 'calr', 'jp') and k in ('abs', 'rel') and t is not None and t not in internal:
            sym = extnames.get('0x%x' % t, 'x%x' % t)
            sym = '_' + sym.lstrip('_')
            if sym not in externs:
                externs.append(sym)
            if i.mn == 'calr':
                text = '%s\t%s' % (i.mn, sym)
            else:
                # jp keeps its condition code: the target is the only address in the text
                text = re.sub(r'<<0>>%[0-9A-F]{4}', sym, text, count=1)
        elif i.mn in ('calr', 'jr', 'jpr', 'djnz', 'dbjnz') and k == 'rel' and t in internal:
            text = re.sub(r'%[0-9A-F]{4}', 'L%x' % t, text, count=1)
        elif i.mn in ('call', 'jp', 'lda') and k == 'abs' and t in internal:
            text = re.sub(r'<<0>>%[0-9A-F]{4}', 'L%x' % t, text, count=1)
        # the stack segment (127) is the symbol _stkseg of the libc: the original writes _stkseg+N
        if '<<127>>' in text:
            # keep the bars the instruction has: ldb uses |..| around the stack address, ldm does not
            text = re.sub(r'<<127>>%([0-9A-F]{4})', lambda m: '_stkseg+%d' % int(m.group(1), 16), text)
            if '_stkseg' not in externs:
                externs.append('_stkseg')
        body.append('\t' + text)
    lines = ['\t.seg'] + ['\t.extern\t%s' % e for e in externs] + ['\t.code', '', '_%s::' % name] + body
    src = os.path.join(outdir, name + '.s')
    with open(src, 'w') as f:
        f.write('\n'.join(lines) + '\n')
    return src


def verify(prog, seg, a, b, src, outdir):
    """assemble src with cas -u and compare with the program bytes; relocated words are masked"""
    base = os.path.splitext(os.path.basename(src))[0]
    cwd = outdir
    r = subprocess.run([EMU, '-r', WEGA, '/bin/cas', '-u', '-o', base + '.o', base + '.s'], cwd=cwd,
                       capture_output=True, text=True, timeout=300)
    if r.returncode != 0:
        return None, (r.stdout + r.stderr)[:400]
    o = SOut(open(os.path.join(cwd, base + '.o'), 'rb').read())
    t = o.segs[0]['code']
    obj = bytes(o.image[:t])
    ref = prog[a:b]
    diff = 0
    for k in range(0, min(len(obj), len(ref)), 2):
        masked = any(o.reloc[k + j] for j in (0, 1)) if k + 1 < len(o.reloc) else False
        if masked:
            continue
        if obj[k:k + 2] != ref[k:k + 2]:
            diff += 1
    return diff, 'size %d against %d' % (len(obj), len(ref))


def main():
    path, a, b, name, outdir = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16), sys.argv[4], sys.argv[5]
    extnames = json.load(open(sys.argv[6])) if len(sys.argv) > 6 else {}
    s = SOut(open(path, 'rb').read())
    codesz = s.segs[0]['code']
    prog = bytes(s.image[0:codesz])
    seg = (s.magic >> 8) == 0xE6
    os.makedirs(outdir, exist_ok=True)
    src = write_source(prog, seg, a, b, name, outdir, extnames)
    print('source', src)
    diff, info = verify(prog, seg, a, b, src, outdir)
    print('verify: differing words', diff, info)


if __name__ == '__main__':
    main()
