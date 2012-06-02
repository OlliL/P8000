/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_if_disk.c,v 1.3 2012/06/02 23:04:30 olivleh1 Exp $
 *
 */

#include "mmc.h"
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

uint8_t wdc_write_sector ( uint16_t addr, uint8_t *sector )
{
    return mmc_write_sector ( addr, sector );
}

uint8_t wdc_read_sector ( uint16_t addr, uint8_t *sector )
{
    return mmc_read_sector ( addr, sector );
}