/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_main.c,v 1.3 2012/05/30 16:41:00 olivleh1 Exp $
 * 
 */ 


#include <string.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "main.h"
#include "uart.h"

void atmega_setup(void);

int main(void)
{
    uint8_t cmdbuffer[9];
    uint8_t cmdcnt;
    uint8_t buffer[9];
    uint8_t datacnt;
    uint16_t data_counter;
    uint16_t data_address;

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

            for(data_counter=1000;data_counter>0;data_counter--) asm("nop");

                data_counter = (cmdbuffer[7]<<8)|cmdbuffer[6];

                configure_port_data_read();
                datacnt = 0;
                port_info_set( INFO_STAT_RDATA );
                while ( !isset_info_te() );
                while ( !isset_info_wdardy() );
                while ( datacnt < data_counter )
                {
                    while ( isset_info_wdardy() );
                    port_info_set( INFO_ASTB | INFO_STAT_RDATA );
                    buffer[datacnt] = port_data_get();
                    port_info_set( INFO_STAT_RDATA );
                    while ( !isset_info_wdardy() );

                    datacnt++;
                }

                port_info_set( INFO_CLEAR );

                data_address = convert_ram_address((cmdbuffer[2]<<8)+cmdbuffer[1]);
                do 
                {
                    data_counter--;
                    wdc_ram[data_address+data_counter] = buffer[+data_counter];
                } while (data_counter > 0);

                uart_puts_p(PSTR("DATA Buffer:"));
                uart_putc_hex(wdc_ram[data_address+0]);
                uart_putc_hex(wdc_ram[data_address+1]);
                break;

            case CMD_RD_WDC_RAM:

                data_counter = (cmdbuffer[7]<<8)|cmdbuffer[6];
                data_address = convert_ram_address((cmdbuffer[2]<<8)+cmdbuffer[1]);
                do
                {
                    data_counter--;
                    buffer[+data_counter] = wdc_ram[data_address+data_counter];
                } while (data_counter > 0);

            for(data_counter=1000;data_counter>0;data_counter--) asm("nop");

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
            for(data_counter=1000;data_counter>0;data_counter--) asm("nop");
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