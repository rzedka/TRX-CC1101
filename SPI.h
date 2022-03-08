#ifndef SPI_H_INCLUDED
#define SPI_H_INCLUDED

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"

void SPI_setup(void);

uint8_t SPI_ByteTransfer(uint8_t TXData);

#endif // SPI_H_INCLUDED
