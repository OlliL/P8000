#!/usr/bin/env python3
"""Reader for the intermediate file (base.I) that cparse writes for codgen/scodgen (see KNOWLEDGE.md,
"codgen / scodgen"). Used to check that the format is understood: every file of a corpus must parse.

Usage: cgread.py /lib/codgen file.I [file.I ...]   (the first argument: the codgen of the original system, for the op table)

Records, one per line, first character selects:
  { n TAB autooff TAB size TAB nregs TAB      begin of a function (numbers in decimal, tab terminated; LF follows)
  } ...                                       end of a function
  [ autooff TAB maxtreg TAB x TAB             begin of a block        ]  end of a block
  . lineno TAB "file"                         followed by an expression tree (prefix order)
  L n TAB   label     B n TAB   branch     F n TAB   S n TAB   T n TAB   R a TAB b [TAB c]
  C n TAB l1 TAB l2 ... TAB                   switch table
  # text, / text                              lines that are copied or ignored
A tree node: op TAB [lval TAB] [rval TAB] [x TAB (op 121)] type(octal) TAB LF, STASG/STARG/STCALL/USTCALL: stsize TAB stalign TAB LF;
left, right follow (FLD has two subtrees). A tree may be preceded by lines that begin with '/' (copied to the output).
"""
import struct
import sys

sys.path.insert(0, __import__("os").path.dirname(__file__))
from sout import SOut

LTYPE, UTYPE, BITYPE = 2, 4, 8


def optable(path):
    """(op -> (name, flags)) from the op table (12 byte entries: op, name[8], flags) of the original."""
    s = SOut(open(path, "rb").read())
    d = s.image[s.segs[0]["code"]:]
    i = d.index(b"\0\0\0\2NAME\0")
    i += 2
    ops = {}
    while True:
        op, = struct.unpack(">H", d[i:i + 2])
        name = d[i + 2:i + 10].split(b"\0")[0].decode()
        flags, = struct.unpack(">H", d[i + 10:i + 12])
        if op == 0 or not name:
            break
        ops[op] = (name.strip('"') or name, flags)
        i += 12
    return ops


class Reader:
    def __init__(self, data, ops):
        self.d = data
        self.i = 0
        self.ops = ops
        self.nodes = 0

    def getc(self):
        if self.i >= len(self.d):
            return -1
        c = self.d[self.i]
        self.i += 1
        return c

    def rdin(self, base=10):
        sign, val = 1, 0
        while True:
            c = self.getc()
            if c < 0:
                raise ValueError("unexpected EOF")
            if c == 0x2d:
                if val:
                    raise ValueError("illegal -")
                sign = -sign
            elif c == 9:
                return val
            elif 0x30 <= c <= 0x39:
                val = val * base + sign * (c - 0x30)
            else:
                raise ValueError("illegal character %r at %d" % (chr(c), self.i))

    def nl(self):
        if self.getc() != 10:
            raise ValueError("illegal newline at %d" % self.i)

    def line(self):
        a = self.i
        while True:
            c = self.getc()
            if c < 0 or c == 10:
                return self.d[a:self.i - 1]

    def tree(self):
        op = self.rdin()
        if op not in self.ops:
            raise ValueError("unknown op %d at %d" % (op, self.i))
        flags = self.ops[op][1]
        kind = flags & 0xE
        self.nodes += 1
        if kind == LTYPE:
            self.rdin()
        if kind != BITYPE and op != 109:
            self.rdin()
        if op == 121:
            self.rdin()
        self.rdin(8)
        if op in (101, 102, 103, 105):
            self.rdin()
            self.rdin()
        if self.getc() != 10:
            raise ValueError("illegal newline at %d" % self.i)
        if kind != LTYPE:
            self.tree()
        if kind == BITYPE or op == 109:
            self.tree()

    def run(self):
        count = {}
        while True:
            c = self.getc()
            if c < 0:
                break
            ch = chr(c)
            count[ch] = count.get(ch, 0) + 1
            if ch == "{":
                for _ in range(4):
                    self.rdin()
                self.nl()
            elif ch == "[":
                for _ in range(3):
                    self.rdin()
                self.nl()
            elif ch == ".":
                self.rdin()
                self.line()
                while self.d[self.i] == 0x2f:
                    self.getc()
                    self.line()
                self.tree()
            elif ch in "LBFST":
                self.rdin()
                self.nl()
            elif ch == "R":
                self.rdin()
                self.rdin()
                self.line() if False else None
                # the third number is optional: read to the end of the line
                self.line()
            elif ch == "C":
                while self.d[self.i] == 0x20:
                    self.i += 1
                self.rdin()
                while self.d[self.i] != 10:
                    self.rdin()
                self.getc()
            elif ch in "}]#/":
                self.line()
            else:
                raise ValueError("record %r at %d" % (ch, self.i))
        return count


def main():
    ops = optable(sys.argv[1])
    bad = 0
    tot = {}
    for f in sys.argv[2:]:
        r = Reader(open(f, "rb").read(), ops)
        try:
            c = r.run()
            for k, v in c.items():
                tot[k] = tot.get(k, 0) + v
            tot["nodes"] = tot.get("nodes", 0) + r.nodes
        except Exception as e:
            bad += 1
            print("%s: %s" % (f, e))
    print("files %d bad %d records %s" % (len(sys.argv) - 2, bad, tot))


if __name__ == "__main__":
    main()
