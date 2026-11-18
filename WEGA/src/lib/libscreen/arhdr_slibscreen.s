// arhdr_slibscreen.s: the magic and the member headers of the shipped slibscreen, in the order of the archive (ar(5): name 14,
// date 4, uid, gid, mode 2, size 4 bytes); mkar.sh joins them with the members. Written by host/recon/libsetup.py.
	.psec
	.data
_arhdr::
	.byte	%ff,%65
// io.o
	.byte	%69,%6f,%2e,%6f,%00,%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%18,%9b,%06,%7e,%3f,%00,%81,%b4,%00,%00,%04,%5e
// mesg.o
	.byte	%6d,%65,%73,%67,%2e,%6f,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%18,%9b,%06,%a2,%3f,%00,%81,%b4,%00,%00,%02,%10
// mvcursor.o
	.byte	%6d,%76,%63,%75,%72,%73,%6f,%72,%2e,%6f,%00,%00,%00
	.byte	%00,%18,%9b,%06,%fb,%3f,%00,%81,%b4,%00,%00,%0c,%08
// scrn.o
	.byte	%73,%63,%72,%6e,%2e,%6f,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%18,%9b,%07,%4a,%3f,%00,%81,%b4,%00,%00,%0f,%14
// Library.SC
	.byte	%4c,%69,%62,%72,%61,%72,%79,%2e,%53,%43,%00,%00,%00
	.byte	%00,%18,%9b,%07,%4e,%3f,%00,%81,%b4,%00,%00,%00,%ca
