/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_main.h,v 1.8 2012/06/03 13:38:03 olivleh1 Exp $
 *
 */


#ifndef MAIN_H_
#define MAIN_H_

#define CMD_RD_SECTOR         0x01              /* read data beginning at given sector              */
#define _CMD_WR_SECTOR        0x02              /* write sector                                     */
#define _CMD_FMT_TRACK        0x04              /* format a track                                   */
#define CMD_RD_WDC_RAM        0x08              /* read data from WDC RAM                           */
#define NO_CMD_RDBTT_SECTOR   0x11              /* read sector taking BTT into account              */
#define NO_CMD_WRBTT_SECTOR   0x12              /* write sector taking BTT into account             */
#define NO_CMD_FMTRD_TRACK    0x14              /* format and reread a track                        */
#define CMD_WR_WDC_RAM        0x18              /* write data to WDC RAM                            */
#define CMD_RD_BLOCK          0x21              /* read data beginning at given block               */
#define CMD_WR_BLOCK          0x22              /* write data beginning at given block              */
#define CMD_FMTBTT_TRACK      0x24              /* format a track and provide BTT info              */
#define CMD_RD_PARTAB         0x28              /* read Parameter block from WDC RAM                */
#define CMD_RD_WDCERR         0x38              /* read error statistics                            */
#define CMD_VER_TRACK         0x44              /* verify track                                     */
#define CMD_DL_BTTAB          0x48              /* delete Bad Track Table from WDC RAM              */
#define CMD_RD_BTTAB          0x58              /* read Bad Track Table from WDC RAM                */
#define CMD_WR_BTTAB          0x68              /* write Bad Track Table to WDC RAM                 */
#define CMD_WR_PARTAB         0x78              /* write Parameter block to WDC RAM                 */
#define NO_CMD_VER_SECTOR     0x81              /* verify sector                                    */
#define NO_CMD_WRRD_SECTOR    0x82              /* write and reread sector                          */
#define NO_CMD_DL_TRACK       0x84              /* delete a track                                   */
#define NO_CMD_WRRDBTT_SECTOR 0x92              /* write and reread sector taking BTT into account  */
#define NO_CMD_CP_BLOCK       0xa1              /* copy block from drive 1 to drive 0               */
#define NO_CMD_WRRD_BLOCK     0xa2              /* write and reread block                           */
#define CMD_ST_PARBTT         0xc2              /* store PAR and BTT on harddisk                    */

#endif /* MAIN_H_ */
