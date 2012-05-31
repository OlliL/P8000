/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_main.h,v 1.5 2012/05/31 20:21:51 olivleh1 Exp $
 *
 */


#ifndef MAIN_H_
#define MAIN_H_

#define CMD_WR_WDC_RAM      0x18                /* P8000 Command: write data to WDC RAM */
#define CMD_RD_WDC_RAM      0x08                /* P8000 Command: read data from WDC RAM */
#define CMD_RD_PARTAB       0x28                /* P8000 Command: read Parameter block from WDC RAM */
#define CMD_WR_PARTAB       0x78                /* P8000 Command: write Parameter block to WDC RAM */
#define CMD_DL_BTTAB        0x48                /* P8000 Command: delete Bad Track Table from WDC RAM */
#define CMD_RD_BTTAB        0x58                /* P8000 Command: read Bad Track Table from WDC RAM */
#define CMD_WR_BTTAB        0x68                /* P8000 Command: write Bad Track Table to WDC RAM */

#endif /* MAIN_H_ */
