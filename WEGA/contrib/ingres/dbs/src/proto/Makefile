HOME=	/z/dbs/src/proto
SHELL=	/bin/sh
DISK=	/dev/dbs

SRC=	access		amx_cc		amx_lib		dbu		\
	decomp		eql		etc.daba	etc.ing_db	\
	form		gutil		h		iutil		\
	libq		monitor		oamx_lib	olibq		\
	ovqp		parser		pqr		qrymod		\
	redabas		samx_lib	slibq		support.daba	\
	support.ing_db	sys		yyyacc		Makefile

.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/proto
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
