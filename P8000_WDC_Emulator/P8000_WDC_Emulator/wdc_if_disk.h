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
 * $Id: wdc_if_disk.h,v 1.6 2012/06/07 18:01:02 olivleh1 Exp $
 */

#ifndef WDC_IF_SDCARD_H_
#define WDC_IF_SDCARD_H_

extern uint8_t wdc_init_sdcard();
extern uint32_t wdc_sector2sdblock ( uint16_t req_cylinder, uint8_t req_head, uint8_t req_sector );
extern uint32_t wdc_p8kblock2sdblock ( uint32_t blockno );
extern uint8_t wdc_write_sector ( uint32_t addr, uint8_t *sector );
extern uint8_t wdc_read_sector ( uint32_t addr, uint8_t *sector );
extern uint8_t wdc_read_multiblock ( uint32_t addr, uint8_t *sector, uint8_t numblocks );
extern uint8_t wdc_write_multiblock ( uint32_t addr, uint8_t *sector, uint8_t numblocks );

#define WDC_BLOCKLEN 512

#endif /* WDC_IF_SDCARD_H_ */