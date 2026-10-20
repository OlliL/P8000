static char sccsid[] = "@[$]ntable.c\t1.1 03/24/83 11:52:32 - 87wega3.2";

/*
 * ntable - the name table of the assembler
 *
 * The table lives in a segment of its own (the linker places this module there, see the
 * Makefile); ssymtab.c allocates from it at the offsets from 0x30 on and grows the segment
 * with ssgbrk. Only the room is needed here: 1024 bytes after the id, which makes 1280
 * bytes with the click.
 */
char ntable[1024] = {0};
