/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_if_p8000.c,v 1.2 2012/05/31 20:22:42 olivleh1 Exp $
 *
 */

#include "config.h"
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "wdc_if_pio.h"

void wdc_init_ports()
{
    configure_pin_status0();
    configure_pin_status1();
    configure_pin_status2();
    configure_pin_astb();
    configure_pin_te();
    configure_pin_wdardy();
    configure_pin_tr();
    configure_pin_reset();

    configure_port_data_read();
}

void wdc_wait_for_reset()
{
    while ( !isset_info_reset() ) {
        port_data_set ( DATA_CLEAR );
        port_info_set ( INFO_CLEAR );
    }

}

void wdc_read_data_from_p8k ( uint8_t *buffer, uint16_t count, uint8_t wdc_status )
{
    uint16_t datacnt;

    _delay_us ( DELAY_PIO_US );

    configure_port_data_read();
    datacnt = 0;
    port_info_set ( wdc_status );
    while ( !isset_info_te() );
    while ( !isset_info_wdardy() );
    do {
        while ( isset_info_wdardy() );
        port_info_set ( INFO_ASTB | wdc_status );
        buffer[datacnt] = ( uint8_t ) port_data_get();
        port_info_set ( wdc_status );
        while ( !isset_info_wdardy() );
        datacnt++;
    } while ( datacnt < count );
    port_info_set ( INFO_CLEAR );

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
        while ( isset_info_wdardy() );
        port_info_set ( INFO_TR | INFO_ASTB | wdc_status );
        port_data_set ( buffer[datacnt] );
        port_info_set ( INFO_TR | wdc_status );
        while ( !isset_info_wdardy() );

        datacnt++;
    } while ( datacnt < count );
    while ( isset_info_wdardy() );
    port_info_set ( INFO_TR | INFO_ASTB | wdc_status );
    asm ( "nop" );
    asm ( "nop" );
    port_info_set ( INFO_TR | wdc_status );
    configure_port_data_read();
    while ( !isset_info_wdardy() );
    port_info_set ( INFO_CLEAR );

}

void wdc_receive_cmd ( uint8_t *buffer, uint16_t count )
{
    wdc_read_data_from_p8k ( buffer
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
    uint8_t data_buffer[1];

    data_buffer[0] = 0x01;

    wdc_write_data_to_p8k ( data_buffer
                            , 1
                            , INFO_STAT_ERROR
                          );
}