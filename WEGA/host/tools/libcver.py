#!/usr/bin/env python3
"""Which libc members does a linked WEGA program contain, and which revision are they?

Usage: libcver.py [-a LIB] [-o OWNEND] program ...
       libcver.py -g [-a LIB] program ...      compare several programs (variant groups)
       libcver.py --catalog program ...        markdown catalogue: versions and members per program

For every program the text is decoded (linear sweep) and normalised: addresses, symbols and
large immediates become "A", stack offsets and small constants stay.  Every member of the
archive LIB (default: /lib/libc.a of the file tree WEGA) is searched in the text as a
contiguous instruction sequence.  What is found is "same as the shipped member".  What is left
behind the own code (from OWNEND, default: the start of the first member found) is split into
functions at the call targets; each function gets a fingerprint and the member that looks most
like it.  With -g the programs are compared: functions with the same fingerprint belong to the
same libc variant.

The tool needs the file tree of the original system: WEGA=dir (environment) or -w dir.
"""
import difflib
import hashlib
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import war  # noqa: E402
import z8k  # noqa: E402
from sout import SOut  # noqa: E402


def decode(image, tsz, seg=False):
    buf = z8k.Buf(image[:tsz], 0)
    out, pc = [], 0
    while pc < tsz:
        i = z8k.decode(buf, pc, seg)
        if not i:
            out.append((pc, ".word"))
            pc += 2
            continue
        out.append((pc, z8k.text(i)))
        pc = i.end
    return out


def norm(t):
    t = re.sub(r"%[0-9A-F]{4}\((r(?!15\b)\w+)\)", r"A(\1)", t)
    t = re.sub(r"#%([0-9A-F]{4})", lambda m: "#%" + m.group(1) if int(m.group(1), 16) < 0x100 else "#A", t)
    t = re.sub(r"(?<![#\w])%[0-9A-F]{4}(?!\()", "A", t)
    return re.sub(r"\s+", " ", t)


def load_program(path):
    s = SOut(open(path, "rb").read())
    g = s.segs[0]
    seg = (s.magic >> 8) == 0xE6
    ins = decode(s.image, g["code"], seg)
    return s, [(p, norm(t)) for p, t in ins], [t for p, t in ins]


def load_members(lib):
    mem = {}
    for m in war.members(open(lib, "rb").read()):
        if m["name"].startswith("__.SYMDEF"):
            continue
        try:
            s = SOut(m["body"])
        except ValueError:
            continue
        seg = (s.magic >> 8) == 0xE6
        img = bytearray(s.image)
        for a in range(0, len(s.reloc) - 1, 2):      # relocated words get a recognisable value
            if s.reloc[a] or s.reloc[a + 1]:
                img[a:a + 2] = b"\xff\xff"
        ins = decode(bytes(img), s.segs[0]["code"], seg)
        mem[m["name"]] = [norm(t) for p, t in ins]
    return mem


def find_members(prog, mem):
    """exact matches: member -> start index (in instructions)"""
    texts = [t for p, t in prog]
    idx = {}
    for i in range(len(texts) - 2):
        idx.setdefault((texts[i], texts[i + 1], texts[i + 2]), []).append(i)
    found = {}
    for name, seq in mem.items():
        if len(seq) < 3:
            continue
        for i in idx.get((seq[0], seq[1], seq[2]), []):
            if texts[i:i + len(seq)] == seq:
                found.setdefault(name, []).append(i)
                break
    return found


def functions(prog, raw, lo, hi, cov):
    """split the uncovered instructions lo..hi at call targets; returns [(first, last)]"""
    addr = {p: i for i, (p, t) in enumerate(prog)}
    starts = set()
    for t in raw:
        m = re.match(r"calr?\s+%([0-9A-F]{4})", t)
        if m and int(m.group(1), 16) in addr:
            starts.add(addr[int(m.group(1), 16)])
    out, cur = [], None
    for k in range(lo, hi):
        if cov[k]:
            if cur:
                out.append(tuple(cur))
            cur = None
            continue
        if cur is None or k in starts:
            if cur:
                out.append(tuple(cur))
            cur = [k, k]
        cur[1] = k
    if cur:
        out.append(tuple(cur))
    return out


def best_member(seq, mem, cand=None):
    """most similar member; cand limits the choice to the members that can stand here"""
    best = (0.0, "?")
    n = len(seq)
    for name, m in mem.items():
        if cand is not None and name not in cand:
            continue
        if not (0.5 * n <= len(m) <= 2 * n + 4):
            continue
        r = difflib.SequenceMatcher(None, seq, m, autojunk=False).ratio()
        if r > best[0]:
            best = (r, name)
    return best


def analyse(path, mem, own=None):
    """returns dict: instructions, libc start, identical members (in order), other functions"""
    s, prog, raw = load_program(path)
    found = find_members(prog, mem)
    total = len(prog)
    cov = [0] * total
    for name, starts in found.items():
        for st in starts:
            for k in range(st, st + len(mem[name])):
                cov[k] = name
    first = min((st[0] for st in found.values()), default=total)
    lo = first if own is None else next((i for i, (p, t) in enumerate(prog) if p >= own), total)
    names = list(mem)
    pos = {n: i for i, n in enumerate(names)}
    placed = sorted((st[0], pos[n]) for n, st in found.items())
    other = []
    for a, b in functions(prog, raw, lo, total, cov):
        seq = [t for p, t in prog[a:b + 1]]
        fp = hashlib.md5("\n".join(seq).encode()).hexdigest()[:8]
        before = max([ai for pi, ai in placed if pi < a], default=-1)
        after = min([ai for pi, ai in placed if pi > b], default=len(names))
        cand = set(names[before + 1:after]) - set(found)
        r, name = best_member(seq, mem, cand)
        if r < 0.3:
            r, name = best_member(seq, mem)
        other.append(dict(first=prog[a][0], last=prog[b][0], n=len(seq), fp=fp, like=name[:-2], ratio=r))
    order = sorted(found.items(), key=lambda kv: kv[1][0])
    ids = sorted(set(m.group(1).decode("latin-1").split() and " ".join(m.group(1).decode("latin-1").split())
                     for m in re.finditer(rb"@\[\$\]\s*([\w.]+\s+[\d.]+\s+\d\d/\d\d/\d\d)", open(path, "rb").read())))
    return dict(ids=ids, n=total, start=prog[lo][0] if lo < total else 0,
                same=[n[:-2] for n, st in order], other=other)


SMALL = 3     # functions of up to 3 instructions (stubs like __callex) are not counted as a difference


def catalog(paths, mem, root):
    res = {}
    for p in paths:
        res[p] = analyse(os.path.join(root, p), mem)
    sets = {}
    for p, r in res.items():
        key = tuple(sorted(o["fp"] for o in r["other"] if o["n"] > SMALL))
        sets.setdefault(key, []).append(p)
    out = ["# libc members inside the programs", "",
           "Made by `host/tools/libcver.py --catalog` from the original file tree. \"identical\" = the member of",
           "the shipped `lib/libc.a` is in the program instruction by instruction (addresses and relocated values",
           "ignored). \"other\" = functions behind the own code that no shipped member explains; the nearest",
           "member is a hint only (ratio below 0.6: do not trust the name).", ""]
    ver = {}
    for key, ps in sets.items():
        ver[tuple(ps)] = "shipped" if not key else os.path.basename(ps[0])
    out.append("## Versions")
    out.append("")
    out.append("| Version | Programs | Functions that differ from the shipped libc |")
    out.append("|---|---|---|")
    for ps, v in ver.items():
        key = tuple(sorted(o["fp"] for o in res[ps[0]]["other"] if o["n"] > SMALL))
        out.append("| %s | %s | %d |" % (v, ", ".join(os.path.basename(p) for p in ps), len(key)))
    out.append("")
    shared = {}
    for ps, v in ver.items():
        for o in res[ps[0]]["other"]:
            if o["n"] > SMALL:
                shared.setdefault(o["fp"], (o, []))[1].append(v)
    out.append("## Functions that occur in more than one version")
    out.append("")
    out.append("| fingerprint | instructions | nearest shipped member | versions |")
    out.append("|---|---|---|---|")
    for fp, (o, vs) in shared.items():
        if len(vs) > 1:
            name = o["like"] if o["ratio"] >= 0.6 else "? (%s %.2f)" % (o["like"], o["ratio"])
            out.append("| %s | %d | %s | %s |" % (fp, o["n"], name, ", ".join(vs)))
    out.append("")
    for ps, v in ver.items():
        for p in ps[:1]:
            r = res[p]
            out.append("## %s (version %s)" % (p, v))
            out.append("")
            out.append("%d instructions, libc part from %04x, %d identical members." % (r["n"], r["start"], len(r["same"])))
            out.append("")
            out.append("SCCS ids in the file: %s" % ("; ".join(r["ids"]) or "none"))
            out.append("")
            out.append("Identical members:")
            out.append("")
            out.append(" ".join(r["same"]))
            out.append("")
            if any(o["n"] > SMALL for o in r["other"]):
                out.append("| address | instructions | fingerprint | nearest shipped member |")
                out.append("|---|---|---|---|")
                for o in r["other"]:
                    if o["n"] > SMALL:
                        name = o["like"] if o["ratio"] >= 0.6 else "? (%s %.2f)" % (o["like"], o["ratio"])
                        out.append("| %04x..%04x | %d | %s | %s |" % (o["first"], o["last"], o["n"], o["fp"], name))
                out.append("")
            if len(ps) > 1:
                out.append("Same libc part: %s." % ", ".join(os.path.basename(q) for q in ps[1:]))
                out.append("")
    return "\n".join(out)


def main():
    args = sys.argv[1:]
    lib = None
    own = None
    group = False
    cat = False
    root = os.environ.get("WEGA")
    progs = []
    while args:
        a = args.pop(0)
        if a == "-a":
            lib = args.pop(0)
        elif a == "-o":
            own = int(args.pop(0), 0)
        elif a == "-g":
            group = True
        elif a == "--catalog":
            cat = True
        elif a == "-w":
            root = args.pop(0)
        else:
            progs.append(a)
    if not progs:
        print(__doc__)
        return 1
    if not root:
        print("set WEGA or use -w", file=sys.stderr)
        return 1
    mem = load_members(lib or os.path.join(root, "lib/libc.a"))
    if cat:
        print(catalog(progs, mem, root))
        return 0
    table = {}
    for pth in progs:
        full = pth if os.path.isabs(pth) or os.path.exists(pth) else os.path.join(root, pth)
        r = analyse(full, mem, own)
        print("%s: %d instructions, libc part from %04x, %d members identical to the shipped ones" % (
            os.path.basename(pth), r["n"], r["start"], len(r["same"])))
        if not group:
            print("  identical: " + " ".join(r["same"]))
        for o in r["other"]:
            table.setdefault(o["fp"], []).append((os.path.basename(pth), o))
            if not group:
                print("  other %04x..%04x %4d instructions  fp %s  like %s (%.2f)" % (
                    o["first"], o["last"], o["n"], o["fp"], o["like"], o["ratio"]))
    if group:
        print("\nfingerprint  instrs  like (ratio)       programs")
        for fp, v in sorted(table.items(), key=lambda kv: (kv[1][0][1]["like"], -len(kv[1]))):
            o = v[0][1]
            print("%s %6d  %-12s %.2f   %s" % (fp, o["n"], o["like"], o["ratio"], " ".join(x[0] for x in v)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
