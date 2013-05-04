/*-
 * Copyright (c) 2013 Oliver Lehmann
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
 * $Id: uart.c,v 1.6 2013/05/04 15:40:14 olivleh1 Exp $
 */

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>

#include "uart.h"
#include "wdc_config.h"

/* Define baud rate */
#define BAUD         9600UL
#define UBBR_VALUE   (( F_CPU / ( BAUD * 16 )) - 1 )


void uart_init ()
{
    /* Set baud rate */
    UBRR0H = (uint8_t)( UBBR_VALUE >> 8 );
    UBRR0L = (uint8_t)UBBR_VALUE;
    /* Set frame format to 8 data bits, no parity, 1 stop bit */
    UCSR0C = ( 0 << USBS0 ) | ( 1 << UCSZ01 ) | ( 1 << UCSZ00 );
    /* Enable receiver and transmitter */
    UCSR0B = ( 1 << RXEN0 ) | ( 1 << TXEN0 );
}

void uart_putc ( uint8_t data )
{
    /* Wait if a byte is being transmitted */
    while ( ( UCSR0A & ( 1 << UDRE0 ) ) == 0 ) {}

    /* Transmit data */
    UDR0 = data;
}

void uart_putc_hex ( uint8_t b )
{
    /* upper nibble */
    if ( ( b >> 4 ) < 0x0a ) {
        uart_putc ( ( b >> 4 ) + '0' );
    } else {
        uart_putc ( ( b >> 4 ) - 0x0a + 'a' );
    }

    /* lower nibble */
    if ( ( b & 0x0f ) < 0x0a ) {
        uart_putc ( ( b & 0x0f ) + '0' );
    } else {
        uart_putc ( ( b & 0x0f ) - 0x0a + 'a' );
    }
}

void uart_putw_dec ( uint16_t w )
{
    uint16_t num = 10000;
    uint8_t  started = 0;

    while ( num > 0 ) {
        uint8_t b = w / num;

        if ( b > 0 || started || num == 1 ) {
            uart_putc ( '0' + b );
            started = 1;
        }

        w -= b * num;
        num /= 10;
    }
}

void uart_putdw_dec ( uint32_t dw )
{
    uint32_t num = 1000000000;
    uint8_t  started = 0;

    while ( num > 0 ) {
        uint8_t b = dw / num;

        if ( b > 0 || started || num == 1 ) {
            uart_putc ( '0' + b );
            started = 1;
        }

        dw -= b * num;
        num /= 10;
    }
}

void uart_put_nl ()
{
    uart_putc ( '\n' );
    uart_putc ( '\r' );
}

void uart_putstring ( PGM_P str, bool newline )
{
    uint8_t i = 0;

    while ( 1 ) {
        char c = pgm_read_byte ( &str[i] );

        if ( c != '\0' ) {
            uart_putc ( c );
            i++;
        } else {
            break;
        }
    }

    if ( newline ) {
        uart_put_nl();
    }
}

EMPTY_INTERRUPT ( USART0_RX_vect )