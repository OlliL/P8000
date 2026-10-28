#!/usr/bin/env python3
"""tryname.py - find a name for an own function that the old compiler does not lose.

Usage: tryname.py CURRENT_NAME CANDIDATE [CANDIDATE ...]

The old cc sometimes loses the name of a function of the big work file (symbol table; see KNOWLEDGE, "limits of the old
compiler"): the assembler text then has an empty label. For each candidate the name is substituted in a copy of the sources, the
files are compiled one by one with scc -S and the number of empty labels is printed (0 = usable). the sources are not changed;
rename.py does the rename.
"""
import os
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fncheck as F  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
EMU = '/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu'
WEGA = '/mnt/share/cvs/P8000/tmp/wega31_root'


def nameless(texts):
    """number of empty labels (lost function names) in the assembler text of all C files; texts = {path: text}"""
    import ldsrc
    outs, errs = ldsrc.compile_all('-O', asm=True, texts=texts)
    if errs:
        return -1
    return sum(open(p).read().count('/*~~:*/') for p in outs.values())


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    cur = sys.argv[1]
    import ldsrc
    texts = ldsrc.read_all()
    print('%-16s %s' % (cur + ' (now)', nameless(texts)))
    for c in sys.argv[2:]:
        print('%-16s %s' % (c, nameless({k: re.sub(r'\b%s\b' % re.escape(cur), c, v) for k, v in texts.items()})))
    return 0


if __name__ == '__main__':
    sys.exit(main())
