/*
** shared header file for relation backup
**	 extractr and insertr
*/
# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"../h/catalog.h"
# include	"../h/access.h"
# include	"../h/lock.h"

/*
** DUMP HEADER
*/
struct d_hdr
{
	struct adminhdr	d_admin;		/* DBA and DB-status	*/
	char		d_owner[2];		/* DUMP-owner		*/
	short		d_status;		/* DUMP-stat, see below	*/
	long		d_date;			/* DUMP-date		*/
	char		d_db[MAXNAME + 2];	/* DUMP from db		*/
	short		d_rels;			/* # of dumped rels	*/
};

/* The following assignments are status bits for d_hdr.d_status		*/
# define	X_32		00001
# define	X_WORDS		00002
# define	X_BYTES		00004
# ifdef BIT_32
# define	D_32		X_32		/* 32-bit computer	*/
# else
# define	D_32		00000		/* 16-bit computer	*/
# endif
# ifdef REV_WORDS
# define	D_WORDS		X_WORDS		/* rev. words computer	*/
# else
# define	D_WORDS		00000		/* normal computer	*/
# endif
# ifdef REV_BYTES
# define	D_BYTES		X_BYTES		/* rev. bytes computer	*/
# else
# define	D_BYTES		00000		/* normal computer	*/
# endif
# define	D_MACHINE	00007		/* computer mask	*/
# define	D_QRYMOD	00010		/* no QRYMOD		*/

/*
** REALTION HEADER
*/
struct d_rel
{
	char		d_relid[MAXNAME + 2];	/* relation name	*/
	short		d_ntups;		/* # of catalog tups	*/
	long		d_tupoff;		/* off for catalog tups	*/
	long		d_npages;		/* # of data pages	*/
	long		d_pageoff;		/* off for data pages	*/
};

/*
** CATALOG TUPLE
*/
union d_tup
{
	struct relation		d_relation;
	struct attribute	d_attribute;
	struct index		d_index;
	struct protect		d_protect;
	struct integrity	d_integrity;
	struct tree		d_tree;
};

/* global declarations */
extern struct d_hdr	Hdr;			/* dump header		*/
extern struct d_rel	Rel;			/* relation header	*/
extern union d_tup	Tup;			/* catalog tuple	*/
extern int		Fh;			/* fd for header	*/
extern int		Fp;			/* fd for tups & pages	*/
extern int		No_qm;			/* no QRYMOD-flag	*/
extern int		D_flag;			/* destroy relation	*/

# define	D_REL		0		/*	AA_REL		*/
# define	D_ATT		1		/*	AA_ATT		*/
# define	D_INDEX		2		/*	AA_INDEX	*/
# define	D_NOQM		3
# define	D_INTEG		3		/*	AA_INTEG	*/
# define	D_PROTECT	4		/*	AA_PROTECT	*/
# define	D_TREE		5		/*	AA_TREE		*/
# define	D_CATS		6
extern DESC		*Cdesc[D_CATS];		/* catalog descriptors	*/
extern char		*Catalog[D_CATS];	/* name of catalogs	*/
extern short		Clen[D_CATS];		/* len of catalog tup	*/

/* data base declarations */
extern char		*Parmvect[];
extern char		*Flagvect[];
extern struct admin	AAdmin;
extern char		*AAdbpath;
