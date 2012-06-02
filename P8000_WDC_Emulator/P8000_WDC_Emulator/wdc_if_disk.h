/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_if_disk.h,v 1.2 2012/06/02 00:51:55 olivleh1 Exp $
 *
 */

#ifndef WDC_IF_SDCARD_H_
#define WDC_IF_SDCARD_H_

uint8_t wdc_init_sdcard();
uint32_t wdc_get_blockno ( uint16_t req_cylinder, uint8_t req_head, uint8_t req_sector );
uint8_t wdc_write_sector ( uint16_t addr, uint8_t *sector );
uint8_t wdc_read_sector ( uint16_t addr, uint8_t *sector );

#endif /* WDC_IF_SDCARD_H_ */