// arhdr_slibu8000.s: the magic and the member headers of the shipped slibu8000, in the order of the archive (ar(5): name 14,
// date 4, uid, gid, mode 2, size 4 bytes); mkar.sh joins them with the members. Written by host/recon/libsetup.py.
	.psec
	.data
_arhdr::
	.byte	%ff,%65
// disasm.o
	.byte	%64,%69,%73,%61,%73,%6d,%2e,%6f,%00,%00,%00,%00,%00
	.byte	%00,%18,%99,%b4,%df,%3f,%00,%81,%b4,%00,%00,%90,%a6
// Library.SC
	.byte	%4c,%69,%62,%72,%61,%72,%79,%2e,%53,%43,%00,%00,%00
	.byte	%00,%18,%99,%b4,%f1,%3f,%00,%81,%b4,%00,%00,%00,%34
