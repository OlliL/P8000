#!/usr/bin/env python3
"""accept.py - accept a checked draft function into sld.c, update own_functions.txt and findings.json.

Usage: accept.py DRAFT.c ENTRY END [CLASS RESULT]

  DRAFT.c  the draft (K&R, one function, plus its declarations); lines after a line '/* test stubs */'
           are test stubs of callees and are not accepted
  ENTRY    entry address of the function (hex, e.g. 0aba), END its end (hex, e.g. 0b04)
  CLASS    optional difference class of the old compiler (D1 .. D17) that remains
  RESULT   optional text: what differs (required with CLASS)

The draft is checked with fncheck.py. Only on OK it is appended to ../sld.c and the status in
own_functions.txt is set: 'done' without CLASS, 'findings:CLASS' with CLASS. With CLASS the D class goes to the
function's audit in findings.json (d_classes, difference_note); findings_pending is only for items without a
D class.
Declarations repeated in sld.c are kept only once.
"""
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fncheck as F  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
SLD = os.path.join(HERE, '..', 'sld.c')
TABLE = os.path.join(HERE, 'own_functions.txt')
FINDINGS = os.path.join(HERE, 'findings.json')
EMU = '/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu'
WEGA = '/mnt/share/cvs/P8000/tmp/wega31_root'


def whole_file_ok():
    tmp = tempfile.mkdtemp(prefix='acceptall', dir='/mnt/share/cvs/P8000/tmp/tmp')
    F.alias_copy(SLD, os.path.join(tmp, 'all.c'))
    r = subprocess.run([EMU, '-r', WEGA, '/bin/scc', '-c', '-O', 'all.c'], cwd=tmp,
                       capture_output=True, text=True, timeout=900)
    if r.returncode != 0:
        print((r.stdout + r.stderr).strip()[:400])
        return False
    # the old compiler loses the name of a function now and then (symbol table, names equal in the first
    # 8 characters): the assembler text then has an empty label; check that none is empty
    r = subprocess.run([EMU, '-r', WEGA, '/bin/scc', '-S', '-O', 'all.c'], cwd=tmp,
                       capture_output=True, text=True, timeout=900)
    if '/*~~:*/' in open(os.path.join(tmp, 'all.s')).read():
        print('a function of the whole file got no name (rename a function; see KNOWLEDGE, sld compiler limits)')
        return False
    return True


def record_evidence(entry, result):
    """the manual reading of a difference goes to pairing_evidence (codgen style); the audit is classify_sld.py"""
    name = [line.split()[1] for line in open(TABLE) if line.startswith(entry + ' ')][0]
    data = json.load(open(FINDINGS))
    data['functions'][name]['pairing_evidence'] = result
    json.dump(data, open(FINDINGS, 'w'), indent=1)
    open(FINDINGS, 'a').write('\n')
    return name


def main():
    args = sys.argv[1:]
    if len(args) not in (3, 5):
        print(__doc__)
        return 2
    draft, entry, end = args[0], args[1].zfill(4), args[2].zfill(4)
    cls = args[3] if len(args) == 5 else None
    result = args[4] if len(args) == 5 else ''
    r = subprocess.run([sys.executable, os.path.join(HERE, 'fncheck.py'), draft, entry, end],
                       capture_output=True, text=True)
    print(r.stdout + r.stderr, end='')
    if r.returncode != 0 and not cls:
        print('not accepted')
        return 1
    body = open(draft).read().split('\n')
    # the draft may carry stubs of callees after the marker; only the function before it is accepted
    if '/* test stubs */' in body:
        body = body[:body.index('/* test stubs */')]
    # global declarations that already are in sld.c are not repeated; K&R parameter declarations are kept
    # (they sit between a function header and its '{'); the compile check below catches any other clash
    sld = open(SLD).read().split('\n')
    # a struct defined in the draft (for the check) is dropped when sld.c defines it already
    drop = []
    i = 0
    while i < len(body):
        m = re.match(r'^struct (\w+) \{$', body[i])
        if m and ('struct %s {' % m.group(1)) in sld:
            j = i
            while body[j] != '};':
                j += 1
            drop.extend(range(i, j + 1))
            i = j
        i += 1
    body = [l for k, l in enumerate(body) if k not in drop]
    out = []
    header = False
    for line in body:
        if line.startswith(('\t', ' ')) or line.strip() == '':
            out.append(line)
        elif line.strip() == '{':
            header = False
            out.append(line)
        elif '(' in line and not line.rstrip().endswith(';'):
            header = True
            out.append(line)
        elif header:
            out.append(line)
        elif line.endswith(';') and '(' not in line and line in sld and line.strip() not in ('};', '}'):
            continue
        else:
            out.append(line)
    text = '\n'.join(out).strip('\n')
    status = 'done' if not cls else 'findings:' + cls
    before = open(SLD).read()
    with open(SLD, 'a') as f:
        f.write('\n/* f%s..%s: %s */\n%s\n' % (entry, end, status, text))
    # the whole work file must still compile (declarations of later functions can clash)
    if not whole_file_ok():
        open(SLD, 'w').write(before)
        print('whole sld.c does not compile with this function; restored, not accepted')
        return 1
    lines = open(TABLE).read().split('\n')
    res = []
    for line in lines:
        if line.startswith(entry + ' '):
            line = line[:-len(line.split()[-1])] + status
        res.append(line)
    open(TABLE, 'w').write('\n'.join(res))
    # names, status and findings in findings.json follow own_functions.txt
    subprocess.run([sys.executable, os.path.join(HERE, 'sync_findings.py')], check=True)
    subprocess.run([sys.executable, os.path.join(HERE, 'findings_md.py')], check=True, capture_output=True)
    if cls:
        name = record_evidence(entry, result or 'see fncheck output')
        print('pairing_evidence of %s: %s' % (name, cls))
    # the audit (D classes, pattern check) of all checked functions
    subprocess.run([sys.executable, os.path.join(HERE, 'classify_sld.py')], check=True, capture_output=True)
    subprocess.run([sys.executable, os.path.join(HERE, 'findings_md.py')], check=True, capture_output=True)
    print('accepted f%s: %s' % (entry, status))
    return 0


if __name__ == '__main__':
    sys.exit(main())
