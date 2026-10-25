/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /bin/cc -O -c doscp.c
/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /bin/cc -O -c dosplit.c
/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /bin/cc -O -c dosdev.c
/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /bin/cc -O -Sl dosdev.c
/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /bin/cc doscp.o dosplit.o dosdev.o dosopn.o dosdel.o dosfat.o dosin.o  dosout.o -o doscp
/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /bin/nm doscp > doscp.nm
/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /bin/cc -O -Sl doscp.c
/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/share/cvs/P8000/tmp/wega31_root /usr/bin/strip doscp
python3 /mnt/share/cvs/P8000/WEGA/host/tools/sdis.py -s doscp > doscp.asm
python3 /mnt/share/cvs/P8000/WEGA/host/tools/sdis.py -l doscp > doscp.l
