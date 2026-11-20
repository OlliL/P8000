"""vt.py SPEC: one variant per statement of a function with a lone label in front of it (codgen forgets its registers at a
label, so the code behind it reloads from the frame). Run from WEGA/: DFN=FUNC DFILE=src/.../file.c RECON_PROG=NAME
host/recon/vt.py DFILE FUNC host/recon/vtspec_labels.py; combine the good ones with vtspec_labelcombo.py."""
import os, re
FN = os.environ['DFN']; F = os.environ['DFILE']
src = open(F).read()
a = re.search(r'(?m)^' + FN + r'\(', src).start()
b = src.index('\n}\n', a)
body = src[a:b].split('\n')
start = next(i for i, l in enumerate(body) if l == '{') + 1
while start < len(body) and re.match(r'\t[a-zA-Z_ ]+[ *]*[a-z_A-Z0-9\[\], *]*;$', body[start]) and not re.match(r'\t(if|for|while|return|switch|do)\b', body[start]) and '=' not in body[start] and '(' not in body[start]:
    start += 1
V = []
for i in range(start, len(body)):
    l = body[i]; prev = body[i - 1].rstrip()
    if not re.match(r'\t+(if|for|while|do|switch|return|goto|[a-zA-Z_*(+-].*;)', l):
        continue
    if re.match(r'\t+(case |default:)', l) or l.strip().startswith('else'):
        continue
    if not (prev.endswith(';') or prev.endswith('{') or prev.endswith('}') or prev.endswith(':')):
        continue
    if prev.strip().startswith('else') and not prev.endswith('{'):
        continue
    new = body[:i] + ['lz%d:' % i] + body[i:]
    old_s = '\n'.join(body); new_s = '\n'.join(new)
    V.append(('L%03d' % i, [(old_s, new_s)]))
