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
 * $Id: wdc_avr.h,v 1.1 2012/06/12 17:34:14 olivleh1 Exp $
 */


#ifndef WDC_AVR_H_
#define WDC_AVR_H_

#define configure_port_data_read()  DDR_DATA = 0x00                /* programs the DATA-Port as Input-only for reading from the P8000*/
#define configure_port_data_write() DDR_DATA = 0xff                /* programs the DATA-Port as Out-only for writing to the P8000*/

#define configure_pin_status0()     DDR_INFO |=  ( 1 << PIN_INFO_STATUS0 )
#define configure_pin_status1()     DDR_INFO |=  ( 1 << PIN_INFO_STATUS1 )
#define configure_pin_status2()     DDR_INFO |=  ( 1 << PIN_INFO_STATUS2 )
#define configure_pin_te()          DDR_INFO &= ~( 1 << PIN_INFO_TE )
#define configure_pin_wdardy()      DDR_INFO &= ~( 1 << PIN_INFO_WDARDY )
#define configure_pin_tr()          DDR_INFO |=  ( 1 << PIN_INFO_TR )
#define configure_pin_reset()       DDR_INFO &= ~( 1 << PIN_INFO_RST )

#define port_data_set(x)            ( PORT_DATA = ( x ) )
#define port_info_set(x)            ( PORT_INFO = ( x ) )
#define port_data_get()             PIN_DATA
#define port_info_get()             PIN_INFO

/* functions dealing with the 74138 address decoder */
#define configure_pin_addr0()       DDR_ADRDEC |= ( 1 << PIN_ADRDEC_ADDR0 )
#define configure_pin_addr1()       DDR_ADRDEC |= ( 1 << PIN_ADRDEC_ADDR1 )
#define configure_pin_addr2()       DDR_ADRDEC |= ( 1 << PIN_ADRDEC_ADDR2 )

#define init_addressdecoder()       PORT_ADRDEC &=  ~( ( 1 << PIN_ADRDEC_ADDR0 ) | ( 1 << PIN_ADRDEC_ADDR1 ) | ( 1 << PIN_ADRDEC_ADDR2 ) )
#define enable_p8000_transeiver()   PORT_ADRDEC |=   ( 1 << PIN_ADRDEC_ADDR0 )
#define disable_p8000_transeiver()  PORT_ADRDEC &=  ~( 1 << PIN_ADRDEC_ADDR0 )

/* functions dealing with the MMC interface */

#define configure_pin_miso()        DDR_MMC &= ~ ( 1 << PIN_MMC_MISO )
#define configure_pin_sck()         DDR_MMC |= ( 1 << PIN_MMC_SCK )
#define configure_pin_mosi()        DDR_MMC |= ( 1 << PIN_MMC_MOSI )
#define configure_pin_mmc_cs()      DDR_MMC |= ( 1 << PIN_MMC_CS )

#define mmc_disable()               PORT_MMC|= (1<<PIN_MMC_CS);
#define mmc_enable()                PORT_MMC&=~(1<<PIN_MMC_CS);

extern void wdc_init_avr();

#endif /* WDC_AVR_H_ */