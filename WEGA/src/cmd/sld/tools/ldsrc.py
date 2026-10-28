#!/usr/bin/env python3
"""ldsrc.py - the source files of sld (src/cmd/sld/*.c and ld.h) as one unit, for the scripts of tools/.

module_ranges.txt gives the C files and their code ranges; ld.h holds the structs and the declarations shared by the files.
  sources()        paths of the existing files (the .c files in link order, then ld.h)
  module_of(entry) the C file that holds the function at the original address ENTRY
  read_all()       {path: text}
  compile_all(flags, asm=False)  every .c file is compiled alone with the old compiler (libc names turned into f<address>, see fncheck);
                   returns ({file: object or assembler file}, error text)
"""
import os
import shutil
import subprocess
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
LD = os.path.join(HERE, '..')


def modules():
    out = []
    for line in open(os.path.join(HERE, 'module_ranges.txt')):
        if line.startswith('#') or not line.strip():
            continue
        f, lo, hi = line.split()
        out.append((f, int(lo, 16), int(hi, 16)))
    return out


def module_of(entry):
    e = int(entry, 16) if isinstance(entry, str) else entry
    for f, lo, hi in modules():
        if lo <= e < hi:
            return f
    return None


def sources():
    paths = [os.path.join(LD, f) for f, _, _ in modules()] + [os.path.join(LD, 'ld.h')]
    return [p for p in paths if os.path.exists(p)]


def read_all():
    return {p: open(p).read() for p in sources()}


def compile_all(flags='-O', asm=False, texts=None):
    """texts: optional {path: text} replacing the files (for trial renames); returns ({file: output path}, errors)"""
    import fncheck as F
    tmp = tempfile.mkdtemp(prefix='ldsrc', dir='/mnt/share/cvs/P8000/tmp/tmp')
    texts = texts or read_all()
    for p, t in texts.items():
        open(os.path.join(tmp, os.path.basename(p)), 'w').write(F.alias_text(t))
    outs, errs = {}, []
    for p in sources():
        base = os.path.basename(p)
        if not base.endswith('.c'):
            continue
        r = subprocess.run([F.EMU, '-r', F.WEGA, '/bin/scc'] + flags.split() + ['-S' if asm else '-c', base], cwd=tmp,
                           capture_output=True, text=True, timeout=900)
        out = os.path.join(tmp, base[:-2] + ('.s' if asm else '.o'))
        if r.returncode != 0 or not os.path.exists(out):
            errs.append('%s: %s' % (base, (r.stdout + r.stderr).strip()[:300]))
        else:
            outs[base] = out
    return outs, '\n'.join(errs)
