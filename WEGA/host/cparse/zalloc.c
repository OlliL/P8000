/*
 * zalloc.c - the storage allocator of the WEGA libc (src/lib/libc/gen/malloc.c: circular first-fit, a freed block is searched first by the next
 * malloc), run on a model of its heap, so that the host gets the same block when the original reuses one.
 *
 * Why: cparse frees the symbols of an inner scope (clearst) and the entries of dimtab keep pointing to a structure tag that was freed. The
 * original reads it afterwards, and the next malloc (a new symbol) has taken its block by then: the output depends on which block that is.
 * The model holds the links of the blocks (in words of the Z8000: 2 bytes, 4 in the segmented libc) and nothing else. The data of a block is a host object
 * that stays where it is; when the model gives the same block again for the same kind of object (a symbol for a symbol), the host object is
 * given again with its old contents, as the original does.
 */
#include <stdint.h>
#include <stdlib.h>
#include "mfile1"

#ifdef SEG
#define WORD 4		/* sizeof(union store) of the segmented libc (malloc.o of slibc.a: nw = (nbytes + 7) / 4) */
#else
#define WORD 2
#endif
#define BLOCK 1024
#define MAXBYTES 0xFFF8u
#define MAXBLOCKS 64

/* the heap of the model: unit i is the word at the address i * WORD; a link is (index << 1) | busy */
static uint32_t *mem;
static size_t memunits;
static int inited;
static unsigned allocp, alloct;
static int grown;
static unsigned model_brk = 1000;		/* the break, in units: the arena starts behind the static data */

static void need(size_t units)
{
	if (units > memunits) {
		size_t n = units + 4096;

		mem = realloc(mem, n * sizeof *mem);
		if (mem == 0)
			abort();
		for (size_t i = memunits; i < n; i++)
			mem[i] = 0;
		memunits = n;
	}
}

#define testbusy(v) ((v) & 1u)
#define setbusy(i) ((uint32_t) (i) << 1 | 1u)
#define clearbusy(v) ((unsigned) ((v) >> 1))

/* malloc of the libc: the index of the first unit of the data, or 0 */
static unsigned model_malloc0(unsigned long nbytes)
{
	unsigned p, q, last;
	unsigned long nw;
	long temp;
	int first;

	first = 0;
	if (nbytes > MAXBYTES)
		return 0;
	if (!inited || grown == 0)
		first = 1;
	if (!inited) {
		inited = 1;
		need(2);
		allocp = 0;
		alloct = 1;
		mem[0] = setbusy(1);
		mem[1] = setbusy(0);
	}
	nw = (nbytes + WORD + WORD - 1) / WORD;
	p = allocp;
	q = clearbusy(mem[p]);
	last = alloct;
	do {
		if (!testbusy(mem[p])) {
			while (!testbusy(mem[q])) {	/* coalesce the idle blocks behind p */
				unsigned nq = clearbusy(mem[q]);

				if (q == allocp)
					allocp = nq;
				mem[p] = (uint32_t) nq << 1;
				q = nq;
			}
			if (q >= p + nw)
				goto found;
			if (q == alloct)
				last = p;
		}
		p = q;
		q = clearbusy(mem[p]);
	} while (p != allocp);
	temp = ((long) (uint16_t) (nw - (alloct - last) + first) * WORD + BLOCK - 1) / BLOCK;	/* nw is an unsigned word on the Z8000 */
	if (temp > MAXBLOCKS)
		return 0;
	nbytes = temp * BLOCK;
	p = model_brk;
	model_brk += nbytes / WORD;
	need(model_brk + 2);
	if (first) {
		mem[alloct] = setbusy(p);
		q = p + temp * (BLOCK / WORD) - 1;
		grown = 1;
	} else {
		p = last;
		q = alloct + temp * (BLOCK / WORD) - 1;
	}
	mem[q] = setbusy(0);
	alloct = q;
found:
	allocp = p + nw;
	if (q > allocp)
		mem[allocp] = (uint32_t) q << 1;
	mem[p] = setbusy(allocp);
	return p + 1;
}

static unsigned model_malloc(unsigned long nbytes)
{
#ifdef SEG
	static int done;

	/* The segmented program has taken some storage before its first symbol: found by trying (40 to 80 bytes give the output of the
	   original, 64 is the middle; the three fdopen of openfiles may be it). Without it the blocks are another ones. */
	if (!done) {
		done = 1;
		model_malloc0(64);
	}
#endif
	return model_malloc0(nbytes);
}

static void model_free(unsigned data)
{
	unsigned p = data - 1;

	allocp = p;
	mem[p] &= ~1u;
}

/* the host objects of the blocks */
struct zh {
	unsigned idx;
	int kind;
	size_t size;
	size_t pad;
};
static struct zh **obj;
static size_t nobj;

void *zalloc(size_t modelsize, size_t hostsize, int kind)
{
	unsigned idx = model_malloc(modelsize);
	struct zh *h;

	if (idx == 0)
		return 0;
	if (idx >= nobj) {
		size_t n = idx + 1024;

		obj = realloc(obj, n * sizeof *obj);
		if (obj == 0)
			abort();
		for (size_t i = nobj; i < n; i++)
			obj[i] = 0;
		nobj = n;
	}
	h = obj[idx];
	if (h == 0 || h->kind != kind || h->size != hostsize) {
		h = calloc(1, sizeof *h + hostsize);
		if (h == 0)
			return 0;
		h->idx = idx;
		h->kind = kind;
		h->size = hostsize;
		obj[idx] = h;
	}
	return h + 1;
}

void zfree(void *p)
{
	model_free(((struct zh *) p - 1)->idx);
}
