/*
 * sobj - helpers for the s.out object format and the archive format as the
 * WEGA tools (ranlib, strip, size, ...) see them. The structures of the files
 * are kept as byte arrays in the Z8000 layout (big endian, no padding) and
 * read with the accessors below, so they work on any host.
 *
 * s_exec, 24 bytes:  magic 0 (16 bit), imsize 2 (32), bss 6 (32), segt 10 (16),
 *                    syms 12 (16), entry 14 (32), flag 18, codesz 20, lines 22 (16)
 * segt entry, 16 bytes: segno 0, coff 1, doff 2, boff 3 (bytes), code 4, data 6, bss 8,
 *                    atr 10 (16 bit), unused 12 (32)
 * s_nlist, 14 bytes: value 0 (32), type 4, segt 5 (bytes), name 6 (8 bytes)
 * ar_hdr, 26 bytes: name 0 (14), date 14 (32), uid 18, gid 19 (bytes), mode 20 (16), size 22 (32)
 */
#ifndef SOBJ_H
#define SOBJ_H

#include <stdint.h>

#define SEXEC_SIZE	24
#define SEGT_SIZE	16
#define NLIST_SIZE	14
#define ARHDR_SIZE	26

#define S_MAGIC1	0xE607
#define S_MAGIC3	0xE611
#define S_MAGIC4	0xE605
#define N_MAGIC1	0xE707
#define N_MAGIC3	0xE711
#define N_MAGIC4	0xE705
#define X_MAGIC1	0xE507
#define X_MAGIC3	0xE511
#define X_MAGIC4	0xE505
#define ARMAG		0xFF65		/* 0177545 */

#define SF_STRIP	0x0001
#define N_TYPE		0x1F
#define N_EXT		0x20
#define SHORTNAME	8

unsigned sobj_be16(const unsigned char *p);
uint32_t sobj_be32(const unsigned char *p);
void sobj_put16(unsigned char *p, unsigned v);
void sobj_put32(unsigned char *p, uint32_t v);

/*
 * libc goodmagic/swab/longswap. goodmagic returns 0 or the index of the magic
 * (1..9) and sets sobj_swap_flg when the bytes of the magic are swapped (the
 * object comes from a machine with the other byte order within the words).
 */
extern int sobj_swap_flg;
int sobj_goodmagic(unsigned magic);
void sobj_swab(unsigned char *p, int n);		/* swap the bytes of each 16 bit word, in place */
void sobj_longswap(unsigned char *p);			/* swab the two words of a long */

#endif
