/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_if_disk.c,v 1.5 2012/06/05 18:21:48 olivleh1 Exp $
 *
 */

#include <stdint.h>
#include "mmc.h"
#include "uart.h"
extern uint8_t par_table[200];

uint8_t wdc_init_sdcard()
{
    return mmc_init();
}

uint32_t wdc_sector2sdblock ( uint16_t req_cylinder, uint8_t req_head, uint8_t req_sector )
{
    /* TODO: no direct access of par_table - use getter functions instead - but only after speed is sufficient */
    /*            +-------------------------------------------------------------- requested cylinder from P8000
     *            |              +----------------------------------------------- configured heads form the harddisk
     *            |              |               +------------------------------- configured sectors for the harddisk
     *            |              |               |                +-------------- requested head from P8000
     *            |              |               |                |           +-- configured sectors for the harddisk
     *            |              |               |                |           |                  +----  requested sector from P8000 - 1 as we start counting with 0
     *            |              |               |                |           |                  | */
    return ( ( req_cylinder * par_table[25] * par_table[26] ) + ( req_head * par_table[26] ) + ( req_sector - 1 ) );
}

uint32_t wdc_p8kblock2sdblock ( uint32_t blockno )
{
    /* TODO: no direct access of par_table - use getter functions instead - but only after speed is sufficient */
    /*                          +----------------------------------------------- configured heads form the harddisk
     *                          |               +------------------------------- configured sectors for the harddisk
     *                          |               |*/
    return ( blockno +  ( par_table[25] * par_table[26] ) );
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
//    errorcode = mmc_write_multiblock_predef ( addr, sector, numblocks );
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