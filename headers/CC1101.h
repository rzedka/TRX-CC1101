#ifndef CC1101_H_INCLUDED
#define CC1101_H_INCLUDED

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>

#include "main.h"
#include "tim.h" // includes "timer1_cnt" global variable
#include "My_SPI.h"
#include "CC1101_config.h" /// All CC1101 macros and def's

//extern uint16_t system_flag;

uint8_t CC1101_ByteCommand(uint8_t TXData, uint8_t mode);

uint8_t CC1101_STATUSBYTE_RD(void);

uint8_t CC1101_STATUSREG_RD(uint8_t reg_addr);

uint8_t CC1101_INIT(void);

void CC1101_MASTER_FCN(char TRX_message[33], uint8_t TRX_info[5], uint8_t int0_flag_pulse, uint8_t *trx_flag);

#endif // CC1101_H_INCLUDED
