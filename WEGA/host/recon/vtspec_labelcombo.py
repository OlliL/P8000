"""vt.py SPEC: lone labels at several statements at once: LABS="54,83 54,135" (body line numbers as vtspec_labels.py
names them, L054 -> 54), DFN, DFILE as there."""
import os
FN = os.environ['DFN']; F = os.environ['DFILE']
import re
src = open(F).read()
a = re.search(r'(?m)^' + FN + r'\(', src).start()
b = src.index('\n}\n', a)
body = src[a:b].split('\n')
V = []
for combo in os.environ['LABS'].split():
    idx = sorted(map(int, combo.split(',')), reverse=True)
    new = list(body)
    for i in idx:
        new = new[:i] + ['lz%d:' % i] + new[i:]
    V.append(('C' + combo, [('\n'.join(body), '\n'.join(new))]))
