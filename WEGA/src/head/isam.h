/* isam.h */

/*
 *       C-ISAM version 1.03
 *  Indexed Sequential Access Method
 */

#define CHARTYPE	0
#define CHARSIZE	1

#define INTTYPE		1
#define INTSIZE		2

#define LONGTYPE	2
#define LONGSIZE	4

#define DOUBLETYPE	3
#define DOUBLESIZE	sizeof(double)

#define FLOATTYPE	4
#define FLOATSIZE	sizeof(float)

#define MAXTYPE		5
#define ISDESC		0200	/* add to make descending type	*/
#define TYPEMASK	017	/* type mask			*/

#define ldint(p)	(((p)[0]<<8)+((p)[1]&0377))
#define stint(i,p)	((p)[0]=(i)>>8,(p)[1]=(i))
long ldlong();

#ifndef NOFLOAT
float ldfloat();
double lddbl();
#endif

#define ISFIRST		0	/* position to first record	*/
#define ISLAST		1	/* position to last record	*/
#define ISNEXT		2	/* position to next record	*/
#define ISPREV		3	/* position to previous record	*/
#define ISCURR		4	/* position to current record	*/
#define ISEQUAL		5	/* position to equal value	*/
#define ISGREAT		6	/* position to greater value	*/
#define ISGTEQ		7	/* position to >= value		*/

/* isread lock modes */
#define ISLOCK     (1<<8)	/* lock record before reading	*/

/* isopen, isbuild lock modes */
#define ISAUTOLOCK (2<<8)	/* automatic record lock	*/
#define ISMANULOCK (4<<8)	/* manual record lock		*/
#define ISEXCLLOCK (8<<8)	/* exclusive isam file lock	*/

#define ISINPUT		0	/* open for input only		*/
#define ISOUTPUT	1	/* open for output only		*/
#define ISINOUT		2	/* open for input and output	*/

/* audit trail mode parameters */
#define AUDSETNAME	0	/* set new audit trail name	*/
#define AUDGETNAME	1	/* get audit trail name		*/
#define AUDSTART	2	/* start audit trail 		*/
#define AUDSTOP		3	/* stop audit trail 		*/
#define AUDINFO		4	/* audit trail running ?	*/

#define NPARTS		8	/* maximum number of key parts	*/

struct keypart
    {
    int kp_start;		/* starting byte of key part	*/
    int kp_leng;		/* length in bytes		*/
    int kp_type;		/* type of key part		*/
    };

struct keydesc
    {
    int  k_flags;		/* flags			*/
    int  k_nparts;		/* number of parts in key	*/
    struct keypart
	k_part[NPARTS];		/* each key part		*/
		    /* the following is for internal use only	*/
    int  k_len;			/* length of whole key		*/
    long k_rootnode;		/* pointer to rootnode		*/
    };
#define k_start   k_part[0].kp_start
#define k_leng    k_part[0].kp_leng
#define k_type    k_part[0].kp_type

#define ISNODUPS  000		/* no duplicates allowed	*/
#define ISDUPS	  001		/* duplicates allowed		*/
#define DCOMPRESS 002		/* duplicate compression	*/
#define LCOMPRESS 004		/* leading compression		*/
#define TCOMPRESS 010		/* trailing compression		*/
#define COMPRESS  016		/* all compression		*/

struct dictinfo
    {
    int  di_nkeys;		/* number of keys defined	*/
    int  di_recsize;		/* data record size		*/
    int  di_idxsize;		/* index record size		*/
    long di_nrecords;		/* number of records in file	*/
    };

#define EDUPL	  100		/* duplicate record	*/
#define ENOTOPEN  101		/* file not open	*/
#define EBADARG   102		/* illegal argument	*/
#define EBADKEY   103		/* illegal key desc	*/
#define ETOOMANY  104		/* too many files open	*/
#define EBADFILE  105		/* bad isam file format	*/
#define ENOTEXCL  106		/* non-exclusive access	*/
#define ELOCKED   107		/* record locked	*/
#define EKEXISTS  108		/* key already exists	*/
#define EPRIMKEY  109		/* is primary key	*/
#define EENDFILE  110		/* end/begin of file	*/
#define ENOREC    111		/* no record found	*/
#define ENOCURR   112		/* no current record	*/
#define EFLOCKED  113		/* file locked		*/
#define EFNAME    114		/* file name too long	*/
#define ENOLOK    115		/* can't create lock file */

/*
 * For system call errors
 *   iserrno = errno (system error code 1-99)
 *   iserrio = IO_call + IO_file
 *		IO_call  = what system call
 *		IO_file  = which file caused error
 */

#define IO_OPEN	  0x10		/* open()	*/
#define IO_CREA	  0x20		/* creat()	*/
#define IO_SEEK	  0x30		/* lseek()	*/
#define IO_READ	  0x40		/* read()	*/
#define IO_WRIT	  0x50		/* write()	*/
#define IO_LOCK	  0x60		/* locking()	*/
#define IO_IOCTL  0x70		/* ioctl()	*/

#define IO_IDX	  0x01		/* index file	*/
#define IO_DAT	  0x02		/* data file	*/
#define IO_AUD	  0x03		/* audit file	*/
#define IO_LOK	  0x04		/* lock file	*/
#define IO_SEM	  0x05		/* semaphore file */

extern int iserrno;		/* isam error return code	*/
extern int iserrio;		/* system call error code	*/
extern char isstat1;		/* cobol status characters	*/
extern char isstat2;
extern char *isversnumber;	/* C-ISAM version number	*/
extern char *iscopyright;	
extern long isserial;		

struct audhead
    {
    char au_type[1];		/* audit record type A,D,R,W	*/
    char au_time[4];		/* audit date-time		*/
    char au_procid[2];		/* process id number		*/
    char au_userid[2];		/* user id number		*/
    };
#define AUDHEADSIZE  9		/* num of bytes in audit header	*/

