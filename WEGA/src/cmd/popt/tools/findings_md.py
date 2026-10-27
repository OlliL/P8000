#!/usr/bin/env python3
"""findings_md.py - writes findings.json as a readable Markdown report (FINDINGS.md) for people.

The report is generated; findings.json stays the only source. Do not edit FINDINGS.md by hand.

Usage: python3 findings_md.py [--findings findings.json] [--out FINDINGS.md]
"""
import argparse
import collections
import json
import os

HERE = os.path.dirname(os.path.abspath(__file__))

DISCLAIMER = (
    "> **Generated file.** This report is written by `findings_md.py` from `findings.json`. "
    "Do not edit it by hand; change `findings.json` and run the script again."
)


def cell(value):
    """One Markdown table cell: no line breaks, no column separators."""
    if value is None:
        return ''
    if isinstance(value, (list, tuple)):
        value = ', '.join(str(x) for x in value)
    return str(value).replace('\r', ' ').replace('\n', ' ').replace('|', '\\|').strip()


def table(headers, rows):
    lines = ['| ' + ' | '.join(headers) + ' |', '|' + '---|' * len(headers)]
    lines += ['| ' + ' | '.join(cell(c) for c in row) + ' |' for row in rows]
    return lines


def function_rows(functions, manual=None):
    # manual: hand classification (findings.json "manual_classes"); it overrides the pattern audit of classify.py
    manual = manual or {}
    rows = []
    for name in sorted(functions):
        v = functions[name]
        audit = v.get('audit') or {}
        classes = audit.get('d_classes')
        note = audit.get('difference_note', '')
        cls = audit.get('difference_class', '')
        m = manual.get(name)
        if m:
            classes = m.get('d_classes')
            note = m.get('note', '')
            cls = {'read': 'read by hand', 'measured': 'identical', 'open': 'open (not classified)'}.get(m.get('basis'), cls)
        rows.append([
            name,
            v.get('ours', ''),
            v.get('orig', ''),
            v.get('pairing', ''),
            v.get('level', ''),
            cls,
            classes if classes else '',
            note,
        ])
    return rows


def pending_rows(items):
    rows = []
    for x in items:
        rows.append([
            x.get('id', ''),
            x.get('kind', ''),
            x.get('function', ''),
            x.get('status', ''),
            x.get('classification', ''),
            x.get('action', ''),
            x.get('result', ''),
        ])
    return rows


def summary(functions, items):
    pairing = collections.Counter(v.get('pairing', '') for v in functions.values())
    classes = collections.Counter((v.get('audit') or {}).get('difference_class', '-') for v in functions.values())
    status = collections.Counter(x.get('status', '') for x in items)
    lines = ['## Summary', '', '**Functions:** %d' % len(functions), '']
    lines += ['| Pairing | Count |', '|---|---|'] + ['| %s | %d |' % (k, n) for k, n in sorted(pairing.items())] + ['']
    lines += ['| Difference class | Count |', '|---|---|'] + ['| %s | %d |' % (k, n) for k, n in sorted(classes.items())] + ['']
    lines += ['**Pending findings:** %d' % len(items), '', '| Status | Count |', '|---|---|']
    lines += ['| %s | %d |' % (k, n) for k, n in sorted(status.items())]
    return lines


def build(data):
    functions = data.get('functions', {})
    items = data.get('findings_pending', [])
    manual = (data or {}).get('manual_classes') or {}
    out = ['# popt findings', '', DISCLAIMER, '',
           'Source: `findings.json` (single source of truth). Functions are the program part of `popt` '
           'compared with `/lib/popt`; pending findings are the open items of the analysis.', '']
    out += summary(functions, items) + ['']
    out += ['## Functions', '']
    out += table(['Function', 'Ours', 'Original', 'Pairing', 'Level', 'Difference class', 'Old-cc classes', 'Note'],
                 function_rows(functions, manual))
    out += ['', '## Pending findings', '']
    out += table(['ID', 'Kind', 'Function', 'Status', 'Classification', 'Action', 'Result'], pending_rows(items))
    return '\n'.join(out) + '\n'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--findings', default=os.path.join(HERE, 'findings.json'))
    ap.add_argument('--out', default=os.path.join(HERE, 'FINDINGS.md'))
    a = ap.parse_args()
    with open(a.findings) as f:
        data = json.load(f)
    with open(a.out, 'w') as f:
        f.write(build(data))
    print('written', a.out)


if __name__ == '__main__':
    main()
