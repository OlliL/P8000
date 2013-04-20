/*-
 * Copyright (c) 2012, 2013 Oliver Lehmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * $Id: wdc_main.h,v 1.13 2013/04/20 23:22:47 olivleh1 Exp $
 */


#ifndef MAIN_H_
#define MAIN_H_

#define CMD_RD_SECTOR         0x01              /* read data beginning at given sector              */
#define ___CMD_WR_SECTOR      0x02              /* write sector                                     */
#define ___CMD_FMT_TRACK      0x04              /* format a track                                   */
#define CMD_RD_WDC_RAM        0x08              /* read data from WDC RAM                           */
#define ___CMD_RDBTT_SECTOR   0x11              /* read sector taking BTT into account              */
#define ___CMD_WRBTT_SECTOR   0x12              /* write sector taking BTT into account             */
#define CMD_FMTRD_TRACK       0x14              /* format and reread a track                        */
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
#define ___CMD_VER_SECTOR     0x81              /* verify sector                                    */
#define ___CMD_WRRD_SECTOR    0x82              /* write and reread sector                          */
#define ___CMD_DL_TRACK       0x84              /* delete a track                                   */
#define ___CMD_WRRDBTT_SECTOR 0x92              /* write and reread sector taking BTT into account  */
#define ___CMD_CP_BLOCK       0xa1              /* copy block from drive 1 to drive 0               */
#define ___CMD_WRRD_BLOCK     0xa2              /* write and reread block                           */
#define CMD_ST_PARBTT         0xc2              /* store PAR and BTT on hard disk                   */

#define ERR_STANDARD          0x01
#define ERR_SECT_N_ON_SURFACE 0x0A
#define ERR_BTT_FULL          0x16
#define ERR_NO_DRIVE_READY    0x27
#define ERR_CYL0_NOT_READABLE 0x31

#endif /* MAIN_H_ */
