/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_main.c,v 1.6 2012/06/02 00:51:55 olivleh1 Exp $
 *
 * TODO: - Prio 1: more commandcodes
 *       - Prio 2: check if BTT is really cleaned when sa.format gets called all the time with a real drive
 *       - Prio 2: respect BTT entries when calculating the block number to use.
 *       - Prio 3: work out how PAR+BTT where originally stored on the harddisks and store them the same way
 *       - Prio 3: put the whole par_table handling from wdc_ram.[ch] into wdc_par.[ch]
 */

#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include "main.h"
#include "uart.h"
#include "wdc_ram.h"
#include "wdc_if_pio.h"
#include "wdc_if_sdcard.h"

#define DEBUG 1
void atmega_setup ( void );

int main ( void )
{
    uint16_t cmd_counter = 9;
    uint16_t data_counter;
    uint32_t blockno;
    uint8_t  errorcode;
    uint8_t *data_buffer = NULL;
    uint8_t *cmd_buffer = NULL;
    uint8_t i8;
    uint16_t i16;

    atmega_setup();

    uart_puts_p ( PSTR ( "P8000 WDC Emulator 0.01\n" ) );

    /* load Parameter Table into RAM if valid */
    blockno = 0;
    data_counter = 512;
    data_buffer = malloc ( data_counter * sizeof ( *data_buffer ) );
    while ( wdc_read_sector ( blockno, data_buffer ) ) {
#if DEBUG >= 1
        uart_puts_p ( PSTR ( "Block 0 of SD-Card not readable" ) );
#endif
    }

    if ( data_buffer[0]  ==  'W' &&
         data_buffer[1]  ==  'D' &&
         data_buffer[2]  ==  'C' ) {
        wdc_write_par_table ( data_buffer, data_counter );
    };

    while ( 1 ) {

        wdc_wait_for_reset();

        cmd_buffer = malloc ( cmd_counter * sizeof ( *cmd_buffer ) );

        wdc_receive_cmd ( cmd_buffer
                          , cmd_counter
                        );

#if DEBUG == 2
        uart_puts_p ( PSTR ( " CMD:" ) );
        uart_putc_hex ( cmd_buffer[0] );
        uart_putc_hex ( cmd_buffer[1] );
        uart_putc_hex ( cmd_buffer[2] );
        uart_putc_hex ( cmd_buffer[3] );
        uart_putc_hex ( cmd_buffer[4] );
        uart_putc_hex ( cmd_buffer[5] );
        uart_putc_hex ( cmd_buffer[6] );
        uart_putc_hex ( cmd_buffer[7] );
        uart_putc_hex ( cmd_buffer[8] );
        uart_putc ( '\n' );
#endif

        switch ( cmd_buffer[0] ) {
            case CMD_WR_WDC_RAM:

                data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];
                data_buffer  = malloc ( data_counter * sizeof ( *data_buffer ) );

                wdc_receive_data ( data_buffer
                                   , data_counter
                                 );

                wdc_write_data_to_ram ( data_buffer
                                        , convert_ram_address ( ( cmd_buffer[2] << 8 ) | cmd_buffer[1] )
                                        , data_counter
                                      );

                free ( data_buffer );
                break;

            case CMD_RD_WDC_RAM:

                data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];
                data_buffer  = malloc ( data_counter * sizeof ( *data_buffer ) );

                wdc_read_data_from_ram ( data_buffer
                                         , convert_ram_address ( ( cmd_buffer[2] << 8 ) | cmd_buffer[1] )
                                         , data_counter
                                       );

                wdc_send_data ( data_buffer
                                , data_counter
                              );

                free ( data_buffer );
                break;

            case CMD_RD_PARTAB:

                data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];
                data_buffer  = malloc ( data_counter * sizeof ( *data_buffer ) );

                wdc_read_par_table ( data_buffer
                                     , data_counter );

                wdc_send_data ( data_buffer
                                , data_counter
                              );

                free ( data_buffer );
                break;

            case CMD_WR_PARTAB:

                data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];
                data_buffer  = malloc ( data_counter * sizeof ( *data_buffer ) );

                wdc_receive_data ( data_buffer
                                   , data_counter
                                 );

                wdc_write_wdc_par ( data_buffer
                                    , data_counter );

                free ( data_buffer );
                break;

            case CMD_DL_BTTAB:

                wdc_del_wdc_btt();
                break;

            case CMD_RD_BTTAB:

                data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];
                data_buffer  = malloc ( data_counter * sizeof ( *data_buffer ) );

                wdc_read_wdc_btt ( data_buffer
                                   , data_counter );

                wdc_send_data ( data_buffer
                                , data_counter
                              );

                free ( data_buffer );
                break;

            case CMD_WR_BTTAB:

                data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];
                data_buffer  = malloc ( data_counter * sizeof ( *data_buffer ) );

                wdc_receive_data ( data_buffer
                                   , data_counter
                                 );

                wdc_write_wdc_btt ( data_buffer
                                    , data_counter );

                free ( data_buffer );
                break;
            case CMD_FMT_BTT:

                data_counter = 512;
                data_buffer = calloc ( data_counter, sizeof ( *data_buffer ) );

                blockno = wdc_get_blockno ( cmd_buffer[2] | ( cmd_buffer[3] << 8 )
                                            , cmd_buffer[4]
                                            , cmd_buffer[5]
                                          );

                errorcode = 1;
                for ( i8 = 0; i8 < wdc_get_hdd_sectors(); i8++ ) {
                    errorcode = wdc_write_sector ( blockno, data_buffer );
                    if ( errorcode ) break;
                    blockno++;
                }

                if ( errorcode ) {
                    if ( wdc_add_btt_entry ( cmd_buffer[2] | ( cmd_buffer[3] << 8 ), cmd_buffer[4] ) ) {
                        wdc_send_error();
                    } else {
                        free ( data_buffer );
                        data_buffer  = malloc ( 3 * sizeof ( *data_buffer ) );
                        data_buffer[0] = cmd_buffer[2]; // Track Low
                        data_buffer[1] = cmd_buffer[3]; // Track High
                        data_buffer[2] = cmd_buffer[4]; // Head

                        wdc_send_data ( data_buffer
                                        , 3
                                      );
                    }
                }

                free ( data_buffer );
                break;

            case CMD_ST_PARBTT:

                blockno = 0;

                data_counter = 512;
                data_buffer = malloc ( data_counter * sizeof ( *data_buffer ) );
                wdc_read_par_table ( data_buffer
                                     , data_counter );

                errorcode = wdc_write_sector ( blockno, data_buffer );
                if ( errorcode )
                    wdc_send_error();

                free ( data_buffer );
                break;

            case CMD_RD_WDCERR:

                data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];
                data_buffer = calloc ( data_counter, sizeof ( *data_buffer ) );
                wdc_send_data ( data_buffer
                                , data_counter
                              );

                free ( data_buffer );
                break;

            case CMD_VER_TRACK:

                data_counter = 512;
                data_buffer = calloc ( data_counter, sizeof ( *data_buffer ) );

                blockno = wdc_get_blockno ( cmd_buffer[2] | ( cmd_buffer[3] << 8 )
                                            , cmd_buffer[4]
                                            , cmd_buffer[5]
                                          );

                errorcode = 1;
                for ( i8 = 0; i8 < wdc_get_hdd_sectors(); i8++ ) {
                    errorcode = wdc_read_sector ( blockno, data_buffer );
                    if ( errorcode ) break;
                    blockno++;
                }

                if ( errorcode )
                    wdc_send_error();

                free ( data_buffer );
                break;

            case CMD_RD_SECTOR:

                data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];
                if ( data_counter % 512 > 0 )
                    data_buffer = malloc ( ( data_counter + 512 - ( data_counter % 512 ) ) * sizeof ( *data_buffer ) );
                else
                    data_buffer = malloc ( data_counter * sizeof ( *data_buffer ) );

                blockno = wdc_get_blockno ( cmd_buffer[2] | ( cmd_buffer[3] << 8 )
                                            , cmd_buffer[4]
                                            , cmd_buffer[5]
                                          );

                errorcode = 1;
                for ( i16 = 0; i16 < data_counter; i16 += 512 ) {
                    errorcode = wdc_read_sector ( blockno, data_buffer );
                    if ( errorcode ) break;
                }

                if ( errorcode )
                    wdc_send_error();

                free ( data_buffer );
                break;

            default:
#if DEBUG >= 1
                uart_puts_p ( PSTR ( " Unknown command:" ) );
                uart_putc_hex ( cmd_buffer[0] );
                uart_putc_hex ( cmd_buffer[1] );
                uart_putc_hex ( cmd_buffer[2] );
                uart_putc_hex ( cmd_buffer[3] );
                uart_putc_hex ( cmd_buffer[4] );
                uart_putc_hex ( cmd_buffer[5] );
                uart_putc_hex ( cmd_buffer[6] );
                uart_putc_hex ( cmd_buffer[7] );
                uart_putc_hex ( cmd_buffer[8] );
                uart_putc ( '\n' );
#endif
                wdc_send_error();
                break;
        }

        free ( cmd_buffer );
    }
    return 0;
}

void atmega_setup ( void )
{
    set_sleep_mode ( SLEEP_MODE_IDLE );
    uart_init();
    wdc_init_ports();
    while ( wdc_init_sdcard() ) {
#if DEBUG >= 1
        uart_puts_p ( PSTR ( "SDCard initialisation failed!" ) );
#endif
    }

}