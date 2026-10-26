#!/usr/bin/env python3
"""confirm.py - confirms the pairings of our functions with the functions of /lib/codgen (step 2 of the analysis).

Input: our binary (codgen), the original, and FUNCTIONS.md (the pairings written by funcs.py).
Output: findings.json, the key "functions" (pairing, evidence, level). Nothing else is changed.

A pairing (ours f -> original g) is confirmed when
  - its strict level is `exact` or `order` (the same instructions), or
  - the callees agree: the own functions f calls, mapped through the confirmed pairings, are the functions g calls
    (and the number of calls to the C library agrees), or
  - the callers agree in the same way (the functions that call f, mapped, are the functions that call g).
A pairing whose callees or callers are not yet confirmed is left open and retried in the next round.
Leaf functions without a confirmed partner stay `unconfirmed`: they need a look at the body.

Usage: python3 confirm.py --mine ./codgen --orig $WEGA/lib/codgen [--findings findings.json]
"""
import argparse
import collections
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import funcs as F  # noqa: E402

LIBC_FROM = 0xbbc4   # start of the C library in /lib/codgen (host/KNOWLEDGE.md)
WRAPPERS = {'cgprintf', 'static_86a4'}   # our printf wrapper (cg.h, comm3.c): the original calls printf


def read_table(path):
    """(name, ours start, ours end, orig start or None, source, level) for every row of FUNCTIONS.md."""
    rows = []
    for l in open(path):
        if not l.startswith('| ') or l.startswith('| Function') or l.startswith('| ---'):
            continue
        f = [x.strip() for x in l.split('|')]
        if len(f) < 8:
            continue
        name = f[1]
        try:
            st = int(f[2], 16)
            en = int(f[3], 16)
        except ValueError:
            continue
        o = int(f[4], 16) if f[4].startswith('0x') else None
        level = f[6].split(' ')[0]
        rows.append((name, st, en, o, f[5], level))
    return rows


def calls(rows, start, end, names, owner):
    """Targets of the calls in [start, end): ('own', start of our function) or ('ext', None)."""
    out = []
    for raw in F.extent(rows, start, end):
        m = re.match(r'^(call|calr)\s+(.*)$', raw)
        if not m:
            continue
        t = m.group(2).strip()
        if t.startswith('_') and t[1:] in names:
            out.append(('own', names[t[1:]]))
            continue
        m2 = re.match(r'^%([0-9A-F]{4})$', t) or re.match(r'^L0t_([0-9a-f]{4})$', t)
        if not m2:
            out.append(('ext', None))
            continue
        a = int(m2.group(1), 16)
        o = owner(a)
        out.append(('own', o) if o is not None else ('ext', None))
    return out


NEEDLE = b'freeing blank tree!'   # the string of free1 (comm3.c), the same in both binaries


def string_offset(rows, data, start):
    """File offset minus address of the data (constant per binary), found through the string of free1."""
    for raw in F.extent(rows, start, start + 0x40):
        m = re.match(r'^ld\s+r\d+,#%([0-9A-F]{4})$', raw)
        if m:
            x = int(m.group(1), 16)
            if data.find(NEEDLE) >= 0 and data[x + data.find(NEEDLE) - x:][:len(NEEDLE)] == NEEDLE:
                return data.find(NEEDLE) - x
    raise SystemExit('string offset not found (free1 at 0x%04x)' % start)


def strings(rows, data, off, start, end):
    """The printable strings (>= 2 characters) that the function loads with ld rN,#%XXXX."""
    out = set()
    for raw in F.extent(rows, start, end):
        m = re.match(r'^ld\s+r\d+,#%([0-9A-F]{4})$', raw)
        if not m:
            continue
        x = int(m.group(1), 16) + off
        if x < 0 or x >= len(data):
            continue
        t = data[x:x + 200].split(b'\0')[0]
        if len(t) >= 2 and all(32 <= c < 127 or c in (9, 10) for c in t):
            out.add(t)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mine', default='./codgen')
    ap.add_argument('--orig', default=os.path.expandvars('$WEGA/lib/codgen'))
    ap.add_argument('--table', default=os.path.join(os.path.dirname(os.path.abspath(__file__)), 'FUNCTIONS.md'))
    ap.add_argument('--findings', default=os.path.join(os.path.dirname(os.path.abspath(__file__)), 'findings.json'))
    a = ap.parse_args()

    table = read_table(a.table)
    from sout import SOut
    mine_lim = F.own_limit(SOut(open(a.mine, 'rb').read()))   # our objects go beyond LIM (the C library starts there in the original)
    mine_rows = F.disasm(a.mine, mine_lim)
    orig_rows = F.disasm(a.orig)
    mine_data = open(a.mine, 'rb').read()
    orig_data = open(a.orig, 'rb').read()
    free1_mine = next(st for name, st, en, o, src, lev in table if name == 'free1')
    off_mine = string_offset(mine_rows, mine_data, free1_mine)
    off_orig = string_offset(orig_rows, orig_data, 0x8758)
    _, cand = F.orig_candidates(orig_rows)
    cand = sorted(set(cand))

    def orig_end(o):
        nxt = [c for c in cand if c > o]
        return nxt[0] if nxt else o + 0x200

    # our functions: start -> name; owner of an address = the last start at or before it
    ours = sorted((st, name, en) for name, st, en, _, _, _ in table)
    starts = [x[0] for x in ours]
    names = {name: st for name, st, en, _, _, _ in table}
    ours_by_start = {st: name for name, st, en, _, _, _ in table}

    def owner_ours(addr):
        i = None
        for k, s0 in enumerate(starts):
            if s0 <= addr:
                i = k
        if i is None:
            return None
        s0 = starts[i]
        end = ours[i][2]
        return s0 if addr < end else None

    # candidate pairings (table entries with an original start)
    pair = {}
    level = {}
    for name, st, en, o, src, lev in table:
        if o is not None:
            pair[st] = o
        level[st] = lev

    # callees and callers of our functions (own ones only, by start) and of the original functions
    o_calls = {}
    for name, st, en, o, src, lev in table:
        if o is None:
            continue
        o_calls[st] = calls(mine_rows, st, en, names, owner_ours)

    orig_owner_starts = sorted(set(c for c in cand if c >= 0))

    def owner_orig(addr):
        best = None
        for c in orig_owner_starts:
            if c <= addr:
                best = c
        return best

    def orig_calls(o):
        out = []
        for raw in F.extent(orig_rows, o, orig_end(o)):
            m = re.match(r'^(call|calr)\s+(.*)$', raw)
            if not m:
                continue
            t = m.group(2).strip()
            m2 = re.match(r'^%([0-9A-F]{4})$', t) or re.match(r'^L0t_([0-9a-f]{4})$', t)
            if not m2:
                out.append(('ext', None))
                continue
            x = int(m2.group(1), 16)
            if x >= LIBC_FROM:
                out.append(('ext', None))
            else:
                out.append(('own', owner_orig(x)))
        return out

    o_calls_orig = {}
    for name, st, en, o, src, lev in table:
        if o is not None:
            o_calls_orig[o] = orig_calls(o)

    def mapped(seq, mp):
        """Our callees in original addresses: ('own', orig) if paired, ('unp', name) if not, ('ext', None)."""
        out = []
        for kind, s0 in seq:
            if kind == 'ext' or s0 is None or ours_by_start.get(s0) in WRAPPERS:
                out.append(('ext', None))
            elif s0 in mp:
                out.append(('own', mp[s0]))
            else:
                out.append(('unp', ours_by_start.get(s0, '?')))
        return out

    def key(seq):
        own = collections.Counter(x for k, x in seq if k == 'own')
        ext = sum(1 for k, _ in seq if k == 'ext')
        return own, ext

    # callers: which of our functions call a function (by start), in original addresses
    callers_ours = collections.defaultdict(list)
    for st, seq in o_calls.items():
        for kind, s0 in seq:
            if kind == 'own' and s0 is not None:
                callers_ours[s0].append(st)
    callers_orig = collections.defaultdict(list)
    for o, seq in o_calls_orig.items():
        for kind, s0 in seq:
            if kind == 'own' and s0 is not None:
                callers_orig[s0].append(o)

    fj = json.load(open(a.findings)) if os.path.exists(a.findings) else {}
    confirmed = {}   # our start -> (orig start, evidence)
    # manual corrections recorded in findings.json (section "manual"): they replace the table claim
    for name, ent in fj.get('manual', {}).items():
        if name not in names:
            continue
        st = names[name]
        if ent.get('orig') is None:
            pair.pop(st, None)
        else:
            pair[st] = int(ent['orig'], 16)
        if ent.get('confirm'):
            confirmed[st] = (int(ent['orig'], 16), ent.get('evidence', 'review'))
    # reject: functions whose rules-based confirmation is refused (findings.json, key "reject")
    rejected = {names[n] for n in fj.get('reject', {}) if n in names}
    # a rule (strings, callees, callers) confirms only a body that is not too different: at most half of the longer one
    def body_ok(st, o):
        en = next(e for n, s0, e, _, _, _ in table if s0 == st)
        a = F.extent(mine_rows, st, en)
        b = F.extent(orig_rows, o, orig_end(o))
        lv, d = F.strict_compare(a, b)
        return lv in ('exact', 'order') or d <= 0.5 * max(len(a), len(b), 1)
    # strict level for the others
    def strict(st, o):
        en = next(e for n, s0, e, _, _, _ in table if s0 == st)
        lv, _ = F.strict_compare(F.extent(mine_rows, st, en), F.extent(orig_rows, o, orig_end(o)))
        return lv

    for st, o in list(pair.items()):
        if st in confirmed:
            continue
        if st not in rejected and strict(st, o) in ('exact', 'order'):
            confirmed[st] = (o, 'same instructions')
    for st, o in list(pair.items()):
        if st in confirmed:
            continue
        en = next(e for n, s0, e, _, _, _ in table if s0 == st)
        so = strings(mine_rows, mine_data, off_mine, st, en)
        sg = strings(orig_rows, orig_data, off_orig, o, orig_end(o))
        if st not in rejected and so and so == sg and body_ok(st, o):
            confirmed[st] = (o, 'same strings: %d' % len(so))

    changed = True
    rounds = 0
    while changed and rounds < 20:
        changed = False
        rounds += 1
        mp = {s0: o for s0, (o, _) in confirmed.items()}
        for st, o in pair.items():
            if st in confirmed or st in rejected:
                continue
            # callees
            ok_c = False
            ok_r = False
            mc = mapped(o_calls.get(st, []), mp)
            if not any(k == 'unp' for k, _ in mc):
                own_m, ext_m = key([(k, s0) for k, s0 in mc])
                own_o, ext_o = key(o_calls_orig.get(o, []))
                if own_m == own_o and ext_m == ext_o and (own_m or ext_m):
                    ok_c = True
            cs = [mp.get(c) for c in callers_ours.get(st, [])]
            if cs and all(c is not None for c in cs):
                if collections.Counter(cs) == collections.Counter(callers_orig.get(o, [])) and cs:
                    ok_r = True
            if (ok_c or ok_r) and body_ok(st, o):
                why = []
                if ok_c:
                    why.append('callees agree')
                if ok_r:
                    why.append('callers agree')
                confirmed[st] = (o, ', '.join(why))
                mp[st] = o
                changed = True

    # proposals: an unconfirmed function whose confirmed callees are called by exactly one free original function
    # (and by no other one) gets that function as a proposal; a proposal is not a confirmation
    mp = {s0: o for s0, (o, _) in confirmed.items()}
    used = set(mp.values())
    proposed = {}
    for st, o in pair.items():
        if st in confirmed:
            continue
        mc = [mp[c] for k, c in o_calls.get(st, []) if k == 'own' and c in mp]
        if not mc:
            continue
        cands = [g for g in o_calls_orig if g not in used and set(mc) <= {x for k, x in o_calls_orig[g] if k == 'own'}]
        if len(cands) == 1:
            proposed[st] = (cands[0], 'callees %s of the original are called by it (only candidate)' %
                            ', '.join('0x%04x' % x for x in sorted(set(mc))))
    for st, (g, ev) in proposed.items():
        pass

    # write the result into findings.json (the "functions" key)
    import datetime  # noqa: F401
    funcs_out = {}
    for name, st, en, o, src, lev in table:
        ent = {'ours': '0x%04x-0x%04x' % (st, en), 'table_orig': ('0x%04x' % o) if o is not None else None}
        if st in confirmed:
            ent['orig'] = '0x%04x' % confirmed[st][0]
        else:
            ent['orig'] = None
        if name in fj.get('manual', {}) and fj['manual'][name].get('orig') is None:
            ent['pairing'] = 'none'
            ent['pairing_evidence'] = fj['manual'][name]['evidence']
        elif st in confirmed:
            ent['pairing'] = 'confirmed'
            ent['pairing_evidence'] = confirmed[st][1]
        elif st in proposed:
            ent['pairing'] = 'proposed'
            ent['pairing_evidence'] = proposed[st][1]
            ent['proposed_orig'] = '0x%04x' % proposed[st][0]
        elif o is None:
            ent['pairing'] = 'none'
            ent['pairing_evidence'] = ''
        else:
            ent['pairing'] = 'unconfirmed'
            ent['pairing_evidence'] = 'not confirmed by strict level, callees or callers'
        eff = confirmed[st][0] if st in confirmed else None
        if eff is not None:
            ent['level'] = F.strict_compare(F.extent(mine_rows, st, en), F.extent(orig_rows, eff, orig_end(eff)))[0]
        else:
            ent['level'] = lev
        if name in fj.get('functions', {}) and 'audit' in fj['functions'][name]:
            ent['audit'] = fj['functions'][name]['audit']   # kept: written by classify.py
        funcs_out[name] = ent
    fj['functions'] = funcs_out
    json.dump(fj, open(a.findings, 'w'), indent=1)
    cnt = collections.Counter(e['pairing'] for e in funcs_out.values())
    print('rounds %d; %s' % (rounds, dict(cnt)))


if __name__ == '__main__':
    main()
