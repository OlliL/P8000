HOME=	/z/dbs/src/struct
SHELL=	/bin/sh
SYS=	../../sys/bin
DISK=	/dev/dbs
MV=	mv
CHMOD=	chmod
UMODE=	0700
RM=	rm -f

SRC=	DESCXX.c	DESC.c		RELATION.c	ATTRIBUTE.c	\
	INDEX.c		ACCBUF.c	TREE.c		PROTECT.c	\
	INTEGRITY.c	REFS.c		TUP_ID.c	ADMIN.c		\
	ADMINHDR.c	LOCKREQ.c	PIPFRMT.c	TUP_HDR.c	\
	OPTAB.c		SPECIAL.c	LASTOK.c	BATCHBUF.c	\
	SI_DOMS.c	BATCHHD.c	ACCESSPARAM.c	RETCODE.c	\
	DESXX.c		OUT_ARG.c	SYMBOL.c	QTREE.c		\
	VAR_NODE.c	ROOT_NODE.c	RES_NODE.c	AOP_NODE.c	\
	OP_NODE.c	SVAR_NODE.c	SRCID.c		C.c		\
	S.c		Makefile

ALL=	DESCXX		DESC		RELATION	ATTRIBUTE	\
	INDEX		ACCBUF		TREE		PROTECT		\
	INTEGRITY	REFS		TUP_ID		ADMIN		\
	ADMINHDR	LOCKREQ		PIPFRMT		TUP_HDR		\
	OPTAB		SPECIAL		LASTOK		BATCHBUF	\
	SI_DOMS		BATCHHD		ACCESSPARAM	RETCODE		\
	DESXX		OUT_ARG		SYMBOL		QTREE		\
	VAR_NODE	ROOT_NODE	RES_NODE	AOP_NODE	\
	OP_NODE		SVAR_NODE	SRCID		C		\
	S

install:	$(ALL)

C:		C.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

S:		S.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

DESCXX:		DESCXX.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

TUP_HDR:	TUP_HDR.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

DESC:		DESC.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

RELATION:	RELATION.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

ATTRIBUTE:	ATTRIBUTE.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

INDEX:		INDEX.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

ACCBUF:		ACCBUF.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

TREE:		TREE.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

PROTECT:	PROTECT.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

INTEGRITY:	INTEGRITY.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

REFS:		REFS.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

TUP_ID:		TUP_ID.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

ADMIN:		ADMIN.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

ADMINHDR:	ADMINHDR.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

LOCKREQ:	LOCKREQ.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

PIPFRMT:	PIPFRMT.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

OPTAB:		OPTAB.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

SPECIAL:	SPECIAL.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

LASTOK:		LASTOK.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

BATCHBUF:	BATCHBUF.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

SI_DOMS:	SI_DOMS.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

BATCHHD:	BATCHHD.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

ACCESSPARAM:	ACCESSPARAM.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

RETCODE:	RETCODE.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

DESXX:		DESXX.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

OUT_ARG:	OUT_ARG.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

SYMBOL:		SYMBOL.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

QTREE:		QTREE.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

VAR_NODE:	VAR_NODE.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

ROOT_NODE:	ROOT_NODE.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

RES_NODE:	RES_NODE.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

AOP_NODE:	AOP_NODE.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

OP_NODE:	OP_NODE.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

SVAR_NODE:	SVAR_NODE.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

SRCID:		SRCID.c
	$(CC) -O -s -o $@ $@.c
	@-$(CHMOD) $(UMODE) $@
	$(MV) $@ $(SYS)

clean:
	$(RM) $(ALL)

.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/struct
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
