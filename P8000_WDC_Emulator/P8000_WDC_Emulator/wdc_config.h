/*
 * P8000 WDC Emulator
 *
 * $Id: wdc_config.h,v 1.5 2012/06/05 18:21:48 olivleh1 Exp $
 *
 */


#ifndef CONFIG_H_
#define CONFIG_H_

#define F_CPU        18432000UL

/*
 * P8000 Interface
 */

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

/*
 * SD-Card Interface
 */
#define PORT_MMC        PORTB
#define PIN_MMC         PINB
#define DDR_MMC         DDRB

#define PIN_MMC_CS      PINB4       //AVR Port where the MMC Card /CS signal is connected to
#define PIN_MMC_MOSI    PINB5       //AVR Port: MOSI / MMC Card: Data-In
#define PIN_MMC_MISO    PINB6       //AVR Port: MISO / MMC Card: Data-Out
#define PIN_MMC_SCK     PINB7       //AVR Port: SCk / MMC Card: CLK

//maximum performance with both turned off
//#define MMC_PRESET_MULTIBLOCKCOUNT 1
//#define SPI_CRC       1

#endif /* CONFIG_H_ */