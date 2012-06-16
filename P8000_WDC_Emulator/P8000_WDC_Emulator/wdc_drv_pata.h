/*-
 * Copyright (c) 2012 Oliver Lehmann
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
 * $Id: wdc_drv_pata.h,v 1.6 2012/06/16 19:34:39 olivleh1 Exp $
 */

#ifndef WDC_DRV_PATA_H_
#define WDC_DRV_PATA_H_

extern uint8_t pata_init ();

extern uint8_t pata_read_block ( uint32_t addr, uint8_t *buffer );
extern uint8_t pata_write_block ( uint32_t addr, uint8_t *buffer );

extern uint8_t pata_read_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks );
extern uint8_t pata_write_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks );


#define ATA_STAT_BSY  0x80  // ATA busy
#define ATA_STAT_RDY  0x40  // ATA ready
#define ATA_STAT_DF   0x20  // ATA device fault
#define ATA_STAT_SKC  0x10  // ATA seek complete
#define ATA_STAT_DRQ  0x08  // ATA data request
#define ATA_STAT_CORR 0x04  // ATA corrected
#define ATA_STAT_IDX  0x02  // ATA index
#define ATA_STAT_ERR  0x01  // ATA error

#define ATA_LBA_DRIVE_0 0xE0
#define ATA_LBA_DRIVE_1 0xF0

//ATA commands (from ATA-3),
#define CMD_CFA_ERASE_SECTORS            0xC0
#define CMD_CFA_REQUEST_EXT_ERR_CODE     0x03
#define CMD_CFA_TRANSLATE_SECTOR         0x87
#define CMD_CFA_WRITE_MULTIPLE_WO_ERASE  0xCD
#define CMD_CFA_WRITE_SECTORS_WO_ERASE   0x38
#define CMD_CHECK_POWER_MODE1            0xE5
#define CMD_CHECK_POWER_MODE2            0x98
#define CMD_DEVICE_RESET                 0x08
#define CMD_EXECUTE_DEVICE_DIAGNOSTIC    0x90
#define CMD_FLUSH_CACHE                  0xE7
#define CMD_FORMAT_TRACK                 0x50
#define CMD_IDENTIFY_DEVICE              0xEC
#define CMD_IDENTIFY_DEVICE_PACKET       0xA1
#define CMD_IDENTIFY_PACKET_DEVICE       0xA1
#define CMD_IDLE1                        0xE3
#define CMD_IDLE2                        0x97
#define CMD_IDLE_IMMEDIATE1              0xE1
#define CMD_IDLE_IMMEDIATE2              0x95
#define CMD_INITIALIZE_DRIVE_PARAMETERS  0x91
#define CMD_INITIALIZE_DEVICE_PARAMETERS 0x91
#define CMD_NOP                          0x00
#define CMD_PACKET                       0xA0
#define CMD_READ_BUFFER                  0xE4
#define CMD_READ_DMA                     0xC8
#define CMD_READ_DMA_QUEUED              0xC7
#define CMD_READ_MULTIPLE                0xC4
#define CMD_READ_SECTORS                 0x20
#define CMD_READ_VERIFY_SECTORS          0x40
#define CMD_RECALIBRATE                  0x10
#define CMD_SEEK                         0x70
#define CMD_SET_FEATURES                 0xEF
#define CMD_SET_MULTIPLE_MODE            0xC6
#define CMD_SLEEP1                       0xE6
#define CMD_SLEEP2                       0x99
#define CMD_STANDBY1                     0xE2
#define CMD_STANDBY2                     0x96
#define CMD_STANDBY_IMMEDIATE1           0xE0
#define CMD_STANDBY_IMMEDIATE2           0x94
#define CMD_WRITE_BUFFER                 0xE8
#define CMD_WRITE_DMA                    0xCA
#define CMD_WRITE_DMA_QUEUED             0xCC
#define CMD_WRITE_MULTIPLE               0xC5
#define CMD_WRITE_SECTORS                0x30
#define CMD_WRITE_VERIFY                 0x3C

#define PATA_BLOCKLEN 512

// CB_DH bits 7-4 OF THE DEVICE/HEAD REGISTER
#define ATA_DH_DEV0 0xE0    // select device 0 LBA MODE
#define ATA_DH_DEV1 0xF0    // select device 1 LBA MODE

#endif /* WDC_DRV_PATA_H_ */