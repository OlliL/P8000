# etc: the files of /etc that no program owns (configuration of the system: passwd, group, inittab, rc, profile, ...), as they
# are in the WEGA root (mnttab comes from the installation floppies and is kept as it is there). The commands of /etc are in ../cmd (scripts as NAME.sh, as in System III).
#
#   make install PREFIX=dir    copy them below dir/etc with the modes of the originals

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/etc/default
	cp checklist $(PREFIX)/etc/checklist ; chmod 644 $(PREFIX)/etc/checklist
	cp cshprofile $(PREFIX)/etc/cshprofile ; chmod 644 $(PREFIX)/etc/cshprofile
	cp default/dos $(PREFIX)/etc/default/dos ; chmod 644 $(PREFIX)/etc/default/dos
	cp group $(PREFIX)/etc/group ; chmod 644 $(PREFIX)/etc/group
	cp inittab $(PREFIX)/etc/inittab ; chmod 664 $(PREFIX)/etc/inittab
	cp motd $(PREFIX)/etc/motd ; chmod 664 $(PREFIX)/etc/motd
	cp passwd $(PREFIX)/etc/passwd ; chmod 644 $(PREFIX)/etc/passwd
	cp profile $(PREFIX)/etc/profile ; chmod 644 $(PREFIX)/etc/profile
	cp rc $(PREFIX)/etc/rc ; chmod 744 $(PREFIX)/etc/rc
	cp rc_csh $(PREFIX)/etc/rc_csh ; chmod 744 $(PREFIX)/etc/rc_csh
	cp termcap $(PREFIX)/etc/termcap ; chmod 644 $(PREFIX)/etc/termcap
	cp ttytype $(PREFIX)/etc/ttytype ; chmod 644 $(PREFIX)/etc/ttytype
	cp whois $(PREFIX)/etc/whois ; chmod 666 $(PREFIX)/etc/whois
	cp mnttab $(PREFIX)/etc/mnttab ; chmod 666 $(PREFIX)/etc/mnttab

clean:

.PHONY: all install clean
