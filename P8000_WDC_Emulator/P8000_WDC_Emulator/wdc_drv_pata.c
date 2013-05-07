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
 * $Id: wdc_drv_pata.c,v 1.14 2013/05/07 17:38:06 olivleh1 Exp $
 */

#include "wdc_config.h"
#include <avr/io.h>
#include "wdc_avr.h"
#include "uart.h"
#include "wdc_drv_pata.h"

uint8_t pata_bsy ();
uint8_t pata_rdy ();
uint8_t pata_drq ();
uint8_t pata_err ();
void    ata_identify ();
void    pata_set_highbyte ( uint8_t byte );
void    pata_set_lowbyte ( uint8_t byte );
uint8_t pata_get_highbyte ();
uint8_t pata_get_lowbyte ();
void    set_io_register ( uint8_t ioreg );
uint8_t read_io_register ( uint8_t ioreg );
void    write_io_register ( uint8_t ioreg, uint8_t byte );
void    pata_read_bytes ( uint8_t *buffer, uint16_t bytes );


/*
 *                                                  +----- CS0
 *                                                  |+---- CS1
 *                                                  ||+--- DA0
 *                                                  |||+-- DA1
 *                                                  ||||+- DA2
 *                                                  ||||| */
#define PATA_W_DEVICE_CONTROL_REGISTER   0x0B  /* 0b10011 */
#define PATA_R_STATUS_REGISTER           0x0F  /* 0b01111 */
#define PATA_R_ALTERNATE_STATUS_REGISTER 0x0B  /* 0b10011 */
#define PATA_R_ERROR_REGISTER            0x0C  /* 0b01100 */
#define PATA_W_FEATURE_REGISTER          0x0C  /* 0b01100 */
#define PATA_W_COMMAND_REGISTER          0x0F  /* 0b01111 */

#define PATA_RW_DATA_REGISTER            0x08  /* 0b01000 */
#define PATA_RW_SECTOR_COUNT_REGISTER    0x0A  /* 0b01010 */
#define PATA_RW_SECTOR_NUMBER_REGISTER   0x0E  /* 0b01110 */
#define PATA_RW_CYLINDER_LOW_REGISTER    0x09  /* 0b01001 */
#define PATA_RW_CYLINDER_HIGH_REGISTER   0x0D  /* 0b01101 */
#define PATA_RW_DEVICE_HEAD_REGISTER     0x0B  /* 0b01011 */

#define ata_ctl_high() ata_cs0_disable(); ata_cs1_disable(); ata_da0_disable(); ata_da1_disable(); ata_da2_disable();


/*
 * Private functions
 */


uint8_t pata_rdy ()
{
    return ( read_io_register ( PATA_R_STATUS_REGISTER ) & ATA_STAT_RDY ) ? 1 : 0;
}

uint8_t pata_bsy ()
{
    return ( read_io_register ( PATA_R_STATUS_REGISTER ) & ATA_STAT_BSY ) ? 1 : 0;
}

uint8_t pata_drq ()
{
    return ( read_io_register ( PATA_R_STATUS_REGISTER ) & ATA_STAT_DRQ ) ? 1 : 0;
}

uint8_t pata_err ()
{
    return ( read_io_register ( PATA_R_STATUS_REGISTER ) & ATA_STAT_ERR ) ? 1 : 0;
}

void set_io_register ( uint8_t ioreg )
{
    if ( !( ioreg & ( 1 << 0 ) ) ) {
        ata_da2_enable();
    }
    if ( !( ioreg & ( 1 << 1 ) ) ) {
        ata_da1_enable();
    }
    if ( !( ioreg & ( 1 << 2 ) ) ) {
        ata_da0_enable();
    }
    if ( !( ioreg & ( 1 << 3 ) ) ) {
        ata_cs1_enable();
    }
    if ( !( ioreg & ( 1 << 4 ) ) ) {
        ata_cs0_enable();
    }
}

void pata_set_lowbyte ( uint8_t byte )
{
    port_data_set ( byte );
    enable_rdwrtoata();
    nop();
    nop();
    disable_rdwrtoata();
}

void pata_set_highbyte ( uint8_t byte )
{
    port_data_set ( byte );
    enable_atalatch();
    nop();
    nop();
    disable_atalatch();
}

uint8_t pata_get_lowbyte ()
{
    uint8_t byte;

    enable_rdwrtoata();
    nop();
    nop();
    nop();
    byte = port_data_get();
    disable_rdwrtoata();

    return byte;
}

uint8_t pata_get_highbyte ()
{
    uint8_t byte;

    enable_atalatch();
    nop();
    nop();
    byte = port_data_get();
    disable_atalatch();

    return byte;
}

uint8_t read_io_register ( uint8_t ioreg )
{
    uint8_t byte;

    configure_port_data_read();

    set_io_register ( ioreg );
    ata_rd_enable();
    byte = pata_get_lowbyte();
    ata_rd_disable();
    ata_ctl_high();

    return byte;
}

void write_io_register ( uint8_t ioreg, uint8_t byte )
{
    configure_port_data_write();

    set_io_register ( ioreg );
    ata_wr_enable();
    pata_set_lowbyte ( byte );
    ata_wr_disable();
    ata_ctl_high();
}

void pata_read_bytes ( uint8_t *buffer, uint16_t bytes )
{
    uint16_t i;
    uint8_t  blocks;

    configure_port_data_read();

    for ( blocks = bytes / PATA_BLOCKLEN; blocks; blocks-- ) {
        /* Wait for BSY goes low and DRQ goes high */
        while ( pata_bsy() & !pata_drq() ) {}

        /* PATA_RW_DATA_REGISTER */
        ata_cs0_enable();
        ata_da0_enable();
        ata_da1_enable();
        ata_da2_enable();
        ata_rd_enable();

        for ( i = 0; i < PATA_BLOCKLEN / 2; i++ ) {
            *buffer++ = pata_get_lowbyte();
            *buffer++ = pata_get_highbyte();
        }

        ata_rd_disable();
        ata_cs0_disable();
        ata_da0_disable();
        ata_da1_disable();
        ata_da2_disable();
    }
}

void pata_write_bytes ( uint8_t *buffer, uint16_t bytes )
{
    uint16_t i, a;
    uint8_t  blocks;

    a = 0;
    for ( blocks = bytes / PATA_BLOCKLEN; blocks; blocks-- ) {
        /* Wait for BSY goes low and DRQ goes high */
        while ( pata_bsy() & !pata_drq() ) {}

        configure_port_data_write();

        /* PATA_RW_DATA_REGISTER */
        ata_cs0_enable();
        ata_da0_enable();
        ata_da1_enable();
        ata_da2_enable();
        ata_wr_enable();

        for ( i = 0; i < PATA_BLOCKLEN / 2; i++ ) {
            pata_set_highbyte ( buffer[a + 1] );
            pata_set_lowbyte ( buffer[a] );
            a += 2;
        }

        ata_wr_disable();
        ata_cs0_disable();
        ata_da0_disable();
        ata_da1_disable();
        ata_da2_disable();
    }
}

void ata_identify ()
{
    uint8_t  buffer[512], i;
    uint16_t word[256], n;

    while ( pata_bsy() ) {}
    write_io_register ( PATA_W_COMMAND_REGISTER, CMD_IDENTIFY_DEVICE );

    pata_read_bytes ( buffer, 512 );

    i = 0;
    for ( n = 0; n < 512; n += 2 ) {
        word[i] = ( ( buffer[n + 1] << 8 ) | buffer[n] );
        i++;
    }

    uart_putstring ( PSTR ( "INFO: Number of logical cylinders: " ), false );
    uart_putw_dec ( word[1] );
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Number of logical heads: " ), false );
    uart_putw_dec ( word[3] );
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Number of logical sectors per logical track: " ), false );
    uart_putw_dec ( word[6] );
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Serial number: " ), false );
    for ( i = 10; i <= 19; i++ ) {
        uart_putc ( word[i] >> 8 );
        uart_putc ( word[i] & 0x00FF );
    }
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Firmware revision: " ), false );
    for ( i = 23; i <= 26; i++ ) {
        uart_putc ( word[i] >> 8 );
        uart_putc ( word[i] & 0x00FF );
    }
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Model number: " ), false );
    for ( i = 27; i <= 46; i++ ) {
        uart_putc ( word[i] >> 8 );
        uart_putc ( word[i] & 0x00FF );
    }
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Capabilities: " ), false );
    uart_putw_dec ( word[49] );
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Number of current logical cylinders: " ), false );
    uart_putw_dec ( word[54] );
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Number of current logical heads: " ), false );
    uart_putw_dec ( word[55] );
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Number of current logical sectors per track: " ), false );
    uart_putw_dec ( word[56] );
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Current capacity in sectors: " ), false );
    uart_putdw_dec ( word[57] | (uint32_t)word[58] << 16 );
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Total number of user addressable sectors (LBA mode only): " ), false );
    uart_putdw_dec ( word[60] | (uint32_t)word[61] << 16 );
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Minimum PIO transfer cycle time without flow control: " ), false );
    uart_putdw_dec ( word[67] );
    uart_putstring ( PSTR ( "ns" ), true );
}

/*
 * Public functions
 */

uint8_t pata_init ()
{
    uart_putstring ( PSTR ( "INFO: PATA init start" ), true );
    while ( ( !pata_rdy() ) & pata_bsy() ) {}

    /*set drive 0 to LBA mode*/
    write_io_register ( PATA_RW_DEVICE_HEAD_REGISTER, ATA_LBA_DRIVE_0 );
    while ( ( !pata_rdy() ) & pata_bsy() ) {}

    /*recalibrate*/
    write_io_register ( PATA_W_COMMAND_REGISTER, CMD_RECALIBRATE );
    while ( pata_bsy() ) {}


    uart_putstring ( PSTR ( "INFO: PATA disk has been found" ), true );
    ata_identify();

    return 0;
}

uint8_t pata_read_block ( uint32_t addr, uint8_t *buffer )
{
    return pata_read_multiblock ( addr, buffer, 1 );
}

uint8_t pata_write_block ( uint32_t addr, uint8_t *buffer )
{
    return pata_write_multiblock ( addr, buffer, 1 );
}

typedef union {
    uint32_t value32;
    struct {
        uint8_t ll;
        uint8_t lh;
        uint8_t hl;
        uint8_t hh;
    } value8;
} sint32;

uint8_t pata_read_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks )
{
    sint32 x;

    x.value32 = addr;

    configure_port_data_write();
    ata_wr_enable();

    /*  PATA_RW_SECTOR_COUNT_REGISTER */
    ata_cs0_enable();
    ata_da0_enable();
    ata_da2_enable();
    pata_set_lowbyte ( numblocks );

    /* PATA_RW_SECTOR_NUMBER_REGISTER */
    ata_da0_disable();
    pata_set_lowbyte ( x.value8.ll );

    /*  PATA_RW_CYLINDER_LOW_REGISTER */
    ata_da0_enable();
    ata_da1_enable();
    ata_da2_disable();
    pata_set_lowbyte ( x.value8.lh );

    /* PATA_RW_CYLINDER_HIGH_REGISTER */
    ata_da0_disable();
    pata_set_lowbyte ( x.value8.hl );

    /* PATA_RW_DEVICE_HEAD_REGISTER */
    ata_da0_enable();
    ata_da1_disable();
    pata_set_lowbyte ( ( x.value8.hh & 0x0F ) | ATA_LBA_DRIVE_0 );

    /* PATA_W_COMMAND_REGISTER */
    ata_da0_disable();
    pata_set_lowbyte ( CMD_READ_SECTORS );
    ata_cs0_disable();

    ata_wr_disable();

    pata_read_bytes ( buffer, numblocks * PATA_BLOCKLEN );

    return pata_err();
}

uint8_t pata_write_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks )
{
    sint32 x;

    x.value32 = addr;

    configure_port_data_write();
    ata_wr_enable();

    /*  PATA_RW_SECTOR_COUNT_REGISTER */
    ata_cs0_enable();
    ata_da0_enable();
    ata_da2_enable();
    pata_set_lowbyte ( numblocks );

    /* PATA_RW_SECTOR_NUMBER_REGISTER */
    ata_da0_disable();
    pata_set_lowbyte ( x.value8.ll );

    /*  PATA_RW_CYLINDER_LOW_REGISTER */
    ata_da0_enable();
    ata_da1_enable();
    ata_da2_disable();
    pata_set_lowbyte ( x.value8.lh );

    /* PATA_RW_CYLINDER_HIGH_REGISTER */
    ata_da0_disable();
    pata_set_lowbyte ( x.value8.hl );

    /* PATA_RW_DEVICE_HEAD_REGISTER */
    ata_da0_enable();
    ata_da1_disable();
    pata_set_lowbyte ( ( x.value8.hh & 0x0F ) | ATA_LBA_DRIVE_0 );

    /* PATA_W_COMMAND_REGISTER */
    ata_da0_disable();
    pata_set_lowbyte ( CMD_WRITE_SECTORS );
    ata_cs0_disable();

    ata_wr_disable();

    pata_write_bytes ( buffer, numblocks * PATA_BLOCKLEN );

    /* wait until drive completed write */
    while ( pata_bsy() ) {}

    return pata_err();
}