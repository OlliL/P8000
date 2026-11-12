"""EPU (Z8070) instruction text from the template tables in epu_data.py."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from epu_data import EPU  # noqa: E402


def _field(w1, w2, spec):
    word, mask = spec
    w = w1 if word == 1 else w2
    sh = (mask & -mask).bit_length() - 1
    return (w & mask) >> sh


def decode(w1, w2, seg=False):
    """Return 'mnemonic\\toperands' for a two-word EPU instruction, or None.
    seg: segmented code, a memory operand is a register pair (@rrN)."""
    for m1, v1, m2, v2, name, shape, fields in EPU:
        if (w1 & m1) == v1 and (w2 & m2) == v2:
            ops = []
            for s, f in zip(shape, fields):
                if seg and s == "@r":
                    s = "@rr"
                ops.append("%s%d" % (s, _field(w1, w2, f)))
            return name + "\t" + ",".join(ops)
    return None
