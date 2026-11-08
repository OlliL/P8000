#!/usr/bin/env python3
"""K&R function definitions -> ANSI; writes the prototypes to PROTO. usage: kr2ansi.py PROTO file... (changes the files in place; see HANDOFF, Port rules)
Handles top-level  [type] name( a, b ) decls { ; the yacc file is processed only in its %% C parts (not done here)."""
import re, sys

def mask(s):
    """same-length copy of s with comments, strings and char constants blanked (newlines kept)"""
    out = []; i = 0; n = len(s)
    while i < n:
        c = s[i]
        if s.startswith('/*', i):
            j = s.find('*/', i + 2); j = n if j < 0 else j + 2
            out.append(''.join(ch if ch == '\n' else ' ' for ch in s[i:j])); i = j
        elif c in '"\'':
            j = i + 1
            while j < n and s[j] != c:
                j += 2 if s[j] == '\\' else 1
            j += 1
            out.append(c + ' ' * (j - i - 2) + c); i = j
        else:
            out.append(c); i += 1
    return ''.join(out)

KW = {'register', 'auto', 'static', 'extern', 'const', 'volatile'}

def split_decl(d):
    """'register struct a *p, q[10]' -> list of (full ANSI decl text) per declarator"""
    d = d.strip()
    parts = []; depth = 0; cur = ''
    for ch in d:
        if ch in '([': depth += 1
        if ch in ')]': depth -= 1
        if ch == ',' and depth == 0:
            parts.append(cur); cur = ''
        else:
            cur += ch
    parts.append(cur)
    first = parts[0].strip()
    toks = re.findall(r'\w+|[*]|\(|\)|\[[^\]]*\]', first)
    # base: leading words up to the declarator (name is last \w token not inside parens)
    # find the name = last word token
    wi = [i for i, t in enumerate(toks) if re.match(r'\w', t)]
    name_i = wi[-1]
    # base words: all word tokens before the first '*' or '(' or the name
    k = 0
    while k < len(toks) and re.match(r'\w', toks[k]) and k < name_i:
        k += 1
    base = [t for t in toks[:k] if t not in KW]
    if not base:
        base = ['int']
    # a lone modifier sequence like 'unsigned' stays; 'struct x' fine
    res = []
    declr = [first[first.index(toks[k]):] if k < len(toks) else first]
    # declarator of the first part: tokens from k on
    d0 = ' '.join(toks[k:]).replace('* ', '*').replace('( ', '(').replace(' )', ')')
    res.append(d0)
    for p in parts[1:]:
        res.append(p.strip())
    return ' '.join(base), res

def find_name(declr):
    m = re.search(r'\(\s*\*\s*(\w+)\s*\)', declr)
    if m: return m.group(1)
    return re.findall(r'\w+', re.sub(r'\[[^\]]*\]', '', declr))[-1]

def process(src, protos):
    m = mask(src)
    n = len(src); i = 0; depth = 0; out = []; last = 0
    stmt_start = 0
    pos = 0
    res = ''
    # scan top-level for ')' followed by decls and '{'
    pat = re.compile(r'(?m)^[ \t]*((?:[A-Za-z_]\w*[ \t\*]+)*)([A-Za-z_]\w*)[ \t]*\(([^()]*)\)([^;{}()]*(?:;[^;{}()]*)*)\{', re.S)
    # process sequentially keeping track of brace depth via mask
    d = 0; idx = 0; pieces = []; k = 0
    top = []  # positions at depth 0
    depths = []
    d = 0
    for ch in m:
        depths.append(d)
        if ch == '{': d += 1
        elif ch == '}': d -= 1
    pos = 0
    for mm in pat.finditer(m):
        s = mm.start()
        if depths[s] != 0: continue
        # reject if preceded by '=' / not a definition start: preceding non-space char must be ; } or newline context
        # require that previous non-blank masked char is one of ; } or start or a '#' line end or ')'... just check no '=' pending
        prev = m[:s].rstrip()
        start1 = mm.start(1)
        types = mm.group(1)
        if prev:
            lines = prev.split('\n')
            ll = lines[-1]
            if prev[-1] in ';}' or ll.lstrip().startswith('#'):
                pass
            elif re.fullmatch(r'[ \t]*(?:[A-Za-z_]\w*[ \t\*]*)+', ll) and not mm.group(1).strip():
                start1 = len(prev) - len(ll)
                types = ll
            else:
                continue
        name, args, decls = mm.group(2), mm.group(3), mm.group(4)
        if name in ('if', 'while', 'for', 'switch', 'return', 'sizeof'): continue
        # raw text pieces from original source (comments kept in decl block are dropped)
        raw_decls = src[mm.start(4):mm.end(4)]
        raw_decls = re.sub(r'/\*.*?\*/', '', raw_decls, flags=re.S)
        if re.search(r'#', raw_decls): continue
        arglist = [a.strip() for a in src[mm.start(3):mm.end(3)].split(',')] if args.strip() else []
        # already ANSI?
        if any(re.search(r'\s', re.sub(r'\s+', ' ', a)) and not re.fullmatch(r'\w+', a) for a in arglist):
            continue
        ptypes = {}
        for dl in [x.strip() for x in raw_decls.split(';') if x.strip()]:
            base, ds = split_decl(dl)
            for dd in ds:
                nm = find_name(dd)
                ptypes[nm] = (base + ' ' + dd.replace(nm, '@@')).replace(' *', ' *').strip()
        params = []
        for a in arglist:
            if a in ptypes:
                t = ptypes[a]
                params.append(t.replace('@@', a).replace('* ', '*').replace(' [', '['))
            else:
                params.append('int ' + a)
        rtype = re.sub(r'\bregister\b', '', types).strip()
        stat = ''
        if re.search(r'\bstatic\b', rtype):
            stat = 'static '; rtype = re.sub(r'\bstatic\b', '', rtype).strip()
        # pointer in return type attaches to name
        stars = ''
        mt = re.match(r'(.*?)([\s\*]*)$', rtype, re.S)
        rt = rtype
        if not rt: rt = 'int'
        # split trailing stars
        mt = re.match(r'^(.*?)(\**)$', rt.replace(' *', '*').strip())
        rt, stars = mt.group(1).strip() or 'int', mt.group(2)
        head = '%s%s %s%s(%s)' % (stat, rt, stars, name, ', '.join(params) if params else 'void')
        protos.append((name, head + ';', bool(stat)))
        # replace from start of the type line (mm.start(1)) to the '{'
        pieces.append((start1, mm.end() - 1, head + '\n'))
    for a, b, t in reversed(pieces):
        src = src[:a] + t + src[b:]
    return src

def main():
    proto = sys.argv[1]; protos = []
    for f in sys.argv[2:]:
        s = open(f).read()
        s2 = process(s, protos)
        open(f, 'w').write(s2)
    with open(proto, 'w') as o:
        seen = set()
        for name, h, st in protos:
            if h in seen: continue
            seen.add(h)
            o.write(h + '\n')

main()
