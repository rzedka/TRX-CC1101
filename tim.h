#ifndef TIM_H_INCLUDED
#define TIM_H_INCLUDED

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>

#include "main.h"


extern volatile uint16_t timer1_cnt;

#ifdef MASTER

//extern volatile uint8_t timer0_flag;
//extern volatile uint8_t dcc_tx_level;
#endif

#ifdef SLAVE

#endif // SLAVE

void TIMER1_setup(void);

uint16_t TIMER1_get_value(void);


#endif // TIM_H_INCLUDED
