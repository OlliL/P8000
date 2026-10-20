static char sccsid[] = "@[$]stable.c\t1.1 03/24/83 15:49:32 - 87wega3.2";

/*
 * stable - the symbol table of the assembler
 *
 * The table lives in a segment of its own (the linker places this module there, see the
 * Makefile); ssymtab.c allocates from it at the offsets from 0x30 on and grows the segment
 * with ssgbrk. Only the room is needed here: 1024 bytes after the id, which makes 1280
 * bytes with the click.
 */
char stable[1024] = {0};
