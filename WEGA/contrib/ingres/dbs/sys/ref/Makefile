SHELL=	/bin/sh


#
#	FOLGENDE FUNKTION SIND INTERN
#
HOME=	/z/dbs/sys/ref
DISK=	/dev/dbs
SRC=	amx		append		copy		copydb		\
	copyr		creatdb		create		creater		\
	dayfile		define		delete		destroy		\
	destroydb	destroyr	eqc		error		\
	help		helpr		index		indexr		\
	integrity	iql		manr		modify		\
	modifyr		print		printr		purge		\
	range		rc		rd		refr		\
	replace		report		restore		retrieve	\
	rg		rp_desc		save		saver		\
	startup		sysmod		toc		users		\
	usersetup	view		Makefile

.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/ref
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
