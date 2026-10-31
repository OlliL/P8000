#!/usr/bin/env python3
"""d12model.py - our program linked under the data layout of the old cc (D12), compared with the original data.

Usage: d12model.py [-q]      (-q: only the summary lines)

The shipped cc (cparse) puts the string literals of a function in front of the function and pads each one to an even
address. The old cc (measured on /bin/ld, src/cmd/ld/tools/HANDOFF_ld.md) puts initialised data and switch tables in
source order and all string literals of a file at the end of the file's data, unpadded (the pool starts and ends even).

The modules progcfg MODULES (in SRCDIR; the include files without extension, like mfile1, are copied too; a module
with only a .s file, cut from the original, is assembled as it is) are compiled with progcfg CC -O -S, every .s is rewritten into that layout (a
label Lnnn whose body is only .byte lines ending in a zero byte and that is referenced in the file is a string literal;
a static array has a name, so it stays), assembled with cas -u and linked by progcfg model_link(work, objs). The data
of the result is compared with the original byte for byte; the result (progcfg MODEL_OUT) is the input of a strict
comparison (strictcmp.py), because its data and bss addresses are those of the original.
"""
import difflib
import os
import re
import shutil
import subprocess
import sys

import progcfg

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
from sout import SOut  # noqa: E402

CAS = os.environ.get('CAS', os.path.join(CFG.REPO, 'host', 'cas', 'cas'))
CCHOST = os.environ.get('CCHOST', os.path.join(CFG.REPO, 'host', 'cc', 'scc' if CFG.SEG else 'cc'))


def d6tables(lines):
    """D6 (progcfg D6TABLES): the shipped cc writes a cpir switch as a value list and a separate target list, from one case
    on; the old cc writes the values and the targets together and compares one after the other up to 3 cases (no
    table). The value and target lists are put together at the place of the value list; the lists of a switch with up
    to 3 cases go to the end of the text (the model is only compared, never run)."""
    sw = {}
    for i, l in enumerate(lines):
        if 'cpir' in l and i >= 2:
            mv = re.match(r'^\s*ld\s+r\d+,#(L\d+)\s*$', lines[i - 2])
            mn = re.match(r'^\s*ld\s+r\d+,#(\d+)\s*$', lines[i - 1])
            for k in range(i + 1, min(i + 6, len(lines))):
                mt = re.match(r'^\s*ld\s+r\d+,(L\d+)\+\d+\(r\d+\)\s*$', lines[k])
                if mt:
                    break
            if mv and mn and mt:
                sw[mv.group(1)] = (mt.group(1), int(mn.group(1)))
    if not sw:
        return lines

    def block(lab):
        """the label line index and the lines of its .word list"""
        i = lines.index(lab + ':')
        j = i + 1
        while j < len(lines) and lines[j].strip().startswith('.word'):
            j += 1
        return i, j
    drop, put, tail = set(), {}, []
    for v, (t, n) in sw.items():
        vi, vj = block(v)
        ti, tj = block(t)
        drop.update(range(ti, tj))
        if n <= 3:
            drop.update(range(vi, vj))
            tail += lines[vi:vj] + lines[ti:tj]
        else:
            put[vj] = lines[ti:tj]
    out = []
    for i, l in enumerate(lines):
        if i in put:
            out += put[i]
        if i not in drop:
            out.append(l)
    if tail:
        out += ['.code'] + tail
    return out


def rewrite(lines):
    refs = set()
    for l in lines:
        if not re.match(r'^L\d+:', l):
            refs.update(re.findall(r'\bL\d+\b', l))
    out, pool, i, sect = [], [], 0, 'data'
    while i < len(lines):
        l = lines[i]
        s = l.strip()
        if s in ('.data', '.code'):
            sect = s[1:]
        m = re.match(r'^(L\d+):\s*$', l)
        if sect == 'data' and m and m.group(1) in refs:
            j = i + 1
            body = []
            while j < len(lines) and lines[j].lstrip().startswith('.byte'):
                body.append(lines[j])
                j += 1
            if body and re.search(r'%0\s*$', body[-1]):
                pool.append(l)
                pool.extend(body)
                if j < len(lines) and lines[j].strip() == '.even':
                    j += 1
                i = j
                continue
        out.append(l)
        i += 1
    if pool:
        out += ['.data', '.even'] + pool + ['.even']
    return out


def run(cmd, cwd):
    r = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True)
    if r.returncode:
        sys.exit('%s: %s%s' % (' '.join(cmd), r.stdout, r.stderr))


def main():
    quiet = '-q' in sys.argv
    work = os.path.dirname(CFG.MODEL_OUT)
    shutil.rmtree(work, ignore_errors=True)
    os.makedirs(work)
    for f in os.listdir(CFG.SRCDIR):
        p = os.path.join(CFG.SRCDIR, f)
        if os.path.isfile(p) and not f.endswith(('.o', '.py')) and f != 'Makefile':
            shutil.copy(p, work)
    for m in CFG.MODULES:
        if not os.path.exists(os.path.join(work, m + '.c')) and os.path.exists(os.path.join(work, m + '.s')):
            # an assembler module (cut from the original): its data layout is the original's already
            run([CAS, '-u', '-o', m + '.o', m + '.s'], work)
            continue
        run([CCHOST, '-O', '-S', m + '.c'], work)
        src = open(os.path.join(work, m + '.s')).read().split('\n')
        if getattr(CFG, 'D6TABLES', False):
            src = d6tables(src)
        open(os.path.join(work, m + '_old.s'), 'w').write('\n'.join(rewrite(src)) + '\n')
        run([CAS, '-u', '-o', m + '.o', m + '_old.s'], work)
    run(CFG.model_link(work, [m + '.o' for m in CFG.MODULES]), work)
    o = SOut(open(CFG.MODEL_OUT, 'rb').read())
    g = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())

    def data(x):
        c = x.segs[0]['code']
        return bytes(x.image)[c:c + x.segs[0]['data']]
    a, b = data(g), data(o)
    eq = sum(1 for k in range(min(len(a), len(b))) if a[k] == b[k])
    print('data: original %d, model %d bytes; equal at the same address: %d' % (len(a), len(b), eq))
    print('code: original %d, model %d; bss: original %d, model %d' % (
        g.segs[0]['code'], o.segs[0]['code'], g.segs[0]['bss'], o.segs[0]['bss']))
    if quiet:
        return
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    try:
        for op, i1, i2, j1, j2 in sm.get_opcodes():
            if op != 'equal':
                print('%-7s orig %04x..%04x %-40r model %04x..%04x %r' % (op, i1, i2, a[i1:i2][:24], j1, j2, b[j1:j2][:24]))
    except BrokenPipeError:
        pass


if __name__ == '__main__':
    main()
