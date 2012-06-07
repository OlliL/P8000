/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_drv_mmc.h,v 1.6 2012/06/07 01:03:17 olivleh1 Exp $
 *
 */

#ifndef MMC_H_
#define MMC_H_

extern uint8_t mmc_init ();

extern uint8_t mmc_read_sector ( uint32_t addr, uint8_t *buffer );
extern uint8_t mmc_write_sector ( uint32_t addr, uint8_t *buffer );

extern uint8_t mmc_read_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks );
extern uint8_t mmc_write_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks );

#define MMC_BLOCKLEN 512

#endif /* MMC_H_ */


