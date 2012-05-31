/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_main.c,v 1.5 2012/05/31 20:21:51 olivleh1 Exp $
 *
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
    uint8_t *data_buffer = NULL;
    uint8_t *cmd_buffer = NULL;

    atmega_setup();

    uart_puts_p ( PSTR ( "P8000 WDC Emulator 0.01\n" ) );

    while ( 1 ) {

        wdc_wait_for_reset();

        cmd_buffer = malloc ( cmd_counter * sizeof ( *cmd_buffer ) );

        wdc_receive_cmd ( cmd_buffer
                          , cmd_counter
                        );


#ifdef DEBUG
        uart_puts_p ( PSTR ( " CMD Buffer:" ) );
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

                wdc_read_partab ( data_buffer
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

                wdc_write_partab ( data_buffer
                                   , data_counter );

                free ( data_buffer );
                break;

            case CMD_DL_BTTAB:

                wdc_del_bttab();
                break;

            case CMD_RD_BTTAB:

                data_counter = ( cmd_buffer[7] << 8 ) | cmd_buffer[6];
                data_buffer  = malloc ( data_counter * sizeof ( *data_buffer ) );

                wdc_read_bttab ( data_buffer
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

                wdc_write_bttab ( data_buffer
                                  , data_counter );

                free ( data_buffer );
                break;
            default:

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
    wdc_init_ports();
    wdc_init_sdcard();
    uart_init();
}