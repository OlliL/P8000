HOME=	/z/dbs/data/base/ingres
SHELL=	/bin/sh
DISK=	dbs
RM=	rm -f

SRC=	au.ii		doc.ii		indexes.ii	attribute.ii	\
	integrities.ii	protect.ii	relation.ii	tree.ii		\
	admin		ingres.lit	new.q		print.q		\
	Makefile	

PGM=	new		print

clean:
	$(RM) $(PGM)

.save:		$(SRC) $(PGM)
	mount /dev/$(DISK) /wrk
	cp $? $@ /wrk/ingres
	> .save
	umount /dev/$(DISK)

$(HOME)/.save:	$(SRC) $(PGM)
	cp $? $(HOME)
	> $(HOME)/.save

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
