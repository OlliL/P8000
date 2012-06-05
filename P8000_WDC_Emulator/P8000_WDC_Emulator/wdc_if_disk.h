/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_if_disk.h,v 1.4 2012/06/05 18:21:48 olivleh1 Exp $
 *
 */

#ifndef WDC_IF_SDCARD_H_
#define WDC_IF_SDCARD_H_

extern uint8_t wdc_init_sdcard();
extern uint32_t wdc_sector2sdblock ( uint16_t req_cylinder, uint8_t req_head, uint8_t req_sector );
extern uint32_t wdc_p8kblock2sdblock ( uint32_t blockno );
extern uint8_t wdc_write_sector ( uint32_t addr, uint8_t *sector );
extern uint8_t wdc_read_sector ( uint32_t addr, uint8_t *sector );
extern uint8_t wdc_read_multiblock ( uint32_t addr, uint8_t *sector, uint8_t numblocks );
extern uint8_t wdc_write_multiblock ( uint32_t addr, uint8_t *sector, uint8_t numblocks );
#endif /* WDC_IF_SDCARD_H_ */