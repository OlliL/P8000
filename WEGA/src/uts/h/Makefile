
IFILES=	mdsize.h sysparm.h ver.h
IDIR=	/usr/sys/h

OWN=	bin
GRP=	system
MOD=	644

clean:
	
all:
	
install: all
	sh -c 'test -d $(IDIR) || mkdir $(IDIR)'
	sh -c 'for file in $(IFILES) ; do \
		cp $$file $(IDIR) ; \
		chmog $(MOD) $(OWN) $(GRP) $(IDIR)/$$file ; \
	done'
