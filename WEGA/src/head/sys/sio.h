/*
 *   Deklarationen zum Terminal-Treiber fuer Schaltkreis  U880-SIO
 */

/* ----------Konfigurier- und Variablenvereinbarungen ---------------------- */

caddr_t msgbufp = msgbuf;
long brkflag = 0;
	/* ------------ Tabelle der Baudraten-Zeitkonstanten */
char siospeed[] = {
	0, 0, 0,
	0xaf,	/* 110 */
	0x8f,	/* 135 */
	0x80,	/* 150 */
	0x60,	/* 225 */
	0x40,	/* 300 */
	0x20,	/* 600 */
	0x10,	/* 1200 */
	0xc,	/* 1800 */
	8,	/* 2400 */
	4,	/* 4800 */
	2,	/* 9600 */
	1,	/* 19200 */
	1,	/* 19200 */
};
/* -------------- Programmiertabellen fuer SIO ----------------------------- */
	/* -------------- Grundinitialisierung */
unsigned char siotab[] = {
	res_xi|WR4, clkx32,	/* x32, keine Paritaet */
	WR3, 0,
	WR5, dtr|enb_xmt|rts,
	WR2, 0,			/* Int. vector */
	res_xi|WR1, stat_v,	/* alle Int. disable */
	res_xi,
};

#define BAUD 0x47

long sioset = 0;        /* ein initialisierter Kanal : entspr. Bit = 1 */
unsigned isinit = 1;	/* Initialisierungsflag fuer Console */
int nsiochan = 8;	/* Anzahl der Terminalkanaele */ 
unsigned siovec = 0x20;	/* Interruptvektor fuer 1. SIO-Kanal */
int console = 1;	/* Device-Nr. d. Systemconsole */
int siomajor = 8;	/* Major-Device-Nr. aller Terminalkanaele */
struct tty sio[8];	/* Terminalstrukturen */

struct sioaddrs {	/* Zuordnung SIO/CTC-Adressen */
	unsigned sio;
	unsigned ctc;
	} sioaddrs[] = {
				/* Adressen siehe sioconst.h */
	S8_0D_A, C8_1_0,
	S8_0D_B, C8_1_1,
	S8_1D_A, C8_1_2,
	S8_1D_B, C8_0_0,
	S16_0D_A, C16_0_0,
	S16_0D_B, C16_0_1,
	S16_1D_A, C16_0_2,
	S16_1D_B, C16_1_0,
}; 
