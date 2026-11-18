// arhdr_libtermlib.s: the magic and the member headers of the shipped libtermlib, in the order of the archive (ar(5): name 14,
// date 4, uid, gid, mode 2, size 4 bytes); mkar.sh joins them with the members. Written by host/recon/libsetup.py.
	.psec
	.data
_arhdr::
	.byte	%ff,%65
// termcap.o
	.byte	%74,%65,%72,%6d,%63,%61,%70,%2e,%6f,%00,%00,%00,%00
	.byte	%00,%18,%91,%d2,%00,%50,%00,%81,%b4,%00,%00,%0b,%9a
// tgoto.o
	.byte	%74,%67,%6f,%74,%6f,%2e,%6f,%00,%00,%00,%00,%00,%00
	.byte	%00,%18,%91,%d2,%48,%50,%00,%81,%b4,%00,%00,%04,%b2
// tputs.o
	.byte	%74,%70,%75,%74,%73,%2e,%6f,%00,%00,%00,%00,%00,%00
	.byte	%00,%18,%91,%d2,%7c,%50,%00,%81,%b4,%00,%00,%02,%a0
// Library.SC
	.byte	%4c,%69,%62,%72,%61,%72,%79,%2e,%53,%43,%00,%00,%00
	.byte	%00,%18,%91,%d2,%82,%50,%00,%81,%b4,%00,%00,%00,%9b
// padding
	.byte	%74
