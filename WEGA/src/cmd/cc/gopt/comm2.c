/*
 * comm2.c - gopt: errors, the tree pool, tree walks, small helpers (the pcc file "common", SCCS 2.4, included in the
 * original by comm2.c 1.1 07/07/82 of the optimizer). Written from /lib/gopt (no source known); the relative is
 * ../codgen/comm3.c (the same file one version later: 2.5 08/24/83).
 */
static char sccsid[] = "@[$]comm2.c\t1.1  07/07/82 15:54:49 Copr. 1981 87wega3.2.";
#include "mfile2.h"

extern NODE *lastfree;
extern char *mkseg(), *ssgbrk(), *sgbrk();
#include "../mip/common.2.4"
