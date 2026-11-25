"""Corpus of fse (host/recon/corpusrun.py): the original /bin/fse against the shipped build (src/cmd/fse: fsen and the
parameter block of fsepar), on the cases of compare/fse/tests.txt (host/recon/casefile.py): the public interface of the
man page (the options, the commands of the text and the dump mode, the backup, .fse, the metakeys, errmsg of -e) and what
the source has besides (-a, J in the question, the end of the input as F10, the keys of the P8000). fse is a full screen
editor: every case runs with the minimal terminal of the emulator (terminal: console, /dev/tty is it), TERM=P8 (the
terminal of the parameter block; other ones through /etc/termcap), the keys typed by the operator. Each case has its own
directory: .fse and errmsg (fse reads and writes them in its current directory) are files of the case.
Not in the corpus: the end of the input at "Press <ET> to continue" and at "Save before quit ?" (fse waits there for
ever: KNOWLEDGE.md, quirks), fse without TERM (getenv gives 0: strcmp reads the data at address 0), ^UM to a directory without
permission. normalize(): the bytes of .fse behind the NUL of the file name are set to 0 (left in the buffer, a time among
them: KNOWLEDGE.md, quirks)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'fse'
ORIG = 'bin/fse'
PROG = 'fse'
CANDIDATES = {'wega': 'src/cmd/fse/fse'}
TIMEOUT = 60


FNAMELEN = 30	# src/cmd/fse/fse.h: kbd.fname, the first field of .fse


def normalize(what, data):
    """.fse: delwind writes kbd.fname as memcpy of the whole fcb.fname, so what the buffer held behind the name comes along,
    the mtime of a file among it (stat gives the host's time of the write: no time: fixes it). Those bytes are set to 0; the
    name, strtline, longmem and the keys k stay compared."""
    if os.path.basename(what) == '.fse' and isinstance(data, bytes):
        n = data.find(b'\0', 0, FNAMELEN)
        if n >= 0:
            data = data[:n] + bytes(FNAMELEN - n) + data[FNAMELEN:]
    return data


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'fse -c t.in',
                             prefix=('terminal: console', 'env: TERM=P8'))
