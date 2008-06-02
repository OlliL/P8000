/* mmu.h */

/*  
 *  segment descriptor
 */
struct	segd 
{
	unsigned int	sg_base;	/* phys addr click pointer	*/
	unsigned char 	sg_limit;	/* number of clicks 0=>1	*/
	unsigned char	sg_attr;	/* attributes			*/
};

# define RD_AT		0001
# define SYS_AT		0002
# define CPUI_AT	0004
# define EXC_AT		0010
# define DMAI_AT	0020
# define DIRW_AT	0040
# define CHG_AT		0100
# define REF_AT		0200

# define RO		RD_AT
# define RW		0

# define SBREAK		0xFFC9
# define UBREAK		0xFFD1

# define USEGW		0x3F00		
# define USEG		0x3F

# define MASKSEG 	0x7F00		

# define TEXT		-1		/* code for non-segmented user	 */
# define DATA		0x3F		/* data for non-segmented user	 */
# define STAK		0x7F		/* stack for non-seg and new seg */
# define OLDSEGSTAK	0x3F		/* stack for old segmented user	 */
# define DISKSEG	0x3A		/* segment for hard disk driver  */
# define FDISKSEG	0x39		/* segment for floppy disk driver*/
# define MTSEG		0x38		/* segment for 1/2" tape         */

# define NOCHANGE 	-1		
