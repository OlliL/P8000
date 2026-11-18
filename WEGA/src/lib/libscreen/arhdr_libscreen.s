// arhdr_libscreen.s: the magic and the member headers of the shipped libscreen, in the order of the archive (ar(5): name 14,
// date 4, uid, gid, mode 2, size 4 bytes); mkar.sh joins them with the members. Written by host/recon/libsetup.py.
	.psec
	.data
_arhdr::
	.byte	%ff,%65
// io.o
	.byte	%69,%6f,%2e,%6f,%00,%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%18,%9b,%05,%7a,%3f,%00,%81,%b4,%00,%00,%03,%98
// mesg.o
	.byte	%6d,%65,%73,%67,%2e,%6f,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%18,%9b,%05,%9b,%3f,%00,%81,%b4,%00,%00,%01,%c2
// mvcursor.o
	.byte	%6d,%76,%63,%75,%72,%73,%6f,%72,%2e,%6f,%00,%00,%00
	.byte	%00,%18,%9b,%05,%e9,%3f,%00,%81,%b4,%00,%00,%0b,%36
// scrn.o
	.byte	%73,%63,%72,%6e,%2e,%6f,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%18,%9b,%06,%44,%3f,%00,%81,%b4,%00,%00,%0d,%76
// Library.SC
	.byte	%4c,%69,%62,%72,%61,%72,%79,%2e,%53,%43,%00,%00,%00
	.byte	%00,%18,%9b,%06,%49,%3f,%00,%81,%b4,%00,%00,%00,%ca
