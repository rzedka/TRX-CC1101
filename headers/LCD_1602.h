#ifndef LCD_1602_H_INCLUDED
#define LCD_1602_H_INCLUDED


/// Author: Radim Zedka
/// Date:   01.10.2017  (DD.MM.YYYY) (updated 17.5.2021)
/// Description: Library works for alphanumeric LCD 16x2 interfacing MCU through I2C (TWI)


#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>

#include "main.h"
#include "i2cmaster.h"
#include "tim.h" // includes "timer1_cnt" global variable
/// Note: this library doesn't use "system_flag" global variable


void lcd_i2c_cmd(uint8_t RS, uint8_t BL, uint8_t Data, uint8_t Instr_length);
    /// This function provides Data & Instruction write into LCM1602 IIC V1
    /// (16x2 LCD in 4-bit mode interfaced through PCF8574 I2C port expander).
    /// The I2C data byte is = [D7 D6 D5 D4 BL En RW RS] (BL = LCD Backlight)
    /// The I2C slave address is 0x27 (but in WRITE mode it is 0x4E)
    /// The function provides sending two types of LCD Instructions (initial 4bit ones,
    /// and 8bit ones which are cut in half and sent out twice).
    /// Then it provides writing
    /// In 4-bit INSTRUCTION mode:
    ///     - only Higher 4-bits of "Data" input are valid.
    /// In 8-bit INSTRUCTION mode:
    ///     - first Higher 4-bits and right after Lower 4-bits of "Data" input are sent.

uint8_t lcd_i2c_init(uint8_t backlight, const uint8_t uart_debug);
     /// LCD RW1063 initialization through I2C (TWI) - For details see datasheet RW1063 page 28 and 32.

void lcd_i2c_print_char(char s[], uint8_t backlight);

uint8_t lcd_i2c_print_whole(char s[], uint8_t backlight);

uint8_t lcd_i2c_clrscr(uint8_t backlight);

void lcd_i2c_home(uint8_t *proc_flag, uint8_t proc_num);

void lcd_i2c_putc(unsigned char character, uint8_t *proc_flag, uint8_t proc_num);

 void lcd_i2c_puts(const char *s);

 void lcd_i2c_gotoxy(unsigned char x, unsigned char y);

 uint8_t lcd_i2c_trx_info(uint8_t TRX_info[5], uint8_t backlight);

#endif // LCD_1602_H_INCLUDED
