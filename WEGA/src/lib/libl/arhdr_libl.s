// arhdr_libl.s: the magic and the member headers of the shipped /lib/libl.a, in the order of the archive (ar(5): name 14, date 4,
// uid, gid, mode 2, size 4 bytes), and the padding byte of the original when the last member has an odd size; mkar.sh joins them
// with the members.
	.psec
	.data
_arhdr::
	.byte	%ff,%65
// allprint.o
	.byte	%61,%6c,%6c,%70,%72,%69,%6e,%74,%2e,%6f,%00,%00,%00
	.byte	%00,%18,%aa,%22,%c6,%00,%00,%81,%a0,%00,%00,%02,%98
// main.o
	.byte	%6d,%61,%69,%6e,%2e,%6f,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%18,%aa,%22,%e0,%00,%00,%81,%a0,%00,%00,%00,%6a
// reject.o
	.byte	%72,%65,%6a,%65,%63,%74,%2e,%6f,%00,%00,%00,%00,%00
	.byte	%00,%18,%aa,%22,%fa,%00,%00,%81,%a0,%00,%00,%03,%70
// yyless.o
	.byte	%79,%79,%6c,%65,%73,%73,%2e,%6f,%00,%00,%00,%00,%00
	.byte	%00,%18,%aa,%23,%0e,%00,%00,%81,%a0,%00,%00,%01,%40
// yywrap.o
	.byte	%79,%79,%77,%72,%61,%70,%2e,%6f,%00,%00,%00,%00,%00
	.byte	%00,%18,%aa,%23,%1d,%00,%00,%81,%a0,%00,%00,%00,%46
// Library.SC
	.byte	%4c,%69,%62,%72,%61,%72,%79,%2e,%53,%43,%00,%00,%00
	.byte	%00,%18,%aa,%23,%1f,%00,%00,%81,%a0,%00,%00,%01,%04
