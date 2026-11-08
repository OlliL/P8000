#
# "@[$]halt	1.4  07/23/82 20:19:53 - 87wega3.2"

	onintr fixer

	/bin/echo -n  "HALT PROCEDURE STARTED AT " ; date

	wall << 'EOF'
	System is coming down.  Thirty seconds to forced log-off.
'EOF'

	echo "Current logged in users:"
	who

	sleep 30

# restore motd

	if ( -r /etc/motd.save ) then
		cp /etc/motd.save /etc/motd
		rm -f /etc/motd.save
	endif

	onintr

# update the superblocks of all filesystems
	sync
	sync

	echo "HALT PROCEDURE COMPLETED AT " ; date
	echo Now terminating all processes

# now, kill everyone
	kill -1 1
	exit 0

fixer:
	if ( -r /etc/motd.save ) then
		cp /etc/motd.save /etc/motd
		rm -f /etc/motd.save
	endif
