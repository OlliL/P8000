#
# "@[$]Makefile fuer getfile, putfile, local und remote"
#

ECHO = @/bin/echo
INCL = /usr/include

# Installationsnachricht
BIN =  is ready for installation in /bin

# Cflags
CFLAGS+=	-Wall

#
# Abhaengigkeiten und Aktionen
#

default:	putfile getfile local remote

putfile:	putfile.c $(INCL)/sgtty.h $(INCL)/sys/types.h \
	$(INCL)/sys/stat.h $(INCL)/signal.h
	$(CC) $(CFLAGS) putfile.c -o $@
	$(ECHO) $@ $(BIN)
	$(ECHO) with mode: 751  owner: bin  group: 0

local:	local.c $(INCL)/sgtty.h
	$(CC) $(CFLAGS) local.c -o $@
	$(ECHO) $@ $(BIN)
	$(ECHO) with mode: 751  owner: bin  group: 0

getfile:	getfile.c $(INCL)/sgtty.h $(INCL)/signal.h
	$(CC) $(CFLAGS) getfile.c -o $@
	$(ECHO) $@ $(BIN)
	$(ECHO) with mode: 751  owner: bin  group: 0

remote:	remote.c $(INCL)/termios.h $(INCL)/signal.h $(INCL)/stdio.h \
	$(INCL)/sys/types.h $(INCL)/sys/stat.h $(INCL)/pwd.h
	$(CC) $(CFLAGS) remote.c -o $@
	$(ECHO) $@ $(BIN)
	$(ECHO) with mode: 751  owner: bin  group: 0

clean:
	rm -f putfile getfile local remote
