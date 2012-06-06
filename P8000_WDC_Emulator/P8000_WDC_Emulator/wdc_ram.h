/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_ram.h,v 1.4 2012/06/06 22:05:06 olivleh1 Exp $
 *
 */

#ifndef WDC_RAM_H_
#define WDC_RAM_H_

extern void wdc_write_data_to_ram ( uint8_t *buffer, uint16_t address, uint16_t count );
extern void wdc_read_data_from_ram ( uint8_t *buffer, uint16_t address, uint16_t count );

#define convert_ram_address(x) ((x) - 0x2000)        /* The S-RAM of the original WDC hardware starts at 0x2000. We use an array as RAM here and no direct RAM access     */

#endif /* WDC_RAM_H_ */