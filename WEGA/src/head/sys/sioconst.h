/*
 *   Deklarationen zum Schaltkreis  U880-SIO
 *   SIO-Adressen
 */

/* ------------------- Adressen der internen SIO-Register */

#define WR0	0   /* WRITE - Register */
#define WR1	1
#define WR2	2
#define WR3	3
#define WR4	4
#define WR5	5
#define WR6	6
#define WR7	7
#define WR8	8

#define RDR0	WR0    /* READ - Register */
#define RDR1	WR1
#define RDR2	WR2

/* ------------ SIO - Kommandos */

/**** Read - Register 0 (SIO - Status) ****/
#define	rcv_rdy		0x1	/* receive ready		*/
#define	xmt_rdy		0x4	/* transmitter empty		*/
#define	int_pend	0x2	/* int pending (nur Kanal A)	*/
 
/*--- nur fuer External/Status Interrupt-Mode ---*/
#define	dcd		0x8	/* data carrier dedect		*/
#define	sync_hunt	0x10	/* Sync/Hunt-Zustand		*/
#define ifss 		sync_hunt /* 0 = v24, 1 = ifss		*/
#define	cts		0x20	/* clear to send		*/
#define	tx_under	0x40
#define	siobreak	0x80

/**** Read - Register 1 (SIO - Status) ****/
#define	ovr_err		0x20	/* Rx overrrun error		*/
#define	par_err		0x10	/* parity error			*/
#define	fram_err	0x40	/* crc/framing error		*/
 
/**** Write - Register 0 ****/
#define	abort		0x8	/* send abort			*/
#define	res_xi		0x10	/* reset ext. int		*/
#define	res_ch		0x18	/* reset channel		*/
#define	res_rc1		0x20	/* enable int on next Rx-Character*/
#define	res_tx		0x28	/* reset Tx-int pending		*/
#define	res_err		0x30	/* reset error			*/
#define	sreti		0x38	/* return from int (nur Kanal A)*/
 
#define	res_rxcrc	0x40	/* reset Rx-CRC checker		*/
#define	res_txcrc	0x80	/* reset Tx-CRC generator	*/
#define	res_txlatch	0xc0	/* reset Tx underrun/EOM latch	*/
 
/**** Write - Register 1 ****/
#define	enb_xi		0x1	/* enable ext int		*/
#define	enb_tx		0x2	/* enable Tx int		*/
#define	stat_v		0x4	/* status affect vector	(nur B)	*/
 
#define	enb_rx1		0x8	/* Rx int on first character	*/
#define	enb_rxp		0x10	/* Rx int on all character, 	*/
				/* parity affect vector		*/
#define	enb_rx		0x18	/* Rx int on all character	*/
 
/**** Write - Register 2 ****/
	/* Interrupt-Vektor des SIO (nur fuer Kanal B) */
 
/**** Write - Register 3 ****/
#define	enb_rcv		0x1	/* enable Rx			*/
#define	sync_loadinhib	0x2	/* synchr. character load inhibit*/
#define	addr_search	0x4	/* address search mode		*/
#define	enb_rcvcrc	0x8	/* enable Rx CRC		*/
#define	enter_hunt	0x10	/* enter in hunt phase		*/
#define	enb_auto	0x20	/* auto enable			*/
 
#define	rcv5		0	/* rcv 5 bit/char		*/
#define	rcv6		0x80	/* rcv 6 bit/char		*/
#define	rcv7		0x40	/* rcv 7 bit/char		*/
#define	rcv8		0xc0	/* rcv 8 bit/char		*/
 
/**** Write - Register 4 ****/
#define	parity		0x1	/* enable parity check		*/
#define	even		0x2	/* parity = even		*/
 
#define	sync_mode	0	/* enable synchr mode		*/
#define	stop1		0x4	/* 1 stop bit			*/
#define	stop1x		0x8	/* 1.5 stop bit			*/
#define	stop2		0xc	/* 2 stop bit			*/
 
#define	sync_mono	0	/* 8 bit synchron character	*/
#define	sync_bi		0x10	/* 16 bit synchron character	*/
#define	sdlc		0x20	/* SDLC mode (01111110 sync flag)*/
#define	sync_ext	0x30	/* ext sync mode		*/
 
#define	clkx1		0	/* clk * 1			*/
#define	clkx16		0x40	/* clk * 16			*/
#define	clkx32		0x80	/* clk * 32			*/
#define	clkx64		0xc0	/* clk * 64			*/
 
/**** Write - Register 5 ****/
#define	enb_xmtcrc	0x1	/* enable Tx CRC		*/
#define	rts		0x2	/* ready to send		*/
#define	crc16		0x4	/* SDLC CRC-16			*/
#define	enb_xmt		0x8	/* enable Tx			*/
#define	xmt_break	0x10	/* send break			*/
 
#define	xmt5		0	/* xmt 5 bit/char		*/
#define	xmt6		0x40	/* xmt 6 bit/char		*/
#define	xmt7		0x20	/* xmt 7 bit/char		*/
#define	xmt8		0x60	/* xmt 8 bit/char		*/
#define dtr		0x80

/* ---- Hardwaare-Adressen ---- */

#define S8_0		 0x24
#define S8_0D_A		 S8_0 + 0
#define S8_0D_B		 S8_0 + 2
#define S8_0C_A		 S8_0 + 1
#define S8_0C_B		 S8_0 + 3

#define S8_1		 0x28
#define S8_1D_A		 S8_1 + 0
#define S8_1D_B		 S8_1 + 2
#define S8_1C_A		 S8_1 + 1
#define S8_1C_B		 S8_1 + 3

#define C8_0		 0x8
#define C8_0_0		 C8_0 + 0	/* Baud 3 */
#define C8_0_1		 C8_0 + 1
#define C8_0_2		 C8_0 + 2
#define C8_0_3		 C8_0 + 3

#define C8_1		 0x2c
#define C8_1_0		 C8_1 + 0	/* Baud 0 */
#define C8_1_1		 C8_1 + 1	/* Baud 1 */
#define C8_1_2		 C8_1 + 2	/* Baud 2 */
#define C8_1_3		 C8_1 + 3

#define S16_0		 0xFF81
#define S16_0D_A	 S16_0 + 0
#define S16_0D_B	 S16_0 + 2
#define S16_0C_A	 S16_0 + 4
#define S16_0C_B	 S16_0 + 6

#define S16_1		 0xFF89		
#define S16_1D_A	 S16_1 + 0
#define S16_1D_B	 S16_1 + 2
#define S16_1C_A	 S16_1 + 4
#define S16_1C_B	 S16_1 + 6

#define C16_0		 0xFFA9
#define C16_0_0		 C16_0 + 0	/* Baud 0 */
#define C16_0_1		 C16_0 + 2	/* Baud 1 */
#define C16_0_2		 C16_0 + 4	/* Baud 2 */
#define C16_0_3		 C16_0 + 6	/* Next */

#define C16_1		 0xFFB1
#define C16_1_0		 C16_1 + 0	/* Baud 3 */
#define C16_1_1		 C16_1 + 2
#define C16_1_2		 C16_1 + 4
#define C16_1_3		 C16_1 + 6

#define ctl	4	/* Maske f. Steuerwortadresse des Kanals */
#define ca	0xfffd	/* Maske f. Kanal A */
#define cb	2	/* Maske f. Kanal B */
