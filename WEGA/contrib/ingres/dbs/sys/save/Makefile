HOME=	/z/dbs/sys/save
SHELL=	/bin/sh
DISK=	/dev/dbs
RM=	rm -f

SRC=	S0		S0.save		S1		S1.save		\
	S2		S2.save		S3		S3.save		\
	S4		S4.save		S5		S5.save		\
	S6		S6.save		S7		S7.save		\
	cat1.save	doc.save	get		lib.save	\
	put		SAVE		SAVE.bin	SAVE.file

clean:

.save:		Makefile $(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/save
	> $@
	umount $(DISK)

$(HOME)/.save:	Makefile $(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
