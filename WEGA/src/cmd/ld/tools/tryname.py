#!/usr/bin/env python3
"""tryname.py - find a name for an own function that the old compiler does not lose.

Usage: tryname.py CURRENT_NAME CANDIDATE [CANDIDATE ...]

The old cc sometimes loses the name of a function of the big work file (symbol table; see KNOWLEDGE, "limits of the old
compiler"): the assembler text then has an empty label. For each candidate the name is substituted in a copy of sld.c, the
copy is compiled with scc -S and the number of empty labels is printed (0 = usable). sld.c is not changed;
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


def nameless(text):
    tmp = tempfile.mkdtemp(prefix='tryname', dir='/mnt/share/cvs/P8000/tmp/tmp')
    open(os.path.join(tmp, 'all.c'), 'w').write(F.alias_text(text))
    subprocess.run([EMU, '-r', WEGA, '/bin/scc', '-S', '-O', 'all.c'], cwd=tmp, capture_output=True, text=True,
                   timeout=900)
    try:
        return open(os.path.join(tmp, 'all.s')).read().count('/*~~:*/')
    except OSError:
        return -1


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    cur = sys.argv[1]
    text = open(os.path.join(HERE, '..', 'sld.c')).read()
    print('%-16s %s' % (cur + ' (now)', nameless(text)))
    for c in sys.argv[2:]:
        print('%-16s %s' % (c, nameless(re.sub(r'\b%s\b' % re.escape(cur), c, text))))
    return 0


if __name__ == '__main__':
    sys.exit(main())
