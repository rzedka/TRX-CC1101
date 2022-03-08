#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>

#include "main.h"

#include "LCD_1602.h"

extern uint16_t system_flag;


void CMD_LCD_System(uint8_t *UI_flag, uint8_t *trx_flag, char CMD_word[33], uint8_t TRX_info[5]);

void CMD_TRX_System(uint8_t *UI_flag, char CMD_word[33], uint8_t TRX_info[5]);

#endif // SYSTEM_H_INCLUDED
