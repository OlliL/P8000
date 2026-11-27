/*
 * comm3.c - errors, the tree pool, tree walks, small helpers, memory
 */
#include "mfile3"

extern NODE *lastfree;
static char sccsid[] __attribute__((unused)) = "@[$]comm3.c\t2.1  11/15/82 12:48:27 Copr. 1981 87wega3.2.";
char unused6[16] = { 0 };
char ftitle[256] = { 0 };	/* 16 in the original: a longer name overwrote the version string behind it (harmless there, but nerrors on macOS) */
#include "../mip/common.2.5"
