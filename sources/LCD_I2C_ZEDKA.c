//#include <avr/io.h>

#include "main.h"
#include "LCD_I2C_ZEDKA.h"
#include "i2cmaster.h"
#include "project_functions.h"
//#include <util/delay.h>
//#include <stdlib.h>


/// Author: Radim Zedka
/// Date:   1.10.2017  (DD.MM.YYYY)
/// Description: Library works for alphanumeric LCD 16x2 interfacing MCU through I2C (TWI)

void lcd_i2c_cmd(uint8_t RS, uint8_t BL, uint8_t Data, uint8_t Instr_length)
{
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
    uint8_t data_x = 0;
    uint8_t backlight_x = 0;

    if(BL > 0)
        backlight_x = 0x08;
    else
        backlight_x = 0;

    if(RS > 0){ /// Sending DATA ======================
        data_x = Data&0xF0;  /// Higher 4 bits
        i2c_start_wait(LCD_I2C_ADDR_WR);
        i2c_write(0b00000001|backlight_x); /// Set RS
        i2c_write(0b00000101|data_x|backlight_x); /// Rise Enable and Data
        i2c_write(0b00000001|data_x|backlight_x); /// Fall Enable
        i2c_write(0b00000001|backlight_x); /// Reset RS
        data_x = Data&0x0F; /// Lower 4 bits
        data_x = (data_x << 4)&0xF0;
        i2c_write(0b00000001|backlight_x); /// Set RS
        i2c_write(0b00000101|data_x|backlight_x); /// Rise Enable and Data
        i2c_write(0b00000001|data_x|backlight_x); /// Fall Enable
        i2c_write(0b00000001|backlight_x); /// Reset RS
        i2c_stop();
    }else{ /// Sending Instruction ====================

        data_x = Data&0xF0;  /// Higher 4 bits
        i2c_start_wait(LCD_I2C_ADDR_WR);
        i2c_write(0b00000000|backlight_x); /// Set RS
        i2c_write(0b00000100|data_x|backlight_x); /// Rise Enable and Data
        i2c_write(0b00000000|data_x|backlight_x); /// Fall Enable
        i2c_write(0b00000000|backlight_x); /// Reset RS

        if(Instr_length <= 1){  /// 4bit INSTRUCTION
            i2c_stop();

        }else{                  /// 8bit INSTRUCTION
            data_x = Data&0x0F; /// Lower 4 bits
            data_x = (data_x << 4)&0xF0;
            i2c_write(0b00000000|backlight_x); /// Set RS
            i2c_write(0b00000100|data_x|backlight_x); /// Rise Enable and Data
            i2c_write(0b00000000|data_x|backlight_x); /// Fall Enable
            i2c_write(0b00000000|backlight_x); /// Reset RS
            i2c_stop();
        }
    }

}

//void lcd_i2c_init(uint8_t *proc_flag, uint8_t proc_num, uint8_t backlight, uint8_t uart_debug)
uint8_t lcd_i2c_init(uint8_t backlight, const uint8_t uart_debug)
{
    /// === LCD RW1063 initialization through I2C (TWI) - For details see datasheet RW1063 page 28 and 32.
    static uint8_t mode_cnt =0;
    static uint8_t mode_stack =0;
    static uint16_t ref_time = 0;
    static uint16_t wait_period =0;
    #define WAIT_mode 12

    if(mode_cnt==0){
        i2c_init();     /// set TWI control registers... bitrate 100kHz
        mode_cnt = WAIT_mode; /// WAIT
        mode_stack = 1; /// Next task after WAIT
        wait_period = 50; /// Wait duration [ms]
        ref_time = timer_cnt;
        return 0;

    }else if(mode_cnt==1){ /// Make sure the SLAVE is READY:
        if(i2c_start(LCD_I2C_ADDR_WR)){ /// NOT ACK,
            i2c_stop(); ///STOP I2C Transmission
            if(uart_debug){
                /// UART message:
                USART_TX_STRING_WAIT("\n I2C N-ACK \n");
            }
            /// WAIT for xxx ms and try again
            mode_cnt = WAIT_mode; /// WAIT
            mode_stack = 1; /// Next task after WAIT
            wait_period = 50; /// Wait duration [ms]
            ref_time = timer_cnt;

        }else{ /// ACK successful
            i2c_stop(); ///STOP I2C Transmission
            if(uart_debug){
                /// UART message:
                USART_TX_STRING_WAIT("\n I2C ACK \n");
            }
            mode_cnt = 2; /// NEXT mode
        }
        return 0;

    }else if(mode_cnt==2){      /// --- FUNCTION SET 1
        lcd_i2c_cmd(0, backlight, 0x30, 1); /// 4bit instruction
        mode_cnt = WAIT_mode; /// WAIT
        mode_stack = 3; /// Next task after WAIT
        wait_period = 5; /// Wait duration [ms]
        ref_time = timer_cnt;
        return 0;

    }else if(mode_cnt==3){      /// --- FUNCTION SET 2
        lcd_i2c_cmd(0, backlight, 0x30, 1); /// 4bit instruction
        mode_cnt = WAIT_mode; /// WAIT
        mode_stack = 4; /// Next task after WAIT
        wait_period = 5; /// Wait duration [ms]
        ref_time = timer_cnt;
        return 0;

    }else if(mode_cnt==4){      /// --- FUNCTION SET 3
        lcd_i2c_cmd(0, backlight, 0x30, 1); /// 4bit instruction
        mode_cnt = WAIT_mode; /// WAIT
        mode_stack = 5; /// Next task after WAIT
        wait_period = 5; /// Wait duration [ms]
        ref_time = timer_cnt;
        return 0;

    }else if(mode_cnt==5){      /// --- FUNCTION SET 4
        lcd_i2c_cmd(0, backlight, 0x20, 1); /// 4bit instruction
        mode_cnt = WAIT_mode; /// WAIT
        mode_stack = 6; /// Next task after WAIT
        wait_period = 2; /// Wait duration [ms]
        ref_time = timer_cnt;
        return 0;

    }else if(mode_cnt==6){      ///--- FUNCTION SET 5
        lcd_i2c_cmd(0, backlight, 0x28, 2); /// 8bit instruction [0 0 1 0 N F X X]
        mode_cnt = WAIT_mode; /// WAIT
        mode_stack = 7; /// Next task after WAIT
        wait_period = 2; /// Wait duration [ms]
        ref_time = timer_cnt;
        return 0;

    }else if(mode_cnt==7){      ///--- Display OFF
        lcd_i2c_cmd(0, backlight, 0x08, 2); /// 8bit instruction [0 0 0 0 1 0 0 0]
        mode_cnt = WAIT_mode; /// WAIT
        mode_stack = 8; /// Next task after WAIT
        wait_period = 2; /// Wait duration [ms]
        ref_time = timer_cnt;
        return 0;

    }else if(mode_cnt==8){      ///--- Clear Display
        lcd_i2c_cmd(0, backlight, 0x01, 2); /// 8bit instruction [0 0 0 0 0 0 0 1]
        mode_cnt = WAIT_mode; /// WAIT
        mode_stack = 9; /// Next task after WAIT
        wait_period = 2; /// Wait duration [ms]
        ref_time = timer_cnt;
        return 0;

    }else if(mode_cnt==9){      ///--- Entry mode set
        lcd_i2c_cmd(0, backlight, 0x06, 2); /// 8bit instruction [0 0 0 0 0 1 I/D S] = [0 0 0 0 0 1 1 0]
        mode_cnt = WAIT_mode; /// WAIT
        mode_stack = 10; /// Next task after WAIT
        wait_period = 2; /// Wait duration [ms]
        ref_time = timer_cnt;
        return 0;

    }else if(mode_cnt==10){      ///--- Display ON/OFF
        lcd_i2c_cmd(0, backlight, 0x0C, 2); /// 8bit instruction [0 0 0 0 1 D C B] = [0 0 0 0 1 1 0 0]
        mode_cnt = WAIT_mode; /// WAIT
        mode_stack = 11; /// Next task after WAIT
        wait_period = 2; /// Wait duration [ms]
        ref_time = timer_cnt;
        return 0;

    }else if(mode_cnt==11){      /// Final mode
        mode_cnt = 0;
        return 1; /// FCN won't be called again

    }else if(mode_cnt==WAIT_mode){ /// WAIT
        /// keep skipping until the WAIT period elapses.
        if((timer_cnt - ref_time)>= wait_period){
            ref_time = timer_cnt;
            mode_cnt = mode_stack;
        }
        return 0;

    }else{ /// INVALID mode:
        mode_cnt = 0;
        return 0;
    }// endif

}

void lcd_i2c_print_char(char s[], uint8_t backlight)
{
    /// display only the last char in the array. Make sure it goes to the right Line..
    uint8_t char_idx = 0;
    char_idx = strlen(s) -1;

    if( char_idx == 16 ){
        /// Set cursor to Line 2:
        lcd_i2c_cmd(0, backlight, 0xC0, 2);/// 8bit instruction [1 A6 A5 A4 A3 A2 A1 A0],
    }
    lcd_i2c_cmd(1, backlight, s[char_idx], 2);/// print one character
}

uint8_t lcd_i2c_print_whole(char s[], uint8_t backlight)
{
    static uint8_t mode_cnt =0;
    static uint8_t mode_stack =0;
    static uint16_t ref_time =0;
    #define WAIT_mode_1 35

    /// This FCN Prints all the characters from s[] to the display. It doesn't clear the rest of the display!

    /// DDRAM Addresses:
    /// position 1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16   ...  40
    /// Line 1   00 01 02 03 04 05 06 07 08 09  0A  0B  0C  0D  0E  0F   ...  27
    /// Line 2   40 41 42 43 44 45 46 47 48 49  4A  4B  4C  4D  4E  4F        67

    if(mode_cnt == 0){     /// ============================================
        /// Set Cursor to the beginning of Line 1:
        lcd_i2c_cmd(0, backlight, 0x80, 2);/// 8bit instruction [1 A6 A5 A4 A3 A2 A1 A0],
        mode_stack = 1; /// Next task after WAIT
        mode_cnt = WAIT_mode_1; /// WAIT
        ref_time = timer_cnt;
        //*proc_flag |= proc_num; /// Set the LATCH
        return 0;

    }else if(mode_cnt <= 16){           /// Line 1 ==========================
        if(s[(mode_cnt-1)] != '\0'){ /// Line 1
            lcd_i2c_cmd(1, backlight, s[(mode_cnt-1)], 2);/// Data
            mode_stack = mode_cnt+1; /// Next task after WAIT
            mode_cnt = WAIT_mode_1; /// WAIT
            ref_time = timer_cnt;
            return 0;

        }else{ /// End of the string
            mode_cnt = 0;
            //*proc_flag &= ~proc_num; /// Release the LATCH
            return 1;
        }

    }else if(mode_cnt == 17){/// set cursor to Line 2 =======================
        lcd_i2c_cmd(0, backlight, 0xC0, 2);/// 8bit instruction [1 A6 A5 A4 A3 A2 A1 A0],
        mode_stack = mode_cnt+1; /// Next task after WAIT
        mode_cnt = WAIT_mode_1; /// WAIT
        ref_time = timer_cnt;
        return 0;

    }else if(mode_cnt <= 33){            /// Line 2 ==========================
        if(s[(mode_cnt-2)] != '\0'){
            lcd_i2c_cmd(1, backlight, s[(mode_cnt-2)], 2);/// Data
            mode_stack = mode_cnt+1; /// Next task after WAIT
            mode_cnt = WAIT_mode_1; /// WAIT
            ref_time = timer_cnt;
            return 0;

        }else{ /// End of the string
            mode_cnt = 0;
            //*proc_flag &= ~proc_num; /// Release the LATCH
            return 1;
        }
    }else if(mode_cnt ==34){            /// FINAL mode ======================
        mode_cnt = 0;
        //*proc_flag &= ~proc_num; /// Release the LATCH
        return 1;

    }else if(mode_cnt == WAIT_mode_1){  /// WAIT ============================
        if((timer_cnt - ref_time)>= 1){
            ref_time = timer_cnt;
            mode_cnt = mode_stack;
        }
        return 0;

    }else{ /// INVALID mode
        mode_cnt = 0;
        return 0;
    }//emdif


}

void lcd_i2c_gotoxy(unsigned char x, unsigned char y)
{
    /// x can be 0 - 15
    /// y can be 0 - 1
   if(y == 0){  /// Upper line
        i2c_start_wait(LCD_I2C_ADDR_WR);
        i2c_write(0b10000000);      ///  control byte, INSTRUCTION Ahead
        i2c_write(0b10000000 | x); ///  Data byte, SET DDRAM position
        i2c_stop();
   }else{      /// Lower line
        i2c_start_wait(LCD_I2C_ADDR_WR);
        i2c_write(0b10000000);     ///  control byte, INSTRUCTION Ahead
        i2c_write(0b11000000 | x); ///  Data byte, SET DDRAM position
        i2c_stop();
   }

}

uint8_t lcd_i2c_clrscr(uint8_t backlight)
{
    static uint8_t mode_cnt =0;
    static uint8_t mode_stack =0;
    static uint16_t ref_time =0;

    if(mode_cnt == 0){
        lcd_i2c_cmd(0, backlight, 0x01, 2); /// 8bit instruction [0 0 0 0 0 0 0 1]
        mode_cnt = 2; /// WAIT
        mode_stack = 1; /// Next mode
        ref_time = timer_cnt;
        return 0;

    }else if(mode_cnt == 1){ /// Final mode
        mode_cnt = 0;
        return 1;

    }else if(mode_cnt == 2){ /// WAIT
        /// keep skipping until the WAIT period elapses.
        if((timer_cnt - ref_time)>= 5){ /// 5 ms
            ref_time = timer_cnt;
            mode_cnt = mode_stack;
        }
        return 0;

    }else{ /// INVALID mode
        mode_cnt = 0;
        return 0;
    }//endif

}

void lcd_i2c_home(uint8_t *proc_flag, uint8_t proc_num)
{
    static uint8_t mode_cnt =0;
    static uint8_t mode_stack =0;
    static uint16_t ref_time =0;

    if(mode_cnt==0){
        lcd_i2c_cmd(0, 1, 0x02, 2); /// 8bit Data to DDRAM[0 0 0 0 0 0 0 1]
        /// backlight is ON.
        *proc_flag |= proc_num; /// Hold the Latch
        mode_cnt = 2; /// WAIT
        mode_stack = 1; /// NEXT mode
        ref_time = timer_cnt;

    }else if(mode_cnt==1){/// FINAL mode
        mode_cnt = 0;
        *proc_flag &= ~proc_num; /// Release the Latch
    }else if(mode_cnt==2){ /// WAIT
        /// keep skipping until the WAIT period elapses.
        if((timer_cnt - ref_time)>= 2){ ///
            ref_time = timer_cnt;
            mode_cnt = mode_stack;
        }
    }else{/// INVALID STATE
         mode_cnt = 0;
    }//endif

}

void lcd_i2c_putc(unsigned char character, uint8_t *proc_flag, uint8_t proc_num)
{
    static uint8_t mode_cnt =0;
    static uint8_t mode_stack =0;
    static uint16_t ref_time =0;
    if(mode_cnt==0){
        lcd_i2c_cmd(1, 1, character, 2); /// 8bit Data to DDRAM[0 0 0 0 0 0 0 1]
        /// backlight is ON.
        *proc_flag |= proc_num; /// Hold the Latch
        mode_cnt = 2; /// WAIT
        mode_stack = 1; /// NEXT mode
        ref_time = timer_cnt;

    }else if(mode_cnt==1){ /// FINAL mode
        mode_cnt = 0;
        *proc_flag &= ~proc_num; /// Release the Latch
    }else if(mode_cnt==2){ /// WAIT
        /// keep skipping until the WAIT period elapses.
        if((timer_cnt - ref_time)>= 2){ ///
            ref_time = timer_cnt;
            mode_cnt = mode_stack;
        }
    }else{/// INVALID STATE
        mode_cnt = 0;
    }//endif


}

uint8_t lcd_i2c_trx_info(uint8_t TRX_info[5], uint8_t backlight)
{
    /// TRX_info[5] == {His-Station ID,
    ///                 His-RSSI
    ///                 My-RSSI
    ///                 CRC OK + LQI
    ///                 '\0'            }

    char buffer[5];
    int16_t RSSI_dec = 0;
    static char disp_buf[33];
    //char aux_buf[4];
    static uint8_t mode_cnt = 0;

    if(mode_cnt == 0){
        //memset(aux_buf,'\0',4);
        /// Line 1
        disp_buf[0] = ' ';
        disp_buf[1] = ' ';
        disp_buf[2] = ' ';
        disp_buf[3] = ' ';
        disp_buf[4] = 'S';
        disp_buf[5] = 'T';
        disp_buf[6] = 'A';
        disp_buf[7] = 'T';
        disp_buf[8] = 'I';
        disp_buf[9] = 'O';
        disp_buf[10] = 'N';
        disp_buf[11] = '-';
        disp_buf[12] = 0x7f&(TRX_info[0]); // ignore the TYPING flag
        disp_buf[13] = ' ';
        if(0x80&(TRX_info[0]))
            disp_buf[14] = 'T';
        else
            disp_buf[14] = ' ';
        disp_buf[15] = ' ';

        /// ============= Calculating the RSSI values=============
        if(TRX_info[2]>=128) // RSSI 1
            RSSI_dec = (((TRX_info[2]-256)>>1))-RSSI_offset;
        else
            RSSI_dec = ((TRX_info[2])>>1)-RSSI_offset;
        /// RSSI range is (-138,-11) dBm...

        itoa(RSSI_dec, buffer, 10); /// Int -> char buffer
        if(RSSI_dec > -10){/// RSSI value -9 -> 0 (impossible)
            disp_buf[20] = ' ';
            disp_buf[21] = ' ';
            disp_buf[22] = buffer[0];
            disp_buf[23] = buffer[1];
        }else if (RSSI_dec > -100){ /// values -99 -> -10
            disp_buf[20] = ' ';
            disp_buf[21] = buffer[0];
            disp_buf[22] = buffer[1];
            disp_buf[23] = buffer[2];
        }else{ /// values below -100 (including -100 dBm)
            disp_buf[20] = buffer[0];
            disp_buf[21] = buffer[1];
            disp_buf[22] = buffer[2];
            disp_buf[23] = buffer[3];
        }
        /// ====================================================
        if(TRX_info[1]>=128) // RSSI 2
            RSSI_dec = (((TRX_info[1]-256)>>1))-RSSI_offset;
        else
            RSSI_dec = ((TRX_info[1])>>1)-RSSI_offset;
        /// RSSI range is (-138,-11) dBm...

        itoa(RSSI_dec, buffer, 10); /// Int -> char buffer
        if(RSSI_dec > -10){/// RSSI value -9 -> 0 (impossible)
            disp_buf[28] = ' ';
            disp_buf[29] = ' ';
            disp_buf[30] = buffer[0];
            disp_buf[31] = buffer[1];
        }else if (RSSI_dec > -100){ /// values -99 -> -10
            disp_buf[28] = ' ';
            disp_buf[29] = buffer[0];
            disp_buf[30] = buffer[1];
            disp_buf[31] = buffer[2];
        }else{ /// values below -100 (including -100 dBm)
            disp_buf[28] = buffer[0];
            disp_buf[29] = buffer[1];
            disp_buf[30] = buffer[2];
            disp_buf[31] = buffer[3];
        }
        /// ====================================================
        /// Line 2
        disp_buf[16] = 'R';
        disp_buf[17] = 'S';
        disp_buf[18] = 'S';
        disp_buf[19] = '1';
        //disp_buf[20] = '-';

        disp_buf[24] = 'R';
        disp_buf[25] = 'S';
        disp_buf[26] = 'S';
        disp_buf[27] = '2';
        //disp_buf[28] = '=';


        // I dont need to display LQI info...
        /*
        itoa((TRX_info[2]&0x7F), buffer, 10); /// without CRC OK = 7th bit
        if((TRX_info[2]&0x7F) < 10){/// RSSI value 0 - 9
            disp_buf[29] = '0';
            disp_buf[30] = '0';
            disp_buf[31] = buffer[0];
        }else if ((TRX_info[2]&0x7F) < 100){ /// 10 - 99
            disp_buf[29] = '0';
            disp_buf[30] = buffer[0];
            disp_buf[31] = buffer[1];
        }else{ /// 100 - 255
            disp_buf[29] = buffer[0];
            disp_buf[30] = buffer[1];
            disp_buf[31] = buffer[2];
        }
        */
        lcd_i2c_print_whole(disp_buf, backlight); /// Call the FCN for the first time..
        mode_cnt = 1;
        return 0; /// FCN not finished yet..

    } else if(mode_cnt == 1){
        if(lcd_i2c_print_whole(disp_buf, backlight)){
            mode_cnt = 0;
            return 1; /// FCN finished its DUTY
        }else{
            mode_cnt = 1;
            return 0; /// FCN not finished yet..
        }
    }else{
        mode_cnt = 0;
        return 0;
    }//end if

}

/*
/// Put string
void lcd_i2c_puts(const char *s, uint8_t *proc_flag, uint8_t proc_num)
{
    static uint8_t mode_cnt =0;
    static uint8_t mode_stack =0;
    static uint16_t ref_time =0;
    uint8_t c;

    if(mode_cnt==0){
        lcd_i2c_cmd(0, 1, 0x02, 2); /// 8bit Data to DDRAM[0 0 0 0 0 0 0 1]
        /// backlight is ON.
        *proc_flag |= proc_num; /// Hold the Latch
        mode_cnt = 2; /// WAIT
        mode_stack = 1; /// NEXT mode
        ref_time = timer_cnt;

    }else if(mode_cnt==1){/// FINAL mode
        mode_cnt = 0;
        *proc_flag &= ~proc_num; /// Release the Latch
    }else if(mode_cnt==2){ /// WAIT
        /// keep skipping until the WAIT period elapses.
        if((timer_cnt - ref_time)>= 2){ ///
            ref_time = timer_cnt;
            mode_cnt = mode_stack;
        }
    }else{/// INVALID STATE
         mode_cnt = 0;
    }//endif

  i2c_start_wait(LCD_I2C_ADDR_WR);
  while ((c = *s++))
  {
    i2c_write(0b11000000); ///  control byte, DATA Ahead
    i2c_write(c); ///  Data byte, = Character
  }
  i2c_stop();
}
*/
