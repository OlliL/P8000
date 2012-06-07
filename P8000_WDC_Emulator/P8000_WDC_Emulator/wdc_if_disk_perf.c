/*
 * mmc_perf.c
 *
 * Created: 07.06.2012 21:42:41
 *  Author: olivleh1
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include "config.h"
#include "wdc_if_sdcard.h"
#include "wdc_ram.h"
#include "uart.h"

#ifdef MEASURE_SDCARD_TIME
#define BLOCKNO 550000

extern  uint32_t overflow;

/* switched from local to global for keeping an eye on memory usage */
uint8_t data_buffer[4096];
uint8_t cmd_buffer[9];


void test_write4k(uint8_t numblocks, uint8_t nr_of_tests)
{
    uint32_t starttime;
    uint32_t blockno = 0;
    uint8_t i8, errorcode;

    uart_puts_p ( PSTR ( "Test Write 5000 512B-Blocks with Multiblock-Write (4KB Blocksize):" ) );
    uart_putc ( '\n' );
    memset ( &data_buffer[0], 0xCC, WDC_BLOCKLEN * numblocks );
    for ( i8 = 0; i8 < nr_of_tests; i8++ ) {
        blockno = BLOCKNO;
        for ( starttime = overflow; blockno < ( BLOCKNO + 5000 ); blockno += numblocks ) {
            errorcode = wdc_write_multiblock ( blockno, data_buffer, numblocks );
            if ( errorcode ) {
                break;
            }
        }
        if ( errorcode )
        break;
        uart_putdw_dec ( overflow - starttime );
        uart_putc ( '\n' );
    }

}

void test_read4k(uint8_t numblocks, uint8_t nr_of_tests)
{
    uint32_t starttime;
    uint32_t blockno = 0;
    uint8_t i8, errorcode;

    uart_puts_p ( PSTR ( "Test Read 5000 512B-Blocks with Multiblock-Read (4KB Blocksize):" ) );
    uart_putc ( '\n' );

    for ( i8 = 0; i8 < nr_of_tests; i8++ ) {
        blockno = BLOCKNO;
        for ( starttime = overflow; blockno < ( BLOCKNO + 5000 ); blockno += numblocks ) {
            errorcode = wdc_read_multiblock ( blockno, data_buffer, numblocks );
            if ( errorcode ) {
                break;
            }
        }
        if ( errorcode )
        break;
        uart_putdw_dec ( overflow - starttime );
        uart_putc ( '\n' );
    }

}

void test_write512(uint8_t numblocks, uint8_t nr_of_tests)
{
    uint32_t starttime;
    uint32_t blockno = 0;
    uint8_t i8, errorcode;

    uart_puts_p ( PSTR ( "Test Write 5000 512B-Blocks with Singleblock-Write (512B Blocksize):" ) );
    uart_putc ( '\n' );
    memset ( &data_buffer[0], 0xCC, WDC_BLOCKLEN * numblocks );
    for ( i8 = 0; i8 < nr_of_tests; i8++ ) {
        blockno = BLOCKNO;
        for ( starttime = overflow; blockno < ( BLOCKNO + 5000 ); blockno++ ) {
            errorcode = wdc_write_sector ( blockno, data_buffer );
            if ( errorcode ) {
                break;
            }
        }
        if ( errorcode )
        break;
        uart_putdw_dec ( overflow - starttime );
        uart_putc ( '\n' );
    }

}

void test_read512(uint8_t numblocks, uint8_t nr_of_tests)
{
    uint32_t starttime;
    uint32_t blockno = 0;
    uint8_t i8, errorcode;

    uart_puts_p ( PSTR ( "Test Read 5000 512B-Blocks with Multiblock-Read (4KB Blocksize):" ) );
    uart_putc ( '\n' );

    for ( i8 = 0; i8 < nr_of_tests; i8++ ) {
        blockno = BLOCKNO;
        for ( starttime = overflow; blockno < ( BLOCKNO + 5000 ); blockno += numblocks ) {
            errorcode = wdc_read_multiblock ( blockno, data_buffer, numblocks );
            if ( errorcode ) {
                break;
            }
        }
        if ( errorcode )
        break;
        uart_putdw_dec ( overflow - starttime );
        uart_putc ( '\n' );
    }

}

void measure_performance()
{
    uint8_t numblocks = 4;
    uint8_t nr_of_tests = 2;

    sei();
    TIMSK0 |= ( 0x01 << TOIE0 );
    TCCR0B = ( 1 << CS01 ); /* Prescaler 8 */
    TCNT0 = 0x00;

    uart_puts_p ( PSTR ( "Performance Measurement. Displayed are the Timer-Overflows." ) );
    uart_putc ( '\n' );
    uart_puts_p ( PSTR ( "Timer is initialized with a Prescaler 8" ) );
    uart_putc ( '\n' );

    test_write512(numblocks,nr_of_tests);
    test_read512(numblocks,nr_of_tests);
    test_write512(numblocks,nr_of_tests);
    test_read4k(numblocks,nr_of_tests);
    test_write4k(numblocks,nr_of_tests);
    test_read4k(numblocks,nr_of_tests);
    test_write512(numblocks,nr_of_tests);
    test_write4k(numblocks,nr_of_tests);
    test_read512(numblocks,nr_of_tests);
    test_read4k(numblocks,nr_of_tests);
    test_read512(numblocks,nr_of_tests);
    test_write4k(numblocks,nr_of_tests);
    test_write512(numblocks,nr_of_tests);

    uart_puts_p ( PSTR ( "End" ) );
    uart_putc ( '\n' );

    while ( 1 );
}

#endif