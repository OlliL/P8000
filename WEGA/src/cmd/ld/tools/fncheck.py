#!/usr/bin/env python3
"""fncheck.py - compile a C source with the original scc (emulator) and compare its code with /bin/sld.

Usage: fncheck.py SOURCE.c LO HI [ORIGINAL]

  SOURCE.c  the C source (one or more functions, K&R style); it is copied to a scratch directory
  LO HI     the code range of the original (hex), e.g. 0762 078c; the instructions are compared in order
  ORIGINAL  the original segmented program (default: /bin/sld of the WEGA 3.1 root)

The object is built with 'scc -O -c' (flags from the environment variable SCCFLAGS, default -O). Addresses in operands are normalized (ADDR), because the object
is not linked; calr and call are equal here (D17, see norm). Frame offsets are compared (environment STRICT=0 turns that off). Prints OK when the instruction sequences are equal, otherwise the differing instructions.
"""
import difflib
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import z8k  # noqa: E402
from sout import SOut  # noqa: E402

EMU = '/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu'
WEGA = '/mnt/share/cvs/P8000/tmp/wega31_root'
ORIG = os.path.join(WEGA, 'bin', 'sld')


def insns(code, lo, hi, seg):
    buf = z8k.Buf(code, 0)
    out, pc = [], lo
    while pc < hi:
        i = z8k.decode(buf, pc, seg)
        if i is None:
            out.append((pc, '.word %04x' % buf.w(pc)))
            pc += 2
            continue
        out.append((pc, z8k.text(i)))
        pc = i.end
    return out


def norm(t):
    # segment numbers are set by the linker (the stack is segment 127 only after the link)
    t = re.sub(r'<<\d+>>', '', t)
    if os.environ.get('STRICT', '1') != '0':
        # frame displacements (...)(r15) and addresses of locals (#%7Fxxxxxx) are kept: they are not
        # changed by the link, so a wrong offset is a real difference
        keep = []

        def hold(m):
            keep.append(m.group(0))
            return '\0%d\0' % (len(keep) - 1)
        t = re.sub(r'%[0-9A-Fa-f]{3,}\|?\(r15\)', hold, t)
        t = re.sub(r'#%7F([0-9A-Fa-f]{6})', r'#%00\1', t)   # stack segment is set by the linker
        # a local address is #%7Fxxxxxx in the original and #%00xxxxxx in the object; whether an object immediate
        # is a local address (followed by add rN,r15) or the offset of a global is decided in keep_locals()
        t = re.sub(r'#%00[0-9A-Fa-f]{6}', hold, t)
        t = re.sub(r'%[0-9A-Fa-f]{3,}', 'ADDR', t)
        t = re.sub(r'\0(\d+)\0', lambda m: keep[int(m.group(1))], t)
    else:
        t = re.sub(r'%[0-9A-Fa-f]{3,}', 'ADDR', t)
    # calr or call: the call form depends on the distance to the target in the final object (D17);
    # it is checked again when the whole file is compiled
    return re.sub(r'^calr\t', 'call\t', t)


def keep_locals(lst):
    """an immediate #%00xxxxxx stays only in ldl followed by an add of the frame register (address of a local);
    in every other instruction it is the offset of a global (relocated in the link) and is masked"""
    out = []
    for i, t in enumerate(lst):
        local = re.match(r'^ldl\trr\d+,#%00[0-9A-Fa-f]{6}$', t) and i + 1 < len(lst) and \
            re.match(r'^add\tr\d+,r15$', lst[i + 1])
        if not local:
            t = re.sub(r'#%00[0-9A-Fa-f]{6}', '#ADDR', t)
        out.append(t)
    return out


def libc_aliases():
    """name of a libc function in sld.c -> f<address> (tools/libc_names.txt)"""
    al = {}
    for line in open(os.path.join(HERE, 'libc_names.txt')):
        if line.startswith('#') or not line.strip():
            continue
        a, c = line.split()[:2]
        al[c] = 'f' + a
    return al


_TOK = re.compile(r'/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|[A-Za-z_][A-Za-z0-9_]*', re.S)


def alias_text(text):
    """The old compiler loses function names depending on the exact identifiers (symbol table, see KNOWLEDGE, sld compiler
    limits); the compile checks therefore see the libc names as f<address> (as before they were named). Comments and
    strings are not touched."""
    al = libc_aliases()

    def rep(m):
        t = m.group(0)
        return al.get(t, t) if (t[0].isalpha() or t[0] == '_') else t
    return _TOK.sub(rep, text)


def alias_copy(src, dst):
    text = open(src).read()
    open(dst, 'w').write(alias_text(text))


def compile_obj(src):
    tmp = tempfile.mkdtemp(prefix='fncheck', dir='/mnt/share/cvs/P8000/tmp/tmp')
    base = os.path.splitext(os.path.basename(src))[0]
    alias_copy(src, os.path.join(tmp, base + '.c'))
    r = subprocess.run([EMU, '-r', WEGA, '/bin/scc'] + os.environ.get('SCCFLAGS', '-O').split() + ['-c', base + '.c'], cwd=tmp,
                       capture_output=True, text=True, timeout=600)
    if r.returncode != 0 or not os.path.exists(os.path.join(tmp, base + '.o')):
        return None, (r.stdout + r.stderr).strip()
    return os.path.join(tmp, base + '.o'), ''


def main():
    src, lo, hi = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
    orig = sys.argv[4] if len(sys.argv) > 4 else ORIG
    obj, err = compile_obj(src)
    if obj is None:
        print('COMPILE ERROR\n' + err)
        return 1
    o = SOut(open(obj, 'rb').read())
    tsz = o.segs[0]['code']
    mine = insns(bytes(o.image[:tsz]), 0, tsz, True)
    s = SOut(open(orig, 'rb').read())
    cs = s.segs[0]['code']
    ref = insns(bytes(s.image[:cs]), lo, hi, (s.magic >> 8) == 0xE6)
    A = keep_locals([norm(t) for _, t in mine])
    B = keep_locals([norm(t) for _, t in ref])
    # instructions after the range (stubs of callees in the test file) are not compared
    extra = len(A) - len(B)
    A = A[:len(B)]
    if A == B:
        print('OK: %d instructions equal (original %d bytes)%s' % (len(A), hi - lo,
              ', %d trailing instructions ignored' % extra if extra > 0 else ''))
        return 0
    print('DIFF: object %d instructions, original %d instructions' % (len(A), len(B)))
    mine = mine[:len(B)]
    for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, A, B, autojunk=False).get_opcodes():
        if tag == 'equal':
            continue
        for k in range(i1, i2):
            print('  mine %04x  %s' % (mine[k][0], mine[k][1]))
        for k in range(j1, j2):
            print('  orig %04x  %s' % (ref[k][0], ref[k][1]))
        print('  --')
    return 1


if __name__ == '__main__':
    sys.exit(main())
