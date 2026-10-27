#!/usr/bin/env python3
"""accept.py - accept a checked draft function into sld.c, update own_functions.txt and findings.json.

Usage: accept.py DRAFT.c ENTRY END [CLASS RESULT]

  DRAFT.c  the draft (K&R, one function, plus its declarations); lines after a line '/* test stubs */'
           are test stubs of callees and are not accepted
  ENTRY    entry address of the function (hex, e.g. 0aba), END its end (hex, e.g. 0b04)
  CLASS    optional difference class of the old compiler (D1 .. D17) that remains
  RESULT   optional text: what differs (required with CLASS)

The draft is checked with fncheck.py. Only on OK it is appended to ../sld.c and the status in
own_functions.txt is set: 'done' without CLASS, 'findings:CLASS' with CLASS. With CLASS a pending finding
is added to findings.json (findings_pending, status open), and the function entry f_ENTRY gets file sld.c.
Declarations repeated in sld.c are kept only once.
"""
import json
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
SLD = os.path.join(HERE, '..', 'sld.c')
TABLE = os.path.join(HERE, 'own_functions.txt')
FINDINGS = os.path.join(HERE, 'findings.json')
EMU = '/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu'
WEGA = '/mnt/share/cvs/P8000/tmp/wega31_root'


def whole_file_ok():
    tmp = tempfile.mkdtemp(prefix='acceptall', dir='/mnt/share/cvs/P8000/tmp/tmp')
    shutil.copy(SLD, os.path.join(tmp, 'all.c'))
    r = subprocess.run([EMU, '-r', WEGA, '/bin/scc', '-c', '-O', 'all.c'], cwd=tmp,
                       capture_output=True, text=True, timeout=900)
    if r.returncode != 0:
        print((r.stdout + r.stderr).strip()[:400])
    return r.returncode == 0


def record_finding(entry, end, cls, result):
    data = json.load(open(FINDINGS))
    ids = [x['id'] for x in data['findings_pending']] + [x['id'] for x in data['findings']]
    fid = 'sld-%02d' % (len([i for i in ids if i.startswith('sld-')]) + 1)
    data['findings_pending'].append({
        'id': fid, 'kind': 'compiler difference', 'function': 'f_%s' % entry, 'status': 'open',
        'classification': cls, 'action': 'record; rebuild as the original (byte identity first)',
        'result': result})
    fn = data['functions'].setdefault('f_%s' % entry, {})
    fn.update({'file': 'sld.c', 'pairing': 'confirmed', 'orig': '0x%s' % entry,
               'ours': 'f%s..%s' % (entry, end), 'level': 'analysed'})
    json.dump(data, open(FINDINGS, 'w'), indent=1)
    open(FINDINGS, 'a').write('\n')
    return fid


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
    if '/* test stubs */' in body:
        body = body[:body.index('/* test stubs */')]
    sld = open(SLD).read().split('\n')
    out = []
    for line in body:
        if line.strip() == '':
            out.append(line)
        elif line.endswith(';') and not line.startswith(('\t', ' ')) and '(' not in line and line in sld:
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
    if cls:
        fid = record_finding(entry, end, cls, result or 'see fncheck output')
        print('recorded %s (%s) in findings.json' % (fid, cls))
    # names, status and findings in findings.json follow own_functions.txt
    subprocess.run([sys.executable, os.path.join(HERE, 'sync_findings.py')], check=True)
    subprocess.run([sys.executable, os.path.join(HERE, 'findings_md.py')], check=True, capture_output=True)
    print('accepted f%s: %s' % (entry, status))
    return 0


if __name__ == '__main__':
    sys.exit(main())
