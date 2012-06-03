/*#######################################################################################
Connect AVR to MMC/SD

Copyright (C) 2004 Ulrich Radig

Bei Fragen und Verbesserungen wendet euch per EMail an

mail@ulrichradig.de

oder im Forum meiner Web Page : www.ulrichradig.de

Dieses Programm ist freie Software. Sie können es unter den Bedingungen der
GNU General Public License, wie von der Free Software Foundation veröffentlicht,
weitergeben und/oder modifizieren, entweder gemäß Version 2 der Lizenz oder
(nach Ihrer Option) jeder späteren Version.

Die Veröffentlichung dieses Programms erfolgt in der Hoffnung,
daß es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE,
sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT
FÜR EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License.

Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
Programm erhalten haben.
Falls nicht, schreiben Sie an die Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
#######################################################################################*/

#include "mmc.h"

#ifdef SPI_CRC
const uint16_t crc_itu_t_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

#define CRC7_POLY 0x09
unsigned char CRC_7Bit ( unsigned char *pData, unsigned int uiLen )
{

    unsigned int i, c, bit;
    unsigned int crc = 0;

    while ( uiLen-- ) {

        c = ( unsigned long ) * pData++;

        for ( i = 0x80; i; i >>= 1 ) {

            bit = crc & 0x40;
            crc <<= 1;
            if ( c & i ) crc |= 1;
            if ( bit ) crc ^= CRC7_POLY;
        }
    }

    for ( i = 0; i < 7; i++ ) {

        if ( crc & 0x40 ) {
            crc = ( crc << 1 ) ^ CRC7_POLY;
        } else
            crc <<= 1;
    }

    return ( crc & 0x7F );
}


unsigned short CRC_16Bit ( unsigned char *pData, unsigned int uiLen )
{
    unsigned short usCRC;

    //Init CRC register to zero (Required by SD card)
    usCRC = 0x0000;

    while ( uiLen-- ) {
        //Do table lookup and xor result
        //see "Painless guide to CRC error detection algorithms" section 10
        usCRC = ( usCRC << 8 ) ^ crc_itu_t_table[ ( usCRC >> 8 ) ^ *pData++];
    }

    return usCRC;
}
#endif

unsigned int SDSetCRC ( unsigned int uiCRC )
{
    unsigned char ucSDSetCRC[] = {0x3B, 0x00, 0x00, 0x00, 0x00, 0xFF};

    ucSDSetCRC[0] = 0x3B;
    //tail bit of uiCRC sets or clears CRC mode
    ucSDSetCRC[4] = uiCRC & 0x01;

    if ( mmc_write_command ( ucSDSetCRC ) != 0 ) {
        return 1;
    }
    return 0;
}

//############################################################################
//Routine zur Initialisierung der MMC/SD-Karte (SPI-MODE)
uint8_t mmc_init ()
//############################################################################
{
    uint8_t Timeout = 0;

    //Konfiguration des Ports an der die MMC/SD-Karte angeschlossen wurde
    MMC_Direction_REG &= ~ ( 1 << SPI_DI );     //Setzen von Pin MMC_DI auf Input
    MMC_Direction_REG |= ( 1 << SPI_Clock );    //Setzen von Pin MMC_Clock auf Output
    MMC_Direction_REG |= ( 1 << SPI_DO );       //Setzen von Pin MMC_DO auf Output
    MMC_Direction_REG |= ( 1 << MMC_Chip_Select ); //Setzen von Pin MMC_Chip_Select auf Output
    MMC_Write |= ( 1 << MMC_Chip_Select );      //Setzt den Pin MMC_Chip_Select auf High Pegel

    for ( uint8_t a = 0; a < 200; a++ ) {
        nop();
    };      //Wartet eine kurze Zeit

#if SPI_Mode
    //Aktiviren des SPI - Bus, Clock = Idel LOW
    //SPI Clock teilen durch 128
    SPCR = ( 1 << SPE ) | ( 1 << MSTR ) | ( 1 << SPR0 ) | ( 1 << SPR1 ); //Enable SPI, SPI in Master Mode
    SPSR = ( 0 << SPI2X );
#endif

    //Initialisiere MMC/SD-Karte in den SPI-Mode
    for ( uint8_t b = 0; b < 0x0f; b++ ) { //Sendet min 74+ Clocks an die MMC/SD-Karte
        mmc_write_byte ( 0xff );
    }

    //Sendet Commando CMD0 an MMC/SD-Karte
    uint8_t CMD[] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x95};
    while ( mmc_write_command ( CMD ) != 1 ) {
        if ( Timeout++ > 200 ) {
            MMC_Disable();
            return ( 1 ); //Abbruch bei Commando1 (Return Code1)
        }
    }
    //Sendet Commando CMD1 an MMC/SD-Karte
    Timeout = 0;
    CMD[0] = 0x41;//Commando 1
    CMD[5] = 0xFF;
    while ( mmc_write_command ( CMD ) != 0 ) {
        if ( Timeout++ > 400 ) {
            MMC_Disable();
            return ( 2 ); //Abbruch bei Commando2 (Return Code2)
        }
    }
#if SPI_Mode
    //SPI Bus auf max Geschwindigkeit
    SPCR &= ~ ( ( 1 << SPR0 ) | ( 1 << SPR1 ) );
    SPSR = SPSR | ( 1 << SPI2X );
#endif

#ifdef SPI_CRC
    SDSetCRC ( 1 );
#else
    SDSetCRC ( 0 );
#endif

    //set MMC_Chip_Select to high (MMC/SD-Karte Inaktiv)
    MMC_Disable();
    return ( 0 );
}

//############################################################################
//Sendet ein Commando an die MMC/SD-Karte
uint8_t mmc_write_command ( uint8_t *cmd )
//############################################################################
{
    uint8_t tmp = 0xff;
    uint8_t Timeout = 0;
#ifdef SPI_CRC
    unsigned char ucCRC;

    //Calculate CRC and framing bits
    cmd [0] = ( cmd[0] | ( 1 << 6 ) ) & 0x7F;
    ucCRC = CRC_7Bit ( cmd, 5 ) << 1;
    cmd[5] =  ucCRC;
#endif
    //set MMC_Chip_Select to high (MMC/SD-Karte Inaktiv)
    MMC_Disable();

    //sendet 8 Clock Impulse
    mmc_write_byte ( 0xFF );

    //set MMC_Chip_Select to low (MMC/SD-Karte Aktiv)
    MMC_Enable();

    //sendet 6 Byte Commando
    for ( uint8_t a = 0; a < 0x06; a++ ) { //sendet 6 Byte Commando zur MMC/SD-Karte
        mmc_write_byte ( *cmd++ );
    }

    //Wartet auf ein gültige Antwort von der MMC/SD-Karte
    while ( tmp == 0xff ) {
        tmp = mmc_read_byte();
        if ( Timeout++ > 500 ) {
            break; //Abbruch da die MMC/SD-Karte nicht Antwortet
        }
    }
    return ( tmp );
}

//############################################################################
//Routine zum Empfangen eines Bytes von der MMC-Karte
uint8_t mmc_read_byte ( void )
//############################################################################
{
    uint8_t Byte = 0;
#if SPI_Mode    //Routine für Hardware SPI
    SPDR = 0xff;
    while ( ! ( SPSR & ( 1 << SPIF ) ) ) {};
    Byte = SPDR;

#else           //Routine für Software SPI
    for ( uint8_t a = 8; a > 0; a-- ) { //das Byte wird Bitweise nacheinander Empangen MSB First
        MMC_Write &= ~ ( 1 << SPI_Clock ); //erzeugt ein Clock Impuls (Low)

        if ( bit_is_set ( MMC_Read, SPI_DI ) > 0 ) { //Lesen des Pegels von MMC_DI
            Byte |= ( 1 << ( a - 1 ) );
        } else {
            Byte &= ~ ( 1 << ( a - 1 ) );
        }
        MMC_Write |= ( 1 << SPI_Clock ); //setzt Clock Impuls wieder auf (High)
    }
#endif
    return ( Byte );
}


//############################################################################
//Routine zum Senden eines Bytes zur MMC-Karte
void mmc_write_byte ( uint8_t Byte )
//############################################################################
{
#if SPI_Mode        //Routine für Hardware SPI

    SPDR = Byte;    //Sendet ein Byte

    while ( ! ( SPSR & ( 1 << SPIF ) ) ) { //Wartet bis Byte gesendet wurde
    }

#else           //Routine für Software SPI
    for ( uint8_t a = 8; a > 0; a-- ) { //das Byte wird Bitweise nacheinander Gesendet MSB First
        if ( bit_is_set ( Byte, ( a - 1 ) ) > 0 ) { //Ist Bit a in Byte gesetzt
            MMC_Write |= ( 1 << SPI_DO ); //Set Output High
        } else {
            MMC_Write &= ~ ( 1 << SPI_DO ); //Set Output Low
        }
        MMC_Write &= ~ ( 1 << SPI_Clock ); //erzeugt ein Clock Impuls (LOW)

        MMC_Write |= ( 1 << SPI_Clock ); //setzt Clock Impuls wieder auf (High)
    }
    MMC_Write |= ( 1 << SPI_DO ); //setzt Output wieder auf High
#endif
}

//############################################################################
//Routine zum schreiben eines Blocks(512Byte) auf die MMC/SD-Karte
uint8_t mmc_write_sector ( uint32_t addr, uint8_t *Buffer )
//############################################################################
{
    uint8_t tmp;
    //Commando 24 zum schreiben eines Blocks auf die MMC/SD - Karte
    uint8_t cmd[] = {0x58, 0x00, 0x00, 0x00, 0x00, 0xFF};
#ifdef SPI_CRC
    unsigned int uiCheckSum;
#endif
    /*Die Adressierung der MMC/SD-Karte wird in Bytes angegeben,
      addr wird von Blocks zu Bytes umgerechnet danach werden
      diese in das Commando eingefügt*/
    addr = addr << 9; //addr = addr * 512

    cmd[1] = ( ( addr & 0xFF000000 ) >> 24 );
    cmd[2] = ( ( addr & 0x00FF0000 ) >> 16 );
    cmd[3] = ( ( addr & 0x0000FF00 ) >> 8 );

    //Sendet Commando cmd24 an MMC/SD-Karte (Write 1 Block/512 Bytes)
    tmp = mmc_write_command ( cmd );
    if ( tmp != 0 ) {
        return ( tmp );
    }

    SPDR = 0xFE;    //Sendet Startbyte

#ifdef SPI_CRC
    uiCheckSum = CRC_16Bit ( Buffer, 512 );
#endif

    //Schreiben des Bolcks (512Bytes) auf MMC/SD-Karte
    for ( uint16_t a = 0; a < 512; a++ ) {
        uint8_t data = *Buffer;
        Buffer++;
        while ( ! ( SPSR & ( 1 << SPIF ) ) );
        SPDR = data;    //Sendet ein Byte
    }
    while ( ! ( SPSR & ( 1 << SPIF ) ) );

    //CRC-Byte schreiben
#ifdef SPI_CRC
    SPDR = uiCheckSum >> 8;
    while ( ! ( SPSR & ( 1 << SPIF ) ) );
    SPDR = uiCheckSum & 0xFF;
#else
    SPDR = 0xFF;
    while ( ! ( SPSR & ( 1 << SPIF ) ) );
    SPDR = 0xFF;
#endif
    while ( ! ( SPSR & ( 1 << SPIF ) ) );

    //Fehler beim schreiben? (Data Response XXX00101 = OK)
    SPDR = 0xff;
    while ( ! ( SPSR & ( 1 << SPIF ) ) ) {};
    if ( ( SPDR & 0x1F ) != 0x05 ) return ( 0x02 );

    //Wartet auf MMC/SD-Karte Bussy
    while ( 1 ) {
        SPDR = 0xff;
        while ( ! ( SPSR & ( 1 << SPIF ) ) ) {};
        if ( SPDR == 0xff )
            break;
    }

    //set MMC_Chip_Select to high (MMC/SD-Karte Inaktiv)
    MMC_Disable();

    return ( 0 );
}

//############################################################################
//Routine zum lesen des CID Registers von der MMC/SD-Karte (16Bytes)
uint8_t mmc_read_block ( uint8_t *cmd, uint8_t *Buffer, uint16_t Bytes )
//############################################################################
{
#ifdef SPI_CRC
    uint16_t crc;
#endif
    //Sendet Commando cmd an MMC/SD-Karte
    if ( mmc_write_command ( cmd ) != 0 ) {
        return ( 1 );
    }

    //Wartet auf Start Byte von der MMC/SD-Karte (FEh/Start Byte)
    while ( 1 ) {
        SPDR = 0xff;
        while ( ! ( SPSR & ( 1 << SPIF ) ) ) {};
        if ( SPDR == 0xfe )
            break;
    }

    SPDR = 0xff;
    while ( ! ( SPSR & ( 1 << SPIF ) ) );
    *Buffer = SPDR;

    //Lesen des Bolcks (normal 512Bytes) von MMC/SD-Karte
    for ( uint16_t a = 0; a < Bytes - 1; a++ ) {
        SPDR = 0xff;
        Buffer++;
        while ( ! ( SPSR & ( 1 << SPIF ) ) );
        *Buffer = SPDR;
    }

    Buffer = Buffer - ( Bytes - 1 );

    //CRC-Byte auslesen
#ifdef SPI_CRC
    SPDR = 0xff;
    while ( ! ( SPSR & ( 1 << SPIF ) ) );
    crc = SPDR << 8;
    SPDR = 0xff;
    while ( ! ( SPSR & ( 1 << SPIF ) ) );
    crc |= SPDR;

    if ( crc != CRC_16Bit ( Buffer, Bytes ) ) {
        MMC_Disable();
        return ( 1 );
    }
#else
    SPDR = 0xff;
    while ( ! ( SPSR & ( 1 << SPIF ) ) );
    SPDR;
    SPDR = 0xff;
    while ( ! ( SPSR & ( 1 << SPIF ) ) );
    SPDR;
#endif


    //set MMC_Chip_Select to high (MMC/SD-Karte Inaktiv)
    MMC_Disable();

    return ( 0 );
}

//############################################################################
//Routine zum lesen eines Blocks(512Byte) von der MMC/SD-Karte
uint8_t mmc_read_sector ( uint32_t addr, uint8_t *Buffer )
//############################################################################
{
    //Commando 16 zum lesen eines Blocks von der MMC/SD - Karte
    uint8_t cmd[] = {0x51, 0x00, 0x00, 0x00, 0x00, 0xFF};

    /*Die Adressierung der MMC/SD-Karte wird in Bytes angegeben,
      addr wird von Blocks zu Bytes umgerechnet danach werden
      diese in das Commando eingefügt*/

    addr = addr << 9; //addr = addr * 512

    cmd[1] = ( ( addr & 0xFF000000 ) >> 24 );
    cmd[2] = ( ( addr & 0x00FF0000 ) >> 16 );
    cmd[3] = ( ( addr & 0x0000FF00 ) >> 8 );

    return mmc_read_block ( cmd, Buffer, 512 );

}

//############################################################################
//Routine zum lesen des CID Registers von der MMC/SD-Karte (16Bytes)
uint8_t mmc_read_cid ( uint8_t *Buffer )
//############################################################################
{
    //Commando zum lesen des CID Registers
    uint8_t cmd[] = {0x4A, 0x00, 0x00, 0x00, 0x00, 0xFF};

    return mmc_read_block ( cmd, Buffer, 16 );
}

//############################################################################
//Routine zum lesen des CSD Registers von der MMC/SD-Karte (16Bytes)
uint8_t mmc_read_csd ( uint8_t *Buffer )
//############################################################################
{
    //Commando zum lesen des CSD Registers
    uint8_t cmd[] = {0x49, 0x00, 0x00, 0x00, 0x00, 0xFF};

    return mmc_read_block ( cmd, Buffer, 16 );
}
