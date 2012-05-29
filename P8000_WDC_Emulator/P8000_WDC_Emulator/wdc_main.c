/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_main.c,v 1.2 2012/05/29 20:23:09 olivleh1 Exp $
 * 
 */ 


#include <string.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "main.h"

void atmega_setup(void);

int main(void)
{
    uint8_t cmdbuffer[9];
    uint8_t cmdcnt;

    atmega_setup();
    
    uart_puts_p(PSTR("P8000 WDC Emulator 1.0\n"));

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
        
        uart_puts_p(PSTR("CMD Buffer:"));
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