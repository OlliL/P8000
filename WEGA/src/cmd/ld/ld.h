/*
 * ld.h - declarations shared by the C files of ld (structs, variables). The variables are defined in ld.c.
 * Names follow sld's ld.h where the use is the same; the others keep their address names (v<addr>) until
 * their use is known.
 */

/* header of a page of the input buffer (sld: page); nuser counts the streams that use it */
struct pghdr {
	int nuser;
	int bno;
};
/* input stream: the words are read from the page pno (ptr at 0: get and mget use it, layout proven by the code) */
struct stream {
	int *ptr;
	int bno;
	int nibuf;
	int size;
	struct pghdr *pno;
};
/* input buffering (sld: page) */
struct page {
	int nuser;
	int bno;
	int nibuf;
	int buff[256];
};
/* tab entry of the object list (t0..t10: the fields are not named yet) */
struct tent {
	int t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10;
};
/* output or input buffer with the byte count left (sld: struct buf) */
struct buf {
	int fildes;
	int nleft;
	char *xnext;
	char iobuf[256];
};
/* archive member header (sld: archdr) */
struct archdr {
	char aname[14];
	long atime;
	char auid;
	char agid;
	int amode;
	long asize;
};
/* local symbol table entry: index -> value, 4 bytes (sld: struct local with a symbol pointer, 6 bytes) */
struct local {
	int locindex;
	int locsymbol;
};

extern struct page page[2];
extern struct pghdr fpage;
extern int infil;
extern int ntab;
extern struct tent tab[1];
extern int symindex, v0070;
extern int v66c2, v66c6, v66cc, v66ce, v66d0, v66d2, v66d4;
extern char *ofilnam;
extern int v007c, v0080;
extern char tmpname[];
extern int v669a, v66a4;
extern struct stream text;
extern struct buf toutb;
extern unsigned char v67e4[256];
extern struct archdr archdr;
extern char *pname;
extern char *filname;
extern int exstat;
extern struct local local[1];
/* symbol table: functions that return a symbol pointer (sld: table16_entry, next_entry) */
struct symbol *table16_entry();
struct symbol *next_entry();
/* current symbol (sld: cursym). The layout is the one of ld's code: svalue at 0x1076, stype at 4, sname at 6 (the
   address of pack_name's target), 14 bytes; sld's struct symbol has sseg and a 16 byte entry. */
struct symbol {
	long svalue;
	char stype;
	char sxtra;
	char sname[8];
};
extern struct symbol cursym;
/* hash table of the symbols (sld: hshtab, indexed from 2; ld hashes with 0x0923), the name buffer of the
   current symbol (sld: lnbuf) and the flag of the -N option (sld: nlflag) */
extern struct symbol *hshtab[];
extern char lnbuf[];
extern int nlflag;
struct symbol **lookup_name();
/* symbol table (sld: symtab, nsym); entries are 14 bytes in ld */
extern struct symbol *symtab;
extern int nsym;
/* flag of -w (sld: wflag); put_name writes a symbol name and returns its length (copyname.c / printname.c) */
extern int wflag;
int put_name();
