/*
 * wdc_drv_pata.h
 *
 * Created: 12.06.2012 21:57:08
 *  Author: olivleh1
 */


#ifndef WDC_DRV_PATA_H_
#define WDC_DRV_PATA_H_

extern uint8_t pata_init ();

extern uint8_t pata_read_block ( uint32_t addr, uint8_t *buffer );
extern uint8_t pata_write_block ( uint32_t addr, uint8_t *buffer );

extern uint8_t pata_read_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks );
extern uint8_t pata_write_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks );

#endif /* WDC_DRV_PATA_H_ */