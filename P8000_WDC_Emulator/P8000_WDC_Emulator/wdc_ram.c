/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_ram.c,v 1.4 2012/06/06 22:05:06 olivleh1 Exp $
 *
 */

#include <avr/pgmspace.h>
#include "wdc_ram.h"

uint8_t wdc_ram[2048];

void wdc_write_data_to_ram ( uint8_t *buffer, uint16_t address, uint16_t count )
{
    do {
        count--;
        wdc_ram[address + count] = buffer[count];
    } while ( count > 0 );
}

void wdc_read_data_from_ram ( uint8_t *buffer, uint16_t address, uint16_t count )
{
    do {
        count--;
        buffer[count] = wdc_ram[address + count];
    } while ( count > 0 );
}
