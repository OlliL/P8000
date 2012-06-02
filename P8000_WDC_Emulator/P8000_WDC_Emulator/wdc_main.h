/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_main.h,v 1.6 2012/06/02 00:51:55 olivleh1 Exp $
 *
 */


#ifndef MAIN_H_
#define MAIN_H_

#define CMD_WR_WDC_RAM      0x18                /* write data to WDC RAM                  */
#define CMD_RD_WDC_RAM      0x08                /* read data from WDC RAM                 */
#define CMD_RD_PARTAB       0x28                /* read Parameter block from WDC RAM      */
#define CMD_WR_PARTAB       0x78                /* write Parameter block to WDC RAM       */
#define CMD_DL_BTTAB        0x48                /* delete Bad Track Table from WDC RAM    */
#define CMD_RD_BTTAB        0x58                /* read Bad Track Table from WDC RAM      */
#define CMD_WR_BTTAB        0x68                /* write Bad Track Table to WDC RAM       */
#define CMD_FMT_BTT         0x24                /* format a track and provide BTT info    */
#define CMD_ST_PARBTT       0xC2                /* store PAR and BTT on harddisk          */
#define CMD_RD_WDCERR       0x38                /* read error statistics                  */
#define CMD_VER_TRACK       0x44                /* verify track                           */
#define CMD_RD_SECTOR       0x01                /* read data beginning at given sector    */

#endif /* MAIN_H_ */
