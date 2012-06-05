/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_drv_mmc.h,v 1.3 2012/06/05 18:21:48 olivleh1 Exp $
 *
 */

#ifndef MMC_H_
#define MMC_H_

extern uint8_t mmc_init ();

extern uint8_t mmc_read_sector ( uint32_t addr, uint8_t *Buffer );
extern uint8_t mmc_write_sector ( uint32_t addr, uint8_t *Buffer );

extern uint8_t mmc_read_multiblock ( uint32_t addr, uint8_t *Buffer, uint8_t numblocks );
extern uint8_t mmc_write_multiblock_predef ( uint32_t addr, uint8_t *Buffer, uint8_t numblocks );
extern uint8_t mmc_write_multiblock ( uint32_t addr, uint8_t *Buffer, uint8_t numblocks );


#endif /* MMC_H_ */


