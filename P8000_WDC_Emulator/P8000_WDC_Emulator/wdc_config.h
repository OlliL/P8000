/*-
 * Copyright (c) 2012 Oliver Lehmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * $Id: wdc_config.h,v 1.11 2012/06/09 00:22:28 olivleh1 Exp $
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

#define PIN_MMC_CS      PINB4       /* AVR Port where the MMC Card /CS signal is connected to */
#define PIN_MMC_MOSI    PINB5       /* AVR Port: MOSI / MMC Card: Data-In */
#define PIN_MMC_MISO    PINB6       /* AVR Port: MISO / MMC Card: Data-Out */
#define PIN_MMC_SCK     PINB7       /* AVR Port: SCk / MMC Card: CLK */

/*
 * maximum performance with both turned off
 *
 *
 * #define MMC_PRESET_MULTIBLOCKCOUNT 1
 * #define SPI_CRC       1
 */

#define MMC_MULTIBLOCK 1

#define MEASURE_SDCARD_TIME 1
#undef  MEASURE_SDCARD_TIME

#endif /* CONFIG_H_ */