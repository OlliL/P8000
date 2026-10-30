"""chunks.py - split a K&R C source into top-level items: ('func', name, text) and ('other', None, text).

Line based: a function starts at a line in column 0 of the form NAME( ... (a line with only a type, like "NODE *", in
front of it belongs to it, and so does a comment right before), and ends at the line where the braces counted from its
first '{' are balanced. Braces in strings, character constants and comments are not counted.
"""
import re

HEAD = re.compile(r'^(?:[A-Za-z_][\w \t]*[\s*])?\**([A-Za-z_]\w*)\s*\(')
KEYW = {'if', 'while', 'for', 'switch', 'return', 'sizeof', 'case', 'do', 'else'}


def braces(line, state):
    """count { and } of a line; state['c'] is True inside a /* comment"""
    n = 0
    i = 0
    while i < len(line):
        if state['c']:
            j = line.find('*/', i)
            if j < 0:
                return n
            state['c'] = False
            i = j + 2
            continue
        if line.startswith('/*', i):
            state['c'] = True
            i += 2
            continue
        ch = line[i]
        if ch in '"\'':
            j = i + 1
            while j < len(line) and line[j] != ch:
                j += 2 if line[j] == '\\' else 1
            i = j + 1
            continue
        if ch == '{':
            n += 1
        elif ch == '}':
            n -= 1
        i += 1
    return n


def items(src):
    lines = src.split('\n')
    out = []
    other = []
    i = 0
    state = {'c': False}
    while i < len(lines):
        l = lines[i]
        m = HEAD.match(l) if not state['c'] else None
        if m and m.group(1) not in KEYW and not l.rstrip().endswith(';') and not l.startswith(('#', ' ', '\t')):
            # find the body: the first line with { at depth 0, then the balance
            j = i
            depth = 0
            seen = False
            st = {'c': False}
            while j < len(lines):
                d = braces(lines[j], st)
                if '{' in lines[j]:
                    seen = True
                depth += d
                if seen and depth == 0:
                    break
                if not seen and lines[j].rstrip().endswith(';') and j > i and depth == 0 and lines[j].lstrip().startswith(('extern',)):
                    break
                j += 1
            # take a type line and a comment in front of it
            k = len(other)
            while k > 0 and (re.match(r'^[A-Za-z_][\w \t]*\**\s*$', other[k - 1]) and other[k - 1].strip() not in ('',)):
                k -= 1
            while k > 0 and other[k - 1].strip().startswith(('/*', '*', '**')) and not other[k - 1].strip().endswith(';'):
                k -= 1
            pre = other[k:]
            del other[k:]
            if other:
                out.append(('other', None, '\n'.join(other) + '\n'))
                other = []
            out.append(('func', m.group(1), '\n'.join(pre + lines[i:j + 1]) + '\n'))
            i = j + 1
            continue
        braces(l, state)
        other.append(l)
        i += 1
    if other:
        out.append(('other', None, '\n'.join(other) + ('\n' if src.endswith('\n') else '')))
    return out


if __name__ == '__main__':
    import sys
    for k, n, t in items(open(sys.argv[1], errors='replace').read()):
        print(k, n, len(t), repr(t[:50]))
