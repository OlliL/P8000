SHELL=	/bin/sh


#
#	FOLGENDE FUNKTION SIND INTERN
#
HOME=	/z/dbs/cat
DISK=	/dev/dbs
SRC1=	1doc.1		1doc.2		1doc.3		1doc.4		\
	1doc.5		1doc.6		2doc.1		2doc.2		\
	2doc.3		Makefile

SRC2=	2doc.4		2doc.5		2doc.6		3doc		\
	4sachwort	5inhalt		Makefile

.save1:		$(SRC1)
	mount $(DISK) /wrk
	cp $? $@ /wrk/cat1
	> $@
	umount $(DISK)

.save2:		$(SRC2)
	mount $(DISK) /wrk
	cp $? $@ /wrk/cat2
	> $@
	umount $(DISK)

$(HOME)/.save1:	$(SRC1)
	cp $? $(HOME)
	> $@

$(HOME)/.save2:	$(SRC2)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save1
.PRECIOUS:	.save2
.PRECIOUS:	$(HOME)/.save1
.PRECIOUS:	$(HOME)/.save2
