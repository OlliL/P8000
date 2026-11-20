#!/usr/bin/env python3
"""formtry.py - automatic source forms for one function (the patterns that found original forms in gopt).

Usage: RECON_PROG=NAME formtry.py FILE.c FUNC [-k]
Each rule is applied at each place in the text of FUNC alone; every variant is measured with fnc.py. Prints the
variants that change the DIFF count; -k keeps the best variant if it is better than the start (else the file is restored).
Rules: empty loop body `;` -> `continue;`; `if (A && B)` with one statement -> nested ifs; `if (C) V = A; else V = B;`
-> `V = C ? A : B;`; if/else with one statement each -> swapped with the negated condition; `while (C)` -> `for (; C; )`.
"""
import os, re, subprocess, sys

def run(f, fn):
    r = subprocess.run(['python3', 'host/recon/fnc.py', f, fn], capture_output=True, text=True)
    m = re.search(r'(OK|DIFF (\d+))', r.stdout)
    if not m:
        return 9999, r.stdout.strip()[:60]
    return (0 if m.group(1) == 'OK' else int(m.group(2))), r.stdout.strip()

def funcspan(src, fn):
    m = re.search(r'^%s\(' % re.escape(fn), src, re.M)
    if not m:
        return None
    i = m.start()
    j = src.find('\n}\n', i)
    return i, j + 3

def split_cond(c):
    """top-level && parts of c"""
    parts, depth, cur, k = [], 0, '', 0
    while k < len(c):
        ch = c[k]
        if ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
        if depth == 0 and c[k:k + 2] == '&&':
            parts.append(cur.strip()); cur = ''; k += 2; continue
        if depth == 0 and c[k:k + 2] == '||':
            return None
        cur += ch; k += 1
    parts.append(cur.strip())
    return parts if len(parts) > 1 else None

def variants(body):
    out = []
    lines = body.split('\n')
    for i, l in enumerate(lines):
        ind = re.match(r'\t*', l).group(0)
        # empty loop body
        if re.match(r'\t*(for|while) \(.*\)$', l) and i + 1 < len(lines) and lines[i + 1].strip() == ';':
            v = lines[:]; v[i + 1] = lines[i + 1].replace(';', 'continue;'); out.append(('continue@%d' % i, v))
        # while -> for
        m = re.match(r'(\t*)while \((.*)\)( \{)?$', l)
        if m:
            v = lines[:]; v[i] = '%sfor (; %s; )%s' % (m.group(1), m.group(2), m.group(3) or ''); out.append(('for@%d' % i, v))
        # if (A && B) single statement -> nested
        m = re.match(r'(\t*)if \((.*)\)$', l)
        if m and i + 1 < len(lines) and not lines[i + 1].strip().startswith('{') and lines[i + 1].rstrip().endswith(';'):
            parts = split_cond(m.group(2))
            if parts and (i + 2 >= len(lines) or not lines[i + 2].strip().startswith('else')):
                v = lines[:i]
                for d, p in enumerate(parts):
                    v.append('%s%sif (%s)' % (m.group(1), '\t' * d, p))
                v.append('\t' * (len(parts) - 1) + lines[i + 1])
                v += lines[i + 2:]
                out.append(('nested@%d' % i, v))
        # if (C) V = A; else V = B;  -> ?:
        if m and i + 3 < len(lines) and lines[i + 2].strip() == 'else':
            a = re.match(r'\t*(\S.*?) = (.*);$', lines[i + 1]); b = re.match(r'\t*(\S.*?) = (.*);$', lines[i + 3])
            if a and b and a.group(1) == b.group(1):
                v = lines[:i] + ['%s%s = %s ? %s : %s;' % (m.group(1), a.group(1), m.group(2), a.group(2), b.group(2))] + lines[i + 4:]
                out.append(('?:@%d' % i, v))
            # swap one-statement branches
            if lines[i + 1].rstrip().endswith(';') and lines[i + 3].rstrip().endswith(';') and not lines[i + 1].strip().startswith('{'):
                v = lines[:i] + ['%sif (!(%s))' % (m.group(1), m.group(2)), lines[i + 3], lines[i + 2], lines[i + 1]] + lines[i + 4:]
                out.append(('swap@%d' % i, v))
    return out

def main():
    f, fn = sys.argv[1], sys.argv[2]
    keep = '-k' in sys.argv
    src = open(f).read()
    sp = funcspan(src, fn)
    if not sp:
        sys.exit('no function ' + fn)
    base, txt = run(f, fn)
    print('start', txt)
    body = src[sp[0]:sp[1]]
    best = (base, None)
    for name, v in variants(body):
        open(f, 'w').write(src[:sp[0]] + '\n'.join(v) + src[sp[1]:])
        n, txt = run(f, fn)
        if n != base:
            print('%-14s %s' % (name, txt))
        if n < best[0]:
            best = (n, '\n'.join(v))
    if keep and best[1] is not None:
        open(f, 'w').write(src[:sp[0]] + best[1] + src[sp[1]:])
        print('kept', best[0])
    else:
        open(f, 'w').write(src)

main()
