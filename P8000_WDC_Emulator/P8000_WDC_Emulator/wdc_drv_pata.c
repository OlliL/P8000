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
 * $Id: wdc_drv_pata.c,v 1.5 2012/06/16 00:33:26 olivleh1 Exp $
 */

#include "wdc_config.h"
#include <avr/io.h>
#include <util/delay.h>
#include "wdc_avr.h"
#include "uart.h"
#include "wdc_drv_pata.h"

uint8_t pata_bsy();
uint8_t pata_rdy();
uint8_t pata_drq();
void ata_identify();
void pata_set_highbyte(uint8_t byte);
void pata_set_lowbyte(uint8_t byte);
uint8_t pata_get_highbyte();
uint8_t pata_get_lowbyte();
void set_io_register( uint8_t ioreg );
uint8_t read_io_register( uint8_t ioreg );
void write_io_register( uint8_t ioreg, uint8_t byte );
uint16_t pata_read_single_word ();


/*
 *                                                 +----- CS0
 *                                                 |+---- CS1
 *                                                 ||+--- DA0
 *                                                 |||+-- DA1
 *                                                 ||||+- DA2
 *                                                 |||||*/
#define PATA_RD_STATUS_REGISTER         0x0F  /* 0b01111*/
#define PATA_WR_COMMAND_REGISTER        0x0F  /* 0b01111*/
#define PATA_WR_DEVICE_CONTROL_REGISTER 0x0B  /* 0b01011*/
#define PATA_RD_DATA_REGISTER           0x08  /* 0b01000*/

#define ata_ctl_high() ata_cs0_disable();ata_cs1_disable();ata_da0_disable();ata_da1_disable();ata_da2_disable();


/*
 * Private functions
 */


uint8_t pata_rdy()
{
    return (read_io_register ( PATA_RD_STATUS_REGISTER ) & ATA_STAT_RDY )?1:0;
}

uint8_t pata_bsy()
{
    return (read_io_register ( PATA_RD_STATUS_REGISTER ) & ATA_STAT_BSY )?1:0;
}

uint8_t pata_drq()
{
    return (read_io_register ( PATA_RD_STATUS_REGISTER ) & ATA_STAT_DRQ )?1:0;
}

void set_io_register ( uint8_t ioreg )
{
    if ( ! ( ioreg & ( 1 << 0 ) ) )
        ata_da2_enable();
    if ( ! ( ioreg & ( 1 << 1 ) ) )
        ata_da1_enable();
    if ( ! ( ioreg & ( 1 << 2 ) ) )
        ata_da0_enable();
    if ( ! ( ioreg & ( 1 << 3 ) ) )
        ata_cs1_enable();
    if ( ! ( ioreg & ( 1 << 4 ) ) )
        ata_cs0_enable();
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
    byte = port_data_get();
    disable_rdwrtoata();

    return(byte);
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

uint16_t pata_read_single_word ()
{
    uint16_t word;
    uint8_t byte_l;
    uint8_t byte_h;

    configure_port_data_read();

    set_io_register ( PATA_RD_DATA_REGISTER );
    ata_rd_enable();
    byte_l = pata_get_lowbyte();
    byte_h = pata_get_highbyte();
    ata_rd_disable();
    ata_ctl_high();

    word = byte_h << 8;
    word |= byte_l;

    return ( word );
}

void ata_identify()
{
    uint32_t dword;
    uint16_t word;
    uint8_t byte_l, byte_h;

    while ( pata_bsy() );
    write_io_register ( PATA_WR_COMMAND_REGISTER, CMD_IDENTIFY_DEVICE );

    /* Wait for BSY goes low and DRQ goes high */
    while ( pata_bsy() & !pata_drq() );

    word = pata_read_single_word(); // General configuration bit
    word = pata_read_single_word();
    uart_puts_p ( PSTR ( "Number of logical cylinders: " ) );
    uart_putw_dec ( word );
    uart_putc ( '\n' );
    word = pata_read_single_word(); // Specific configuration
    word = pata_read_single_word();
    uart_puts_p ( PSTR ( "Number of logical heads: " ) );
    uart_putw_dec ( word );
    uart_putc ( '\n' );
    word = pata_read_single_word();
    word = pata_read_single_word();
    word = pata_read_single_word();
    uart_puts_p ( PSTR ( "Number of logical sectors per logical track: " ) );
    uart_putw_dec ( word );
    uart_putc ( '\n' );
    word = pata_read_single_word(); // CompactFlash
    word = pata_read_single_word(); // CompactFlash
    word = pata_read_single_word(); // Retired
    uart_puts_p ( PSTR ( "Serial number: " ) );
    for ( uint8_t i = 0; i < 10; i++ ) {
        word = pata_read_single_word();
        byte_h = word >> 8;
        byte_l = word & 0x00FF;
        uart_putc ( byte_h );
        uart_putc ( byte_l );
    }
    uart_putc ( '\n' );
    word = pata_read_single_word(); // Retired
    word = pata_read_single_word(); // Retired
    word = pata_read_single_word(); // Obsolete
    uart_puts_p ( PSTR ( "Firmware revision: " ) );
    for ( uint8_t i = 0; i < 4; i++ ) {
        word = pata_read_single_word();
        byte_h = word >> 8;
        byte_l = word & 0x00FF;
        uart_putc ( byte_h );
        uart_putc ( byte_l );
    }
    uart_putc ( '\n' );
    uart_puts_p ( PSTR ( "Model number: " ) );
    for ( uint8_t i = 0; i < 20; i++ ) {
        word = pata_read_single_word();
        byte_h = word >> 8;
        byte_l = word & 0x00FF;
        uart_putc ( byte_h );
        uart_putc ( byte_l );
    }
    uart_putc ( '\n' );
    word = pata_read_single_word(); // no idea
    word = pata_read_single_word(); // Reserved
    word = pata_read_single_word(); // Capabilities
    word = pata_read_single_word(); // Capabilities
    word = pata_read_single_word(); // Obsolete
    word = pata_read_single_word(); // Obsolete
    word = pata_read_single_word(); // Reserved
    word = pata_read_single_word();
    uart_puts_p ( PSTR ( "Number of current logical cylinders: " ) );
    uart_putw_dec ( word );
    uart_putc ( '\n' );
    word = pata_read_single_word();
    uart_puts_p ( PSTR ( "Number of current logical heads: " ) );
    uart_putw_dec ( word );
    uart_putc ( '\n' );
    word = pata_read_single_word();
    uart_puts_p ( PSTR ( "Number of current logical sectors per track: " ) );
    uart_putw_dec ( word );
    uart_putc ( '\n' );
    word = pata_read_single_word();
    dword = word;
    word = pata_read_single_word();
    dword |= ( uint32_t ) word << 16;
    uart_puts_p ( PSTR ( "Current capacity in sectors: " ) );
    uart_putdw_dec ( dword );
    uart_putc ( '\n' );
    word = pata_read_single_word(); // Reserved
    word = pata_read_single_word();
    dword = word;
    word = pata_read_single_word();
    dword |= ( uint32_t ) word << 16;
    uart_puts_p ( PSTR ( "Total number of user addressable sectors (LBA mode only): " ) );
    uart_putdw_dec ( dword );
    uart_putc ( '\n' );
    word = pata_read_single_word(); // Obsolete
    for ( uint8_t i = 0; i < 193; i++ )
        word = pata_read_single_word(); // ignore it
}



/*
 * Public functions
 */

uint8_t pata_init ()
{

    // is needed for some disks (for example Maxtor 6L080J4)
    _delay_ms ( 200 );

    uart_puts_p ( PSTR ( "Init start\n" ) );
    while ( ( !pata_rdy() ) & pata_bsy() );

    /*set drive 0 to LBA mode*/
    write_io_register ( PATA_WR_DEVICE_CONTROL_REGISTER, ATA_LBA_DRIVE_0 );
    while ( ( !pata_rdy() ) & pata_bsy() );

    /*recalibrate*/
    write_io_register ( PATA_WR_COMMAND_REGISTER, CMD_RECALIBRATE );
    while ( pata_bsy() );

    uart_puts_p ( PSTR ( "disk is now ready\n" ) );

    ata_identify();

    return 1;
}

uint8_t pata_read_block ( uint32_t addr, uint8_t *buffer )
{
    return 1;
}

uint8_t pata_write_block ( uint32_t addr, uint8_t *buffer )
{
    return 1;
}

uint8_t pata_read_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks )
{
    return 1;
}

uint8_t pata_write_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks )
{
    return 1;
}
