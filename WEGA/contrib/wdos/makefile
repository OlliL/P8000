all:	build docs floppy

clean:
	rm -f image/INSTALL
	rm -f image/README
	rm -f image/bin/wm
	rm -f image/bin/wdos
	rm -f image/wdoslib/disks/wdos_sys
	rm -f image/wdoslib/bios.p8
	rm -f image/wdoslib/bios.wy
	rm -f image/wdoslib/bios
	rm -f image/wdoslib/run_lpr
	rm -f image/wdoslib/help
	rm -f image/wdoslib/wdos.doc
	rmdir image/bin
	rmdir image/wdoslib/disks
	rmdir image/wdoslib
	cd src ; make clean
	cd doc ; make clean

build:
	cd src ; make all

docs:
	cd doc ; make all
	
floppy:
	-mkdir image/bin
	-mkdir image/wdoslib
	-mkdir image/wdoslib/disks
	cp disks/wdos_sys	image/wdoslib/disks
	cp src/bios.p8		image/wdoslib
	cp src/bios.wy		image/wdoslib
	cp src/bios		image/wdoslib
	cp src/run_lpr		image/wdoslib
	cp doc/help		image/wdoslib
	cp doc/wdos.doc 	image/wdoslib
	cp src/wm		image/bin
	cp src/wdos		image/bin
	cp doc/README		image
	cp src/INSTALL		image
	chmod 755 image/bin image/wdoslib image/wdoslib/disks
	chmod 755 image/bin/wm image/bin/wdos
	chmod 755 image/INSTALL image/wdoslib/run_lpr
	chmod 644 image/wdoslib/bios* image/wdoslib/help
	chmod 644 image/wdoslib/wdos.doc image/wdoslib/disks/wdos_sys
	@echo "Now mv into image/ and tar the content on a floppy\!"
