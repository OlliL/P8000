/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_main.c,v 1.4 2012/05/30 20:07:10 olivleh1 Exp $
 * 
 */ 


#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "main.h"
#include "uart.h"

#define DEBUG 1
void atmega_setup(void);
void wdc_read_data_from_p8k(uint8_t **data_buffer, uint8_t data_count);
void wdc_write_data_to_ram(uint8_t **data_buffer, uint16_t data_address, uint8_t data_count);

int main(void)
{
    uint8_t cmdbuffer[9];
    uint8_t cmdcnt;
    uint8_t buffer[9];
    uint8_t datacnt;
    uint16_t data_counter;
    uint16_t data_address;
    uint8_t *data_buffer = NULL;

    atmega_setup();
    
    uart_puts_p(PSTR("P8000 WDC Emulator 0.01\n"));

    while(1)
    {

        while( !isset_info_reset() )
        {
            port_data_set( DATA_CLEAR );
            port_info_set( INFO_CLEAR );
        }

        configure_port_data_read();
        cmdcnt = 0;
        port_info_set( INFO_STAT_GCMD );
        
        while ( !isset_info_te() );
        while ( !isset_info_wdardy() );
        while ( cmdcnt < 9 )
        {
            while ( isset_info_wdardy() );
            port_info_set( INFO_ASTB | INFO_STAT_GCMD );
            cmdbuffer[cmdcnt] = port_data_get();
            port_info_set( INFO_STAT_GCMD );
            while ( !isset_info_wdardy() );
            cmdcnt++;
        }
        port_info_set( INFO_CLEAR );        

        switch( cmdbuffer[0] )
        {
            case CMD_WR_WDC_RAM:

                data_counter = (cmdbuffer[7]<<8)|cmdbuffer[6];

                wdc_read_data_from_p8k(&data_buffer
                                      ,data_counter
                                      );

                wdc_write_data_to_ram(&data_buffer
                                     ,convert_ram_address((cmdbuffer[2]<<8)+cmdbuffer[1])
                                     ,data_counter
                                     );
                
                free(data_buffer);
                break;

            case CMD_RD_WDC_RAM:

                data_counter = (cmdbuffer[7]<<8)|cmdbuffer[6];
                data_address = convert_ram_address((cmdbuffer[2]<<8)+cmdbuffer[1]);
                do
                {
                    data_counter--;
                    buffer[+data_counter] = wdc_ram[data_address+data_counter];
                } while (data_counter > 0);

                _delay_us(560);

                data_counter = (cmdbuffer[7]<<8)|cmdbuffer[6];
                datacnt = 0;

                port_info_set( INFO_TR |INFO_STAT_WDATA );
                while ( isset_info_te() );
                configure_port_data_write();
                while ( !isset_info_wdardy() );
                while ( datacnt < data_counter )
                {
                    while ( isset_info_wdardy() );
                    port_info_set( INFO_TR | INFO_ASTB | INFO_STAT_WDATA );
                    port_data_set(buffer[datacnt]);
                    port_info_set( INFO_TR |INFO_STAT_WDATA );
                    while ( !isset_info_wdardy() );

                    datacnt++;
                }
                while ( isset_info_wdardy() );
                port_info_set( INFO_TR | INFO_ASTB | INFO_STAT_WDATA );
                asm("nop");
                port_info_set( INFO_TR |INFO_STAT_WDATA );
                configure_port_data_read();
                port_info_set( INFO_CLEAR );
                break;
            default:
                _delay_us(560);
                port_info_set( INFO_TR | INFO_STAT_ERROR );
                while ( isset_info_te() );
                configure_port_data_write();
                while ( !isset_info_wdardy() );
                    while ( isset_info_wdardy() );
                    port_info_set( INFO_TR | INFO_ASTB | INFO_STAT_ERROR );
                    port_data_set(0x01);
                    port_info_set( INFO_TR | INFO_STAT_ERROR );
                    while ( !isset_info_wdardy() );
                while ( isset_info_wdardy() );
                port_info_set( INFO_TR | INFO_ASTB | INFO_STAT_ERROR );
                asm("nop");
                asm("nop");
                port_info_set( INFO_TR | INFO_STAT_ERROR );
                configure_port_data_read();
                while ( !isset_info_wdardy() );
                port_info_set( INFO_CLEAR );
                break;
        }

#ifdef DEBUG
        uart_puts_p(PSTR(" CMD Buffer:"));
        uart_putc_hex(cmdbuffer[0]);
        uart_putc_hex(cmdbuffer[1]);
        uart_putc_hex(cmdbuffer[2]);
        uart_putc_hex(cmdbuffer[3]);
        uart_putc_hex(cmdbuffer[4]);
        uart_putc_hex(cmdbuffer[5]);
        uart_putc_hex(cmdbuffer[6]);
        uart_putc_hex(cmdbuffer[7]);
        uart_putc_hex(cmdbuffer[8]);
        uart_putc('\n');
#endif
    }
    return 0;
}

void atmega_setup(void)
{
    set_sleep_mode(SLEEP_MODE_IDLE);

    configure_pin_status0();
    configure_pin_status1();
    configure_pin_status2();
    configure_pin_astb();
    configure_pin_te();
    configure_pin_wdardy();
    configure_pin_tr();
    configure_pin_reset();
    
    configure_port_data_read();
    
    uart_init();
}

void wdc_read_data_from_p8k(uint8_t **data_buffer, uint8_t data_count)
{
    int datacnt = 0;
    
    (*data_buffer) = (uint8_t *)malloc(data_count);

    _delay_us(560);

    configure_port_data_read();
    port_info_set( INFO_STAT_RDATA );
    while ( !isset_info_te() );
    while ( !isset_info_wdardy() );
    do 
    {
        while ( isset_info_wdardy() );
        port_info_set( INFO_ASTB | INFO_STAT_RDATA );
        (*data_buffer)[datacnt] = port_data_get();
        port_info_set( INFO_STAT_RDATA );
        while ( !isset_info_wdardy() );

        datacnt++;
    } while ( datacnt < data_count );

    port_info_set( INFO_CLEAR );
}

void wdc_write_data_to_ram(uint8_t **data_buffer, uint16_t data_address, uint8_t data_count)
{
    do
    {
        data_count--;
        wdc_ram[data_address+data_count] = (*data_buffer)[data_count];
    } while (data_count > 0);
}