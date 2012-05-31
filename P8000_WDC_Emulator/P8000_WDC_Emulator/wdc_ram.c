/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_ram.c,v 1.2 2012/05/31 20:34:11 olivleh1 Exp $
 *
 */

#include <avr/pgmspace.h>

uint8_t par_table[200] = {   'W',  'D',  'C',  '_',  '4',  '.',  '2', 0x00,  'R',  'O',  'B',  ' ',  'K',  '5',  '5',  '0',
                             '4',  '.',  '5',  '0', 0x00, 0x00, 0x00, 0x00, 0x04, 0x05, 0x12, 0x00, 0x04, 0x01, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x01, 0x00, 0x01, 0x01,
                             0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                         };
uint8_t bt_table[125];
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

void wdc_read_partab ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        buffer[count] = par_table[count];
    } while ( count > 0 );
}

void wdc_write_partab ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        par_table[count + 8] = buffer[count];
    } while ( count > 0 );
}

void wdc_del_bttab()
{
    uint8_t i;
    for ( i = 0; i < 125; i++ )
        bt_table[i] = 0x00;
}

void wdc_read_bttab ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        buffer[count] = bt_table[count];
    } while ( count > 0 );
}

void wdc_write_bttab ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        bt_table[count] = buffer[count];
    } while ( count > 0 );
}