"""The corpus of ranlib, strip and size (compare/{ranlib,strip,size}/progcfg.py): objects, executables and libraries of the WEGA tree,
copies with swapped bytes, damaged files, archives that are swapped, stripped, mixed or with too many symbols. A case is one command
line (the program and its arguments, relative to the corpus tree); the program runs in a copy of the tree (run.sh), the output, the
exit status and every file afterwards are compared with the original. The inputs are made by the original ar and strip (z8k-emu)."""
import glob
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import corpusc  # noqa: E402


def swab(b):
    b = bytearray(b)
    for i in range(0, len(b) - 1, 2):
        b[i], b[i + 1] = b[i + 1], b[i]
    return bytes(b)


def build(dest):
    """the corpus tree in dest"""
    R = corpusc.root()
    shutil.rmtree(dest, ignore_errors=True)
    for d in 'osbwajx':
        os.makedirs(os.path.join(dest, d))

    def orig(argv, cwd):
        corpusc.run(argv, cwd, 300)

    # objects: the members of libc.a and slibc.a, binaries, libraries
    orig(['/bin/ar', 'x', '/lib/libc.a'], os.path.join(dest, 'o'))
    orig(['/bin/ar', 'x', '/lib/slibc.a'], os.path.join(dest, 's'))
    for d in 'os':
        try:
            os.remove(os.path.join(dest, d, '__.SYMDEF'))
        except OSError:
            pass
    for f in 'bin/ar bin/size bin/cc bin/cas bin/ld bin/sld bin/gld lib/cpp lib/cparse lib/gopt lib/crt0.o lib/scrt0.o'.split():
        p = os.path.join(R, f)
        if os.path.isfile(p):
            shutil.copy(p, os.path.join(dest, 'b', f.replace('/', '_')))
    for p in glob.glob(os.path.join(R, 'lib', '*.a')) + glob.glob(os.path.join(R, 'usr', 'lib', '*.a')):
        shutil.copy(p, os.path.join(dest, 'a', os.path.basename(p)))
    # byte swapped copies: swab of the header and the segment table (what swab does in the tools)
    srcs = (sorted(glob.glob(dest + '/o/*.o'))[:6] + sorted(glob.glob(dest + '/s/*.o'))[:6] + sorted(glob.glob(dest + '/b/*'))[:3])
    for p in srcs:
        d = open(p, 'rb').read()
        n = 24 + int.from_bytes(d[10:12], 'big')
        open(os.path.join(dest, 'w', os.path.basename(p)), 'wb').write(swab(d[:n]) + d[n:])
    d = open(srcs[0], 'rb').read()
    j = os.path.join(dest, 'j')
    open(j + '/text', 'wb').write(b'just some text\n')
    open(j + '/empty', 'wb').write(b'')
    open(j + '/one', 'wb').write(b'x')
    open(j + '/short.o', 'wb').write(d[:30])
    open(j + '/hdronly.o', 'wb').write(d[:24])
    open(j + '/zeros', 'wb').write(bytes(100))
    # more archives for ranlib: swapped, stripped and damaged members, text members, too many symbols
    x = os.path.join(dest, 'x')
    w = sorted(os.listdir(os.path.join(dest, 'w')))[:8]
    orig(['/bin/ar', 'r', '../x/swapped.a'] + w, os.path.join(dest, 'w'))
    st = os.path.join(dest, 'st')
    os.makedirs(st)
    for f in ('read.o', 'write.o', 'open.o', 'close.o'):
        shutil.copy(os.path.join(dest, 'o', f), st)
    orig(['/usr/bin/strip', 'read.o', 'write.o'], st)
    shutil.copy(j + '/text', st)
    shutil.copy(j + '/short.o', st)
    shutil.copy(j + '/zeros', st)
    orig(['/bin/ar', 'r', '../x/stripped.a', 'read.o', 'write.o', 'open.o'], st)
    orig(['/bin/ar', 'r', '../x/mixed.a', 'text', 'read.o', 'open.o', 'short.o', 'zeros', 'close.o'], st)
    shutil.rmtree(st)
    orig(['/bin/ar', 'r', '../x/textonly.a', 'text', 'empty', 'one'], j)
    orig(['/bin/ar', 'r', 'onlyone.a', '../o/read.o'], x)
    big = os.path.join(dest, 'big')
    ex = os.path.join(dest, 'ex')
    os.makedirs(big)
    os.makedirs(ex)
    for f in glob.glob(dest + '/s/*.o'):
        shutil.copy(f, os.path.join(big, 's_' + os.path.basename(f)))
    for f in glob.glob(dest + '/o/*.o'):
        shutil.copy(f, os.path.join(big, 'o_' + os.path.basename(f)))
    for l in 'libcurses.a libtermlib.a slibcurses.a libPW.a slibPW.a libp.a libmp.a slibmp.a slibtermlib.a slibscreen.a'.split():
        for f in glob.glob(ex + '/*'):
            os.remove(f)
        orig(['/bin/ar', 'x', '/usr/lib/' + l], ex)
        for f in glob.glob(ex + '/*.o'):
            shutil.copy(f, os.path.join(big, l[:-2] + '_' + os.path.basename(f)))
    orig(['/bin/ar', 'r', '../x/big.a'] + sorted(os.listdir(big)), big)
    shutil.rmtree(big)
    shutil.rmtree(ex)


def cases(prog, tree):
    """the command lines (arguments, as one string) of a program; globs are expanded by the shell in the tree"""
    def g(pat):
        return sorted(os.path.relpath(p, tree) for p in glob.glob(os.path.join(tree, pat)))
    c = []
    if prog == 'size':
        c = ['b/bin_ar', 'b/bin_ar b/bin_size b/lib_cpp', 'b/*', 'o/*.o', 's/*.o', 'w/*', 'j/*', 'nonexistent',
             'b/bin_ar nonexistent j/text', 'a/libc.a', '', 'x/swapped.a']
    elif prog == 'strip':
        # the original leaves the file it writes open and has 20 FILEs: from the 17th file of one call on it fails (host: no limit,
        # quirks table of KNOWLEDGE.md); a call has at most 16 files
        def chunks(opt, pat):
            files = g(pat)
            return [(opt + ' ' + ' '.join(files[i:i + 16])).strip() for i in range(0, len(files), 16)]
        c = ['', '-h', '-x o/read.o', 'nonexistent', 'j/text', 'j/empty j/one j/short.o j/hdronly.o j/zeros', 'b/bin_ar'] \
            + chunks('', 'o/*.o') + chunks('-h', 'o/*.o') + chunks('', 's/*.o') + chunks('-h', 's/*.o') \
            + ['w/*', '-h w/*', 'b/lib_cpp b/lib_crt0.o b/lib_scrt0.o', '-h b/lib_cpp b/lib_scrt0.o', 'o/read.o nonexistent o/write.o',
               'o/read.o j/short.o o/write.o', 'o/read.o o/read.o']
    elif prog == 'ranlib':
        c = ['', 'nonexistent', 'j/text', 'j/empty', 'j/one'] + g('a/*.a') + ['a/libc.a a/slibc.a', 'a/libm.a nonexistent a/liby.a'] \
            + g('x/*.a') + ['x/swapped.a x/mixed.a x/stripped.a x/swapped.a']
    return ['%s %s' % (prog, a) for a in c]


def prepare(label, work, tree):
    shutil.copytree(tree, work, dirs_exist_ok=True)
    open(os.path.join(work, '.args'), 'w').write(label.split(' ', 1)[1] + '\n')
    return label
