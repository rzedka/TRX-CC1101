#include "Matrix_Keypad.h"

void Matrix_Keypad_Debounc_Edge(uint8_t *key_pressed, uint8_t *key_risedge )
{


    static uint8_t debounc_cnt = 0x00; // Needs to be static, otherwise it is erased every time the function is called...
    static uint16_t ref_time = 0x0000;
    static uint16_t wait_period = 0;
    static uint8_t mode_cnt = 0;
    static uint8_t mode_stack = 0;
    static uint8_t pressed_flag = 0;
    static uint8_t edge_flag = 0;
    uint8_t key_reg = 0;

    if(mode_cnt == 0){ /// Set VERT 0 ================================================
        /// Set Vertical pins:
        DDRBTN_V &= ~((1<<BTN_V1_PIN)|(1<<BTN_V2_PIN)); /// High Z
        PORTBTN_V &= ~((1<<BTN_V1_PIN)|(1<<BTN_V2_PIN));/// Pullup OFF
        DDRBTN_V |= (1<<BTN_V0_PIN);    /// Output
        PORTBTN_V |= (1<<BTN_V0_PIN);   /// HIGH
        /// Give the output pins some time to settle down...
        wait_period = 2;/// [ms]
        mode_stack = 1; /// Next mode after WAIT
        mode_cnt = WAIT_mode_key; /// WAIT
        ref_time = tim_tick_get(); /// reset the timer

    }else if(mode_cnt == 2){ /// Set VERT 1 ================================================
        DDRBTN_V &= ~((1<<BTN_V0_PIN)|(1<<BTN_V2_PIN)); /// High Z
        PORTBTN_V &= ~((1<<BTN_V0_PIN)|(1<<BTN_V2_PIN));/// Pullup OFF
        DDRBTN_V |= (1<<BTN_V1_PIN);    /// Output
        PORTBTN_V |= (1<<BTN_V1_PIN);   /// HIGH
        /// Give the output pins some time to settle down...
        wait_period = 2;/// [ms]
        mode_stack = 3;
        mode_cnt = WAIT_mode_key;
        ref_time = tim_tick_get();

    }else if(mode_cnt == 4){ /// Set VERT 2 ================================================
        DDRBTN_V &= ~((1<<BTN_V0_PIN)|(1<<BTN_V1_PIN)); /// High Z
        PORTBTN_V &= ~((1<<BTN_V0_PIN)|(1<<BTN_V1_PIN));/// Pullup OFF
        DDRBTN_V |= (1<<BTN_V2_PIN);    /// Output
        PORTBTN_V |= (1<<BTN_V2_PIN);   /// HIGH
        /// Give the output pins some time to settle down...
        wait_period = 2; /// [ms]
        mode_stack = 5;
        mode_cnt = WAIT_mode_key;
        ref_time = tim_tick_get();

    }else if((mode_cnt == 1)||(mode_cnt == 3)||(mode_cnt == 5)){ /// Read HORIZONTAL PINs==================
        ref_time = tim_tick_get(); /// reset the timer
        /// Read Horizontal inputs:
        key_reg = BTN_H_PIN&0xF0;
        if(key_reg != 0){ /// Any key pressed
            pressed_flag = mode_cnt; /// to specify the column of the key pressed.
            if(debounc_cnt >= keypad_deb_no){ /// The key is officially pressed
                *key_pressed = key_reg|mode_cnt;
                if(!edge_flag){/// EDGE detector
                    edge_flag = 1; /// set the latch
                    *key_risedge = key_reg|mode_cnt; /// edge present
                }else{
                    *key_risedge = 0; /// End of the edge
                }
            }else{/// still not certain if debounced..
                debounc_cnt++;
                *key_pressed = 0;
            }
            wait_period = 2; /// check BTNs MORE often

        }else{ /// No Key pressed (for this keyboard column)
            if(pressed_flag != 0){ /// A key was pressed in previous mode (V0, V1 or V2)
                if(mode_cnt == pressed_flag){ /// A glitch occurred for the key which was pressed in previous cycle/mode
                    /// RESET the debouncer counter:
                    pressed_flag = 0; /// Clear the flag
                    debounc_cnt = 0;  ///
                    *key_pressed = 0;

                }else{/// Different column is being checked now
                    /// Do not reset the debouncer counter, just skip.
                }
                wait_period = 2; /// check BTNs MORE often

            }else{ /// No key was pressed in previous mode
                wait_period = 4; /// check BTNs LESS often
                edge_flag = 0;
            }
            *key_risedge = 0;
        }//end if

        mode_stack = mode_cnt + 1; /// Next mode (after WAIT mode)
        mode_cnt = WAIT_mode_key; /// WAIT

    }else if(mode_cnt == WAIT_mode_key){ /// WAIT ================================================
        *key_risedge = 0;
        if((tim_tick_get() - ref_time)>= wait_period){
            ref_time = tim_tick_get();
            mode_cnt = mode_stack;
        }

    }else{ /// INVALID mode
        mode_cnt = 0;
    }// endif
}


void Matrix_Keypad_Char_Generator(char *char_gen, uint8_t key_risedge)
{

    static uint8_t  mode_cnt = 0;
    //static uint8_t  char_cnt = 0; /// How many times was it pressed
    static uint8_t  key_idx = 0;
    static uint8_t  key_history[5]; /// Up to 4 key presses are saved here
    static uint8_t  key_issued = 0; /// Up to 4 key presses are saved here
    static uint8_t  order_issued = 0; /// asserted when pressing second key immediately after the first one
    static uint16_t ref_time = 0;


    if(mode_cnt == 0){ /// IDLE mode =================================================
        ///Waiting for the rising edge
        if(key_risedge){
            key_history[0] = key_risedge;
            key_idx = 1;
            ref_time = tim_tick_get(); /// Reset & Start the timer
            mode_cnt = 1;
        }
        *char_gen = 0x00; /// Clear the output

    }else if(mode_cnt == 1){ /// TIMEOUT mode ========================================
        *char_gen = 0x00; /// Clear the output
        if(key_risedge){ /// ANY Key is pressed
            ref_time = tim_tick_get(); /// RESET the timer
            if(key_risedge == key_history[(key_idx-1)]){ ///The Key is the SAME
                key_history[key_idx] = key_risedge;
                if(key_idx >= 4){ /// Key pressed 5-times
                    mode_cnt = CHAR_GEN_mode;
                    key_issued = key_history[(key_idx-1)];
                    order_issued = ++key_idx;
                    key_idx = 0;
                }else{/// Key pressed less than 4-times
                    key_idx++;
                    mode_cnt = 1;
                }

            }else{  /// This key is NOT the SAME as the previous one...
                ///1) Issue the previous key + number of presses:
                key_issued = key_history[(key_idx-1)];
                order_issued = key_idx;
                mode_cnt = CHAR_GEN_mode;
                ///2) Manage the next key pressed:
                key_history[0] = key_risedge;
                key_idx = 1;
            }

        } else if((tim_tick_get() - ref_time)>= CHAR_TIMEOUT){ /// No EDGE...
            /// If there are no more Rising Edges, generate the last pressed char automatically:
            ref_time = tim_tick_get();
            mode_cnt = CHAR_GEN_mode;
            key_issued = key_history[(key_idx-1)];
            order_issued = key_idx;
            key_idx = 0;
        }//end if

    }else if(mode_cnt == CHAR_GEN_mode){ /// CharGen Mode =============================

        switch(key_issued){
        case 0b00010001:/// V0 H3   KEY "*"
                if(order_issued == 1)
                    *char_gen = 0x06; /// "ENTER"
                else
                    *char_gen = 0x07; /// "BACKLIGHT TOGGLE"


            break;
        case 0b00100001:/// V0 H2   KEY "7"
                if(order_issued == 1)
                    *char_gen = 'P';
                else if(order_issued == 2)
                    *char_gen = 'R';
                else if(order_issued == 3)
                    *char_gen = 'S';
                //else if(order_issued == 4)
                  //  *char_gen = 'S';
                else
                    *char_gen = '7';
            break;
        case 0b01000001:/// V0 H1   KEY "4"
                if(order_issued == 1)
                    *char_gen = 'G';
                else if(order_issued == 2)
                    *char_gen = 'H';
                else if(order_issued == 3)
                    *char_gen = 'I';
                else
                    *char_gen = '4';
            break;

        case 0b10000001:/// V0 H0   KEY "1"
                if(order_issued == 1)
                    *char_gen = '.';    /// "PERIOD"
                else if(order_issued == 2)
                    *char_gen = ',';    /// "COMMA"
                else if(order_issued == 3)
                    *char_gen = '?';    /// "Question mark"
                else if(order_issued == 4)
                    *char_gen = '!';    /// "Exclamation mark"
                else
                    *char_gen = '1';
            break;

        case 0b00010011:/// V1 H3   KEY "0"
                if(order_issued == 1)
                    *char_gen = ' ';    /// "SPACE" 0x20
                else
                    *char_gen = '0';
            break;
        case 0b00100011:/// V1 H2   KEY "8"
                if(order_issued == 1)
                    *char_gen = 'T';
                else if(order_issued == 2)
                    *char_gen = 'U';
                else if(order_issued == 3)
                    *char_gen = 'V';
                else
                    *char_gen = '8';
            break;
        case 0b01000011:/// V1 H1   KEY "5"
                if(order_issued == 1)
                    *char_gen = 'J';
                else if(order_issued == 2)
                    *char_gen = 'K';
                else if(order_issued == 3)
                    *char_gen = 'L';
                else
                    *char_gen = '5';
            break;
        case 0b10000011:/// V1 H0   KEY "2"
                if(order_issued == 1)
                    *char_gen = 'A';
                else if(order_issued == 2)
                    *char_gen = 'B';
                else if(order_issued == 3)
                    *char_gen = 'C';
                else
                    *char_gen = '2';
            break;

        case 0b00010101:/// V2 H3   KEY "#"
                if(order_issued == 1)
                    *char_gen = 0x08;/// "BACKSPACE"
                else if(order_issued == 3)
                    *char_gen = 0x09;    /// "CLEAR ALL"
                else
                    *char_gen = 0x00;

            break;
        case 0b00100101:/// V2 H2   KEY "9"
                if(order_issued == 1)
                    *char_gen = 'W';
                else if(order_issued == 2)
                    *char_gen = 'X';
                else if(order_issued == 3)
                    *char_gen = 'Y';
                else if(order_issued == 4)
                    *char_gen = 'Z';
                else
                    *char_gen = '9';
            break;
        case 0b01000101:/// V2 H1   KEY "6"
                if(order_issued == 1)
                    *char_gen = 'M';
                else if(order_issued == 2)
                    *char_gen = 'N';
                else if(order_issued == 3)
                    *char_gen = 'O';
                else
                    *char_gen = '6';
            break;
        case 0b10000101:/// V2 H0   KEY "3"
                if(order_issued == 1)
                    *char_gen = 'D';
                else if(order_issued == 2)
                    *char_gen = 'E';
                else if(order_issued == 3)
                    *char_gen = 'F';
                else
                    *char_gen = '3';
            break;

        default:    /// KEY combination not defined
                    *char_gen = 0x00;
        }//end switch

        if(key_idx ==0){ /// No more keys were pressed
            memset(key_history,'\0',5); /// Clear the array
            order_issued = 0;
            mode_cnt = 0; /// Back to IDLE

        }else{           /// Some other key has been pressed
            mode_cnt = 1;
        }

    }else{ /// INVALID mode
        mode_cnt = 0;
    }//endif
}


void Matrix_Keypad_CMD_Proc(uint8_t *UI_flag, char CMD_word[33], char char_gen)
{
    /// the OUTPUT is "CMD_word" array, carrying the body of the message. "UI_flag" then gives instructions to TRX module and LCD display.
    /// Any bit set in *UI_flag will be cleared in the following appropriate function.
    static uint8_t char_idx = 0;

    if(char_gen){ /// ANY Char incoming
        if(char_gen == 0x06){ /// CMD "ENTER / Send"
            *UI_flag |= 0x40|0x01; /// Transmit the message and give away the LCD token.
            char_idx = 0x00; /// reset the index
        }else if(char_gen == 0x07){ /// CMD "TOGGLE LCD light"
            if(system_flag&LCD_BCKLGT)
                system_flag &= ~LCD_BCKLGT; // turn OFF
            else
                system_flag |= LCD_BCKLGT; // turn ON
        }else if(char_gen == 0x08){ /// CMD "BACKSPACE"
            if(char_idx>0){
                CMD_word[char_idx] = '\0';
                char_idx--;
                CMD_word[char_idx] = '\0';
                /// Display any changes on the LCD:
                *UI_flag |= 0x08|0x04; /// Clear DISPLAY and Re-print the whole message
            }

        }else if(char_gen == 0x09){ /// CMD "CLEAR ALL"
            memset(CMD_word,'\0',33);
            /// Display any changes on the LCD:
            *UI_flag |= 0x08|0x01; /// Clear the display and give away the LCD token
            char_idx = 0x00; /// reset the index

        }else{ /// Other printable characters
            if(char_idx<32){ /// 0 - 31
                CMD_word[char_idx++] = char_gen;
            }
            /// Display any changes on the LCD:
            *UI_flag |= 0x02; /// Print one character
        }

    }else{ /// NO CHARS
        /// nothing
        //*UI_flag = 0;
    }
}
