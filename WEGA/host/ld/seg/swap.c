/*
 * swap.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include "ld.h"

static char sccsid[] __attribute__((unused)) = "@[$]swap.c\t2.1  10/27/83 12:25:00 - 87wega3.2";

/* swap_bytes: swap the bytes of a word (0x65c6..0x65ec of /bin/sld) */
int16_t swap_bytes(int16_t x)
{
	return swap(x);
}

/* swap_pair: swap the bytes of the two words at a (the memory image) (0x65ec..0x6620 of /bin/sld) */
void swap_pair(void *a)
{
	swap_words((uint8_t *)a, 2);
}

/* swap_quad: swap the bytes of the four words behind the first two of the segment record at a (the memory image) (0x6620..0x6684 of /bin/sld) */
void swap_quad(void *a)
{
	swap_words((uint8_t *)a + 4, 4);
}
