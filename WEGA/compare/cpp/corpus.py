"""Corpus of cpp (host/recon/corpusrun.py): every C source of src/cmd and src/lib (with the other files of its directory),
`cpp -I. NAME t.i`; the original against the shipped build and the host build. The settings of the reconstruction tools
stay in progcfg.py."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import corpusc  # noqa: E402

NAME = 'cpp'
ORIG = 'lib/cpp'
PROG = 'cpp'
CANDIDATES = {'wega': 'src/cmd/cc/cpp/cpp', 'host': 'host/cpp/cpp'}
ROOT_FILES = ['usr/include']


def HOST_ENV(root):
    """the host cpp reads the include directory of the system from CPP_INCLUDE"""
    return {'CPP_INCLUDE': os.path.join(root, 'usr', 'include')}


def normalize(what, data):
    """the host cpp names the include directory as it was given (CPP_INCLUDE), the original as /usr/include"""
    root = os.path.join(corpusc.root(), 'usr', 'include')
    if isinstance(data, str):
        return data.replace(root, '/usr/include')
    return data.replace(root.encode(), b'/usr/include')


# expectations on chosen cases (the original really preprocesses them)
EXPECT = {'src/lib/libc/common/gen/abs.c': 'expect: exit 0\nexpect: file t.i contains abs(arg)\n'}


def generate(g):
    for src in g.c_sources():
        b = os.path.basename(src)
        yield g.case(src, corpusc.prepare_cpp, 'step: cpp -I. %s t.i\n' % b + EXPECT.get(g.name(src), ''))
