#ifndef LCD_I2C_ZEDKA_H_INCLUDED
#define LCD_I2C_ZEDKA_H_INCLUDED




void lcd_i2c_cmd(uint8_t RS, uint8_t BL, uint8_t Data, uint8_t Instr_length);

uint8_t lcd_i2c_init(uint8_t backlight, const uint8_t uart_debug);

void lcd_i2c_print_char(char s[], uint8_t backlight);

uint8_t lcd_i2c_print_whole(char s[], uint8_t backlight);

uint8_t lcd_i2c_clrscr(uint8_t backlight);

void lcd_i2c_home(uint8_t *proc_flag, uint8_t proc_num);

void lcd_i2c_putc(unsigned char character, uint8_t *proc_flag, uint8_t proc_num);

 void lcd_i2c_puts(const char *s);

 void lcd_i2c_gotoxy(unsigned char x, unsigned char y);

 uint8_t lcd_i2c_trx_info(uint8_t TRX_info[5], uint8_t backlight);

 //void lcd_i2c_clock_refresh(RTC_data RTC_STRUCT, uint8_t Mode);

 //void lcd_i2c_RINGING(uint8_t Mode);

 //void lcd_i2c_Set_Time(RTC_data RTC_STRUCT, uint8_t Mode);

#endif /* LCD_I2C_ZEDKA_H_INCLUDED */
