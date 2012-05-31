/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_ram.h,v 1.1 2012/05/31 20:21:51 olivleh1 Exp $
 *
 */

#ifndef WDC_RAM_H_
#define WDC_RAM_H_

void wdc_write_data_to_ram ( uint8_t *buffer, uint16_t address, uint16_t count );
void wdc_read_data_from_ram ( uint8_t *buffer, uint16_t address, uint16_t count );

void wdc_read_partab ( uint8_t *buffer, uint16_t count );
void wdc_write_partab ( uint8_t *buffer, uint16_t count );

void wdc_del_bttab();
void wdc_read_bttab ( uint8_t *buffer, uint16_t count );
void wdc_write_bttab ( uint8_t *buffer, uint16_t count );

#define convert_ram_address(x) ((x) - 0x2000)        /* The S-RAM of the original WDC hardware starts at 0x2000. We use an array as RAM here and no direct RAM access     */

#endif /* WDC_RAM_H_ */