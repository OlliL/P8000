#!/usr/bin/env python3
"""corpus_prune.py - remove from the store of the corpora (WEGA/.corpus, host/recon/corpusrun.py) what a run has not used.

  corpus_prune.py STAMP [-n] [--store DIR]

Every use of a result (results/orig, results/ok) or of prepared starting files (inputs) sets its time (corpusrun.py
store_read, Generated._valid); a new one is written then. After a full run of all corpora (checkall), everything older
than the file STAMP (made just before the corpora started) belongs to keys that no longer arise: another emulator,
runner, binary, case or tree. Those are removed, and locks/ is emptied (only a running corpusrun.py uses it).
Nothing is done while a corpusrun.py runs (another checkall, a corpus.sh): it may be reading the store.
-n: only count. The result of a variant (CHECK_PATCH) not run since is removed too: it is made again when needed."""
import os
import shutil
import stat
import sys

HERE = os.path.dirname(os.path.abspath(__file__))


def running():
    """the pids of the processes that run corpusrun.py"""
    out = []
    if not os.path.isdir('/proc'):	# macOS
        import subprocess
        for l in subprocess.run(['ps', '-axo', 'pid=,command='], capture_output=True, text=True).stdout.splitlines():
            p, _, cmd = l.strip().partition(' ')
            if p.isdigit() and int(p) != os.getpid() and 'corpusrun.py' in cmd:
                out.append(int(p))
        return out
    for p in os.listdir('/proc'):
        if p.isdigit() and int(p) != os.getpid():
            try:
                cmd = open('/proc/%s/cmdline' % p, 'rb').read().split(b'\0')
            except OSError:
                continue
            if any(c.endswith(b'corpusrun.py') for c in cmd):
                out.append(int(p))
    return out


def rmtree(p):
    def again(fn, path, exc):	# a file or directory without the rights to remove it (a mode of the original)
        try:
            os.chmod(os.path.dirname(path), 0o755)
            os.chmod(path, 0o755)
            fn(path)
        except OSError:
            pass
    shutil.rmtree(p, onerror=again)


def base(name):
    """the key of an entry of inputs: KEY, KEY.skip, KEY.deps, KEY.tmpPID.TID, KEY.oldPID.TID"""
    return name.split('.', 1)[0]


def main():
    a = sys.argv[1:]
    dry = '-n' in a
    a = [x for x in a if x != '-n']
    store = os.path.join(HERE, '..', '..', '.corpus')
    if '--store' in a:
        i = a.index('--store')
        store = a[i + 1]
        del a[i:i + 2]
    if len(a) != 1:
        print(__doc__)
        return 2
    t0 = os.stat(a[0]).st_mtime
    busy = running()
    if busy:
        print('corpus store: not pruned (corpusrun.py runs: pid %s)' % ' '.join(map(str, busy[:5])))
        return 0
    n = {'results': 0, 'inputs': 0, 'locks': 0}
    kept = {'results': 0, 'inputs': 0}
    rd = os.path.join(store, 'results')
    for kind in sorted(os.listdir(rd)) if os.path.isdir(rd) else []:
        for sub in os.listdir(os.path.join(rd, kind)):
            d = os.path.join(rd, kind, sub)
            for f in os.listdir(d):
                p = os.path.join(d, f)
                if os.lstat(p).st_mtime < t0:
                    n['results'] += 1
                    if not dry:
                        os.remove(p)
                else:
                    kept['results'] += 1
    ind = os.path.join(store, 'inputs')
    for sub in os.listdir(ind) if os.path.isdir(ind) else []:
        d = os.path.join(ind, sub)
        groups = {}
        for f in os.listdir(d):
            groups.setdefault(base(f), []).append(f)
        for k, names in groups.items():
            live = any(os.lstat(os.path.join(d, f)).st_mtime >= t0 for f in names if f in (k, k + '.skip'))
            if live:
                kept['inputs'] += 1
                continue
            n['inputs'] += 1
            if not dry:
                for f in names:
                    p = os.path.join(d, f)
                    if stat.S_ISDIR(os.lstat(p).st_mode):
                        rmtree(p)
                    else:
                        os.remove(p)
    ld = os.path.join(store, 'locks')
    for f in os.listdir(ld) if os.path.isdir(ld) else []:
        n['locks'] += 1
        if not dry:
            os.remove(os.path.join(ld, f))
    print('corpus store: %s %d results, %d inputs, %d locks; kept %d results, %d inputs' % (
        'would remove' if dry else 'removed', n['results'], n['inputs'], n['locks'], kept['results'], kept['inputs']))
    return 0


if __name__ == '__main__':
    sys.exit(main())
