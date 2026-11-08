#!/usr/bin/env python3
"""corpusdeps.py DIR PROG...: checks the dependencies a corpus declares (CORPUS_ROOT_FILES, CORPUS_DEPENDS of compare/PROG/progcfg.py, the original
binary) against what the runs really do: DIR holds the strace files of host/tools/corpusdeps.sh (STAGE_STRACE of stagecorpus.py). Reports every
file of the tree ROOT (not tmp) or of the repository that a run opened and the declaration does not cover, and every program a run started
beyond the emulator. Exit status 1 when something is missing."""
import glob
import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'recon'))
import progcfg

rc = 0
for prog in sys.argv[2:]:
    os.environ['RECON_PROG'] = prog
    cfg = progcfg.load()
    root = os.path.realpath(cfg.ROOT)
    cover = [os.path.join(root, cfg.ORIG_BIN)] + [os.path.join(root, d) for d in getattr(cfg, 'CORPUS_ROOT_FILES', [])]
    cover += [os.path.join(cfg.REPO, d) for dd in getattr(cfg, 'CORPUS_DEPENDS', {}).values() for d in dd]
    cands = getattr(cfg, 'CANDIDATES', {})
    cover += [os.path.join(cfg.REPO, v) for v in cands.values()]
    missing = set()
    for f in glob.glob(os.path.join(sys.argv[1], prog + '.*')):
        for l in open(f, errors='replace'):
            m = re.search(r'(openat|open|execve|access|stat|newfstatat)\((?:AT_FDCWD, )?"([^"]+)"', l)
            if not m or 'ENOENT' in l and m.group(1) != 'execve' and 'O_CREAT' not in l and not m.group(2).startswith(root):
                continue
            path = os.path.realpath(m.group(2))
            if m.group(1) == 'execve':
                if 'ENOENT' not in l and not path.endswith(('z8k-emu', 'strace')) and not path.startswith(('/tmp/', root + '/tmp/', '/bin/', '/usr/bin/')) \
                        and not any(path == c for c in cover):
                    missing.add('runs ' + path)
                continue
            if path.startswith(root + '/') and not path.startswith(root + '/tmp'):
                if not any(path == c or path.startswith(c.rstrip('/') + '/') for c in cover):
                    missing.add('reads ' + path)
    for x in sorted(missing):
        print('%s: not declared: %s' % (prog, x))
        rc = 1
sys.exit(rc)
