##############################################################################
#+--------------------------------------------------------------------------+#
#|                                                                          |#
#| +----------------------------------------------------------------------+ |#
#| |                                                                      | |#
#| |    |||||||    |     |     |||||                ||||||     ||||||     | |#
#| |       |       ||    |    |     |               |     |    |     |    | |#
#| |       |       | |   |    |                     |     |    |     |    | |#
#| |       |       |  |  |    |  |||                |     |    ||||||     | |#
#| |       |       |   | |    |     |               |     |    |     |    | |#
#| |       |       |    ||    |     |               |     |    |     |    | |#
#| |    |||||||    |     |     |||||                ||||||     ||||||     | |#
#| |                                     |||||||                          | |#
#| |                                                                      | |#
#| +----------------------------------------------------------------------+ |#
#|                                                                          |#
#|                   A relational database managment system                 |#
#|                                                                          |#
#|                               RELEASE 6.3                                |#
#|                               VERSION -4-                                |#
#|                                                                          |#
#+--------------------------------------------------------------------------+#
##############################################################################



##############################
#+--------------------------+#
#|   General system paths   |#
#+--------------------------+#
##############################

#	the DBS subtree
HOME=	/z/dbs/src/conf
SHELL=	/bin/sh
DBS=	../..

#	location of binaries
BIN=	$(DBS)/bin
SYS_BIN=$(DBS)/sys/bin

#	residence of system libraries
LIB=	../lib
SYSLIB=	$(DBS)/lib

#	the sources
SRC=	..

#	the configurations
CONF=	../proto


######################
#+------------------+#
#|   The commands   |#
#+------------------+#
######################

#	C-preprocessor
CPP=	/lib/cpp -P

#	make
MAKE=	make
MFLAGS=	-k

#
TOUCH=	touch
RM=	-rm -f
CHMOD=	chmod
MODE=	0644
SED=	sed -f cpp.sed


########################
#+--------------------+#
#|   And the moduls   |#
#+--------------------+#
########################

ACCESS=	access
AMX_CC=	amx_cc
AMX_LIB=amx_lib
DBU=	dbu
DECOMP=	decomp
EQL=	eql
FILES=	etc
FORM=	form
ETC=	$(DBS)/sys/etc
GUTIL=	gutil
H=	h
IUTIL=	iutil
LIBQ=	libq
MONITOR=monitor
OAMX_LIB=oamx_lib
OLIBQ=	olibq
OVQP=	ovqp
PARSER=	parser
PQR=	pqr
QRYMOD=	qrymod
REDABAS=redabas
SAMX_LIB=samx_lib
SLIBQ=	slibq
SUPPORT=support
SYS=	sys
YYYACC=	yyyacc


###################
#+---------------+#
#|   The files   |#
#+---------------+#
###################

#	The machine configuration
CONFIG=	m.h conf.h

#	makefile name
MKFILE=	Makefile

#	all Makefiles

MFILES=	$(ETC)/$(MKFILE)						\
	$(SRC)/$(REDABAS)/$(MKFILE)	$(SRC)/$(FORM)/$(MKFILE)	\
	$(SRC)/$(ACCESS)/$(MKFILE)	$(SRC)/$(AMX_CC)/$(MKFILE)	\
	$(SRC)/$(AMX_LIB)/$(MKFILE)	$(SRC)/$(DBU)/$(MKFILE)		\
	$(SRC)/$(DECOMP)/$(MKFILE)	$(SRC)/$(EQL)/$(MKFILE)		\
	$(SRC)/$(GUTIL)/$(MKFILE)	$(SRC)/$(H)/$(MKFILE)		\
	$(SRC)/$(IUTIL)/$(MKFILE)	$(SRC)/$(LIBQ)/$(MKFILE)	\
	$(SRC)/$(MONITOR)/$(MKFILE)	$(SRC)/$(OAMX_LIB)/$(MKFILE)	\
	$(SRC)/$(OLIBQ)/$(MKFILE)	$(SRC)/$(OVQP)/$(MKFILE)	\
	$(SRC)/$(PARSER)/$(MKFILE)	$(SRC)/$(PQR)/$(MKFILE)		\
	$(SRC)/$(QRYMOD)/$(MKFILE)	$(SRC)/$(SAMX_LIB)/$(MKFILE)	\
	$(SRC)/$(SLIBQ)/$(MKFILE)	$(SRC)/$(SUPPORT)/$(MKFILE)	\
	$(SRC)/$(SYS)/$(MKFILE)		$(SRC)/$(YYYACC)/$(MKFILE)



###########################
#+-----------------------+#
#|   Install the world   |#
#+-----------------------+#
###########################

allinstall:			conf install

conf:				$(MFILES)

install:
	cd $(SRC)/$(YYYACC);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(GUTIL);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(IUTIL);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(ACCESS);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(DBU);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(MONITOR);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(PARSER);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(QRYMOD);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(OVQP);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(DECOMP);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SUPPORT);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SYS);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(EQL);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(LIBQ);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(OLIBQ);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SLIBQ);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(PQR);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(AMX_CC);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(AMX_LIB);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(OAMX_LIB);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SAMX_LIB);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(REDABAS);	$(MAKE) $(MFLAGS) $@

unload:
	cd $(SRC)/$(GUTIL);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(IUTIL);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(ACCESS);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(DBU);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(MONITOR);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(PARSER);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(QRYMOD);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(OVQP);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(DECOMP);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(EQL);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(LIBQ);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(OLIBQ);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SLIBQ);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(PQR);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(AMX_CC);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(AMX_LIB);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(OAMX_LIB);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SAMX_LIB);	$(MAKE) $(MFLAGS) $@

pqrconf:
	$(CPP) -DSINGLE $(CONF)/$(PQR)	| $(SED) > $(SRC)/$(PQR)/$(MKFILE)
	@-$(CHMOD) $(MODE) $(SRC)/$(PQR)/$(MKFILE)
	cd $(SRC)/$(PQR);	$(MAKE) $(MFLAGS) conf
	$(TOUCH) $(CONF)/$(PQR)

$(ETC)/$(MKFILE):		$(CONFIG) $(CONF)/$(FILES)
	$(CPP) $(CONF)/$(FILES)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@
	cd $(ETC);		$(MAKE) $(MFLAGS) conf

$(SRC)/$(ACCESS)/$(MKFILE):	$(CONFIG) $(CONF)/$(ACCESS)
	$(CPP) $(CONF)/$(ACCESS)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(AMX_CC)/$(MKFILE):	$(CONFIG) $(CONF)/$(AMX_CC)
	$(CPP) $(CONF)/$(AMX_CC)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(FORM)/$(MKFILE):	$(CONFIG) $(CONF)/$(FORM)
	$(CPP) $(CONF)/$(FORM)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(AMX_LIB)/$(MKFILE):	$(CONFIG) $(CONF)/$(AMX_LIB)
	$(CPP) $(CONF)/$(AMX_LIB)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(DBU)/$(MKFILE):	$(CONFIG) $(CONF)/$(DBU)
	$(CPP) $(CONF)/$(DBU)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(DECOMP)/$(MKFILE):	$(CONFIG) $(CONF)/$(DECOMP)
	$(CPP) $(CONF)/$(DECOMP)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(EQL)/$(MKFILE):	$(CONFIG) $(CONF)/$(EQL)
	$(CPP) $(CONF)/$(EQL)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(GUTIL)/$(MKFILE):	$(CONFIG) $(CONF)/$(GUTIL)
	$(CPP) $(CONF)/$(GUTIL)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@
	cd $(SRC)/$(GUTIL);	$(MAKE) $(MFLAGS) conf

$(SRC)/$(H)/$(MKFILE):		$(CONFIG) $(CONF)/$(H)
	$(CPP) $(CONF)/$(H)		| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(IUTIL)/$(MKFILE):	$(CONFIG) $(CONF)/$(IUTIL)
	$(CPP) $(CONF)/$(IUTIL)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(LIBQ)/$(MKFILE):	$(CONFIG) $(CONF)/$(LIBQ)
	$(CPP) $(CONF)/$(LIBQ)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(MONITOR)/$(MKFILE):	$(CONFIG) $(CONF)/$(MONITOR)
	$(CPP) $(CONF)/$(MONITOR)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(OAMX_LIB)/$(MKFILE):	$(CONFIG) $(CONF)/$(OAMX_LIB)
	$(CPP) $(CONF)/$(OAMX_LIB)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(OLIBQ)/$(MKFILE):	$(CONFIG) $(CONF)/$(OLIBQ)
	$(CPP) $(CONF)/$(OLIBQ)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(OVQP)/$(MKFILE):	$(CONFIG) $(CONF)/$(OVQP)
	$(CPP) $(CONF)/$(OVQP)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(PARSER)/$(MKFILE):	$(CONFIG) $(CONF)/$(PARSER)
	$(CPP) $(CONF)/$(PARSER)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(PQR)/$(MKFILE):	$(CONFIG) $(CONF)/$(PQR)
	$(CPP) $(CONF)/$(PQR)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@
	cd $(SRC)/$(PQR);	$(MAKE) $(MFLAGS) conf

$(SRC)/$(QRYMOD)/$(MKFILE):	$(CONFIG) $(CONF)/$(QRYMOD)
	$(CPP) $(CONF)/$(QRYMOD)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(REDABAS)/$(MKFILE):	$(CONFIG) $(CONF)/$(REDABAS)
	$(CPP) $(CONF)/$(REDABAS)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(SAMX_LIB)/$(MKFILE):	$(CONFIG) $(CONF)/$(SAMX_LIB)
	$(CPP) $(CONF)/$(SAMX_LIB)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(SLIBQ)/$(MKFILE):	$(CONFIG) $(CONF)/$(SLIBQ)
	$(CPP) $(CONF)/$(SLIBQ)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(SUPPORT)/$(MKFILE):	$(CONFIG) $(CONF)/$(SUPPORT)
	$(CPP) $(CONF)/$(SUPPORT)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(SYS)/$(MKFILE):	$(CONFIG) $(CONF)/$(SYS)
	$(CPP) $(CONF)/$(SYS)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@

$(SRC)/$(YYYACC)/$(MKFILE):	$(CONFIG) $(CONF)/$(YYYACC)
	$(CPP) $(CONF)/$(YYYACC)	| $(SED) > $@
	@-$(CHMOD) $(MODE) $@


########################
#+--------------------+#
#|   Make all clean   |#
#+--------------------+#
########################

reset:
	$(RM)	$(BIN)/[a-z]*		\
		$(SYS_BIN)/[a-z]*	\
		$(LIB)/[a-z]*		\
		$(SYSLIB)/[a-z]*	\
		$(ETC)/amx_*		\
		$(ETC)/customer		\
		$(ETC)/dbtmplt6.3	\
		$(ETC)/*_tab6.3		\
		$(ETC)/users		\
		$(DBS)/CONF		\
		$(DBS)/CU		\
		$(DBS)/GEN.?		\
		m.h			\
		$(MFILES)

allclean:
	cd $(SRC)/$(YYYACC);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(GUTIL);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(IUTIL);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(ACCESS);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(DBU);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(MONITOR);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(PARSER);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(QRYMOD);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(OVQP);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(DECOMP);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SUPPORT);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SYS);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(EQL);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(LIBQ);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(OLIBQ);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SLIBQ);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(PQR);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(AMX_CC);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(AMX_LIB);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(OAMX_LIB);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SAMX_LIB);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(REDABAS);	$(MAKE) $(MFLAGS) $@

clean:
	cd $(SRC)/$(YYYACC);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(GUTIL);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(IUTIL);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(ACCESS);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(DBU);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(MONITOR);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(PARSER);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(QRYMOD);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(OVQP);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(DECOMP);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SUPPORT);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SYS);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(EQL);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(LIBQ);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(OLIBQ);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SLIBQ);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(PQR);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(AMX_CC);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(AMX_LIB);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(OAMX_LIB);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(SAMX_LIB);	$(MAKE) $(MFLAGS) $@
	cd $(SRC)/$(REDABAS);	$(MAKE) $(MFLAGS) $@



###############################
#+---------------------------+#
#|  Saving and Distribution  |#
#+---------------------------+#
###############################

ALLINSTALL:			allinstall

CONF:				conf

INSTALL:			install

UNLOAD:				unload

PQRCONF:			pqrconf

RESET:				reset

ALLCLEAN:			allclean

CLEAN:				clean

DISK=	/dev/dbs
SAVE=	conf.h		cpp.sed		dbtmplt.16	dbtmplt.32	\
	gen.h		m.h		machine.h	Makefile	\
	iql_tab.__	iql_tab._d	iql_tab.o_	iql_tab.od

.save:				$(SAVE)
	mount $(DISK) /wrk
	cp $? $@ /wrk/conf
	> $@
	umount $(DISK)

$(HOME)/.save:			$(SAVE)
	cp $? $(HOME)
	> $@

.PRECIOUS:			.save
.PRECIOUS:			$(HOME)/.save
