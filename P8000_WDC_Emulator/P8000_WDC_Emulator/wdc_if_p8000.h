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
 * $Id: wdc_if_p8000.h,v 1.11 2012/06/24 09:10:40 olivleh1 Exp $
 */

#ifndef WDC_IF_P8000_H_
#define WDC_IF_P8000_H_

#define DELAY_PIO_US                550

/* input pin handling */
#define isset_info_reset()          ((PIN_INFO) & (1 << PIN_INFO_RST))
#define isset_info_te()             ((PIN_INFO) & (1 << PIN_INFO_TE))
#define isset_info_wdardy()         ((PIN_INFO) & (1 << PIN_INFO_WDARDY))

/* output pin handling */
#define DATA_CLEAR          0x00
#define INFO_CLEAR          0x00

#define INFO_STAT_GCMD      ((1 << PIN_INFO_STATUS0)                                                    )      /* 0x01 */
#define INFO_STAT_RDATA     (                          (1 << PIN_INFO_STATUS1)                          )      /* 0x02 */
#define INFO_STAT_WDATA     ((1 << PIN_INFO_STATUS0) | (1 << PIN_INFO_STATUS1)                          )      /* 0x03 */
#define _INFO_STAT_CRCERR   (                          (1 << PIN_INFO_STATUS1) | (1 << PIN_INFO_STATUS2))      /* 0x06 */
#define INFO_STAT_ERROR     ((1 << PIN_INFO_STATUS0) | (1 << PIN_INFO_STATUS1) | (1 << PIN_INFO_STATUS2))      /* 0x07 */
#define INFO_TR             (1 << PIN_INFO_TR)                                                                 /* 0x40 */

extern void wdc_wait_for_reset();
extern uint8_t wdc_receive_cmd ( uint8_t *buffer, uint16_t count );
extern void wdc_receive_data ( uint8_t *buffer, uint16_t count );
extern void wdc_send_data ( uint8_t *buffer, uint16_t count );
extern void wdc_send_error();
extern void wdc_send_errorcode ( uint8_t error );

#endif /* WDC_IF_P8000_H_ */