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
 * $Id: wdc_par.c,v 1.7 2013/04/20 23:22:47 olivleh1 Exp $
 */

#include <avr/pgmspace.h>
#include "wdc_main.h"
#include "wdc_par.h"
#include "uart.h"

/*
 * unformatted ST138
 *
 * 57 44 43 5F 34 2E 32 81 81 81 81 81 81 81 81 81
 * 81 81 81 81 81 81 81 64 00 02 12 64 00 01 81 81
 * 81 81 81 81 81 81 81 81 81 81 EB 0D 00 00 41 00
 * 01 01 81 81 81 81 81 81 81 81 81 81 81 81 81 81
 * 81 81 81 81 81 EB 0D 00 00 24 00 00 81 81 81 81
 * 00 81 00 80 FE 00 00 00 01 81 81 73 00 12 81 01
 * 00 02 00 00 81 81 81 00 41 D7 28 81 81 81 81 81
 * 81 81 81 81 81 81 81 81 81 81 81 81 81 81 81 81
 *
 *
 * fresh formatted ST138
 *
 * 57 44 43 5F 34 2E 32 81 53 54 32 35 31 2D 31 20
 * 20 20 20 20 81 00 00 34 03 06 12 34 03 01 00 00
 * 00 00 00 00 CB D1 FB FD F1 F3 83 59 01 00 01 09
 * 00 01 81 81 81 81 81 81 81 81 81 81 81 81 81 81
 * 81 81 81 81 81 83 59 01 00 6C 00 00 81 81 81 E5
 * 00 81 00 80 FE 00 00 00 01 81 81 33 00 12 81 00
 * 00 00 00 00 81 81 81 09 01 D7 28 81 81 81 81 81
 * 81 81 81 81 81 81 81 81 81 81 81 81 81 81 81 81
 *
 *
 * no harddrive attached:
 *
 * 57 44 43 5F 34 2E 32 81 81 81 81 81 81 81 81 81
 * 81 81 81 81 81 81 81 64 00 02 12 64 00 01 81 81
 * 81 81 81 81 81 81 81 81 81 81 81 81 81 81 40 00
 * 00 00 81 81 81 81 81 81 81 81 81 81 81 81 81 81
 * 81 81 81 81 81 81 81 81 81 81 81 81 81 81 81 81
 * 00 81 00 80 81 81 81 81 81 81 81 33 00 12 81 00
 * 81 81 81 81 81 81 81 00 40 D7 81 81 81 81 81 81
 * 81 81 81 81 81 81 81 81 81 81 81 81 81 81 81 81
 *
 */


uint8_t par_table[512];
const uint8_t
par_tab_new_dsk[128] = {'W',  'D',  'C',  '_',  '4',  '.',  '2',  0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
                        0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x64, 0x00, 0x02, 0x12, 0x64, 0x00, 0x01, 0x81, 0x81,
                        0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, /* init with sa.format ends here */
                        /* to be initiated by controller starts here */             0xEB, 0x0D, 0x00, 0x00, 0x41, 0x00,
                        0x01, 0x01, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
                        0x81, 0x81, 0x81, 0x81, 0x81, 0xEB, 0x0D, 0x00, 0x00, 0x24, 0x00, 0x00, 0x81, 0x81, 0x81, 0x81,
                        0x00, 0x81, 0x00, 0x80, 0xfe, 0x00, 0x00, 0x00, 0x01, 0x81, 0x81, 0x73, 0x00, 0x12, 0x81, 0x01,
                        0x00, 0x02, 0x00, 0x00, 0x81, 0x81, 0x81, 0x00, 0x41, 0xd7, 0x28, 0x81, 0x81, 0x81, 0x81, 0x81,
                        0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
                       };

const uint8_t
par_tab_no_dsk[128]  = {'W',  'D',  'C',  '_',  '4',  '.',  '2',  0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
                        0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x64, 0x00, 0x02, 0x12, 0x64, 0x00, 0x01, 0x81, 0x81,
                        0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, /* init with sa.format ends here */
                        /* to be initiated by controller starts here */             0x81, 0x81, 0x81, 0x81, 0x40, 0x00,
                        0x00, 0x00, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
                        0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
                        0x00, 0x81, 0x00, 0x80, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x33, 0x00, 0x12, 0x81, 0x00,
                        0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x00, 0x40, 0xd7, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
                        0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
                       };

uint8_t valid_disk = 1;
uint8_t initialized;

void wdc_read_par_table ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        buffer[count] = par_table[count];
    } while ( count > 0 );
}

void wdc_write_par_table ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        par_table[count] = buffer[count];
    } while ( count > 0 );
}


void wdc_read_wdc_par ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        buffer[count] = par_table[count + POS_PAR_WDC_PAR];
    } while ( count > 0 );
}

void wdc_write_wdc_par ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        par_table[count + POS_PAR_WDC_PAR] = buffer[count];
    } while ( count > 0 );
}

void wdc_del_wdc_btt()
{
    uint8_t i;
    for ( i = 0; i < 125; i++ )
        par_table[i + POS_PAR_WDC_BTT] = 0x00;
    par_table[122 + POS_PAR_WDC_BTT] = 0xff;
}

void wdc_read_wdc_btt ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        buffer[count] = par_table[count + POS_PAR_WDC_BTT];
    } while ( count > 0 );
}

void wdc_write_wdc_btt ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        par_table[count + POS_PAR_WDC_BTT] = buffer[count];
    } while ( count > 0 );
}

uint16_t wdc_get_btt_count()
{
    return ( ( par_table[0 + POS_PAR_WDC_BTT] | ( par_table[1 + POS_PAR_WDC_BTT] << 8 ) ) );
}

void wdc_set_btt_count ( uint16_t btt_count )
{
    par_table[0 + POS_PAR_WDC_BTT] = ( uint8_t ) btt_count;
    par_table[1 + POS_PAR_WDC_BTT] = ( btt_count >> 8 );
}

uint8_t wdc_add_btt_entry ( uint16_t cylinder, uint8_t head )
{
    uint16_t btt_count = wdc_get_btt_count();

    if ( btt_count < 120 ) {
        /* +2 because btt_count is just the amount of bytes for the BTT - but in front of it, there are two bytes */
        /* storing the amount of bytes for the BTT */
        par_table[btt_count + 2 + POS_PAR_WDC_BTT] = ( cylinder >> 8 );
        btt_count++;
        par_table[btt_count + 2 + POS_PAR_WDC_BTT] = ( uint8_t ) cylinder;
        btt_count++;
        par_table[btt_count + 2 + POS_PAR_WDC_BTT] = ( uint8_t ) head;
        btt_count++;
        wdc_set_btt_count ( btt_count );
        return ( 0 );
    } else {
        return ( ERR_BTT_FULL );
    }
}

uint8_t wdc_get_hdd_sectors()
{
    return ( par_table[18 + POS_PAR_WDC_PAR] );
}

uint8_t wdc_get_hdd_heads()
{
    return ( par_table[17 + POS_PAR_WDC_PAR] );
}
uint16_t wdc_get_hdd_cylinder()
{
    return ( ( par_table[16 + POS_PAR_WDC_PAR] << 8 ) | par_table[15 + POS_PAR_WDC_PAR] );
}

void wdc_set_disk_invalid()
{
    uint8_t i;

    valid_disk = 0;
    for ( i = 0; i < 128; i++ )
        par_table[i] = par_tab_new_dsk[i];
}

void wdc_set_disk_valid()
{
    uint32_t blocks;
    uint16_t zw0, zw1;

    blocks = (uint32_t)wdc_get_hdd_sectors() * (uint32_t)wdc_get_hdd_heads() * (uint32_t) wdc_get_hdd_cylinder()
           - (uint32_t)wdc_get_hdd_sectors() * (uint32_t)wdc_get_hdd_heads();

    zw0 = blocks / 65536;
    zw1 = ( blocks % 65536 ) - 1;

    valid_disk = 1;
    par_table[42] = zw1;
    par_table[43] = zw1 >> 8;
    par_table[44] = zw0;
    par_table[45] = zw0 >> 8;
    par_table[46] = 0x01;
    par_table[47] = 0x09;
    par_table[48] = 0x00;
    par_table[69] = 0x83;
    par_table[70] = 0x59;
    par_table[71] = 0x01;
    par_table[73] = 0x6c;
    par_table[91] = 0x33;
    par_table[95] = 0x00;
}

void wdc_set_no_disk()
{
    uint8_t i;
    valid_disk = 0;
    for ( i = 0; i < 128; i++ )
        par_table[i] = par_tab_no_dsk[i];
}

uint8_t wdc_get_disk_valid()
{
    return valid_disk;
}

uint8_t wdc_get_num_of_drvs()
{
    return par_table[49];
}

void wdc_set_initialized(uint8_t num)
{
    initialized=num;
}

uint8_t wdc_get_initialized()
{
    return initialized;
}