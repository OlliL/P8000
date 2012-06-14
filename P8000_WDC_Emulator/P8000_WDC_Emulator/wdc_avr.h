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
 * $Id: wdc_avr.h,v 1.3 2012/06/14 20:24:24 olivleh1 Exp $
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
#define enable_p8000com()           PORT_ADRDEC |=   ( 1 << PIN_ADRDEC_ADDR0 )
#define disable_p8000com()          PORT_ADRDEC &=  ~( 1 << PIN_ADRDEC_ADDR0 )
#define enable_rdwrtoata()          PORT_ADRDEC |=   ( 1 << PIN_ADRDEC_ADDR1 )
#define disable_rdwrtoata()         PORT_ADRDEC &=  ~( 1 << PIN_ADRDEC_ADDR1 )
#define enable_atalatch()           PORT_ADRDEC |=   ( ( 1 << PIN_ADRDEC_ADDR0 ) | ( 1 << PIN_ADRDEC_ADDR1 ))
#define disable_atalatch()          PORT_ADRDEC &=  ~( ( 1 << PIN_ADRDEC_ADDR0 ) | ( 1 << PIN_ADRDEC_ADDR1 ))
#define enable_sysconf()            PORT_ADRDEC |=   ( ( 1 << PIN_ADRDEC_ADDR0 ) | ( 1 << PIN_ADRDEC_ADDR1 ) | ( 1 << PIN_ADRDEC_ADDR2 ) )
#define disable_sysconf()           PORT_ADRDEC &=  ~( ( 1 << PIN_ADRDEC_ADDR0 ) | ( 1 << PIN_ADRDEC_ADDR1 ) | ( 1 << PIN_ADRDEC_ADDR2 ) )

/* functions dealing with the MMC interface */
#define configure_pin_miso()        DDR_MMC &= ~ ( 1 << PIN_MMC_MISO )
#define configure_pin_sck()         DDR_MMC |= ( 1 << PIN_MMC_SCK )
#define configure_pin_mosi()        DDR_MMC |= ( 1 << PIN_MMC_MOSI )
#define configure_pin_mmc_cs()      DDR_MMC |= ( 1 << PIN_MMC_CS )

#define mmc_disable()               PORT_MMC |= (1<<PIN_MMC_CS);
#define mmc_enable()                PORT_MMC &=~(1<<PIN_MMC_CS);

/* functions dealing with the sysconf jumperpad */
#define jumper_pata_set()           ~port_data_get() & (1<<PIN_DATA_D0)
#define jumper_nn2_set()            ~port_data_get() & (1<<PIN_DATA_D1)
#define jumper_nn3_set()            ~port_data_get() & (1<<PIN_DATA_D2)
#define jumper_nn4_set()            ~port_data_get() & (1<<PIN_DATA_D3)

/* functions dealing with the ATA interface */
#define configure_ata_wr()          DDR_ATARDWR |= ( 1 << PIN_ATARDWR_WR )
#define configure_ata_rd()          DDR_ATARDWR |= ( 1 << PIN_ATARDWR_RD )
#define configure_ata_cs0()         DDR_ATACS |= ( 1 << PIN_ATACS_CS0 )
#define configure_ata_cs1()         DDR_ATACS |= ( 1 << PIN_ATACS_CS1 )
#define configure_ata_da0()         DDR_ATADA |= ( 1 << PIN_ATADA_DA0 )
#define configure_ata_da1()         DDR_ATADA |= ( 1 << PIN_ATADA_DA1 )
#define configure_ata_da2()         DDR_ATADA |= ( 1 << PIN_ATADA_DA2 )

#define ata_wr_disable()            PORT_ATARDWR |=  (1<<PIN_ATARDWR_WR)
#define ata_wr_enable()             PORT_ATARDWR &= ~(1<<PIN_ATARDWR_WR)
#define ata_rd_disable()            PORT_ATARDWR |=  (1<<PIN_ATARDWR_RD)
#define ata_rd_enable()             PORT_ATARDWR &= ~(1<<PIN_ATARDWR_RD)
#define ata_cs0_disable()           PORT_ATACS |=  (1<<PIN_ATACS_CS0)
#define ata_cs0_enable()            PORT_ATACS &= ~(1<<PIN_ATACS_CS0)
#define ata_cs1_disable()           PORT_ATACS |=  (1<<PIN_ATACS_CS1)
#define ata_cs1_enable()            PORT_ATACS &= ~(1<<PIN_ATACS_CS1)
#define ata_da0_disable()           PORT_ATADA |=  (1<<PIN_ATADA_DA0)
#define ata_da0_enable()            PORT_ATADA &= ~(1<<PIN_ATADA_DA0)
#define ata_da1_disable()           PORT_ATADA |=  (1<<PIN_ATADA_DA1)
#define ata_da1_enable()            PORT_ATADA &= ~(1<<PIN_ATADA_DA1)
#define ata_da2_disable()           PORT_ATADA |=  (1<<PIN_ATADA_DA2)
#define ata_da2_enable()            PORT_ATADA &= ~(1<<PIN_ATADA_DA2)

extern void wdc_init_avr();
extern void wdc_get_sysconf();

#endif /* WDC_AVR_H_ */