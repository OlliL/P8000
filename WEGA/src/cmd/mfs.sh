: "@[$]mfs	2.4  03/15/83 08:30:28 - 87wega3.2"

	case $0 in
		/etc/mfs | mfs)
			/bin/echo 'mounting:'
			/etc/mount /dev/z /z
			/etc/mount /dev/tmp /tmp
			/etc/mount /dev/usr /usr
			/etc/mount ;;

		/etc/umfs | umfs)
			/bin/echo 'un-mounting:'
			/etc/mount | grep -v root
			/etc/umount /dev/z
			/etc/umount /dev/tmp
			/etc/umount /dev/usr
			exit 0 ;;

		*)
			/bin/echo "$0: Unknown command" ;;
	esac
