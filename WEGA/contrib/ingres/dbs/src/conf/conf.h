# include	"../conf/m.h"

/*	MACHINE DEPENDENCIES					*/
/*	user defined						*/
/*		AMX_SC:		AMX seperate compiling		*/
/*		DBS_LOCK:	/dev/DB_LOCK available		*/
/*		DBS_MAJOR:	major device number of DB_LOCK	*/
/*		DBS_UID:	uid of BS-user DBS		*/
/*		LOCKS:		no of concurrent used DB's	*/
/*		STRIPFLAG:	strip load modules		*/

/* define FLAGS here */
# define	AMX_SC
# define	DBS_LOCK
# define	DBS_MAJOR	10
# define	DBS_UID		9
# define	LOCKS		15
# define	STRIPFLAG	-s


/*	MACHINE DEPENDENCIES					*/
/*	system defined						*/
/*		AMX_QRYMOD:	AMX query modification support	*/
/*		BIT_32:		32-bit wordsize			*/
/*		BMOVE:		no assembler text for bmove()	*/
/*		CODE:		limitted code (4KByte)		*/
/*		DBU_SEPERATE:	seperate dbu modules		*/
/*		DIR_BSD4:	BSD 4.x file system		*/
/*		DISK:		default floppy disk		*/
/*		DYSIZE:		no lib function for dysize()	*/
/*		D_ALIGN:	alignment % 8 needed		*/
/*		EBCDIC:		EBCDIC character set		*/
/*		ENUM_INT:	enumeration are int's		*/
/*		ESER:		ESER runtime support		*/
/*		F77:		FORTRAN 77 support		*/
/*		FP_PROCESSOR:	floating point processor	*/
/*		FREAD:		using fread(III)		*/
/*		FWRITE:		using fwrite(III)		*/
/*		ID_SEPERATE:	seperate I/D available		*/
/*		L_ALIGN:	alignment % 4 needed		*/
/*		MEM_E:		enough memory: 32-bit addresses	*/
/*		MEM_N:		not enough memory: 64KBytes	*/
/*		MEM_S:		segmented memory		*/
/*		MEM_O:		overlayed memory: bsd 2.9	*/
/*		NO_F4:		no float's (float = double)	*/
/*		NO_KILL:	kill(2) not available		*/
/*		NO_PIPE:	pipe(2) not available		*/
/*		NON_V7_FS:	non BS V7 file system		*/
/*		OD_SEPERATE:	seperate ovqp/decomp modules	*/
/*		OS_DIR:		name of the configuration dir	*/
/*		OS_LIB:		name of the device library	*/
/*		OS_NAME:	name of the operating system	*/
/*		PAGE_LOCKING:	enable page locking level	*/
/*		PGBLKS:		no of 512-bytes blocks per page	*/
/*		RANLIB:		ranlib available		*/
/*		REV_BYTES:	revers bytes in word		*/
/*		REV_WORDS:	revers words in long word	*/
/*		SETBUF:		using setbuf(III)		*/
/*		SIGN_EXT:	sign extension (char -> int)	*/
/*		STACK:		limitted stack (4KByte)		*/
/*		S_ALIGN:	alignment % 2 needed		*/
/*		TEXT_SEPERATE:	seperate text segment available	*/
/*		YACC_OPTIM:	cant't optimize y.tab.c		*/


# ifdef A7150
# define	EC1834
# endif

# ifdef BC
# define	DBU_SEPERATE
# define	MACHINE		"BC 5120.16"
# define	MEM_N
# define	OD_SEPERATE
# define	PGBLKS		2
# define	SETBUF
# define	S_ALIGN		0
# define	YACC_OPTIM
# endif

# ifdef BDS
# define	DBU_SEPERATE
# define	MACHINE		"BDS A 5230"
# define	MEM_N
# define	OD_SEPERATE
# define	PGBLKS		2
# define	SETBUF
# define	S_ALIGN		0
# define	YACC_OPTIM
# define	BC
# endif

# ifdef CM1420
# define	MACHINE		"CM1420"
# define	PDP
# endif

# ifdef CM4
# define	MACHINE		"SM4-20"
# define	PDP
# endif

# ifdef CM52_11
# define	MACHINE		"SM 52/11"
# define	PDP
# endif

# ifdef EC1834
# undef		mutos
# define	BMOVE
# ifdef TRACE
# define	DBU_SEPERATE
# endif
# define	FP_PROCESSOR
# define	ID_SEPERATE
# define	MACHINE		"EC 1834"
# ifdef TRACE
# define	OD_SEPERATE
# endif
# define	OS_DIR		/usr/sys/conf
# define	OS_LIB		/usr/sys/io
# define	OS_NAME		mutos
# define	PGBLKS		2
# define	RANLIB
# define	REV_BYTES
# define	SIGN_EXT
# define	S_ALIGN		0
# endif

# ifdef HP_UX
# define	BIT_32
# define	BMOVE
# define	DIR_BSD4
# define	DYSIZE
# define	FP_PROCESSOR
# define	L_ALIGN		0
# define	MACHINE		"HP 9000"
# define	MEM_E
# define	NON_V7_FS
# define	PGBLKS		8
/* # define	RANLIB	already did, see AR(1) */
# define	SETBUF
# define	SIGN_EXT
# define	TEXT_SEPERATE
# endif

# ifdef I100
# define	MACHINE		"I100"
# define	PDP
# endif

# ifdef I102
# define	MACHINE		"I102"
# define	PDP
# endif

# ifdef I105
# define	MACHINE		"I105"
# define	PDP
# endif

# ifdef K1630
# define	MACHINE		"K 1630"
# define	PDP
# endif

# ifdef K1820
# define	MACHINE		"K 1820"
# define	VAX
# endif

# ifdef K1840
# define	MACHINE		"K 1840"
# define	VAX
# endif

# ifdef P8000
# define	ID_SEPERATE
# define	MACHINE		"P8000"
# define	MEM_S
# define	OS_DIR		/usr/sys/conf
# define	OS_LIB		/usr/sys/dev/LIB2
# define	OS_NAME		wega
# define	PGBLKS		2
# define	SETBUF
# define	SIGN_EXT
# define	S_ALIGN		0
# define	TEXT_SEPERATE
# endif

# ifdef PDP34
# define	MACHINE		"PDP 11/34"
# define	PDP
# endif

# ifdef PDP70
# define	FP_PROCESSOR
# define	ID_SEPERATE
# define	MACHINE		"PDP 11/70"
# define	PGBLKS		2
# define	RANLIB
# define	REV_BYTES
# define	SETBUF
# define	SIGN_EXT
# define	S_ALIGN		0
# endif

# ifdef PSU_1040
# define	ESER_PSU
# define	MACHINE		"EC 1040"
# endif

# ifdef PSU_1056
# define	ESER_PSU
# define	MACHINE		"EC 1056"
# endif

# ifdef PSU_1057
# define	ESER_PSU
# define	MACHINE		"EC 1057"
# endif

# ifdef VENIX
# define	BMOVE
# ifdef TRACE
# define	DBU_SEPERATE
# endif
# define	FP_PROCESSOR
# define	ID_SEPERATE
# define	MACHINE		"PC 1640"
# define	OD_SEPERATE
# define	PGBLKS		2
# define	RANLIB
# define	REV_BYTES
# define	REV_WORDS
# define	SETBUF
# define	SIGN_EXT
# define	STACK
# define	S_ALIGN		0
# endif

# ifdef VMX_1056
# define	ESER_VMX
# define	MACHINE		"EC 1056"
# endif

# ifdef VMX_1057
# define	ESER_VMX
# define	MACHINE		"EC 1057"
# endif

# ifdef XENIX286
# define	BMOVE
# define	FP_PROCESSOR
# define	ID_SEPERATE
# define	MACHINE		"IBM/AT"
# define	PGBLKS		2
# define	RANLIB
# define	REV_BYTES
# define	REV_WORDS
# define	SETBUF
# define	SIGN_EXT
# define	S_ALIGN		0
# define	YACC_OPTIM
# endif

# ifdef XENIX386
# define	BIT_32
# define	BMOVE
# define	FP_PROCESSOR
# define	ID_SEPERATE
# define	L_ALIGN		0
# define	MACHINE		"I 80386"
# define	MEM_E
# define	PGBLKS		8
# define	RANLIB
# define	REV_BYTES
# define	REV_WORDS
# define	SIGN_EXT
# endif


/* 	NO REAL MACHINES:					*/
/*		ESER_PSU  == EC 10.. (PSU)			*/
/*		ESER_VMX  == EC 10.. (VMX)			*/
/*		ESER      == EC 10..				*/
/*		MSDOS     == IBM/XT, IBM/AT			*/
/*		PDP       == PDP11/34, K1630, SM4-20, I100 ...	*/
/*		VAX       == K1820, K1840 ...			*/

# ifdef ESER_PSU
# define	CODE
# define	DYSIZE
# define	D_ALIGN		1
# define	EBCDIC
# define	ESER
# define	NO_F4
# define	NO_KILL
# define	NO_PIPE
# define	STACK
# define	YACC_OPTIM
# endif

# ifdef ESER_VMX
# define	ENUM_INT
# define	ESER
# define	L_ALIGN		0
# define	lseek		seek
# endif

# ifdef ESER
# define	BIT_32
# define	BMOVE
# define	FP_PROCESSOR
# define	MEM_E
# define	PGBLKS		8
# define	RANLIB
# define	SETBUF
# define	TEXT_SEPERATE
# endif

# ifdef MSDOS
# define	FP_PROCESSOR
# define	MEM_S
# define	PGBLKS		2
# define	REV_BYTES
# define	REV_WORDS
# define	SETBUF
# define	SIGN_EXT
# define	S_ALIGN		0
# endif

# ifdef PDP
# define	DBU_SEPERATE
# define	F77
# define	FP_PROCESSOR
# define	MEM_N
# define	MEM_O
# define	OD_SEPERATE
# define	PGBLKS		2
# define	REV_BYTES
# define	SETBUF
# define	SIGN_EXT
# define	S_ALIGN		0
# define	TEXT_SEPERATE
# endif

# ifdef VAX
# define	BIT_32
# define	DIR_BSD4
# define	DYSIZE
# define	FP_PROCESSOR
# define	L_ALIGN		0
# define	MEM_E
# define	NON_V7_FS
# define	PGBLKS		8
# define	RANLIB
# define	REV_BYTES
# define	REV_WORDS
# define	SETBUF
# define	SIGN_EXT
# undef         vax
# endif


# ifdef ESER_PSU
# define	CP		-cp
# else
# define	CP		cp
# endif
# define	LN		ln
# define	DISK		/dev/dbs


/*								*/
/*	SIGN EXTENSION DEFINITIONS				*/
/*								*/
/*		ctoi:	char to (signed) integer		*/
/*		ctou:	char to unsigned integer		*/
/*								*/

#	ifdef SIGN_EXT
# define	ctoi(val)	(val)
#	else
# define	ctoi(val)	AAi1toi2(val)
#	endif

# define	ctou(val)	((val) & 0377)


/*								*/
/*	MACRO processor available ?				*/
/*								*/

# define	MACRO
