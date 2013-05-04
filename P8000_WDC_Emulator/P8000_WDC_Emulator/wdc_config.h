/*-
 * Copyright (c) 2012, 2013 Oliver Lehmann
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
 * $Id: wdc_config.h,v 1.22 2013/05/04 15:40:14 olivleh1 Exp $
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

#define PORT_ADRDEC     PORTD
#define PIN_ADRDEC      PIND
#define DDR_ADRDEC      DDRD


#define PIN_DATA_D0         PINA0
#define PIN_DATA_D1         PINA1
#define PIN_DATA_D2         PINA2
#define PIN_DATA_D3         PINA3
#define PIN_DATA_D4         PINA4
#define PIN_DATA_D5         PINA5
#define PIN_DATA_D6         PINA6
#define PIN_DATA_D7         PINA7

#define PIN_INFO_STATUS0    PINC0
#define PIN_INFO_STATUS1    PINC1
#define PIN_INFO_STATUS2    PINC2
#define PIN_INFO_TE         PINC4
#define PIN_INFO_WDARDY     PINC5
#define PIN_INFO_TR         PINC6
#define PIN_INFO_RST        PINC7

#define PIN_ADRDEC_ADDR0    PIND5
#define PIN_ADRDEC_ADDR1    PIND6
#define PIN_ADRDEC_ADDR2    PIND7

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
 * ATA Interface
 */

#define PORT_ATARDWR    PORTB
#define PIN_ATARDWR     PINB
#define DDR_ATARDWR     DDRB

#define PORT_ATACS      PORTB
#define PIN_ATACS       PINB
#define DDR_ATACS       DDRB

#define PORT_ATADA      PORTD
#define PIN_ATADA       PIND
#define DDR_ATADA       DDRD

#define PIN_ATARDWR_WR      PINB2
#define PIN_ATARDWR_RD      PINB3
#define PIN_ATACS_CS0       PINB0
#define PIN_ATACS_CS1       PINB1
#define PIN_ATADA_DA0       PIND2
#define PIN_ATADA_DA1       PIND4
#define PIN_ATADA_DA2       PIND3

/*
 * maximum performance with both turned off
 *
 *
 * #define MMC_PRESET_MULTIBLOCKCOUNT 1
 * #define SPI_CRC       1
 */

#define MMC_MULTIBLOCK 1

#define MEASURE_DISK_PERFORMANCE 1
#undef  MEASURE_DISK_PERFORMANCE

#define DEBUG 1

#define nop()  __asm__ __volatile__ ( "nop" ::)

#endif /* CONFIG_H_ */