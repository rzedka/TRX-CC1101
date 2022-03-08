#include "SPI.h"


void SPI_setup(void)
{
    //SPCR0 |= ();
    /// F_CPU = 16 MHz
    /// Set SCK speed, type of Endian, Edges, etc..
SPCR = (0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(1<<SPR1)|(1<<SPR0); ///SPI control REG.
/// SPIE - INT disabled, I dont need ISR...
/// SPE - SPI function enabled
/// DORD - Data order MSB first = Big Endian
/// MSTR - MASTER mode ON
/// CPOL - SCK LOW when IDLE.. RISING Edge is the LEADING edge (Sampling edge of EEPROM chip)
/// CPHA - MISO data sampled at  RISING edge of SCK.(EEPROM changes MISO at FALLING)
/// SPR1 & SPR0 = 00  F_clk/4. = 4 MHz => 250 ns
/// SPR1 & SPR0 = 11  F_clk/128. = 125 kHz => 8 us

//SPSR &= ~(1<<SPI2X); /// DOuble SPI speed is OFF
}

uint8_t SPI_ByteTransfer(uint8_t TXData)
{
    SPDR = TXData; /// First Shift one byte OUT of MOSI
    while(!(SPSR & (1<<SPIF)));
    return SPDR; /// Simultanously you get one Byte IN to MISO
}
