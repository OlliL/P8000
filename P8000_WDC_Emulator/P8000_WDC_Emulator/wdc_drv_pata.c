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
 * $Id: wdc_drv_pata.c,v 1.3 2012/06/14 20:24:24 olivleh1 Exp $
 */

#include "wdc_config.h"
#include <avr/io.h>
#include <util/delay.h>
#include "wdc_avr.h"
#include "uart.h"

void pata_set_highbyte ( uint8_t byte )
{
    port_data_set ( byte );
    enable_atalatch();
    nop();
    nop();
    disable_atalatch();
}

void pata_set_lowbyte ( uint8_t byte )
{
    port_data_set ( byte );
}

void pata_send_data()
{
    enable_rdwrtoata();
    _delay_us(10);
//    disable_rdwrtoata();
}

uint8_t pata_init ()
{
uart_putc('1');
    /* test write */
    configure_port_data_write();
    ata_wr_enable();
    pata_set_highbyte ( 0xff );
    pata_set_lowbyte ( 0x00 );
    pata_send_data();
//    ata_wr_disable();
    return 1;
}

uint8_t pata_read_block ( uint32_t addr, uint8_t *buffer )
{
    return 1;
}

uint8_t pata_write_block ( uint32_t addr, uint8_t *buffer )
{
    return 1;
}

uint8_t pata_read_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks )
{
    return 1;
}

uint8_t pata_write_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks )
{
    return 1;
}
