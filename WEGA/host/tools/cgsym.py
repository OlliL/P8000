#!/usr/bin/env python3
"""Reader for the symbol table file (base.S) of codgen/scodgen, see KNOWLEDGE.md ("codgen / scodgen").

File: long G (offset of the last section, the global symbols), then sections to the end of the file. Section: word flag, long symbytes, long asciibytes, word numbytes,
word membytes, then the symbols (22 bytes each), the ascii table, the number table (2 byte entries) and the member table
(6 byte entries). Usage: cgsym.py [-v] file.S ...
"""
import struct
import sys


def sections(d):
    g, = struct.unpack(">I", d[:4])
    pos = 4
    out = []
    while pos < len(d):
        flag, ns, na, nn, nm = struct.unpack(">HIIHH", d[pos:pos + 14])
        if ns % 22:
            raise ValueError("symbol bytes %d not a multiple of 22 at %d" % (ns, pos))
        end = pos + 14 + ns + na + nn + nm
        if end > len(d):
            raise ValueError("section at %d runs over the end" % pos)
        syms = [struct.unpack(">11H", d[pos + 14 + 22 * k:pos + 36 + 22 * k]) for k in range(ns // 22)]
        out.append(dict(pos=pos, flag=flag, syms=syms, ascii=d[pos + 14 + ns:pos + 14 + ns + na],
                        nums=d[pos + 14 + ns + na:pos + 14 + ns + na + nn], mems=d[end - nm:end]))
        pos = end
    return g, out


def main():
    verbose = "-v" in sys.argv
    bad = n = 0
    first = 0
    for f in sys.argv[1:]:
        if f == "-v":
            continue
        d = open(f, "rb").read()
        try:
            g, secs = sections(d)
        except Exception as e:
            bad += 1
            print("%s: %s" % (f, e))
            continue
        n += 1
        if secs and g == secs[-1]["pos"]:
            first += 1
        if verbose:
            print(f, "G", g, [(s["pos"], len(s["syms"])) for s in secs])
    print("files %d bad %d (G = start of the last section in %d)" % (n, bad, first))


if __name__ == "__main__":
    main()
