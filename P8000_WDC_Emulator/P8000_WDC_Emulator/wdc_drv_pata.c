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
 * $Id: wdc_drv_pata.c,v 1.4 2012/06/15 22:42:42 olivleh1 Exp $
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
void pata_cmd ( uint8_t cmd );
void ata_identify();

#define ata_ctl_high() ata_cs0_disable();ata_cs1_disable();ata_da0_disable();ata_da1_disable();ata_da2_disable();

void pata_set_highbyte ( uint8_t byte )
{
    port_data_set ( byte );
    enable_atalatch();
    nop();
    nop();
    init_addressdecoder();
}

void pata_set_lowbyte ( uint8_t byte )
{
    port_data_set ( byte );
}

void pata_send_data()
{
    enable_rdwrtoata();
    _delay_us ( 10 );
    init_addressdecoder();
}


uint8_t pata_get_lowbyte ()
{
    nop();
    nop();
    return port_data_get();
}

uint8_t pata_get_highbyte ()
{
    uint8_t byte;
    enable_atalatch();
    nop();
    nop();
    nop();
    byte = port_data_get();
    init_addressdecoder();
    return byte;
}

uint8_t pata_read_byte()
{
    uint8_t byte;

    configure_port_data_read();
    ata_rd_enable();
    enable_rdwrtoata();
    _delay_us ( 1 );
    byte = pata_get_lowbyte();
    init_addressdecoder();
    ata_rd_disable();

    return byte;
}

void pata_write_byte ( uint8_t byte )
{
    configure_port_data_write();
    pata_set_lowbyte ( byte );
    enable_rdwrtoata();
    ata_wr_enable();
    _delay_us ( 1 );
    ata_wr_disable();
    init_addressdecoder();

}


uint8_t ata_get_status_byte ( void )
{
    uint8_t byte;
    ata_cs0_enable();
    byte = pata_read_byte();
    ata_ctl_high();

    return byte;
}

uint8_t ata_get_alt_status_byte ( void )
{
    uint8_t byte;
    ata_cs1_enable();
    ata_da0_enable();
    byte = pata_read_byte();
    ata_ctl_high();

    return byte;
}


uint16_t pata_read_word ( uint8_t ofs, uint8_t count )
{
    uint16_t word;
    uint8_t byte_l;
    uint8_t byte_h;
    uint8_t c = 0;

    ata_da0_enable();
    ata_da1_enable();
    ata_da2_enable();
    ata_cs0_enable();

    do {

        ata_rd_enable();
        enable_rdwrtoata();
        nop();
        nop();
        nop();
        byte_l = port_data_get();
        disable_rdwrtoata();
        enable_atalatch();
        nop();
        nop();
        nop();
        byte_h = port_data_get();
        disable_atalatch();
        ata_rd_disable();

        if ( count && ( c >= ofs ) ) {
            uart_putc ( byte_h );
            uart_putc ( byte_l );
            count--;
        }
    } while ( ++c );

    ata_ctl_high();

    word = byte_h << 8;
    word |= byte_l;

    return ( word );
}


uint16_t pata_read_single_word ()
{
    uint16_t word;
    uint8_t byte_l;
    uint8_t byte_h;

    ata_da0_enable();
    ata_da1_enable();
    ata_da2_enable();
    ata_cs0_enable();

    ata_rd_enable();
    enable_rdwrtoata();
    nop();
    nop();
    nop();
    byte_l = port_data_get();
    disable_rdwrtoata();
    enable_atalatch();
    nop();
    nop();
    nop();
    byte_h = port_data_get();
    disable_atalatch();
    ata_rd_disable();

    ata_ctl_high();

    word = byte_h << 8;
    word |= byte_l;

    return ( word );
}

uint8_t pata_init ()
{
    
    // is needed for some disks (for example Maxtor 6L080J4)
    _delay_ms ( 100 );

    uart_puts_p ( PSTR ( "Init start\n" ) );

    while ( ( !pata_rdy() ) & pata_bsy() );

    uart_putc_hex ( ata_get_status_byte() );

    /*device register*/
    ata_da0_enable();
    ata_cs0_enable();
#ifdef PIO
    pata_write_byte ( 0 );          // Drive 0, PIO
#else
    pata_write_byte ( 0xE0 );          // Drive 0, LBA
#endif
    ata_ctl_high();

#ifdef PIO
    while ( pata_bsy() );

    //device control
    ata_da0_enable();
    ata_cs1_enable();
    pata_write_byte ( 0x40 | 0x20 ); // reset + no interrupt
    ata_ctl_high();

    _delay_ms ( 20 );

    ata_da0_enable();
    ata_cs1_enable();
    pata_write_byte ( 0x20 );     // no interrupt
    ata_ctl_high();

    _delay_ms ( 20 );

    while ( ( !pata_rdy() ) & pata_bsy() );

    //feature register
    ata_da1_enable();
    ata_da2_enable();
    ata_cs0_enable();
    pata_write_byte ( 3 );      // PIO
    ata_ctl_high();

    //sector count
    ata_da0_enable();
    ata_da2_enable();
    ata_cs0_enable();
    pata_write_byte ( 1 );
    ata_ctl_high();

    //command
    ata_cs0_enable();
    pata_write_byte ( 0xEF );   // Set Features
    ata_ctl_high();
#else

    while ( ( !pata_rdy() ) & pata_bsy() );
    /*command*/
    ata_cs0_enable();
    pata_write_byte ( 0x10 );   // Recalibrate
    ata_ctl_high();
#endif
    while ( pata_bsy() );

    uart_puts_p ( PSTR ( "disk is now ready\n" ) );

    ata_identify();

    return 1;
}


void ata_identify()
{
    uint32_t dword;
    uint16_t word;
    uint8_t byte_l,byte_h;

    /*command*/
    while ( pata_bsy() );
    ata_cs0_enable();
    pata_write_byte ( 0xEc );   // Identify
    ata_ctl_high();

    /* Wait for BSY goes low and DRQ goes high */
    while ( ( ata_get_status_byte() & ( ATA_STAT_BSY | ATA_STAT_DRQ ) ) != ATA_STAT_DRQ );

    word = pata_read_single_word(); // General configuration bit
    word = pata_read_single_word();
    uart_puts_p ( PSTR ( "Number of logical cylinders: " ) );
    uart_putw_dec(word);
    uart_putc('\n');
    word = pata_read_single_word(); // Specific configuration
    word = pata_read_single_word();
    uart_puts_p ( PSTR ( "Number of logical heads: " ) );
    uart_putw_dec(word);
    uart_putc('\n');
    word = pata_read_single_word();
    word = pata_read_single_word();
    word = pata_read_single_word();
    uart_puts_p ( PSTR ( "Number of logical sectors per logical track: " ) );
    uart_putw_dec(word);
    uart_putc('\n');
    word = pata_read_single_word(); // CompactFlash
    word = pata_read_single_word(); // CompactFlash
    word = pata_read_single_word(); // Retired
    uart_puts_p ( PSTR ( "Serial number: " ) );
    for(uint8_t i=0;i<10;i++) {
        word = pata_read_single_word();
        byte_h = word >> 8;
        byte_l = word & 0x00FF;
        uart_putc(byte_h);
        uart_putc(byte_l);
    }
    uart_putc('\n');
    word = pata_read_single_word(); // Retired
    word = pata_read_single_word(); // Retired
    word = pata_read_single_word(); // Obsolete
    uart_puts_p ( PSTR ( "Firmware revision: " ) );
    for(uint8_t i=0;i<4;i++) {
        word = pata_read_single_word();
        byte_h = word >> 8;
        byte_l = word & 0x00FF;
        uart_putc(byte_h);
        uart_putc(byte_l);
    }
    uart_putc('\n');
    uart_puts_p ( PSTR ( "Model number: " ) );
    for(uint8_t i=0;i<20;i++) {
        word = pata_read_single_word();
        byte_h = word >> 8;
        byte_l = word & 0x00FF;
        uart_putc(byte_h);
        uart_putc(byte_l);
    }
    uart_putc('\n');
    word = pata_read_single_word(); // no idea
    word = pata_read_single_word(); // Reserved
    word = pata_read_single_word(); // Capabilities
    word = pata_read_single_word(); // Capabilities
    word = pata_read_single_word(); // Obsolete
    word = pata_read_single_word(); // Obsolete
    word = pata_read_single_word(); // Reserved
    word = pata_read_single_word();
    uart_puts_p ( PSTR ( "Number of current logical cylinders: " ) );
    uart_putw_dec(word);
    uart_putc('\n');
    word = pata_read_single_word();
    uart_puts_p ( PSTR ( "Number of current logical heads: " ) );
    uart_putw_dec(word);
    uart_putc('\n');
    word = pata_read_single_word();
    uart_puts_p ( PSTR ( "Number of current logical sectors per track: " ) );
    uart_putw_dec(word);
    uart_putc('\n');
    word = pata_read_single_word();
    dword = word;
    word = pata_read_single_word();
    dword |= (uint32_t)word<<16;
    uart_puts_p ( PSTR ( "Current capacity in sectors: " ) );
    uart_putdw_dec(dword);
    uart_putc('\n');
    word = pata_read_single_word(); // Reserved
    word = pata_read_single_word();
    dword = word;
    word = pata_read_single_word();
    dword |= (uint32_t)word <<16;
    uart_puts_p ( PSTR ( "Total number of user addressable sectors (LBA mode only): " ) );
    uart_putdw_dec(dword);
    uart_putc('\n');
    word = pata_read_single_word(); // Obsolete
    for(uint8_t i=0;i<193;i++)
    word = pata_read_single_word(); // ignore it
}

uint8_t pata_rdy()
{
    uint8_t i;
    if ( ata_get_status_byte() & ATA_STAT_RDY )
        i = 1;
    else
        i = 0;
    return i;
}

uint8_t pata_bsy()
{
    uint8_t i;
    if ( ata_get_status_byte() & ATA_STAT_BSY )
        i = 1;
    else
        i = 0;
    return i;
}

uint8_t pata_drq()
{
    uint8_t i;
    if ( ata_get_status_byte() & ATA_STAT_DRQ )
        i = 1;
    else
        i = 0;
    return i;
}

void pata_cmd ( uint8_t cmd )
{
    ata_cs0_enable();
    pata_write_byte ( cmd );
    ata_ctl_high();
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
