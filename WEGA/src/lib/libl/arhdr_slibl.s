// arhdr_slibl.s: the magic and the member headers of the shipped /lib/slibl.a, in the order of the archive (ar(5): name 14, date 4,
// uid, gid, mode 2, size 4 bytes), and the padding byte of the original when the last member has an odd size; mkar.sh joins them
// with the members.
	.psec
	.data
_arhdr::
	.byte	%ff,%65
// allprint.s.o
	.byte	%61,%6c,%6c,%70,%72,%69,%6e,%74,%2e,%73,%2e,%6f,%00
	.byte	%00,%18,%aa,%20,%1e,%00,%00,%81,%a0,%00,%00,%03,%1a
// main.s.o
	.byte	%6d,%61,%69,%6e,%2e,%73,%2e,%6f,%00,%00,%00,%00,%00
	.byte	%00,%18,%aa,%20,%4f,%00,%00,%81,%a0,%00,%00,%00,%82
// reject.s.o
	.byte	%72,%65,%6a,%65,%63,%74,%2e,%73,%2e,%6f,%00,%00,%00
	.byte	%00,%18,%aa,%20,%89,%00,%00,%81,%a0,%00,%00,%04,%5a
// yyless.s.o
	.byte	%79,%79,%6c,%65,%73,%73,%2e,%73,%2e,%6f,%00,%00,%00
	.byte	%00,%18,%aa,%20,%ac,%00,%00,%81,%a0,%00,%00,%01,%9e
// yywrap.s.o
	.byte	%79,%79,%77,%72,%61,%70,%2e,%73,%2e,%6f,%00,%00,%00
	.byte	%00,%18,%aa,%20,%cb,%00,%00,%81,%a0,%00,%00,%00,%56
// Library.SC
	.byte	%4c,%69,%62,%72,%61,%72,%79,%2e,%53,%43,%00,%00,%00
	.byte	%00,%18,%aa,%20,%d0,%00,%00,%81,%a0,%00,%00,%01,%04
