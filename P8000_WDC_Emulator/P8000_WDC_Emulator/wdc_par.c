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
 * $Id: wdc_par.c,v 1.2 2012/06/07 18:01:02 olivleh1 Exp $
 */

#include <avr/pgmspace.h>
#include "wdc_par.h"

uint8_t par_table[512] = {'W',  'D',  'C',  '_',  '4',  '.',  '2', 0x00,  'R',  'O',  'B',  ' ',  'K',  '5',  '5',  '0',
                          '4',  '.',  '5',  '0', 0x00, 0x00, 0x00, 0x00, 0x04, 0x05, 0x12, 0x00, 0x04, 0x01, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa5, 0x67, 0x01, 0x00, 0x01, 0x01,
                          0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                         };


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
        return ( 1 );
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
