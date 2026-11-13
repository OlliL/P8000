"""Configuration of the recon toolkit (host/recon/progcfg.py) for scodgen: the sources of codgen with -DSEG."""
import importlib.util
import os

HERE = os.path.dirname(os.path.abspath(__file__))
_s = importlib.util.spec_from_file_location('codgen_progcfg', os.path.join(HERE, '..', 'codgen', 'progcfg.py'))
_p = importlib.util.module_from_spec(_s)
_s.loader.exec_module(_p)
globals().update({k: v for k, v in vars(_p).items() if k.isupper()})

NAME = 'scodgen'
ORIG = 'lib/scodgen'
OWN_LIM = 0xc022    # the own code ends here; the libc part follows
SRCDIR = os.path.normpath(os.path.join(HERE, '..', '..', 'src', 'cmd', 'codgen'))
CCFLAGS = ['-DSEG']
MODULES = [m.replace('table3_codgen', 'table3_scodgen') for m in _p.MODULES]
model_link = _p.model_link
MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/home/olivleh1/git/P8000/tmp/tmp'), 'd12model_scodgen', os.path.basename(_p.MODEL_OUT))
STRICT_INPUT = MODEL_OUT


# the corpus: every C source; the input (.I .D .S) from the original /bin/scc -S -:d, compared: the assembler text (stdout)
CANDIDATES = {'wega': 'src/cmd/codgen/build/seg/scodgen', 'host': 'host/codgen/scodgen'}


def CORPUS_SOURCES():
    import corpusc
    return corpusc.c_sources()


def CORPUS_PREPARE(src, work):
    import corpusc
    return corpusc.prepare_ids(src, work, '/bin/scc')


def CORPUS_RUN(binary, base, work):
    return [binary, '-E', base], base + '.I'


def CORPUS_OUTPUTS(base):
    return []
