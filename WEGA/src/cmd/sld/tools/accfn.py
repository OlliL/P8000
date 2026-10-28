#!/usr/bin/env python3
"""accfn.py NAME [NAME ...] - accept functions of the C files that are exact: cut the function out of its file (anchored on the
definition line, never on a call) and hand it to accept.py with its entry and end from own_functions.txt (status done)."""
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ldsrc  # noqa: E402

for name in sys.argv[1:]:
    table = open(os.path.join(HERE, 'own_functions.txt')).read()
    m = re.search(r'^(\w+) %s (\w+) ' % re.escape(name), table, re.M)
    entry, end = m.group(1), m.group(2)
    mod = ldsrc.module_of(entry)
    src = open(os.path.join(HERE, '..', mod)).read()
    d = re.search(r'^(?:struct \w+ \*+|int |char \*|long |)%s\(.*?\n\}\n' % re.escape(name), src, re.S | re.M)
    with tempfile.NamedTemporaryFile('w', suffix='.c', delete=False, dir=HERE) as f:
        f.write('#include "ld.h"\n' + d.group(0))
    r = subprocess.run([sys.executable, os.path.join(HERE, 'accept.py'), f.name, entry, end], capture_output=True, text=True)
    os.unlink(f.name)
    print(name, (r.stdout + r.stderr).strip().split('\n')[-1])
