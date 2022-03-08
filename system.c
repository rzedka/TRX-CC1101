#include "system.h"


void CMD_LCD_System(uint8_t *UI_flag, uint8_t *trx_flag, char CMD_word[33], uint8_t TRX_info[5])
{
    /// This FCN maintains commands from Keypad -> LCD (writing messages) and from TRX -> LCD (incoming messages)
//char buffer [5];
     if( (system_flag&LCD_INIT) == 0x0000){ /// LCD INIT ===========================================================
        if(lcd_i2c_init(1,0)){ /// Test if SPI_TRX_INIT() has finished INIT sequence yet
            system_flag |= LCD_INIT; /// Set the latch, so LCD INIT won't be called again
            system_flag |= LCD_TRXINF; /// Set the LCD-TRX-INFO bit (so the TRX info is displayed automatically - default)
        }

    }else{ /// LCD INIT DONE ==============================================================================

        /// User Interface command processing:
        if( (*UI_flag)&0x0F ){ /// Any user command for LCD...

            if( (*UI_flag)&0x08 ) { ///  "Clear the whole LCD"
                system_flag |= LCD_CLRALL; /// Start CLEAR ALL procedure
                //system_flag |= LCD_TRXINF; /// Set the LCD-TRX-INFO bit (LCD's purpose is TRX info)
                *UI_flag &= ~0x08;

            }else if((*UI_flag)&0x04){ /// "LCD Re-Print Whole user message"
                if( system_flag&LCD_TRXINF ){ /// Is there TRX info displayed now ?
                    system_flag |= LCD_CLRALL; /// Start CLEAR ALL procedure
                }
                system_flag |= LCD_PRTWHL; /// Start the LCD-print-whole procedure
                system_flag &= ~LCD_TRXINF; /// Clear the LCD-TRX-INFO bit
                *UI_flag &= ~0x04; /// Clear the CMD flag.

            }else if((*UI_flag)&0x02){ ///  Append one character:
                if(system_flag&LCD_TRXINF){ /// Is LCD's purpose displaying TRX messages ?
                    system_flag |= LCD_CLRALL; /// Start CLEAR ALL procedure
                }
                system_flag &= ~LCD_TRXINF; /// LCD's purpose is NOT displaying any TRX messages now!
                system_flag |= LCD_PRTONE; /// Set Print-One-Char bit
                system_flag |= LCD_TYPMSG; /// LCD's purpose is typing my own message

                *UI_flag &= ~0x02; /// Clear the CMD flag. CMD complete.

            }else{
                system_flag &= ~LCD_TYPMSG; /// User no longer needs the LCD for message typing.
                system_flag |= LCD_TRXINF; /// It will be used for TRX messages
                *UI_flag &= ~0x01; /// Clear the CMD flag.
            }
        }else{ /// No CMD from the keypad
        /// Just skip...
        }

        /// =========== Commands coming from the TRX system =================================
        if((*trx_flag)&0x02){ /// Incoming message
            //if(system_flag&LCD_TRXINF){ /// LCD's purpose is displaying TRX messages
                system_flag |= LCD_CLRALL; /// Set LCD_CLEAR_ALL bit
                system_flag |= LCD_PRTWHL; /// Set LCD_print_whole bit
                system_flag |= TRX_RXEVNT; /// Set RX event bit
                system_flag &= ~LCD_TRXINF; /// Clear the LCD-TRX-INFO bit
           // }else{/// otherwise just ignore

           // }

            // *trx_flag &= ~0x02;

        }else if((*trx_flag)&0x04){ /// TRX info packet received
            if(system_flag&LCD_TRXINF){ /// LCD's purpose is displaying TRX messages
                system_flag |= TRX_INFO; /// Set the TRX-info bit
            }else{/// otherwise just ignore

            }
            // *trx_flag &= ~0x04;
        }else{

        }

    }//end if
}


void CMD_TRX_System(uint8_t *UI_flag, char CMD_word[33], uint8_t TRX_info[5])
{
    /// This function processes commands from the User interface to the TRX module and asserts the appropriate bits
    /// in the status_flag register.

    if( (system_flag&TRX_INIT) == 0x0000){ /// TRX INIT =====================================================
        if(SPI_TRX_INIT()){ /// Test if SPI_TRX_INIT() has finished the INIT sequence yet,
            system_flag |= TRX_INIT; /// Set the latch, so INIT won't be called again
        }

    }else{ /// TRX INIT DONE =======================================================================
        /// User inteface Command processing:
        if( (*UI_flag)&0xF0 ){ /// Any command for the TRX module
            if((*UI_flag)&0x80){ /// Highest priority command===========================================
                /// Empty command...
               // *UI_flag &= ~0x80; /// Clear the CMD flag. CMD complete.

            }else if((*UI_flag)&0x40){  ///  Send Data  ===========================================
                system_flag |= TRX_TXEVNT; /// TX event started
                *UI_flag &= ~0x40; /// Clear the CMD flag. CMD complete.

            }else if((*UI_flag)&0x20){ /// ===========================================
                /// Empty command...
                //*UI_flag &= ~0x20; /// Clear the CMD flag. CMD complete.

            }else{  /// CMD of Lowest priority ===========================================
                /// Empty command...
                //*UI_flag &= ~0x10; /// Clear the CMD flag. CMD complete.
            }
        }else{ /// No CMD from the keypad:
            /// Just skip...
        }
    }
}
