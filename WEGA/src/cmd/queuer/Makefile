# queuer: the files of the spooler queue in /usr/spool/queuer (the files of the WEGA root; no source in System III, Plexus or 4.4BSD):
# config (the configuration of dqueuer) and logfile (empty). The programs nq and dqueuer are not here yet.
#
#   make install PREFIX=dir    install below dir/usr/spool/queuer (config mode 664, logfile mode 640)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/spool/queuer
	cp config $(PREFIX)/usr/spool/queuer/config ; chmod 664 $(PREFIX)/usr/spool/queuer/config
	cp logfile $(PREFIX)/usr/spool/queuer/logfile ; chmod 640 $(PREFIX)/usr/spool/queuer/logfile

clean:

.PHONY: all install clean
