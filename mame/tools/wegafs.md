# wegafs.py: unpack WEGA file systems

Lists and unpacks WEGA (UNIX System III/V style, 512 byte blocks, big-endian) file systems from **raw**
images. Python 3, no other packages.

Convert the source first:

```sh
dskconv w31root1.cqm w31root1.raw -otype raw      # libdsk, floppy images
chdman extractraw -i wega.chd -o wega.raw          # hard disk CHD
```

```sh
./wegafs.py scan wega.raw                          # find the file systems (hard disk: one per partition)
./wegafs.py info w31root1.raw
./wegafs.py ls -R w31root1.raw
./wegafs.py cat w31root1.raw /CONTENTS
./wegafs.py check w31root1.raw                     # walk the tree, report inconsistencies
./wegafs.py extract w31root1.raw out/              # floppy: the file system starts at 0
sudo ./wegafs.py --offset-blocks 16162 extract wega.raw root/    # partition found with "scan"
```

`extract` keeps directories, names, permission bits (including set-uid, set-gid, sticky), access and
modification times, hard links and holes. Owner and group are set with `chown` when run as root (otherwise
use `--chown` or run with `sudo`); device nodes are created with `mknod` as root. For everything that cannot
be set as a normal user, and for the change time, `<outdir>/.wegafs-manifest.tsv` lists inode, type, mode, link
count, uid, gid, size, atime, mtime, ctime (UTC seconds) and path.

Tested on the WEGA 3.1 floppies `w31root1`, `w31usr1`, `w31doc1` (`check`: 0 problems, dates 1988 to 1989)
and on the installed hard disk (4 partitions found by `scan`; root partition extracted with set-uid files,
device nodes and hard links).
