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
 * $Id: wdc_if_p8000.c,v 1.10 2012/06/12 17:43:38 olivleh1 Exp $
 */

#include "wdc_config.h"
#include <avr/io.h>
#include <util/delay.h>
#include "wdc_avr.h"
#include "wdc_if_p8000.h"
#include "wdc_par.h"

#define nop()  __asm__ __volatile__ ("nop" ::)

void wdc_wait_for_reset()
{
    while ( !isset_info_reset() ) {
        port_data_set ( DATA_CLEAR );
        port_info_set ( INFO_CLEAR );
        wdc_set_initialized ( 1 );
    }

}

uint8_t wdc_read_data_from_p8k ( uint8_t *buffer, uint16_t count, uint8_t wdc_status )
{
    uint16_t datacnt;

    _delay_us ( DELAY_PIO_US );

    configure_port_data_read();
    datacnt = 0;
    port_info_set ( wdc_status );
    while ( !isset_info_te() )
        if ( !isset_info_reset() )
            return 0;
    while ( !isset_info_wdardy() );
    do {
        enable_p8000_transeiver();  /* this also generates /ASTB in the moment /WDARDY gets low with a 7403 */
        while ( isset_info_wdardy() );
        *buffer++ = port_data_get();
        disable_p8000_transeiver(); /* this additionally brings /ASTB to high */
        while ( !isset_info_wdardy() );
        datacnt++;
    } while ( datacnt < count );
    port_info_set ( INFO_CLEAR );

    return 1;
}

void wdc_write_data_to_p8k ( uint8_t *buffer, uint16_t count, uint8_t wdc_status )
{
    uint16_t datacnt = 0;

    _delay_us ( DELAY_PIO_US );

    port_info_set ( INFO_TR | wdc_status );
    while ( isset_info_te() );
    configure_port_data_write();
    while ( !isset_info_wdardy() );
    do {
        enable_p8000_transeiver();  /* this also generates /ASTB in the moment /WDARDY gets low with a 7403 */
        while ( isset_info_wdardy() );
        port_data_set ( *buffer++ );
        disable_p8000_transeiver(); /* this additionally brings /ASTB to high */
        while ( !isset_info_wdardy() );

        datacnt++;
    } while ( datacnt < count );
    while ( isset_info_wdardy() );
    enable_p8000_transeiver();
    nop();
    disable_p8000_transeiver();
    configure_port_data_read();
    while ( !isset_info_wdardy() );
    port_info_set ( INFO_CLEAR );
}

uint8_t wdc_receive_cmd ( uint8_t *buffer, uint16_t count )
{
    return wdc_read_data_from_p8k ( buffer
                                    , count
                                    , INFO_STAT_GCMD
                                  );
}

void wdc_receive_data ( uint8_t *buffer, uint16_t count )
{
    wdc_read_data_from_p8k ( buffer
                             , count
                             , INFO_STAT_RDATA
                           );
}

void wdc_send_data ( uint8_t *buffer, uint16_t count )
{
    wdc_write_data_to_p8k ( buffer
                            , count
                            , INFO_STAT_WDATA
                          );
}

void wdc_send_error()
{
    wdc_send_errorcode ( 0x01 );
}

void wdc_send_errorcode ( uint8_t error )
{
    uint8_t buffer[1];

    buffer[0] = error;

    wdc_write_data_to_p8k ( buffer
                            , 1
                            , INFO_STAT_ERROR
                          );
}