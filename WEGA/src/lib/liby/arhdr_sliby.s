// arhdr_sliby.s: the magic and the member headers of the shipped sliby, in the order of the archive (ar(5): name 14,
// date 4, uid, gid, mode 2, size 4 bytes); mkar.sh joins them with the members. Written by host/recon/libsetup.py.
	.psec
	.data
_arhdr::
	.byte	%ff,%65
// main.s.o
	.byte	%6d,%61,%69,%6e,%2e,%73,%2e,%6f,%00,%00,%00,%00,%00
	.byte	%00,%18,%aa,%0d,%53,%00,%00,%81,%a0,%00,%00,%00,%6c
// yyerror.s.o
	.byte	%79,%79,%65,%72,%72,%6f,%72,%2e,%73,%2e,%6f,%00,%00
	.byte	%00,%18,%aa,%0d,%6d,%00,%00,%81,%a0,%00,%00,%00,%b8
// Library.SC
	.byte	%4c,%69,%62,%72,%61,%72,%79,%2e,%53,%43,%00,%00,%00
	.byte	%00,%18,%aa,%0d,%75,%00,%00,%81,%a0,%00,%00,%00,%67
// padding
	.byte	%28
