/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_if_disk.h,v 1.3 2012/06/02 23:04:30 olivleh1 Exp $
 *
 */

#ifndef WDC_IF_SDCARD_H_
#define WDC_IF_SDCARD_H_

uint8_t wdc_init_sdcard();
uint32_t wdc_sector2sdblock ( uint16_t req_cylinder, uint8_t req_head, uint8_t req_sector );
uint32_t wdc_p8kblock2sdblock ( uint32_t blockno );
uint8_t wdc_write_sector ( uint16_t addr, uint8_t *sector );
uint8_t wdc_read_sector ( uint16_t addr, uint8_t *sector );

#endif /* WDC_IF_SDCARD_H_ */