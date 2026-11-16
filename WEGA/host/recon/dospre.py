"""The common start of the corpora of the dos programs (compare/doscp, dosls, dosdir, ...): the case lines every case starts
with. A floppy of 737280 bytes in drive 1 (the image d1, first all E5), formatted by the original /usr/bin/dosformat
(/dev/rfd1, the default 96ds9 format; its question is answered with y), the clock of the guest at 600000000 (1989), the
terminal and the 8-bit part that /bin/format (run by dosformat) needs. TOUCH sets the time of the WEGA files that doscp
copies (it takes the time of the WEGA file). Used by the corpora: `PRE = dospre.PRE`."""

STAMP = '1989-01-05 05:39:00 UTC'
TOUCH = "host: find . -type f -exec touch -d '%s' {} +" % STAMP


def pre(format_step=True):
    lines = ['time: 600000000', 'terminal: console', 'device: drive1 d1', 'device: ud', TOUCH,
             "host: python3 -c \"open('d1','wb').write(bytes([0xe5]) * 737280)\"", 'host: echo y > y']
    if format_step:
        lines += ['step: /usr/bin/dosformat /dev/rfd1', 'stdin: y']
    return lines


PRE = pre()
