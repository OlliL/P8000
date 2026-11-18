// arhdr_slibtermlib.s: the magic and the member headers of the shipped slibtermlib, in the order of the archive (ar(5): name 14,
// date 4, uid, gid, mode 2, size 4 bytes); mkar.sh joins them with the members. Written by host/recon/libsetup.py.
	.psec
	.data
_arhdr::
	.byte	%ff,%65
// termcap.o
	.byte	%74,%65,%72,%6d,%63,%61,%70,%2e,%6f,%00,%00,%00,%00
	.byte	%00,%18,%91,%d3,%0a,%50,%00,%81,%b4,%00,%00,%0d,%90
// tgoto.o
	.byte	%74,%67,%6f,%74,%6f,%2e,%6f,%00,%00,%00,%00,%00,%00
	.byte	%00,%18,%91,%d3,%36,%50,%00,%81,%b4,%00,%00,%05,%5c
// tputs.o
	.byte	%74,%70,%75,%74,%73,%2e,%6f,%00,%00,%00,%00,%00,%00
	.byte	%00,%18,%91,%d3,%62,%50,%00,%81,%b4,%00,%00,%02,%ee
// Library.SC
	.byte	%4c,%69,%62,%72,%61,%72,%79,%2e,%53,%43,%00,%00,%00
	.byte	%00,%18,%91,%d3,%68,%50,%00,%81,%b4,%00,%00,%00,%9b
// padding
	.byte	%00
