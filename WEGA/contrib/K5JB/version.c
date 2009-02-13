/* Change only the part after the decimal when making local changes.
 * This is pretty complex because I compile for a bunch who want custom
 * versions.  Unix/Coherent and MS-DOS version.c are different.
 * This is for Unix/Coherent
 * If you change versionf[], I suggest you leave the first part of the
 * string alone for future trouble shooting purposes.  Add your own suffix
 * if you want, but keep the total string length LESS THAN the array size.
 * In fact, get carried away with the length of this and your startup
 * screen will be ugly.
 */
#include "options.h"
#include "config.h"

char versionf[28] = "K5JB.k37";	/* for FTP and the mbox's use */

void
version()
{
	char *margin = "                  ";

	printf("%sThis version is %s with options:\n%s",margin,versionf,
		margin);

#ifdef AX25
#ifdef SEGMENT
	printf("AX.25/Seg");
#else
	printf("AX.25");
#endif
#ifdef VCIP_SSID
	printf(", AX.25/vcipcall");
#endif
#ifdef NETROM
	printf(", Netrom");
#endif
#ifdef AXMBX
	printf(", MBOX");
#endif

/* Twiddle things here for cosmetics. */

#ifdef MDKISSPORT
	printf("\n%sMDKISS",margin);
#endif
#ifdef MULPORT
	printf(", Mulport");
#endif
#ifdef AX25_HEARD
	printf(", Heard");
#endif
#else /* AX25 */
	printf("SLIP");
#endif /* AX25 */

#ifdef COH386
	printf(", Coherent");
#else
#ifdef UNIX
	printf(", Unix");
#endif
#ifdef _OSK
	printf(", OS9-68K");
#endif
#endif
	printf("\n\n");
}
