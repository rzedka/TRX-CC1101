#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>


#include "main.h"


#ifdef UART_TERM
    extern volatile unsigned char uart_rx_array[50];
    extern volatile uint8_t uart_flag;
    extern volatile uint8_t uart_idx;
    extern volatile uint8_t rx_array[20];
    extern volatile uint8_t uart_char_idx;
#endif // UART_TERM


void USART_init(void);

void USART_RX_WAIT(uint8_t *RX_Data);

void USART_TX_WAIT(uint8_t TX_Data);

void USART_TX_STRING_WAIT(char s[]);

uint8_t USART_get_flag(void);

void UART_RX_FCN(uint8_t *UI_flag, char *CMD_head, char *CMD_word);

uint8_t CMD_Head_lib(char CMD_head[5]);

uint8_t UART_data_parser(uint8_t *byte1, uint8_t *byte2);

#endif // UART_H_INCLUDED
