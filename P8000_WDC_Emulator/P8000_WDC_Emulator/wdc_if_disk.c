/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_if_disk.c,v 1.8 2012/06/07 17:54:00 olivleh1 Exp $
 *
 */

#include <stdint.h>
#include "mmc.h"
#include "uart.h"
#include "wdc_par.h"

extern uint8_t par_table[];

uint8_t wdc_init_sdcard()
{
    uint8_t errorcode;

    errorcode=mmc_init();
    if ( errorcode ) {
        uart_putc_hex ( errorcode );
        uart_putc ( '\n' );
    }    
    return errorcode;        
}

uint32_t wdc_sector2sdblock ( uint16_t req_cylinder, uint8_t req_head, uint8_t req_sector )
{
    return ( ( req_cylinder * wdc_get_hdd_heads() * wdc_get_hdd_sectors() )
             + ( req_head * wdc_get_hdd_sectors() )
             + ( req_sector - 1 ) );
}

uint32_t wdc_p8kblock2sdblock ( uint32_t blockno )
{
    return ( blockno
             +  ( wdc_get_hdd_heads() * wdc_get_hdd_sectors() ) );
}

uint8_t wdc_write_sector ( uint32_t addr, uint8_t *sector )
{
    uint8_t errorcode;

    errorcode = mmc_write_sector ( addr, sector );
    if ( errorcode ) {
        uart_puts_p ( PSTR ( " write error at address: " ) );
        uart_putdw_dec ( addr );
        uart_puts_p ( PSTR ( " / errorcode is: " ) );
        uart_putc_hex ( errorcode );
        uart_putc ( '\n' );
        return errorcode;
    } else
        return 0;
}

uint8_t wdc_read_sector ( uint32_t addr, uint8_t *sector )
{
    uint8_t errorcode;

    errorcode = mmc_read_sector ( addr, sector );
    if ( errorcode ) {
        uart_puts_p ( PSTR ( " read error at address: " ) );
        uart_putdw_dec ( addr );
        uart_puts_p ( PSTR ( " / errorcode is: " ) );
        uart_putc_hex ( errorcode );
        uart_putc ( '\n' );
        return errorcode;
    } else
        return 0;
}

uint8_t wdc_read_multiblock ( uint32_t addr, uint8_t *sector, uint8_t numblocks )
{
    uint8_t errorcode;

    errorcode = mmc_read_multiblock ( addr, sector, numblocks );
    if ( errorcode ) {
        uart_puts_p ( PSTR ( " read error at address: " ) );
        uart_putdw_dec ( addr );
        uart_puts_p ( PSTR ( " / errorcode is: " ) );
        uart_putc_hex ( errorcode );
        uart_putc ( '\n' );
        return errorcode;
    } else
        return 0;
}

uint8_t wdc_write_multiblock ( uint32_t addr, uint8_t *sector, uint8_t numblocks )
{
    uint8_t errorcode;

    errorcode = mmc_write_multiblock ( addr, sector, numblocks );
    if ( errorcode ) {
        uart_puts_p ( PSTR ( " write error at address: " ) );
        uart_putdw_dec ( addr );
        uart_puts_p ( PSTR ( " / errorcode is: " ) );
        uart_putc_hex ( errorcode );
        uart_putc ( '\n' );
        return errorcode;
    } else
        return 0;
}