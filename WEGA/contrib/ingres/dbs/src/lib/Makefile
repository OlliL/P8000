HOME=	/z/dbs/src/lib
SHELL=	/bin/sh
DISK=	/dev/dbs
RM=	rm -f

SRC=	a.a		amx_cc.a	dbu.a		decomp.a	\
	eql.a		g.a		i.a		qrymod.a	\
	monitor.a	ovqp.a		parser.a			\
	pqr.a		rc.a		rg.a

clean:
	$(RM) $(SRC)

.save:		Makefile $(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/lib
	> $@
	umount $(DISK)

$(HOME)/.save:	Makefile $(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
