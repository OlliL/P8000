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
 * $Id: wdc_par.h,v 1.4 2012/06/09 19:47:31 olivleh1 Exp $
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
extern void wdc_set_disk_invalid();
extern void wdc_set_disk_valid();
extern uint8_t wdc_get_disk_valid();
extern void wdc_set_no_disk();
extern uint8_t wdc_get_num_of_drvs();
extern void wdc_set_initialized ( uint8_t num );
extern uint8_t wdc_get_initialized ();

#define POS_PAR_VERSION     0
#define POS_PAR_WDC_PAR     8
#define POS_PAR_WDC_BTT     0x88

#define SIZE_PAR_VERSION    8
#define SIZE_PAR_WDC_PAR    0x80
#define SIZE_PAR_WDC_BTT    125

#endif /* WDC_PAR_H_ */