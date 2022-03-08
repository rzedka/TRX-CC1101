/*
 */

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>

#include "main.h"
#include "uart.h"
#include "tim.h"
#include "gpio.h"
#include "Matrix_Keypad.h"

/// ============ Global Variables: =============================

#ifdef UART_TERM_DEBUG
    volatile uint8_t uart_flag; /// ISR variable
    volatile unsigned char uart_rx_array[50];       /// ISR variable
    volatile uint8_t uart_idx;          /// ISR variable
    volatile uint8_t uart_char_idx;          /// ISR variable
    volatile uint8_t rx_array[20];
#endif // UART_TERM_DEBUG
volatile uint16_t timer1_cnt; /// ISR variable
uint16_t system_flag;
volatile uint8_t int0_flag; /// ISR variable

int main(void) /// =========================================
{
    uint16_t ref_timer = 0;
    uint8_t int0_flag_f = 0x00; /// INT0 ISR flag follower
    uint8_t int0_flag_pulse = 0x00; /// INT0 ISR flag one-cycle pulse

    #ifdef UART_TERM_DEBUG
    uint8_t UI_flag = 0;
    char CMD_head[5] = {0,0,0,0,0};
    char CMD_word[33];
    memset(CMD_word,'\0',33); /// clear array
    #endif //UART_TERM_DEBUG

    uint8_t trx_flag = 0x00; /// TRX module flag register
    uint8_t TRX_info[5]; /// TRX connection info {Station_ID, HIS-RSSI, MY-RSSI, CRC+LQI, \0 }
    char CMD_word[33];
    /// Global variable initiation:
    timer1_cnt = 0x0000;
    system_flag = 0x0000;
    int0_flag = 0x00;

    GPIO_setup();
    TIMER1_setup();
    SPI_setup();
    #ifdef UART_TERM_DEBUG
    USART_init();
    uart_flag = 0;
    uart_idx = 0;
    #endif // UART_TERM_DEBUG
    /// External Interrupt INT0 Enable (CC1101):
    EICRA |= (1 << ISC01)|(1<< ISC00); /// sensitive to Rising edge of the INT0
    EIMSK |= (1<< INT0); /// Ext interrupt enabled
    memset(CMD_word,'\0',33); /// clear array
    memset(TRX_info,'\0',5);  /// clear array
    sei();
    /// ========================================================
    /// ========================================================
    while(1){
        #ifdef UART_TERM
            UART_RX_FCN(&UI_flag, CMD_head, CMD_word); /// UART Command recognition
        #endif // UART_TERM

        /// =============== INT0 flag processing =========================================
        if(int0_flag != int0_flag_f){
            int0_flag_f = int0_flag;
            int0_flag_pulse = 1;
            PORTD |= (1<< PIND3); /// HIGH
        }else{
            int0_flag_pulse = 0;
            PORTD &= ~(1<< PIND3); /// LOW
        }

        #ifdef UART_TERM_DEBUG
            UART_RX_FCN(&UI_flag, CMD_head, CMD_word);
        #endif // UART_TERM_DEBUG
        #ifdef MATRIX_KEYPAD
            Matrix_Keypad_Debounc_Edge(&key_pressed, &key_risedge );

            Matrix_Keypad_Char_Generator(&char_generated, key_risedge);

            Matrix_Keypad_CMD_Proc(&UI_flag, CMD_word, char_generated);
        #endif // MATRIX_KEYPAD

        #ifdef MASTER_ST
            /// Free-running TRX master function:
            if(system_flag&TRX_INIT) /// if TRX config is done
                TRX_MASTER_FCN(CMD_word, TRX_info, int0_flag_pulse, &trx_flag);

        #endif// MASTER_ST

        #ifdef SLAVE_ST_1
            /// Free-running TRX slave function:
            if(system_flag&TRX_INIT) /// if TRX config is done
                TRX_SLAVE_FCN(CMD_word, TRX_info, int0_flag_pulse, &trx_flag);
        #endif // SLAVE_ST_1

        /// ============ IDLE ===============================================
        if((TIMER1_get_value() - ref_time) >= T_SYNC){
            ref_time = TIMER1_get_value();
            PORTLED0 ^= (1<<LED1_PIN);
            LED_Signalization();
            #ifdef UART_TERM_DEBUG

            USART_TX_STRING_WAIT(CMD_word);
            //USART_TX_WAIT('\n');
            USART_TX_STRING_WAIT("system_flag = 0x");
            USART_TX_STRING_WAIT(itoa(system_flag,buffer,16));
            USART_TX_WAIT('\n');
           // USART_TX_STRING_WAIT("UI_flag = 0x");
           // USART_TX_STRING_WAIT(itoa(UI_flag,buffer,16));
           // USART_TX_WAIT('\n');
           // USART_TX_STRING_WAIT("trx_flag = 0x");
            //USART_TX_STRING_WAIT(itoa(trx_flag,buffer,16));
            //USART_TX_WAIT('\n');
            //USART_TX_WAIT((system_flag >> 8)&0x00FF);
            //USART_TX_WAIT(system_flag&0x00FF);
            #endif // UART_TERM_DEBUG
        }

    }

    return 0;
}


ISR(INT0_vect){
/// Comes with Rising edge of the INT0
/// The packet has been received with CRC == OK
    int0_flag++;
}

