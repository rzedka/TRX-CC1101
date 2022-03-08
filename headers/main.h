#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

/// Definitions:
#define MASTER_ST /// mobile master station
//#define SLAVE_ST_1 /// mobile slave station no.1
//#define SLAVE_ST_IMOB /// immobile slave station (repeater)
#define DEBUG
#define UART_TERM_DEBUG

/// TRX module configurations ==============
/// Carrier frequency:
#define FREQ_433
//#define FREQ_868

/// Baudrate settings:
//#define CFG_1200BAUD
//#define CFG_10000BAUD
#define CFG_38400BAUD
//#define CFG_76800BAUD


#ifdef MASTER_ST
    #define MATRIX_KEYPAD
    #define LCD_1602
    #define LCD_I2C_ADDR_WR 0b01001110 /// PCF8574 port expander
#endif // MASTER_ST

#ifdef SLAVE_ST_1
    #define MATRIX_KEYPAD
    #define LCD_1602
    #define LCD_I2C_ADDR_WR 0b01111110 /// PCF8574A
#endif // SLAVE_ST_1

#ifdef SLAVE_ST_IMOB
    #define UART_TERM
#endif // SLAVE_ST_IMOB


///  CPU settings - Clock frequency:
#define F_CPU_16 /// 16 MHz
//#define F_CPU_4 ///4 MHz

/// UART debugging feature:
//#define UART_TERM

#define T_IDLE 500 /// [ms]



/// ===============================
/// Pin Definitions for Arduino Nano:
/// LED indicator pins:
#define LED0_PIN PINB0
#define DDRLED0 DDRB
#define PORTLED PORTB
#define LED1_PIN PINB1
#define DDRLED1 DDRB

///  SPI pins:
#define DDRSPI DDRB
#define PORTSPI PORTB
#define SS_PIN PINB2
#define MOSI_PIN PINB3
#define MISO_PIN PINB4
#define SCLK_PIN PINB5

/// External Interrupt pins:
#define DDR_EXTINT DDRD
#define INT0_PIN PIND2
#define INT1_PIN PIND3

///  Horizontal BTN Inputs:
#define DDRBTN_H DDRD
#define BTN_H_PIN PIND
#define BTN_H0_PIN PIND7
#define BTN_H1_PIN PIND6
#define BTN_H2_PIN PIND5
#define BTN_H3_PIN PIND4

///  Vertical BTN Outputs:
#define DDRBTN_V DDRC
#define PORTBTN_V PORTC
#define BTN_V0_PIN PINC0
#define BTN_V1_PIN PINC1
#define BTN_V2_PIN PINC2



#endif // MAIN_H_INCLUDED
