HOME=	/z/dbs/data/base/adr
SHELL=	/bin/sh
DISK=	dbs
RM=	rm -f
AMX=	amx -O -s -i -o $@ $@.x

SRC=	admin		adr.ii		attribute.ii	indexes.ii	\
	integrities.ii	protect.ii	relation.ii	tree.ii		\
	dis.ii		 Makefile	a.x		adr.x		\
	d.x		dis.x		kurz.x		name.x		\
	new.x

PGM=	a		adr		d		dis		\
	kurz		name		new

install:	$(PGM)

a:		a.x
	$(AMX)

adr:		adr.x
	$(AMX)

d:		d.x
	$(AMX)

dis:		dis.x
	$(AMX)

kurz:		kurz.x
	$(AMX)

brief:		brief.x
	$(AMX)

name:		name.x
	$(AMX)

new:		new.x
	$(AMX)

clean:
	$(RM) $(PGM)

.save:		$(SRC) $(PGM)
	mount /dev/$(DISK) /wrk
	cp $? $@ /wrk/adr
	> .save
	umount /dev/$(DISK)

$(HOME)/.save:	$(SRC) $(PGM)
	cp $? $(HOME)
	> $(HOME)/.save

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
