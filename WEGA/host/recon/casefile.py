"""casefile.py - the cases of a corpus written as blocks of one text file (compare/NAME/tests.txt), for corpusrun.py.

A block:
  == NAME                 a case; the lines up to the next "-- " line are the file t.in (the default input)
  -- FILE                 the following lines are the file FILE of the case (FILE may name a subdirectory: d/f)
  -- case                 the following lines are lines of the case file (host:, step:, stdin:, env:, expect:, variant:)
                          in their order; without a step the default step of the corpus is added at the end
  -- expect               the following lines are the whole output of the last step (expect: output is ...)
In the expected output @UID@ and @GID@ stand for the user and group number of the host user (@UID-8@: padded to 8 columns).
A line starting with # is a comment of the text file, everywhere (a file of a case cannot start a line with #; write
\\043). \\ooo in a line of a file is the byte of that octal value. A file section without lines is an empty file."""
import functools
import os
import re
import shlex


def _bytes(line):
    return re.sub(r'\\([0-7]{3})', lambda m: chr(int(m.group(1), 8)), line)


def blocks(path, first='t.in'):
    """[(name, {file: text}, case lines, expected output or None)]"""
    out = []
    cur = None
    sect = None
    for line in open(path, encoding='latin-1').read().split('\n'):
        if line.startswith('#'):
            continue
        if line.startswith('== '):
            cur = [line[3:].strip(), {first: ''}, [], None]
            sect = first
            out.append(cur)
            continue
        if cur is None:
            continue
        if line.startswith('-- '):
            sect = line[3:].strip()
            if sect == 'expect':
                cur[3] = ''
            elif sect != 'case':
                cur[1].setdefault(sect, '')
            continue
        if sect == 'case':
            if line.strip():
                cur[2].append(line)
        elif sect == 'expect':
            cur[3] += line + '\n'
        else:
            cur[1][sect] += _bytes(line) + '\n'
    for b in out:	# the blank lines before the next block belong to no file
        for f in list(b[1]):
            b[1][f] = b[1][f].rstrip('\n') + '\n' if b[1][f].strip('\n') else ''
        if b[1][first] == '' and first not in [f for f in b[1] if f != first] and not any(first in l for l in b[2]):
            del b[1][first]	# the default file only when it has lines or is named
        if b[3] is not None:
            b[3] = b[3].rstrip('\n') + '\n'
    return out


def prepare(files, src, work):
    for f, text in files:
        p = os.path.join(work, f)
        os.makedirs(os.path.dirname(p), exist_ok=True)
        with open(p, 'w', encoding='latin-1') as fh:
            fh.write(text)
    return True


def owner(text):
    """@UID@ and @GID@ in an expectation: the numbers of the user who runs the corpus (the files of the host steps belong to him);
    @UID-8@: left-justified in 8 columns (the owner column of ls -l)"""
    def one(m):
        v = str(os.getuid() if m.group(1) == 'UID' else os.getgid())
        return v.ljust(int(m.group(2))) if m.group(2) else v
    return re.sub(r'@(UID|GID)(?:-(\d+))?@', one, text)


def generate(g, path, default_step, first='t.in', prefix=()):
    """the cases of the text file path; prefix: case lines put before the lines of every case"""
    for name, files, lines, want in blocks(path, first):
        lines = list(prefix) + lines
        if not any(l.startswith('step:') for l in lines):
            lines = lines + ['step: ' + default_step]
        text = '\n'.join(lines) + '\n'
        if want is not None:
            text += 'expect: output is %s\n' % shlex.quote(owner(want).replace('\\', '\\\\').replace('\n', '\\n'))
        yield g.case(None, functools.partial(prepare, tuple(sorted(files.items()))), text, name=name, sources=[])
