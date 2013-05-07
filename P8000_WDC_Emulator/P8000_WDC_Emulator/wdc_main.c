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
 * $Id: wdc_main.c,v 1.34 2013/05/07 17:38:06 olivleh1 Exp $
 *
 */

/* TODO:  - right now, BTT entries are not taken into account */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include "wdc_config.h"
#include <util/delay.h>
#include "wdc_main.h"
#ifdef DEBUG
#include "uart.h"
#endif
#include "wdc_avr.h"
#include "wdc_ram.h"
#include "wdc_par.h"
#include "wdc_if_p8000.h"
#include "wdc_if_disk.h"

void atmega_setup ( void );

uint8_t data_buffer[4096];
uint8_t cmd_buffer[9];

#ifdef MEASURE_DISK_PERFORMANCE
extern void measure_performance ();
#else

/* switched from local to global for keeping an eye on memory usage */
extern uint8_t valid_disk;

#endif

int __attribute__( ( OS_main ) )
main ( void )
{
#ifndef MEASURE_DISK_PERFORMANCE
    uint16_t cmd_counter = 9;
    uint16_t data_counter;
    uint32_t blockno;
    uint8_t  errorcode;
    uint8_t  i8;
#endif

    atmega_setup();

#ifdef MEASURE_DISK_PERFORMANCE
    measure_performance();
#else

    /* load Parameter Table into RAM if valid */
    if ( wdc_get_disk_valid() ) {
        blockno = 0;
        data_counter = WDC_BLOCKLEN;
        wdc_read_sector ( blockno, data_buffer );
        if ( data_buffer[0] == 'W' &&
             data_buffer[1] == 'D' &&
             data_buffer[2] == 'C' &&
             data_buffer[3] == '_' &&
             data_buffer[4] == '4' &&
             data_buffer[5] == '.' &&
             data_buffer[6] == '2' ) {
            wdc_write_par_table ( data_buffer, data_counter );
            uart_putstring ( PSTR ( "INFO: Disk found and ready to use." ), true );
            wdc_set_disk_valid();
        } else {
            uart_putstring ( PSTR ( "INFO: Disk found but Sector 0 is invalid, please execute sa.format to initialize this disk properly." ), true );
            wdc_set_disk_invalid();
        }
    }

    while ( 1 ) {

        wdc_wait_for_reset();

        if ( wdc_receive_cmd ( cmd_buffer
                             , cmd_counter
                             ) ) {

#if DEBUG >= 2
#if DEBUG >= 3
            uart_putstring ( PSTR ( "DEBUG: received command is:" ), false );
#endif
            uart_putc_hex ( cmd_buffer[0] );
#if DEBUG >= 3
            uart_putc_hex ( cmd_buffer[1] );
            uart_putc_hex ( cmd_buffer[2] );
            uart_putc_hex ( cmd_buffer[3] );
            uart_putc_hex ( cmd_buffer[4] );
            uart_putc_hex ( cmd_buffer[5] );
            uart_putc_hex ( cmd_buffer[6] );
            uart_putc_hex ( cmd_buffer[7] );
            uart_putc_hex ( cmd_buffer[8] );
#endif
            uart_putnl();
#endif

            /* no drive attached */
            if ( wdc_get_initialized() && wdc_get_num_of_drvs() == 0 ) {
                if ( cmd_buffer[0] == CMD_WR_WDC_RAM
                     || cmd_buffer[0] == CMD_RD_BLOCK ) {
                    wdc_send_errorcode ( 0xc1 );
                } else if ( cmd_buffer[0] == CMD_RD_WDC_RAM ) {
                    wdc_send_errorcode ( ERR_NO_DRIVE_READY );
                    wdc_set_initialized ( 0 );
                }
                /* WDC freshly initialized after RESET, the disk is not valid */
            } else if ( wdc_get_initialized()
                        && !wdc_get_disk_valid()
                        && ( cmd_buffer[0] == CMD_WR_WDC_RAM || cmd_buffer[0] == CMD_RD_BLOCK ) ) {
                wdc_send_errorcode ( ERR_CYL0_NOT_READABLE );
                wdc_set_initialized ( 0 );
                /* if the disk is not valid, only maintenance commands are allowed */
            } else if ( !wdc_get_disk_valid()
                        && cmd_buffer[0] != CMD_RD_WDC_RAM
                        && cmd_buffer[0] != CMD_FMTRD_TRACK
                        && cmd_buffer[0] != CMD_WR_WDC_RAM
                        && cmd_buffer[0] != CMD_FMTBTT_TRACK
                        && cmd_buffer[0] != CMD_RD_PARTAB
                        && cmd_buffer[0] != CMD_RD_WDCERR
                        && cmd_buffer[0] != CMD_VER_TRACK
                        && cmd_buffer[0] != CMD_DL_BTTAB
                        && cmd_buffer[0] != CMD_RD_BTTAB
                        && cmd_buffer[0] != CMD_WR_BTTAB
                        && cmd_buffer[0] != CMD_WR_PARTAB
                        && cmd_buffer[0] != CMD_ST_PARBTT
                      ) {
                if ( cmd_buffer[0] == CMD_RD_BLOCK ) {
                    wdc_send_errorcode ( ERR_SECT_N_ON_SURFACE );
                } else {
                    wdc_send_error();
                }
            } else {

                switch ( cmd_buffer[0] ) {
                    case CMD_WR_BLOCK:
                        data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];

                        blockno = wdc_p8kblock2diskblock ( ( (uint32_t)cmd_buffer[5] << 24 ) | ( (uint32_t)cmd_buffer[4] << 16 ) | ( (uint16_t)cmd_buffer[3] << 8 ) | cmd_buffer[2] );
                        wdc_receive_data ( data_buffer
                                         , data_counter
                                         );
                        errorcode = 1;
                        if ( data_counter == WDC_BLOCKLEN ) {
                            errorcode = wdc_write_sector ( blockno, data_buffer );
                        } else {
                            errorcode = wdc_write_multiblock ( blockno, data_buffer, data_counter / WDC_BLOCKLEN );
                        }

                        if ( errorcode ) {
                            wdc_send_error();
                        }
                        break;

                    case CMD_RD_BLOCK:
                        data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];

                        blockno = wdc_p8kblock2diskblock ( ( (uint32_t)cmd_buffer[5] << 24 ) | ( (uint32_t)cmd_buffer[4] << 16 ) | ( (uint16_t)cmd_buffer[3] << 8 ) | cmd_buffer[2] );

                        errorcode = 1;
                        if ( data_counter == WDC_BLOCKLEN ) {
                            errorcode = wdc_read_sector ( blockno, data_buffer );
                        } else {
                            errorcode = wdc_read_multiblock ( blockno, data_buffer, data_counter / WDC_BLOCKLEN );
                        }

                        if ( errorcode ) {
                            wdc_send_error();
                        } else {
                            wdc_send_data ( data_buffer
                                          , data_counter
                                          );
                        }
                        break;

                    case CMD_WR_WDC_RAM:
                        data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];

                        wdc_receive_data ( data_buffer
                                         , data_counter
                                         );

                        wdc_write_data_to_ram ( data_buffer
                                              , convert_ram_address ( ( cmd_buffer[2] << 8 ) | cmd_buffer[1] )
                                              , data_counter
                                              );
                        break;

                    case CMD_RD_WDC_RAM:
                        data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];

                        wdc_read_data_from_ram ( data_buffer
                                               , convert_ram_address ( ( cmd_buffer[2] << 8 ) | cmd_buffer[1] )
                                               , data_counter
                                               );

                        wdc_send_data ( data_buffer
                                      , data_counter
                                      );

                        wdc_set_initialized ( 0 );
                        break;

                    case CMD_RD_PARTAB:
                        data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];

                        wdc_read_par_table ( data_buffer
                                           , data_counter );

                        wdc_send_data ( data_buffer
                                      , data_counter
                                      );
                        break;

                    case CMD_WR_PARTAB:
                        data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];

                        wdc_receive_data ( data_buffer
                                         , data_counter
                                         );

                        wdc_write_wdc_par ( data_buffer
                                          , data_counter );
                        break;

                    case CMD_DL_BTTAB:
                        wdc_del_wdc_btt();
                        break;

                    case CMD_RD_BTTAB:
                        data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];

                        wdc_read_wdc_btt ( data_buffer
                                         , data_counter );

                        wdc_send_data ( data_buffer
                                      , data_counter
                                      );
                        break;

                    case CMD_WR_BTTAB:
                        data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];

                        wdc_receive_data ( data_buffer
                                         , data_counter
                                         );

                        wdc_write_wdc_btt ( data_buffer
                                          , data_counter );
                        break;

                    case CMD_FMTRD_TRACK:
                    case CMD_FMTBTT_TRACK:

                        /* no cylinder 0 formatting if command is CMD_FMTBTT_TRACK */
                        if ( cmd_buffer[0] == CMD_FMTBTT_TRACK && ( cmd_buffer[2] | ( cmd_buffer[3] << 8 ) ) == 0 ) {
                            break;
                        }

                        data_counter = WDC_BLOCKLEN;

                        memset ( &data_buffer[0], 0x00, data_counter );
                        blockno = wdc_sector2diskblock ( cmd_buffer[2] | ( cmd_buffer[3] << 8 )
                                                       , cmd_buffer[4]
                                                       , cmd_buffer[5]
                                                       );

                        errorcode = 1;
                        for ( i8 = 0; i8 < wdc_get_hdd_sectors(); i8++ ) {
                            errorcode = wdc_write_sector ( blockno, data_buffer );
                            if ( errorcode ) {
                                break;
                            }
                            blockno++;
                        }

                        if ( errorcode ) {
                            if ( cmd_buffer[0] == CMD_FMTRD_TRACK ) {
                                wdc_send_error();
                            } else {
                                errorcode = wdc_add_btt_entry ( cmd_buffer[2] | ( cmd_buffer[3] << 8 ), cmd_buffer[4] );
                                if ( errorcode ) {
                                    wdc_send_errorcode ( errorcode );
                                } else {
                                    data_buffer[0] = cmd_buffer[2]; /* Track Low */
                                    data_buffer[1] = cmd_buffer[3]; /* Track High */
                                    data_buffer[2] = cmd_buffer[4]; /* Head */

                                    wdc_send_data ( data_buffer
                                                  , 3
                                                  );
                                }
                            }
                        }
                        break;

                    case CMD_ST_PARBTT:
                        data_counter = WDC_BLOCKLEN;

                        blockno = 0;
                        wdc_set_disk_valid();
                        wdc_read_par_table ( data_buffer
                                           , data_counter );

                        errorcode = wdc_write_sector ( blockno, data_buffer );
                        if ( errorcode ) {
                            wdc_send_error();
                        }
                        break;

                    case CMD_RD_WDCERR:
                        data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];

                        memset ( &data_buffer[0], 0x00, data_counter );
                        wdc_send_data ( data_buffer
                                      , data_counter
                                      );
                        break;

                    case CMD_VER_TRACK:
                        data_counter = WDC_BLOCKLEN;

                        blockno = wdc_sector2diskblock ( cmd_buffer[2] | ( cmd_buffer[3] << 8 )
                                                       , cmd_buffer[4]
                                                       , cmd_buffer[5]
                                                       );
                        errorcode = 1;
                        for ( i8 = 0; i8 < wdc_get_hdd_sectors(); i8++ ) {
                            errorcode = wdc_read_sector ( blockno, data_buffer );
                            if ( errorcode ) {
                                break;
                            }
                            blockno++;
                        }
                        if ( errorcode ) {
                            wdc_send_error();
                        }
                        break;

                    case CMD_RD_SECTOR:
                        data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];

                        blockno = wdc_sector2diskblock ( cmd_buffer[2] | ( cmd_buffer[3] << 8 )
                                                       , cmd_buffer[4]
                                                       , cmd_buffer[5]
                                                       );
                        errorcode = 1;

                        if ( data_counter == WDC_BLOCKLEN ) {
                            errorcode = wdc_write_sector ( blockno, data_buffer );
                        } else {
                            errorcode = wdc_write_multiblock ( blockno, data_buffer, data_counter / WDC_BLOCKLEN );
                        }

                        if ( errorcode ) {
                            wdc_send_error();
                        }
                        break;

                    default:
#if DEBUG >= 1
                        uart_putstring ( PSTR ( "ERROR: Unknown command received from the P8000:" ), false );
                        uart_putc_hex ( cmd_buffer[0] );
                        uart_putc_hex ( cmd_buffer[1] );
                        uart_putc_hex ( cmd_buffer[2] );
                        uart_putc_hex ( cmd_buffer[3] );
                        uart_putc_hex ( cmd_buffer[4] );
                        uart_putc_hex ( cmd_buffer[5] );
                        uart_putc_hex ( cmd_buffer[6] );
                        uart_putc_hex ( cmd_buffer[7] );
                        uart_putc_hex ( cmd_buffer[8] );
                        uart_put_nl();
#endif
                        wdc_send_error();
                        break;
                }
            }
        }
    }
#endif
    return 0;
}

void atmega_setup ( void )
{
    set_sleep_mode ( SLEEP_MODE_IDLE );
    /* is needed for some disks (for example Maxtor 6L080J4) and SD-Cards*/
    _delay_ms ( 400 );
    uart_init();
    uart_putstring ( PSTR ( "=== P8000 WDC Emulator 0.92 ===" ), true );
   wdc_init_avr();
    wdc_get_sysconf();

    if ( wdc_init_disk() ) {
        uart_putstring ( PSTR ( "ERROR: No disk found!" ), true );
        wdc_set_no_disk();
    }
}