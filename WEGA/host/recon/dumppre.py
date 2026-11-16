"""The common start of the corpora of dump, dumpdir and restor (compare/dump, dumpdir, restor): the file system that is dumped
(an image of 400 blocks made by host/tools/wegafs.py, the raw device /dev/rz = rmd4), the floppies of drive 0 and drive 1 (empty
images: the archives; /dev/dumpdev is drive 1), the date file /etc/ddate (empty, in the layer over the tree), the clock at
600000000 and the console, on which dump, dumpdir and restor ask for the next disk. Used as `PRE = dumppre.pre(...)`.

The file system holds: /a (6 bytes), /dir/b (3001 bytes, 7 blocks), /dir/sub/c (2 bytes), /big (6000 bytes, 12 blocks:
10 direct and 2 through the single indirect block), all with the time 500000000 (before the clock), owner 3 3."""

FS = 'python3 $WEGA_HOST/tools/wegafs.py'


def fs_lines(blocks=400, extra=''):
    return ("host: printf 'alpha\\n' > fa && python3 -c \"print('b' * 3000)\" > fb && printf 'c\\n' > fc && "
            "python3 -c \"print('g' * 5999)\" > fg && " + ' && '.join(
                ['%s format z.img %d 500000000' % (FS, blocks), '%s mkdir z.img /dir 755 3 3 500000000' % FS,
                 '%s mkdir z.img /dir/sub 750 3 3 500000000' % FS, '%s put z.img /a fa 644 3 3 500000000' % FS,
                 '%s put z.img /dir/b fb 640 3 3 500000000' % FS, '%s put z.img /dir/sub/c fc 600 3 3 500000000' % FS,
                 '%s put z.img /big fg 664 3 3 500000000' % FS]) + ' && rm fa fb fc fg' + extra)


def pre(blocks=400):
    return ['user: 0 0', 'time: 600000000', 'terminal: console', 'device: rmd4 image:z.img', 'device: drive0 d0',
            'device: drive1 d1', fs_lines(blocks) + ' && : > d0 && : > d1 && mkdir -p $Z8K_OVERLAY/etc && : > $Z8K_OVERLAY/etc/ddate']
