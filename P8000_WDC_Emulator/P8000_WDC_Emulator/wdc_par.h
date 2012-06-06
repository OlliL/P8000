/*
 * P8000 WDC Emulator
 *
 * $IdS$
 *
 */

#ifndef WDC_PAR_H_
#define WDC_PAR_H_

extern void wdc_read_par_table ( uint8_t *buffer, uint16_t count );
extern void wdc_write_par_table ( uint8_t *buffer, uint16_t count );

extern void wdc_read_wdc_par ( uint8_t *buffer, uint16_t count );
extern void wdc_write_wdc_par ( uint8_t *buffer, uint16_t count );

extern void wdc_del_wdc_btt();
extern void wdc_read_wdc_btt ( uint8_t *buffer, uint16_t count );
extern void wdc_write_wdc_btt ( uint8_t *buffer, uint16_t count );
extern uint16_t wdc_get_btt_count();
extern uint8_t wdc_add_btt_entry ( uint16_t cylinder, uint8_t head );

extern uint8_t wdc_get_hdd_sectors();
extern uint8_t wdc_get_hdd_heads();

#define POS_PAR_VERSION     0
#define POS_PAR_WDC_PAR     8
#define POS_PAR_WDC_BTT     88

#define SIZE_PAR_VERSION    8
#define SIZE_PAR_WDC_PAR    80
#define SIZE_PAR_WDC_BTT    125

#endif /* WDC_PAR_H_ */