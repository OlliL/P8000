/* s.out.h */

/******************************************************************************
** Note:
**      No changes should be made to this file without changing the appropriate
** 	documentation, namely a.out.5.
*******************************************************************************/

/* The object module header structure is as defined below for S.OUT object */
/* file formats. */

    struct s_exec
    {
        int      s_magic;   /* Magic number */
        long     s_imsize;  /* Size of memory image section */
        long     s_bss;     /* Size of bss sections */
        unsigned s_segt;    /* Size of segment table section */
        unsigned s_syms;    /* Size of symbol table section */
        long     s_entry;   /* Entry point address */
        unsigned s_flag;    /* Flags word */
        unsigned s_codesz;  /* 8-bit padded code size */
        unsigned s_lines;   /* Number of line table entries */
    };

/*  The valid magic numbers for the s.out are defined below. */
/*  Valid z.out magic numbers ( defined in z.out.h ) are :   */
/*    e807, e810 and e811                                    */

#define	    S_MAGIC1    0xE607      /* Segmented executable */
#define	    S_MAGIC3    0xE611      /* Segmented separate I & D */
#define	    S_MAGIC4	0xE605	    /* Segmented overlay */
#define	    N_MAGIC1    0xE707      /* Nonsegmented executable */
#define	    N_MAGIC3    0xE711      /* Nonsegmented separate I & D */
#define	    N_MAGIC4	0xE705	    /* Nonsegmented overlay file */
#define     X_MAGIC1    0xE507      /* 8-bit executable */
#define     X_MAGIC3    0xE511      /* 8-bit separate I & D */
#define     X_MAGIC4    0xE505      /* 8-bit overlay */
    
/* Valid flags in the s_flags field are defined as follows:  */

#define	    SF_STRIP    0x0001      /* Relocation info has been stripped */
#define	    SF_OPREP    0x0002      /* Module changed by OPREP program */
#define     SF_Z8       0x0004      /* U8 program */
#define     SF_Z80      0x0008      /* U880 program */
#define     SF_SEND	0x0010      /* module created by SEND */
#define     SF_7FSTK    0x0020      /* Stack resides in segment 0x7f */


/* The segment table is an array of the following structure.  Each */
/* entry relates to one segment in the program.  For file types    */
/* with magic number S_MAGIC1 there is a maximum of 128 such       */
/* entries.  For file types S_MAGIC3 there is a maximum of         */
/* 256 entries. For the remaining file types there is always 1     */
/* entry in the segment table.                                     */

    struct segt
    {
        char     sg_segno;  /* Segment number */
        char     sg_coff;   /* Offset/256 for code section */
        char     sg_doff;   /* Offset/256 for data section */
        char     sg_boff;   /* Offset/256 for bss section */
        unsigned sg_code;   /* Size of the code portion */
        unsigned sg_data;   /* Size of the data portion */
        unsigned sg_bss;    /* Size of the bss portion */
        int      sg_atr;    /* Attributes, defined below */
        long     sg_unused; /* Unused */
    };

/* These definitions apply to the sg_atr field in the segment table*/

#define	    SG_CODE     0x0001      /* Segment contains code */
#define	    SG_DATA     0x0002      /* Contains initialized data */
#define	    SG_BSS      0x0004      /* Contains uninitialized data */
#define	    SG_STACK    0x0008      /* The segment grows downward */
#define	    SG_OCODE    0x0010      /* Code section is offset */
#define	    SG_ODATA    0x0020      /* Data section is offset */
#define	    SG_OBSS     0x0040      /* Bss section is offset */
#define	    SG_BOUND    0x0080      /* Segment number bound to this section */
    
/* The symbol table is an array containing entries with information on */
/* symbols.  All entries are fixed size with the symbol having <= 8    */
/* characters.  If a longer name is used, the first character of the   */
/* name has the high bit set and is interpreted as the length of the   */
/* name (including the length byte) rather than a character.  The      */
/* 'overflow' characters are put into the following storage areas and  */
/* padded out with zeros to fill a symbol table entry exactly. Names   */
/* shorter than eight characters are also padded with zeros.           */

#define	SHORTNAME 8		/* Length of normal sized name */
#define	LONGNAME 127		/* Max characters in a long name */

    struct s_nlist
    {
        long     sn_value;		/* Value */
        char     sn_type;		/* Type field */
        char     sn_segt;		/* Segment table entry # */
        char     sn_name[SHORTNAME];	/* Symbol name padded with 0s */
    };
 
    struct vdb_list
    {			/* VDB equivalent of s_nlist */
        union
	{
            long    vn_value;
            struct
	    {
                unsigned    vn_dtype;
                unsigned    vn_other;
            } db;
        } v;
        char    vn_type;
        char    vn_segt;
        char    vn_name[SHORTNAME];
    };

#ifndef	N_UNDF		/* s.out.h and z.out.h can be included together... */
			/* ...but z.out.h must preceed s.out.h */
#define	    N_UNDF      0           /* Undefined */
#define	    N_ABS       0x01        /* Absolute */
#define	    N_DATA      0x03        /* Data symbol */
#define	    N_BSS       0x04        /* Bss symbol */
#define	    N_TYPE      0x1F        /* Mask for type */
#define	    N_FN        0x1F        /* File name */
#define	    N_EXT       0x20        /* External bit, or'ed in */
/*
** The following types support VDB
*/
#define     N_SIDENT    0x07        /* Structure identification */
#define     N_LENGTH    0x08        /* Length of structure or union */
#define     N_MEMBER    0x09        /* Structure or member offset */
#define     N_BMEMBER   0x0a        /* Bit field offset from beginning of structure */
#define     N_RBRACKET  0x0b        /* Right bracket marker */
#define     N_LBRACKET  0x0c        /* Left bracket marker */
#define     N_ENTRY     0x0d        /* Entry point */
#define     N_SFN       0x0e        /* Source file name */
#define     N_IFN       0x0f        /* Included source file name */
#define     N_REGPARM   0x10        /* Parameter register value */
#define     N_ABSPARM   0x11        /* Parameter offset on stack */
#define     N_PREG      0x12        /* Register parameter assigned to a register */
#define     N_SPREG     0x13        /* Stack parameter assigned to a register */
#define     N_AREG      0x14        /* Automatic variable register value */
#define     N_AABS      0x15        /* Automatic offset on stack */
#define     N_RABS      0x16        /* Register save area offset on stack */
#define     N_ISTATIC   0x17        /* Internal static */
#define     N_LSTATIC   0x18        /* Local static */
#define     N_ABSS      0x19        /* Global symbol in a .comm area */
#define     N_GLOBAL    0x1a        /* Global symbol in .data area */
#endif

#define	    N_CODE      0x02        /* Code symbol */
#define	    N_SN        0x1E        /* Section name */
#define	    N_SEG       0x40        /* Segmented bit, or'ed in */
#define	    LONGBIT	0x80	    /* In sn_name[0], marks long symbol */
#define	    LONGMASK	0x007F	    /* Strip off LONGBIT */

/* Complete s.out header structure */

struct s_head
{
	struct s_exec	s_exc;		/* s.out header */
	struct segt	segtable[2];	/* nonsegmented seg table */
};

/* Line table structure for VDB */

struct v_lines
{
        long     l_addr;
        unsigned l_num;
        char     l_segt;
        char     l_unused;
};
