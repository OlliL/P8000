/*
**	STRUCTURES USED IN THE NODES OF THE QUERYTREE
**
**
**	SUNDRY CONSTANTS
**
**		There are several differences in the handling of data
**		structures on 16 and 32 bit machines:
**			1).  A pointer to  memory is either 2 or 4 bytes.
**			2).  Padding is inserted in structures to insure
**				alignment of 16 and 32 bit numbers.
**
**		For these reasons the following constant definitions
**		are useful for machine independent allocation.
**
**		These are based on the BIT_32 compile flag.
**
**		QT_HDR_SIZ -- size of left and right pointers, typ,
**			len and padding
**		QT_PTR_SIZ -- size of left and right pointers
**		SYM_HDR_SIZ -- size of type and len in symbol
**			structure -- includes any padding before
**			the value field
**		TYP_LEN_SIZ -- size of type and len in symbol
**			structure -- without padding
**
**		DBS FOLKS: don't change these back to sizeof's !!!
**			      The PDP-11 compiler doesn't understand!
*/


/*	Basic symbol structure. "Type" is one of the symbols		*/
/*	in "symbol.h", "len" is the length of the "value"		*/
/*	field (0 to MAXFIELD bytes), "value" is variable length and	*/
/*	holds the actual value (if len != 0) of the node.		*/
struct symbol
{
	char	type;		/* type codes in symbol.h		*/
	char	len;		/* length in bytes of value field	*/
# ifdef BIT_32
	short	aligned;	/* align value to 32-bit offset		*/
# endif
	short	value[1];	/* variable length (0 - MAXFIELD bytes)	*/
};
typedef struct symbol		SYMBOL;

# ifdef BIT_32
# define	TYP_LEN_SIZ	2
# define	SYM_HDR_SIZ	4
# else
# define	TYP_LEN_SIZ	2
# define	SYM_HDR_SIZ	2
# endif

/*	Basic node in the querytree. Each node has a left and		*/
/*	right descendent. If the node is a leaf node then the		*/
/*	left and right pointers will be NULL. Depending on the		*/
/*	"type" field of the symbol structure, there may be additional	*/
/*	information. The cases are itemized below.			*/
struct querytree
{
	struct querytree	*left;
	struct querytree	*right;
	struct symbol		sym;
};
typedef struct querytree	QTREE;

# ifdef BIT_32
# define	QT_PTR_SIZ	8
# define	QT_HDR_SIZ	12
# else
# define	QT_PTR_SIZ	4
# define	QT_HDR_SIZ	6
# endif

/*	Variable nodes of the form:					*/
/*		varno.attno						*/
/*	"Varno" is the variable number which can be translated		*/
/*	to a relation name by indexing into the range table.		*/
/*	"Attno" is the attribute number specifically the		*/
/*	"attid" field in the attribute tuple for the attribute.		*/
/*	"Frmt" and "frml" are the type and length of the attribute.	*/
/*	"Valptr" is used only in decomposition. If the variable		*/
/*	is tuple substituted then "valptr" will have a pointer to	*/
/*	the substituted value for the attribute; otherwise it will	*/
/*	be NULL. If reduction or one variable restriction has changed	*/
/*	the attribute number, then "valptr" will point to the next	*/
/*	active VAR node for this VAR.					*/
struct var_node
{				char	qt_hdr[QT_HDR_SIZ];
/*	QTREE	*left;							*/
/*	QTREE	*right;							*/
/*	char	type;							*/
/*	char	len;							*/
# ifdef BIT_32
/*	short	aligned;						*/
# endif
	char	varno;
	char	attno;
	char	frmt;
	char	frml;
	char	*valptr;
};
typedef struct var_node		VAR_NODE;

# ifdef BIT_32
# define			VAR_SIZ		8
# else
# define			VAR_SIZ		6
# endif
# define			VAR_HDR_SIZ	4

/*	Structure for ROOT, AGHEAD, and AND nodes. In the parser and	*/
/*	qrymod these nodes are of length zero and none of this		*/
/*	information is present. Decomp maintains information about	*/
/*	the variables in the left and right descendents of the nodes.	*/
/*	The "rootuser" flag is present only in ROOT and AGHEAD nodes.	*/
/*	It is TRUE only in the original ROOT node of the query.		*/
struct root_node
{				char	qt_hdr[QT_HDR_SIZ];
/*	QTREE	*left;							*/
/*	QTREE	*right;							*/
/*	char	type;							*/
/*	char	len;							*/
# ifdef BIT_32
/*	short	aligned;						*/
# endif
	char	tvarc;		/* total of var's in sub-tree		*/
	char	lvarc;		/* # of variables in left branch	*/
	short	lvarm;		/* bit map of var's in left branch	*/
	short	rvarm;		/* bit map of var's in right branc	*/
	short	rootuser;	/* TRUE if root of user generated query	*/
};
typedef struct root_node	ROOT_NODE;

# define			ROOT_SIZ	8
# define			AND_SIZ		6

/* RESDOM node */
struct res_node
{				char	qt_hdr[QT_HDR_SIZ];
/*	QTREE	*left;							*/
/*	QTREE	*right;							*/
/*	char	type;							*/
/*	char	len;							*/
# ifdef BIT_32
/*	short	aligned;						*/
# endif
	short	resno;		/* result domain number			*/
/*	char	frmt;							*/
/*	char	frml;							*/
};
typedef struct res_node		RES_NODE;

# define			RES_SIZ		4

/* AOP node */
struct aop_node
{				char	qt_hdr[QT_HDR_SIZ];
/*	QTREE	*left;							*/
/*	QTREE	*right;							*/
/*	char	type;							*/
/*	char	len;							*/
# ifdef BIT_32
/*	short	aligned;						*/
# endif
				char	var_hdr[VAR_HDR_SIZ];
/*	char	varno;							*/
/*	char	attno;							*/
/*	char	frmt;							*/
/*	char	frml;							*/
	char	agfrmt;		/* result format type			*/
	char	agfrml;		/* and length for aggregates		*/
};
typedef struct aop_node		AOP_NODE;

# define			AOP_SIZ		6

/* OP node */
struct op_node
{				char	qt_hdr[QT_HDR_SIZ];
/*	QTREE	*left;							*/
/*	QTREE	*right;							*/
/*	char	type;							*/
/*	char	len;							*/
# ifdef BIT_32
/*	short	aligned;						*/
# endif
	short	opno;		/* operator number			*/
};
typedef struct op_node		OP_NODE;

# define			OP_SIZ		2

struct svar_node
{				char	sym_hdr[SYM_HDR_SIZ];
/*	char	type;							*/
/*	char	len;							*/
# ifdef BIT_32
/*	short	aligned;						*/
# endif
	short	vattno;		/* variable attribute no		*/
	char	vtype;		/* variable type			*/
	char	vlen;		/* variable length			*/
	short	*vpoint;	/* variable pointer to value		*/
};
typedef struct svar_node	SVAR_NODE;

# define			SVAR_SIZ	VAR_SIZ

/* structure for referencing a SOURCEID type symbol for			*/
/*	pipe transmission						*/
/*									*/
/*	SOURCEID is a range table element.				*/
struct srcid
{				char	sym_hdr[SYM_HDR_SIZ];
/*	char	type;							*/
/*	char	len;							*/
# ifdef BIT_32
/*	short	aligned;						*/
# endif
	short	srcvar;		/* variable number			*/
	char	srcname[MAXNAME];	/* relation name		*/
	char	srcown[2];	/* relation owner usercode		*/
	short	srcstat;	/* relstat field			*/
};
typedef struct srcid		SRCID;

# define			SRC_SIZ		(6 + MAXNAME)
