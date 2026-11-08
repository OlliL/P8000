#
# "@[$]down	1.6  07/23/82 20:10:01 - 87wega3.2"
# shell variables used are
#	$* -  passed directly to shut
# executes shut, which sends out the messages

	/bin/echo "DOWN PROCEDURE"
	echo

# if motd.save does not exist, save current motd and make new motd with
# the funny message appended to the current one

	onintr fixer

	if ( -r /etc/motd.save ) rm -f /etc/motd.save

	if ( -r /etc/motd ) cp /etc/motd /etc/motd.save

	echo '*** THE SYSTEM IS GOING DOWN SOON ***' >> /etc/motd

	if ( $#argv < 2 ) then
		shut
		if ( $status == 1 ) exit 1
	else
		shut $*
		if ( $status == 1 ) exit 1
	endif

# now go do halt never to return

	halt
	exit 0

fixer:
	if ( -r /etc/motd.save ) then
		cp /etc/motd.save /etc/motd
		rm -f /etc/motd.save
	endif
