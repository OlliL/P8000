/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_ram.h,v 1.2 2012/06/02 00:51:55 olivleh1 Exp $
 *
 */

#ifndef WDC_RAM_H_
#define WDC_RAM_H_

void wdc_write_data_to_ram ( uint8_t *buffer, uint16_t address, uint16_t count );
void wdc_read_data_from_ram ( uint8_t *buffer, uint16_t address, uint16_t count );

void wdc_read_par_table ( uint8_t *buffer, uint16_t count );
void wdc_write_par_table ( uint8_t *buffer, uint16_t count );

void wdc_read_wdc_par ( uint8_t *buffer, uint16_t count );
void wdc_write_wdc_par ( uint8_t *buffer, uint16_t count );

void wdc_del_wdc_btt();
void wdc_read_wdc_btt ( uint8_t *buffer, uint16_t count );
void wdc_write_wdc_btt ( uint8_t *buffer, uint16_t count );
uint16_t wdc_get_btt_count();
uint8_t wdc_add_btt_entry ( uint16_t cylinder, uint8_t head );

uint8_t wdc_get_hdd_sectors();

#define convert_ram_address(x) ((x) - 0x2000)        /* The S-RAM of the original WDC hardware starts at 0x2000. We use an array as RAM here and no direct RAM access     */

#define POS_PAR_VERSION     0
#define POS_PAR_WDC_PAR     8
#define POS_PAR_WDC_BTT     88

#define SIZE_PAR_VERSION    8
#define SIZE_PAR_WDC_PAR    80
#define SIZE_PAR_WDC_BTT    125

#endif /* WDC_RAM_H_ */