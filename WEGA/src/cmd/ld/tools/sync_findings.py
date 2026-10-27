#!/usr/bin/env python3
"""sync_findings.py - bring findings.json in line with own_functions.txt (names, status, source file).

Usage: sync_findings.py

own_functions.txt is the source for the name and the status of each own function of /bin/sld:
  entry name end size callers first-callers kind last-insn status
findings.json "functions" gets one entry per own function, keyed by the name (f<entry> while unnamed):
  file (sld.c once the function is in the work file), pairing (open | confirmed), orig (entry address),
  ours (entry..end), level (byte-identical or the finding class), note.
An older entry f_<entry> (from the first inventory) is renamed to the new key, keeping its other fields.
findings_pending entries (non-D items only) refer to the function by its key ('function' field) and are updated too.
D classes of the old compiler go to the function's audit (d_classes, difference_class, difference_note), not to
findings_pending.
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
        # an entry is either still under the old key f_<entry> or already under its name
        # a renamed function leaves no entry under its unnamed key f<entry>
        if key != 'f' + r['entry']:
            funcs.pop('f' + r['entry'], None)
        entry = funcs.pop(old, None) or funcs.get(key, {})
        status = r['status']
        # level as in codgen: exact (same instruction sequence, fncheck) or differs (class in audit)
        if status == 'done':
            level, pairing = 'exact', 'confirmed'
        elif status.startswith('findings:'):
            level, pairing = 'differs', 'confirmed'
        else:
            level, pairing = 'open', 'open'
        # the audit (D classes) is written by classify_sld.py, not here
        entry.pop('note', None)
        entry.update({
            'file': 'sld.c' if status != 'open' else None,
            'pairing': pairing,
            'orig': '0x' + r['entry'],
            'ours': entry.get('ours') if status != 'open' and entry.get('audit') else entry.get('ours'),
            'level': level,
        })
        funcs[key] = entry
    # only the own functions of the table are entries; first-inventory candidates (c_<addr>, f_<addr>) and starts
    # above the own range are not functions of sld and are dropped (they carry no audit data)
    names = {r['name'] for r in rows}
    for k in [k for k in funcs if k not in names]:
        del funcs[k]
    for x in data['findings_pending']:
        if x.get('function') in rename:
            x['function'] = rename[x['function']]
    # a deferred (large) function that is no longer open is done: its pending entry goes
    status = {r['name']: r['status'] for r in rows}
    data['findings_pending'] = [x for x in data['findings_pending']
                                if not (x.get('kind', '').startswith('deferred') and x.get('function') in status
                                        and status[x['function']] != 'open')]
    json.dump(data, open(FINDINGS, 'w'), indent=1)
    open(FINDINGS, 'a').write('\n')
    print('functions: %d own, %d entries in total' % (len(rows), len(funcs)))


if __name__ == '__main__':
    main()
