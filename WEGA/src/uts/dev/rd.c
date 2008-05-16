/*
 * `rd.c' 1.1 (3/16/91 18:08:56)
 *
 * P8000 Begemot RamDriver.
 *
 * This driver gives an standard unix block- and raw- interface
 * to ramdisk, which physical address is located higher the user's
 * memory, but lower than cpu2ram address. The start address an size
 * of the ram disk is found itself. If there is no space, OPEN(2)
 * returns ENXIO.
 */

#include	<sys/param.h>
#include	<sys/state.h>
#include	<sys/conf.h>
#include	<sys/dir.h>
#include	<sys/filsys.h>
#include	<sys/file.h>
#include	<sys/mount.h>
#include	<sys/map.h>
#include	<sys/proc.h>
#include	<sys/inode.h>
#include	<sys/mmu.h>
#include	<sys/buf.h>
#include	<sys/s.out.h>
#include	<sys/tty.h>
#include	<sys/ioctl.h>
#include	<sys/sysinfo.h>
#include	<sys/systm.h>
#include	<sys/user.h>


# define	RD_SEG0	0x38	/* to map ram disk memory	*/
# define	RD_SEG1 0x39	/* to map user memory		*/
# define	CPU2RAM	0x7000	/* cpu2 ram start		*/
# define	MAGIC	0x19	/* so as they did it in main.c	*/

# ifdef	DEBUG
#  define	static
# endif

extern int	kclicks, umemory ;
extern int	mmut ;

static	unsigned	rdramstart ;	/* ram disk start & end	*/
static  unsigned	rdramend ;	/* address in clicks	*/
static  struct segd	sdr = { 0, 0xFF, 0 } ;
					/* to load SDR of mmut	*/

rdint()
{
	printf( "rd: unexpexted interrupt\n" ) ;
}

rdopen( dev, flag )
dev_t	 dev ;
unsigned flag ;
{
	register unsigned raddr ;	/* current ram address (phys) */
	register long	  physaddr ;
	unsigned char	  byte ;
	static	int	  raminitflag = 0 ;


	/*
	 * Check legal minor number
	 */
	if( minor( dev ) != 0 ) {
		u.u_error = ENODEV ;
		return ;
	}

	switch( raminitflag ) {
	case 0 :
		/*
		 * Check for ram
		 */
		break ;
	case 1 :
		/*
		 * Fine, its ok already
		 */
		return ;
	case 2 :
		/*
		 * Fatal, no ram
		 */
		u.u_error = ENXIO ;
		return ;
	default:
		/*
		 * Shouldn't happen
		 */
		printf( "rd: open program error\n" ) ;
	}

	raminitflag++ ;
	rdramstart = kclicks + umemory  ;

	/*
	 * search for ramstart
	 */
	for( raddr = rdramstart ; raddr < CPU2RAM ; raddr += 0x1000 ) {
		physaddr = ctob( (long)raddr ) ;
		spbyte( physaddr, MAGIC ) ;
		fpbyte( physaddr, &byte ) ;
		if( byte == MAGIC )
			break ;
	}
	if( ( rdramstart = raddr ) == CPU2RAM ) {
		/*
		 * NO RAM
		 */
		raminitflag++ ;
		u.u_error = ENXIO ;
		return ;
	}
	/*
	 * search for ramend
	 */
	for( raddr += 0x1000 ; raddr < CPU2RAM ; raddr += 0x1000 ) {
		physaddr = ctob( (long)raddr ) ;
		spbyte( physaddr, MAGIC ) ;
		fpbyte( physaddr, &byte ) ;
		if( byte != MAGIC )
			break ;
	}
	rdramend = raddr ;
	/*
	 * ram test
	 */
	for( raddr = rdramstart ; raddr < rdramend ; raddr += 0x100 ) {
		sdr.sg_base = raddr ;
		loadsd( mmut, RD_SEG0, &sdr ) ;
		if( ramtest( RD_SEG0 << 8 , 1 ) ) {
			printf( "\07rd: ramtest error at 0x%X\n",
				ctob( (long) raddr ) ) ;
			u.u_error = ENXIO ;
			return ;
		}
	}
}

rdclose()
{

}

rdstrat( bp )
register struct buf	*bp ;
{
	register unsigned	raddr ;

	/*
	 * Get ram address
	 */
	raddr = (int)( bp->b_blkno ) * ( BSIZE / CPAS ) ;

	/*
	 * Check parameter
	 */
	if( raddr + ( ( bp->b_bcount + CPAS - 1 ) / CPAS ) >
				( rdramend - rdramstart ) ) {
		bp->b_flags |= B_ERROR ;
		bp->b_error = ENXIO ;
		iodone( bp ) ;
		return ;
	}
	/*
	 * Programming MMU segments
	 */
	dvi() ;

	/*
	 * Ram
	 */
	sdr.sg_base = rdramstart + raddr ;
	loadsd( mmut, RD_SEG0, &sdr ) ;

	/*
	 * User
	 */
	sdr.sg_base = (int)( bp->b_paddr >> 8 ) ;
	loadsd( mmut, RD_SEG1, &sdr ) ;

	if( bp->b_flags & B_READ )
		/*
		 * Ram to phys. address
		 */
		bcopy( 0, RD_SEG0 << 8, ( (int)bp->b_paddr ) & 0xFF,
				RD_SEG1 << 8, bp->b_bcount ) ;
	else
		/*
		 * Phys. address into ram
		 */
		bcopy( ( (int)bp->b_paddr ) & 0xFF, RD_SEG1 << 8, 0,
				RD_SEG0 << 8, bp->b_bcount ) ;

	evi() ;

	bp->b_resid = 0 ;
	bp->b_error = 0 ;

	iodone( bp ) ;
}

rdread()
{
	rdramcp( B_READ ) ;
}

rdwrite()
{
	rdramcp( B_WRITE ) ;
}

static
rdramcp( flag )
int	 flag ;
{
	register unsigned	raddr ;
	struct segd		sdr ;

	/*
	 * Get ram address
	 */
	raddr = u.u_offset / CPAS ;

	/*
	 * Check parameter
	 */
	if( raddr + ( ( u.u_count + CPAS - 1 ) / CPAS )
					> ( rdramend - rdramstart ) ) {
		u.u_error = ENXIO ;
		return ;
	}
	/*
	 * Programming MMU segments
	 */
	dvi() ;

	sdr.sg_base = rdramstart + raddr ;
	loadsd( mmut, RD_SEG0, &sdr ) ;

	if( flag & B_READ )
		/*
		 * Copy block from ram to user
		 */
		copyout( (int)(u.u_offset)&0xFF, RD_SEG0 << 8,
				u.u_base, u.u_count ) ;
	else
		/*
		 * Copy from user to ram
		 */
		copyin( u.u_base, (int)(u.u_offset)&0xFF, RD_SEG0 << 8,
				u.u_count ) ;

	evi() ;

	u.u_count = 0 ;
}

rdioctl( dev, cmd, addr, flag )
dev_t	dev ;
int	cmd ;
char	*addr ;
int	flag ;
{
	register	blocks ;

	if( cmd == SHOWRD ) {
		blocks = (int)(( rdramend - rdramstart ) / ( BSIZE / CPAS )) ;
		if( suword( addr, blocks ) < 0 )
			u.u_error = EFAULT ;
		return ;
	} else
		u.u_error = EINVAL ;
}
