/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_if_p8000.h,v 1.5 2012/06/05 18:21:48 olivleh1 Exp $
 *
 */

#ifndef WDC_IF_PIO_H_
#define WDC_IF_PIO_H_

#define DELAY_PIO_US                500

#define configure_port_data_read()  DDR_DATA = 0x00                /* programs the DATA-Port as Input-only for reading from the P8000*/
#define configure_port_data_write() DDR_DATA = 0xff                /* programs the DATA-Port as Out-only for writing to the P8000*/

#define configure_pin_status0()     DDR_INFO |=  (1 << PIN_INFO_STATUS0)
#define configure_pin_status1()     DDR_INFO |=  (1 << PIN_INFO_STATUS1)
#define configure_pin_status2()     DDR_INFO |=  (1 << PIN_INFO_STATUS2)
#define configure_pin_astb()        DDR_INFO |=  (1 << PIN_INFO_ASTB)
#define configure_pin_te()          DDR_INFO &= ~(1 << PIN_INFO_TE)
#define configure_pin_wdardy()      DDR_INFO &= ~(1 << PIN_INFO_WDARDY)
#define configure_pin_tr()          DDR_INFO |=  (1 << PIN_INFO_TR)
#define configure_pin_reset()       DDR_INFO &= ~(1 << PIN_INFO_RST)

#define port_data_set(x)            (PORT_DATA = (x))
#define port_info_set(x)            (PORT_INFO = (x))
#define port_data_get(x)            PIN_DATA
#define port_info_get(x)            PIN_INFO

/* input pin handling */
#define isset_info_reset()          ((PIN_INFO) & (1 << PIN_INFO_RST))
#define isset_info_te()             ((PIN_INFO) & (1 << PIN_INFO_TE))
#define isset_info_wdardy()         ((PIN_INFO) & (1 << PIN_INFO_WDARDY))

/* output pin handling */
#define DATA_CLEAR          0x00
#define INFO_CLEAR          0x00

#define INFO_ASTB           (1 << PIN_INFO_ASTB)                                                               /* 0x08 */
#define INFO_STAT_GCMD      (1 << PIN_INFO_STATUS0)                                                            /* 0x01 */
#define INFO_STAT_RDATA     (1 << PIN_INFO_STATUS1)                                                            /* 0x02 */
#define INFO_STAT_WDATA     ((1 << PIN_INFO_STATUS0) | (1 << PIN_INFO_STATUS1))                                /* 0x03 */
#define INFO_STAT_ERROR     ((1 << PIN_INFO_STATUS0) | (1 << PIN_INFO_STATUS1) | (1 << PIN_INFO_STATUS2))      /* 0x07 */
#define INFO_TR             (1 << PIN_INFO_TR)                                                                 /* 0x40 */

extern void wdc_init_ports();
extern void wdc_wait_for_reset();
extern void wdc_receive_cmd ( uint8_t *buffer, uint16_t count );
extern void wdc_receive_data ( uint8_t *buffer, uint16_t count );
extern void wdc_send_data ( uint8_t *buffer, uint16_t count );
extern void wdc_send_error();

#endif /* WDC_IF_PIO_H_ */