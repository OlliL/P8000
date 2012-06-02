/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_ram.c,v 1.3 2012/06/02 00:51:55 olivleh1 Exp $
 *
 */

#include <avr/pgmspace.h>
#include "wdc_ram.h"

uint8_t wdc_ram[2048];
uint8_t par_table[512] = {'W',  'D',  'C',  '_',  '4',  '.',  '2', 0x00,  'R',  'O',  'B',  ' ',  'K',  '5',  '5',  '0',
                          '4',  '.',  '5',  '0', 0x00, 0x00, 0x00, 0x00, 0x04, 0x05, 0x12, 0x00, 0x04, 0x01, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa5, 0x67, 0x01, 0x00, 0x01, 0x01,
                          0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                         };

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


void wdc_read_par_table ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        buffer[count] = par_table[count];
    } while ( count > 0 );
}

void wdc_write_par_table ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        par_table[count] = buffer[count];
    } while ( count > 0 );
}


void wdc_read_wdc_par ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        buffer[count] = par_table[count + POS_PAR_WDC_PAR];
    } while ( count > 0 );
}

void wdc_write_wdc_par ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        par_table[count + POS_PAR_WDC_PAR] = buffer[count];
    } while ( count > 0 );
}

void wdc_del_wdc_btt()
{
    uint8_t i;
    for ( i = 0; i < 125; i++ )
        par_table[i + POS_PAR_WDC_BTT] = 0x00;
    par_table[122 + POS_PAR_WDC_BTT] = 0xff;
}

void wdc_read_wdc_btt ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        buffer[count] = par_table[count + POS_PAR_WDC_BTT];
    } while ( count > 0 );
}

void wdc_write_wdc_btt ( uint8_t *buffer, uint16_t count )
{
    do {
        count--;
        par_table[count + POS_PAR_WDC_BTT] = buffer[count];
    } while ( count > 0 );
}

uint16_t wdc_get_btt_count()
{
    return ( ( par_table[0 + POS_PAR_WDC_BTT] | ( par_table[1 + POS_PAR_WDC_BTT] << 8 ) ) );
}

void wdc_set_btt_count ( uint16_t btt_count )
{
    par_table[0 + POS_PAR_WDC_BTT] = ( uint8_t ) btt_count;
    par_table[1 + POS_PAR_WDC_BTT] = ( btt_count >> 8 );
}

uint8_t wdc_add_btt_entry ( uint16_t cylinder, uint8_t head )
{
    uint16_t btt_count = wdc_get_btt_count();

    if ( btt_count < 120 ) {
        /* +2 because btt_count is just the amount of bytes for the BTT - but in front of it, there are two bytes */
        /* storing the amount of bytes for the BTT */
        par_table[btt_count + 2 + POS_PAR_WDC_BTT] = ( cylinder >> 8 );
        btt_count++;
        par_table[btt_count + 2 + POS_PAR_WDC_BTT] = ( uint8_t ) cylinder;
        btt_count++;
        par_table[btt_count + 2 + POS_PAR_WDC_BTT] = ( uint8_t ) head;
        btt_count++;
        wdc_set_btt_count ( btt_count );
        return ( 0 );
    } else {
        return ( 1 );
    }
}

uint8_t wdc_get_hdd_sectors()
{
    return ( par_table[18 + POS_PAR_WDC_PAR] );
}