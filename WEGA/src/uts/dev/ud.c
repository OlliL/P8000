/*********************************************************************
 *
 * Character Device Driver /dev/ud
 *
 *********************************************************************
*/
#include "sys/param.h"
#include "sys/dir.h"
#include "sys/state.h"
#include "sys/mmu.h"
#include "sys/file.h"
#include "sys/buf.h"
#include "sys/conf.h"
#include "sys/sysinfo.h"
#include "sys/s.out.h"
#include "sys/ioctl.h"
#include "sys/tty.h"
#include "sys/systm.h"
#include "sys/user.h"
#include "sys/koppel.h"
 
/* Flag-Byte */

unsigned int udflag =  0;
 
#define UDHOG          200	/* Ueberlaufgrenze Eingabekette */
 
struct clist ud_inq;	/* Header der Eingabekette */
struct clist ud_outq;	/* Header der Ausgabekette */
 
extern udos;
extern dev_t console;
extern last_int_serv;
extern ud_flag;
extern routb();

/*
 * interrupt routine
 */
udint (c)
register unsigned char c;
{ 
	last_int_serv = UDINT;
	ud_flag = 0;

	/* Pruefen auf Ueberlauf der Eingabekette */

	if (ud_inq.c_cc > UDHOG) {

		/* vernichten aller bisher in die Kette eingelaufenen Zeichen */
 
		while(ud_inq.c_cc)getc(&ud_inq);
		return;
	}
 
	/* Zeichen werden in die Eingabekette gebracht */
	putc(c, &ud_inq);

	/* Prozesse wecken, die auf Eingabezeichen warten */

	if (udflag != 0) {
		udflag = 0;
		wakeup(&ud_inq); 
	}
}

/*
 * open routine
 */
udopen(dev)
dev_t dev;
{ 
	if (udos != 2 || minor(u.u_ttyd) != console || ksflag != 0) {
		u.u_error = ENXIO;
		return;
	}
	ksflag = UD;
}
 
udclose(dev)
dev_t dev;
{
	udflag = 0;
	ksflag = 0;
}
 
/* 
 * read routine
 */
udread(dev)
dev_t dev;
{
	ud_flag++;
	dvi();
	while( ud_inq.c_cc == 0) {

		/* sleep bis Zeichen eingelaufen */
		udflag = 1;
		sleep(&ud_inq, UDPRI);
	}
	evi();

	/* Uebergabe der Zeichen zum User */
	while (ud_inq.c_cc && passc(getc(&ud_inq)) >= 0)
	       ;
}
 
/*
 * write Routine
 */
udwrite(dev)
dev_t dev;
{
	register int c;

	while (u.u_count) { /* solange, bis Syscall abgearbeitet */
 
		/* get next character from user process*/
		if ( (c = cpass() ) < 0)
			break;
 
		/*put the char on the output queue*/
		putc(c, &ud_outq);
	}
 
	/* start the device */
	/* get one character from output queue */
	while ((c = getc(&ud_outq)) >= 0)
		routb(c);
}
