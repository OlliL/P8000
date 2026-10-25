/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /bin/cc -O -c dosformat.c
/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /bin/cc dosformat.o dosplit.o dosdev.o dosopn.o dosdel.o dosfat.o dosin.o  dosout.o -o dosformat
/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /bin/nm dosformat > dosformat.nm
/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /bin/cc -O -Sl dosformat.c
/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /usr/bin/strip dosformat
python3 /mnt/share/cvs/P8000/WEGA/host/tools/sdis.py -s dosformat > dosformat.asm
