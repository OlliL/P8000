"""Configuration of the recon toolkit (host/recon/progcfg.py) for cas (segmented, built by an older scc we do not have)."""
import os

NAME = 'cas'
ORIG = 'bin/cas'
OWN_LIM = 0x9694    # the own code of /bin/cas ends here (listing.c); the members of slibc follow
ORIG_LIM = 0x10000  # the original is stripped
SEG = True
CC = '/bin/scc'

# the corpus: every C source, made into assembler by the original /bin/cc -S -O (n.s) and /bin/scc -S -O (s.s); each is
# assembled as cc does it (cas -u -o X.o X.s); object and messages compared
CORPUS_BINARY = 'build/cas'
CORPUS_VARIANTS = [['n'], ['s']]


def CORPUS_SOURCES():
    import corpusc
    return corpusc.c_sources()


def CORPUS_PREPARE(src, work):
    import corpusc
    for cc, out in (('/bin/cc', 'n'), ('/bin/scc', 's')):
        if corpusc.prepare_s(src, work, cc, ['-O']) is None:
            return None
        os.rename(os.path.join(work, 'in.s'), os.path.join(work, out + '.s'))
    return 'x'


def CORPUS_RUN(binary, base, work, variant):
    return [binary, '-u', '-o', variant[0] + '.o', variant[0] + '.s'], None


def CORPUS_OUTPUTS(base):
    return ['n.o', 's.o']
