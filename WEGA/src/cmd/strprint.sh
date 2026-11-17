# "@[$]strprint	1.4  09/01/82 11:04:36 - 87wega3.2"
# Modification made to print str's in numerical order
set sortlist = `ls *.prob* | sort -n`
unset noclobber
cd /usr/lib/problems
	foreach i ($sortlist)
		pr $i >> str$$
	end
lpr str$$
rm str$$
