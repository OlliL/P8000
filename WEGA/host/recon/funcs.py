#!/usr/bin/env python3
"""funcs.py - library of the analysis (configuration: progcfg.py of the program) (no tables, no findings: those live in findings.json).

  own_limit(sout)            end of our program part (the first object after the codgen objects)
  disasm(path, lim)          (address, instruction) rows of a binary, via host/tools/sdis.py
  canon(raw)                 one instruction in the strict-1 form (addresses masked, see findings.json "metric_def")
  extent(rows, a, b)         the instructions of [a, b) without data words and without what follows the last ret
  strict_compare(a, b)       ('exact' | 'order' | 'differs', n) for two instruction lists
  orig_candidates(rows)      (strong, all) function starts of the original: call targets, and starts after a ret
"""
import collections
import difflib
import os
import re
import subprocess
import sys

import progcfg  # noqa: E402
CFG = progcfg.load()
HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = progcfg.TOOLS
sys.path.insert(0, TOOLS)
from sout import SOut  # noqa: E402,F401

LIM = CFG.OWN_LIM
OWN_OBJ = re.compile(CFG.OWN_OBJ)

def own_limit(s):
    """End of our program part: the first object file after the codgen objects (the C library follows)."""
    marks = sorted((x['value'], x['name']) for x in s.symbols if x['type'] == 31 and x['segt'] == 0)
    for v, n in marks:
        if not OWN_OBJ.match(n):
            return v
    return LIM

def disasm(path, lim=None):
    if lim is None:
        lim = LIM
    out = subprocess.run([sys.executable, os.path.join(TOOLS, 'sdis.py'), '-l', path],
                         capture_output=True, text=True, check=True).stdout
    rows = []
    for l in out.splitlines():
        if l.startswith('// segment') and 'data,' in l:
            break
        m = re.match(r'^([0-9a-f]{4}) [0-9a-f ]+?\t(.*)$', l)
        if m:
            a = int(m.group(1), 16)
            if a < lim:
                rows.append((a, m.group(2).strip()))
    return rows

def canon(raw):
    """Strict form of one instruction (metric 'strict-1'): only the labels of jumps, the targets of calls and
    the address constants (#%XXXX >= 0x3000) are masked; registers, frame offsets and other constants stay."""
    s = re.sub(r'L0t_[0-9a-f]+', 'L', raw)
    s = re.sub(r'^(call|calr)\s+.*', r'\1 T', s)
    s = re.sub(r'#%([0-9A-F]{4})', lambda x: '#A' if int(x.group(1), 16) >= 0x3000 else x.group(0), s)
    s = re.sub(r'%([0-9A-F]{4})\((r\d+)\)', lambda x: 'A(%s)' % x.group(2) if int(x.group(1), 16) >= 0x3000
               else x.group(0), s)
    s = re.sub(r'(?<![A-Za-z])%([0-9A-F]{4})(?!\()', lambda x: 'A' if int(x.group(1), 16) >= 0x3000 else x.group(0), s)
    s = s.replace('%0000(r15)', '@r15')           # the same operand, two encodings (old cc)
    return s

def extent(rows, start, end, explicit=False):
    """The instructions of [start, end) in address order, without the data words (`.word`) and without what
    follows the last `ret` of the range (constants and strings are placed behind the code). With explicit=True the
    end is known (findings.json "orig_end"): internal `ret`s stay, nothing is cut."""
    out = [raw for a, raw in rows if start <= a < end and not raw.startswith('.word')]
    if explicit:
        return out
    last = max((i for i, raw in enumerate(out) if raw == 'ret'), default=len(out) - 1)
    return out[:last + 1]

def strict_compare(mine_seq, orig_seq):
    """Returns (level, n): 'exact' (same sequence), 'order' (same instructions in another order), or
    'differs' with n = the number of instructions that are not matched in a sequence alignment."""
    import difflib
    a = [canon(x) for x in mine_seq]
    b = [canon(x) for x in orig_seq]
    if a == b:
        return 'exact', 0
    if collections.Counter(a) == collections.Counter(b):
        return 'order', 0
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    n = 0
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != 'equal':
            n += max(i2 - i1, j2 - j1)
    return 'differs', n

def orig_candidates(orig_rows):
    """Possible starts of functions in the original: call targets (strong) and the instruction after a ret (weak);
    a prologue after a ret is strong. A function ends at the next strong start."""
    tg = set()
    for a, raw in orig_rows:
        m = re.match(r'^(call|calr)\s+(?:%([0-9A-F]{4})|L0t_([0-9a-f]+))', raw)
        if m:
            tg.add(int(m.group(2) or m.group(3), 16))
    strong = set(tg)
    weak = set()
    prev = None
    for a, raw in orig_rows:
        if prev == 'ret':
            weak.add(a)
            if re.match(r'(dec|sub)\s+r15,', raw):
                strong.add(a)
        prev = raw
    return sorted(strong), sorted(strong | weak)
