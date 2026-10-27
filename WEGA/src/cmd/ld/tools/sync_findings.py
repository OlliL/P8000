#!/usr/bin/env python3
"""sync_findings.py - bring findings.json in line with own_functions.txt (names, status, source file).

Usage: sync_findings.py

own_functions.txt is the source for the name and the status of each own function of /bin/sld:
  entry name end size callers first-callers kind last-insn status
findings.json "functions" gets one entry per own function, keyed by the name (f<entry> while unnamed):
  file (sld.c once the function is in the work file), pairing (open | confirmed), orig (entry address),
  ours (entry..end), level (byte-identical or the finding class), note.
An older entry f_<entry> (from the first inventory) is renamed to the new key, keeping its other fields.
findings_pending entries refer to the function by its key ('function' field) and are updated too.
Then run findings_md.py to regenerate FINDINGS.md.
"""
import json
import os

HERE = os.path.dirname(os.path.abspath(__file__))
TABLE = os.path.join(HERE, 'own_functions.txt')
FINDINGS = os.path.join(HERE, 'findings.json')


def read_table():
    rows = []
    for line in open(TABLE):
        if line.startswith('#') or not line.strip():
            continue
        p = line.split()
        rows.append({'entry': p[0], 'name': p[1], 'end': p[2], 'status': p[-1]})
    return rows


def main():
    data = json.load(open(FINDINGS))
    funcs = data['functions']
    rows = read_table()
    rename = {}
    for r in rows:
        old = 'f_' + r['entry']
        key = r['name']
        rename[old] = key
        entry = funcs.pop(old, {})
        status = r['status']
        if status == 'done':
            level, pairing, note = 'byte-identical (fncheck)', 'confirmed', 'own source sld.c'
        elif status.startswith('findings:'):
            level, pairing = 'finding ' + status.split(':', 1)[1], 'confirmed'
            note = 'own source sld.c; difference recorded in findings_pending'
        else:
            level, pairing, note = 'open', 'open', 'no own source yet'
        entry.update({
            'file': 'sld.c' if status != 'open' else None,
            'pairing': pairing,
            'orig': '0x' + r['entry'],
            'ours': 'f%s..%s' % (r['entry'], r['end']),
            'level': level,
            'note': note,
        })
        funcs[key] = entry
    for x in data['findings_pending']:
        if x.get('function') in rename:
            x['function'] = rename[x['function']]
    json.dump(data, open(FINDINGS, 'w'), indent=1)
    open(FINDINGS, 'a').write('\n')
    print('functions: %d own, %d entries in total' % (len(rows), len(funcs)))


if __name__ == '__main__':
    main()
