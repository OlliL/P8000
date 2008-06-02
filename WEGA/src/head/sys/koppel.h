/*
 * Definitionen fuer die Koppelschnittstelle
 * zwischen 8- und 16-Bit-Rechnerteil
 */

unsigned ksflag;	/* enthaelt UD oder FD */

#define FD0	0x01	/* /dev/fd besetzt Schnittstelle */
#define FD1	0x02	/* /dev/fd besetzt Schnittstelle */
#define FD2	0x04	/* /dev/fd besetzt Schnittstelle */
#define FD3	0x08	/* /dev/fd besetzt Schnittstelle */
#define UD	0x10	/* /dev/ud besetzt Schnittstelle */

#define UDPRI	45
