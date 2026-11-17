: '@[$]nulladm.sh	1.1  01/04/83 09:17:51 - 87wega3.2'
:	"nulladm name..."
:	"creates each named file mode 664"
:	"make sure owned by adm (in case created by root)"
for i
do
	cp /dev/null $i
	/bin/chmod 664 $i
done
