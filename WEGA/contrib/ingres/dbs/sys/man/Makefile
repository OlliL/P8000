SHELL=	/bin/sh


#
#	FOLGENDE FUNKTION SIND INTERN
#
HOME=	/z/dbs/sys/man
DISK=	/dev/dbs
SRC=	00intro		02parser	03qrymod	04aovqp		\
	04bdecom	05dbu		amx		amxi		\
	append		copy		copydb		copyr		\
	creatdb		create		creater		dayfile		\
	dbtmplt		define		delete		destroy		\
	destroydb	destroyr	eqc		error		\
	help		helpr		index		indexr		\
	integrity	iql		iqli		libamx		\
	libq		macros		manr		modify		\
	modifyr		monitor		permit		print		\
	printr		purge		ql		range		\
	rc		rd		refr		replace		\
	report		reporti		restore		retrieve	\
	rg		rp_desc		save		saver		\
	startup		sysmod		toc		users		\
	usersetup	view		Makefile

.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/man
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
