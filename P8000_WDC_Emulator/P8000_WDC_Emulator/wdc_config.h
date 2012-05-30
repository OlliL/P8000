/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_config.h,v 1.4 2012/05/30 20:07:10 olivleh1 Exp $
 * 
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#define F_CPU        18432000UL

#define PORT_DATA       PORTA
#define PIN_DATA        PINA
#define DDR_DATA        DDRA

#define PORT_INFO       PORTC
#define PIN_INFO        PINC
#define DDR_INFO        DDRC

#define PIN_INFO_STATUS0    PINC0
#define PIN_INFO_STATUS1    PINC1
#define PIN_INFO_STATUS2    PINC2
#define PIN_INFO_ASTB       PINC3
#define PIN_INFO_TE         PINC4
#define PIN_INFO_WDARDY     PINC5
#define PIN_INFO_TR         PINC6
#define PIN_INFO_RST        PINC7

#endif /* CONFIG_H_ */