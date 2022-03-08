#include "CC1101.h"

uint8_t CC1101_ByteCommand(uint8_t TXData, uint8_t mode)
{
    /// mode == 0 : CSB Low and HIGH (SPI command terminated)
    /// mode != 0 : CSB LOW (SPI command not terminated)
    uint8_t rx_data = 0;

    PORTSPI &= ~(1<<SS_PIN); // Activate Slave-select
    rx_data = SPI_ByteTransfer(TXData);
    if(mode==0){ /// Normal SPI burst
        PORTSPI |= (1<<SS_PIN);
    }/// Otherwise No rising edge
    return rx_data;
}

uint8_t CC1101_STATUSBYTE_RD(void)
{
    /// This function provides corruption-secured read of CC1101 Status Byte (SNOP command strobe)
    uint8_t rx_array_c[3];

    memset(rx_array_c,'\0',3); /// Clear the RX array
    rx_array_c[0] = CC1101_ByteCommand(0x3D,0)&0xF0; /// first reading
    rx_array_c[1] = CC1101_ByteCommand(0x3D,0)&0xF0; /// second reading
    if(rx_array_c[0] != rx_array_c[1]) /// if corrupted, Read again
        rx_array_c[1] = CC1101_ByteCommand(0x3D,0)&0xF0; /// third reading

    return rx_array_c[1];
}

uint8_t CC1101_STATUSREG_RD(uint8_t reg_addr)
{
    /// This function provides corruption-secured read of CC1101 Status Register
    /// Burst bit = HIGH for distinguishing Status REGs from Command strobes...
    uint8_t rx_array_c[3];

    memset(rx_array_c,'\0',3); /// Clear the RX array
    CC1101_ByteCommand(reg_addr|0xC0,0x01);/// Read  (HEADER)
    rx_array_c[0] = CC1101_ByteCommand(0x00,0x00); /// CSB - HIGH
    CC1101_ByteCommand(reg_addr|0xC0,0x01);/// Read (HEADER) -AGAIN
    rx_array_c[1] = CC1101_ByteCommand(0x00,0x00); /// CSB - HIGH
    if(rx_array_c[0] != rx_array_c[1]){ /// if corrupted, Read again
        CC1101_ByteCommand(reg_addr|0xC0,0x01);/// Read (HEADER) -AGAIN
        rx_array_c[1] = CC1101_ByteCommand(0x00,0x00); /// CSB - HIGH
    }
    return rx_array_c[1];
}


uint8_t CC1101_INIT(void)
{
    ///
    /// 0 -TRX device reset
    /// 1 -Put Device into IDLE READ Device ID
    /// 2 -UPLOAD CONFIGURATION
    /// 3 -VERIFY CONFIGURATION
    /// 4 -FAILURE
    /// 5 -WAIT
    /// other - INVALID mode
    /// "uart_term" == 1  means UART is available

    static uint8_t mode_cnt = 0;
    static uint8_t mode_stack = 0;
    static uint16_t ref_time = 0;
    static uint16_t wait_period = 0; /// value in [ms]
    uint8_t Device_ID = 0;
    char buffer [5];
    uint8_t rx_array_a[48];
    uint8_t err_cnt = 0;
    uint8_t i=0;

    /// Read Chip ID and decide whether the device is functional
    /// Upload the CONFIG REGs and read them back

    if(mode_cnt == 0){ /// TRX DEVICE RESET ==============================================
        /// Device RESET:
        CC1101_ByteCommand(0x30,0x00); /// SRES command strobe
        mode_cnt = 5; /// WAIT mode
        mode_stack = 1; /// Next mode
        wait_period = 2; /// Duration [ms]
        ref_time = TIMER1_get_value();
        return 0;

    }else if(mode_cnt == 1){ /// Check TRX DEVICE ID ====================================
        Device_ID = SPI_TRX_STATUSREG_RD(0x31);
        #ifdef UART_TERM_DEBUG
        USART_TX_STRING_WAIT("Device_ID = 0x");
        USART_TX_STRING_WAIT(itoa(Device_ID,buffer,16));
        #endif // UART_TERM_DEBUG
        USART_TX_WAIT('\n');
        if(Device_ID == CC1101_ID){  /// the ID is correct
            #ifdef UART_TERM_DEBUG
                USART_TX_STRING_WAIT("\n DEVICE ID = 0x14 \n");
            #endif // UART_TERM_DEBUG
            CC1101_ByteCommand(0x36,0x00); /// SIDLE
            /// Go to WAIT and then to mode 1
            mode_cnt = 5; /// WAIT mode
            mode_stack = 2;/// Next mode after WAIT mode
            wait_period = 1; /// Duration [ms]
            ref_time = TIMER1_get_value();

        }else{                  /// Wrong ID
            #ifdef UART_TERM_DEBUG
                USART_TX_STRING_WAIT("\n DEVICE NOT RECOGNIZED \n");
            #endif // UART_TERM_DEBUG
            mode_cnt = 5; /// WAIT
            mode_stack = 0; /// Come back to mode 0 and try again...
            wait_period = 1000; /// [ms]
            ref_time = TIMER1_get_value();
        }
        return 0;


    }else if(mode_cnt == 2){ /// UPLOAD CONFIGURATION ====================================
        /// 2) Burst write 0x00 - 0x2E:  (0 - 46)
        CC1101_ByteCommand(0x40,0x01); /// HEADER - write operation, Burst bit =1, Addr = 0, CSB pin hold LOW
        for(i=0;i<CONFIG1_MAX_ADDR-1;i++){
           CC1101_ByteCommand(TRX_config_1[i],0x01);
        }
        CC1101_ByteCommand(TRX_config_1[CONFIG1_MAX_ADDR-1],0x00); /// CSB - HIGH
        /// Go to WAIT and then to READ CONFIG mode:
        mode_stack = 3; /// After WAIT go to VERIFY CONFIGURATION mode
        mode_cnt = 5;   /// WAIT
        wait_period = 10; /// [ms]
        ref_time = TIMER1_get_value();
        return 0;

    }else if(mode_cnt == 3){ /// VERIFY CONFIGURATION ====================================
        memset(rx_array_a,'\0',48); /// Clear the RX array
        CC1101_ByteCommand(0xC0,0x01); /// HEADER - read operation, Burst bit =1, Addr = 0, CSB keep LOW
        for(i=0;i<CONFIG1_MAX_ADDR-1;i++){
           rx_array_a[i] = CC1101_ByteCommand(0x00,0x01);
        }
        rx_array_a[CONFIG1_MAX_ADDR-1] = CC1101_ByteCommand(0x00,0x00); /// CSB HIGH
        /// Verify all the config registers:
        err_cnt = 0;
        for(i=0;i<CONFIG1_MAX_ADDR;i++){
           if(rx_array_a[i] != TRX_config_1[i]){
                err_cnt++;
           }
        }//end for
        if(err_cnt != 0){ /// Some CONFIG REGs are not written correctly
            #ifdef UART_TERM_DEBUG
                USART_TX_STRING_WAIT("\n CONFIG FAILED \n");
            #endif // UART_TERM_DEBUG
            /// Go to DEVICE RESET mode and try again
            mode_cnt = 0;

            return 0;

        }else{ /// The CONFIG was Correct
            #ifdef UART_TERM_DEBUG
                USART_TX_STRING_WAIT("\n CONFIG DONE \n");
                #ifdef CFG_1200BAUD
                USART_TX_STRING_WAIT(" 1200 Bd \n");
                #endif // CFG_1200BAUD
                #ifdef CFG_38400BAUD
                USART_TX_STRING_WAIT(" 38400 Bd \n");
                #endif // CFG_38400BAUD
            #endif // UART_TERM_DEBUG
            mode_cnt = 0;
            return 1; /// This FCN won't be called again

        }// end if

    }else if(mode_cnt == 5){ /// WAIT ====================================================
        if((TIMER1_get_value() - ref_time)>= wait_period){
            ref_time = TIMER1_get_value();
            mode_cnt = mode_stack; /// Back to the previous mode
        }
        return 0;

    }else{ /// INVALID mode
        mode_cnt = 0; /// Go from the beginning
        return 0;
    }//endif
}

void CC1101_MASTER_FCN(char TRX_message[33], uint8_t TRX_info[5], uint8_t int0_flag_pulse, uint8_t *trx_flag)
{
    /// 24.8.2018 Master FCN, expecting ONE SLAVE only!

    static uint8_t mode_cnt = 0;
    static uint8_t mode_stack = 0;
    static uint16_t wait_period = 0x0000;
    static uint16_t ref_time = 0x0000;
    static uint16_t ref_time_sync = 0x0000; /// used for WAIT-LONG mode
    uint8_t byte_num = 0;
    uint8_t status_byte = 0;
    uint8_t i = 0;
    uint8_t rx_array_a[65];
    char buffer[5];

    if(mode_cnt == 0){ /// ========================= INIT mode Station-A ===============================

        #ifdef UART_TERM_DEBUG/// Announce start of TEST_1
            USART_TX_STRING_WAIT("\n ... Starting Station-A ... \n");
        #endif // UART_TERM_DEBUG
        /// Flush both FIFOs:
        CC1101_ByteCommand(0x3A,0); /// SFRX
        CC1101_ByteCommand(0x3B,0); /// SFTX

        /// Go to IDLE state
        CC1101_ByteCommand(0x36,0x00); /// SIDLE cmd strobe
        memset(rx_array_a,'\0',65); /// Clear the RX array

        ref_time_sync = TIMER1_get_value(); /// Start the T_sync timer.
        mode_cnt = 1;

    }else if(mode_cnt==1){ /// Mode 1 TX - Station-A  ==========================================================
    /// ================================================================================================

        *trx_flag &= ~0x02; /// RESET the RX bit
        *trx_flag &= ~0x04; /// RESET the INFO bit
        /// Check the buffer for any data:
        byte_num = strlen(TRX_message);
        ///Flush FIFOs:
        CC1101_ByteCommand(0x3A,0); /// SFRX
        CC1101_ByteCommand(0x3B,0); /// SFTX
        #ifdef UART_TERM_DEBUG
            //USART_TX_STRING_WAIT("IDLE. message size: ");
           // USART_TX_STRING_WAIT(itoa(byte_num, buffer,10));
           // USART_TX_WAIT('\n');
        #endif // UART_TERM_DEBUG
        /// Fill the TX FIFO
        if(byte_num>0){ ///  data is available in the buffer.
            if( (system_flag&(TRX_TXEVNT|TRX_PKTSNT) ) == TRX_TXEVNT){ /// TX bit == 1, Packet Sent bit == 0
                /// ========== SEND DATA PACKET =========================================
                #ifdef UART_TERM_DEBUG
                    USART_TX_STRING_WAIT("Data TXFIFO\n");
                #endif // UART_TERM_DEBUG
                /// Fill the TX FIFO:
                CC1101_ByteCommand(0x7F,0x01); /// Burst TX FIFO WRITE HEADER
                CC1101_ByteCommand((byte_num+2),0x01); /// Length-byte (Station ID + data)
                CC1101_ByteCommand(MASTER_ID,0x01); /// Lower Byte second, CSB - stays LOW
                CC1101_ByteCommand(TRX_info[2],0x01); /// Send him my RSSI   CSB - LOW
                for(i=0; i<(byte_num-1); i=i+1){
                    CC1101_ByteCommand(TRX_message[i],0x01);
                }//end for
                CC1101_ByteCommand(TRX_message[(byte_num-1)],0x00);/// CSB-LOW

                /// I must NOT Erase the buffer now. In case of TIMEOUT, I'd need to re-send the message...
                system_flag |= TRX_PKTSNT; /// Set the "Packet-Sent" bit

            }else{  /// Data available, but TX flag is CLEARED.. Send Empty packet:
                CC1101_ByteCommand(0x7F,0x01); /// Burst TX FIFO WRITE HEADER CSB - LOW
                CC1101_ByteCommand(2,0x01); /// Length-byte goes first   CSB - LOW
                CC1101_ByteCommand(MASTER_ID|((system_flag&(LCD_TYPMSG))>>5),0x01); /// Lower Byte second, CSB - LOW
                CC1101_ByteCommand(TRX_info[2],0x00); /// Send him my RSSI   CSB - HIGH
            }

        }else{ /// Data NOT available.. Send Empty Packet:
            CC1101_ByteCommand(0x7F,0x01); /// Burst TX FIFO WRITE HEADER, CSB - LOW
            CC1101_ByteCommand(2,0x01); /// Length-byte goes first, CSB - LOW
            CC1101_ByteCommand(MASTER_ID|((system_flag&(LCD_TYPMSG))>>5),0x01); /// Lower Byte second, CSB - LOW
            CC1101_ByteCommand(TRX_info[2],0x00); /// Send him my station's RSSI.  CSB - HIGH
        }

        /// Start transmitting (Put Device into TX state):
        CC1101_ByteCommand(0x35,0x00); /// STX CMD strobe (After TX is done, CC1101 will go to IDLE)
        /// The TRX module goes from TX automatically into IDLE (to save power).
        wait_period = T_SLAVE_1_REACT - T_MASTER_RX_RDY;
        mode_stack = 2; /// After WAIT go to RX-Setup mode
        mode_cnt = 5; /// go to WAIT mode
        ref_time = TIMER1_get_value(); /// Reset the WAIT Timer

    }else if(mode_cnt==2){ /// RX Setup mode Station-A  ================================================================
    /// ================================================================================================================
        /// After elapsing T_react - T_rx_rdy, Put TRX into RX state and wait for the packet.
        CC1101_ByteCommand(0x34,0x00); /// SRX CMD strobe
        mode_cnt = 3;
        ref_time = TIMER1_get_value();/// Reset the TIMEOUT TIMER

    }else if(mode_cnt==3){ /// RX-Ready mode  Station-A  ===============================================================
    /// ================================================================================================================
        if(int0_flag_pulse){ /// Is Packet received ?
            /// Packet Received:
            status_byte = SPI_TRX_STATUSBYTE_RD()&0x70;
            if(status_byte == 0x00){ ///  ============ IDLE state =================
                #ifdef UART_TERM_DEBUG/// Announce the ST-A state through UART
                    USART_TX_STRING_WAIT("MASTER PKT RX\n");
                #endif // UART_TERM_DEBUG
                system_flag |= TRX_PKTREC; /// Set the PACKET-RECEIVED-bit in system flag register
                //wait_period = T_SLAVE_1_REACT;
                //ref_time = TIMER1_get_value(); /// Reset the TIMER for T_react period
                //ref_time_long = timer_cnt; /// Reset the timer for T_sync period
                mode_cnt = 4; /// Go to RX-DONE mode

            }else if(status_byte == 0x60 ){/// RX FIFO OVF
                CC1101_ByteCommand(0x3A,0); /// SFRX
                CC1101_ByteCommand(0x3B,0); /// SFTX
                CC1101_ByteCommand(0x36,0); /// SIDLE
                #ifdef UART_TERM_DEBUG/// Announce the ST-A state through UART
                    USART_TX_STRING_WAIT("MASTER RX OVF\n");
                #endif // UART_TERM_DEBUG
                mode_cnt = 6; /// Go to Wait LONG

            }else{/// Packet received but Not in IDLE (Improbable)
                *trx_flag &= ~0x02; /// RESET the RX bit
                *trx_flag &= ~0x04; /// RESET the INFO bit
                CC1101_ByteCommand(0x3A,0); /// SFRX
                CC1101_ByteCommand(0x3B,0); /// SFTX
                #ifdef UART_TERM_DEBUG/// Announce start of ST-B
                    USART_TX_STRING_WAIT("MASTER PKT RX NIDLE\n");
                #endif // UART_TERM_DEBUG
                mode_cnt = 6; /// Go to Wait LONG
            }

        }else{/// ====== NO INT0 pulse present ====================================
            /// Check the timer for the "TIMEOUT"
            if( (TIMER1_get_value()-ref_time) >= (T_MASTER_RX_RDY + T_TX_RX + T_MASTER_RX_LATE) ){
                ref_time = TIMER1_get_value(); /// Reset the ref_time
                /// ================================================================
                /// ============== TIMEOUT =========================================
                /// ================================================================

                #ifdef UART_TERM_DEBUG
                    USART_TX_STRING_WAIT("... TIMEOUT... \n");
                #endif // UART_TERM_DEBUG
                /// Put the TRX module into IDLE state:
                CC1101_ByteCommand(0x36,0); /// SIDLE
                mode_cnt = 6;/// Go to WAIT-LONG
                /// Announce lost packet ( for the LED signalization):
                system_flag &= ~TRX_PKTREC; /// RESET the LED ACK signalization
            }
        }// if int0_flag_pulse

    }else if(mode_cnt==4){ /// RX-DONE mode  Station-A  =====================================================================
    /// ================================================================================================================
    /// TRX module is in IDLE state. A packet has been received for sure.
        /// Read data from RXFIFO (if any...). Determine whether it's the Empty packet or the Data Packet.
        /// In case of Data Packet,check if TX event has been started. If YES, set the trx_flag |= 0x02 and copy the data into TRX_message[33]
        /// In case of Empty packet, set the trx_flag |= 0x04 and copy data into TRX_info[5];
        /// Then move into Wait mode for the rest of the T_sync period. (which started in INIT mode)

        /// READ RX FIFO STATE:
        byte_num = SPI_TRX_STATUSREG_RD(0x3B); /// RXFIFO STATUS
        if(byte_num>0){ /// It received something:
            #ifdef UART_TERM_DEBUG
                USART_TX_STRING_WAIT("\n Payload has ");
                USART_TX_STRING_WAIT(itoa(byte_num,buffer,10));
                USART_TX_STRING_WAIT("bytes\n");
            #endif // UART_TERM_DEBUG
            memset(rx_array_a,'\0',65); /// Clear the RX array
            /// Read the whole content of the RX FIFO:
            CC1101_ByteCommand(0xFF,0x01); /// Stream RX FIFO READ HEADER
            for(i=0;i<(byte_num-1); i=i+1){
                rx_array_a[i] =CC1101_ByteCommand(0x00,0x01);/// CSB - LOW
            }
            rx_array_a[(byte_num-1)] = CC1101_ByteCommand(0x00,0x00); /// CSB - HIGH

            ///  ===================== Check if CRC is OK:=====================================
            if(rx_array_a[(byte_num-1)]&0x80){ /// CRC is OK ==================================
                /// To LED Signalization system:

                #ifdef UART_TERM_DEBUG
                    USART_TX_STRING_WAIT(" CRC is OK \n");
                #endif // UART_TERM_DEBUG

                if( (system_flag&(TRX_TXEVNT|TRX_PKTSNT)) == (TRX_TXEVNT|TRX_PKTSNT) ){ /// Expecting ACK from slave
                    /// The Data packet has been sent and the ACK received.
                    /// TX event DONE.
                    #ifdef UART_TERM_DEBUG
                        USART_TX_STRING_WAIT("MASTER TXEVENT DONE\n");
                    #endif // UART_TERM_DEBUG
                    system_flag &= ~(TRX_TXEVNT|TRX_PKTSNT);
                    system_flag |= LCD_CLRALL; /// clear the LCD
                    ///Erase the TRX_message[] array:
                    memset(TRX_message,'\0',33);

                }else{ /// Expecting empty OR data packet from SLAVE:
                    if(rx_array_a[0] > 2){ /// Incoming length byte > 2   => Data byte received
                        /// ======================= INCOMING DATA PACKET ======================
                        /// ===================================================================
                        /// Check if previous message is processed:
                        if((system_flag&TRX_RXEVNT) == 0x0000){ /// RX bit is Cleared
                            #ifdef UART_TERM_DEBUG
                                USART_TX_STRING_WAIT("Data from SLAVE =======\n");
                            #endif // UART_TERM_DEBUG
                            for(i=3;i<(byte_num-2); i=i+1){ /// Save just pure DATA, no appended bytes...
                                TRX_message[i-3] = rx_array_a[i];
                            }// end for
                            /// Request processing of the message received:
                            *trx_flag |= 0x02; /// Set the RX bit

                        }else{ /// The previous message is NOT processed yet:
                            /// IGNORE this message:
                        }

                    }else{
                        /// ======================= INCOMING "EMPTY PACKET" =====================
                        /// ===================================================================
                        TRX_info[0] = rx_array_a[1]; ///His-Station ID. rx_array_a[0] is the length byte...
                        TRX_info[1] = rx_array_a[2]; /// His-RSSI
                        TRX_info[2] = rx_array_a[(byte_num-2)]; /// My-RSSI (this station)
                        TRX_info[3] = rx_array_a[(byte_num-1)]; /// CRC-OK + LQI
                        #ifdef UART_TERM_DEBUG
                            USART_TX_STRING_WAIT("TRX info\n");
                        #endif // UART_TERM_DEBUG
                        *trx_flag |= 0x04; /// SET the INFO bit
                    }
                }//end if

                /// Go to WAIT-LONG mode (T_sync):
                mode_cnt = 6; /// WAIT-LONG (Station-A has to be synchronous...)

            }else{ /// CRC NOT OK
                #ifdef UART_TERM_DEBUG
                    USART_TX_STRING_WAIT("CRC Not OK \n");
                #endif // UART_TERM_DEBUG
                system_flag &= ~TRX_PKTREC; /// CLEAR the ACK-bit in system flag register
                if( (system_flag&(TRX_TXEVNT|TRX_PKTSNT)) == (TRX_TXEVNT|TRX_PKTSNT)){
                    /// If  an ACK of previous Transmission was expected:
                    /// Do NOT erase TRX_Message
                    /// Do NOT CLEAR the trx_flag 0x01 !!! This TX operation Must be done again!
                }else{
                    /// No ACK was awaited:
                    memset(TRX_message,'\0',33);
                }
                mode_cnt = 6; /// WAIT-LONG (Station-A has to be synchronous...)

            }//end if
        }else{ /// IDLE state but RX FIFO EMPTY (IMPROBABLE...)
            /// Clear buffers and transmit the same word again (after WAIT-LONG...)
            CC1101_ByteCommand(0x3A,0); /// SFRX
            CC1101_ByteCommand(0x3B,0); /// SFTX
            mode_cnt = 6; /// WAIT-LONG (Station-A has to be synchronous...)
        }// end if byte_num > 0

    }else if(mode_cnt==5){ /// WAIT mode =========================================================
    /// ================================================================================================
        /// Skip until WAIT period is elapsed...
        if((TIMER1_get_value() - ref_time)>= wait_period){
            ref_time = TIMER1_get_value();
            mode_cnt = mode_stack; /// Go back where you came from...
        }

    }else if(mode_cnt==6){ /// WAIT-LONG mode ==========================================================
    /// ================================================================================================
        /// Skip until WAIT period is elapsed...
        *trx_flag &= ~0x02; /// RESET the RX bit
        *trx_flag &= ~0x04; /// RESET the INFO bit
        if((TIMER1_get_value() - ref_time_sync)>= T_SYNC ){
            ref_time_sync = TIMER1_get_value(); /// RESET the long-timer
            mode_cnt = 1; /// Go to Mode 1 (PTX mode)
        }

    }else{ /// invalid mode ============================================================================
    /// ================================================================================================
        /// go to INIT mode
        mode_cnt = 0;
    }///end if mode_cnt

}



void TRX_SLAVE_FCN(char TRX_message[33], uint8_t TRX_info[5], uint8_t int0_flag_pulse, uint8_t *trx_flag)
{
    /// 24.8.2018 SLAVE FCN, expecting ONE MASTER only!
    /// 2.9.2018 upgrade... SYNC-SEARCH mode added.
    /// 4.8.2019 SEARCH SYNC MODE removed. SLAVE stays in RX mode (higher consumption) until the packet is received


    static uint8_t mode_cnt = 0;
    static uint8_t mode_stack = 0;
    static uint16_t wait_period = 0;
    static uint16_t ref_time = 0x0000;
    static uint16_t ref_time_long = 0x0000;
    static uint8_t pkt_lost_cnt = 0x00;
    uint8_t byte_num = 0;
    uint8_t status_byte = 0;
    uint8_t i = 0;
    uint8_t rx_array_a[65];
    char buffer[5];

    if(mode_cnt == 0){ /// ========================= INIT mode Station-B========================================
    /// ========================================================================================================
       // if(uart_term)/// Announce start of ST-B
         //   USART_TX_STRING_WAIT("\n ... Starting Station-B ... \n");

        /// Flush both FIFOs:
        CC1101_ByteCommand(0x3A,0); /// SFRX
        CC1101_ByteCommand(0x3B,0); /// SFTX
        /// Go to IDLE state:
        CC1101_ByteCommand(0x36,0x00); /// SIDLE cmd strobe
        memset(rx_array_a,'\0',65); /// Clear the RX array
        /// Put Device into RX state:
        CC1101_ByteCommand(0x34,0x00); /// SRX CMD strobe
        ref_time = TIMER1_get_value(); /// Reset the timer for the RX mode duration
        mode_cnt = 1; /// RX-Ready mode

    }else if(mode_cnt==1){ /// RX-Ready mode Station-B ================================================================
    /// ===============================================================================================================
        if(int0_flag_pulse == 0x01){ /// Packet Received
            status_byte = SPI_TRX_STATUSBYTE_RD()&0x70;
            pkt_lost_cnt = 0; /// Reset the packet-lost counter
            if(status_byte == 0x00 ){ ///  ============ IDLE state =================
                #ifdef UART_TERM_DEBUG/// Announce start of ST-B
                    USART_TX_STRING_WAIT("ST-B PKT RX\n");
                #endif // UART_TERM
                system_flag |= TRX_PKTREC; /// Set the ACK-bit in system flag register
                wait_period = T_SLAVE_1_REACT;
                ref_time = TIMER1_get_value(); /// Reset the TIMER for T_react period
                ref_time_long = TIMER1_get_value(); /// Reset the timer for T_sync period
                mode_cnt = 2; /// Go to RX-DONE mode
            }else if(status_byte == 0x60 ){/// RX FIFO OVF
                CC1101_ByteCommand(0x3A,0); /// SFRX
                CC1101_ByteCommand(0x3B,0); /// SFTX
                CC1101_ByteCommand(0x36,0); /// SIDLE
                #ifdef UART_TERM_DEBUG/// Announce start of ST-B
                    USART_TX_STRING_WAIT("ST-B RX OVF\n");
                #endif // UART_TERM
                mode_cnt = 5; /// Go to Wait LONG
                ref_time_long = TIMER1_get_value(); /// Reset the timer-long

            }else{/// Packet received but Not in IDLE (Improbable)
                *trx_flag &= ~0x02; /// RESET the RX bit
                *trx_flag &= ~0x04; /// RESET the INFO bit
                CC1101_ByteCommand(0x3A,0); /// SFRX
                CC1101_ByteCommand(0x3B,0); /// SFTX
                #ifdef UART_TERM_DEBUG/// Announce start of ST-B
                    USART_TX_STRING_WAIT("ST-B PKT RX NIDLE\n");
                #endif // UART_TERM
                mode_cnt = 5; /// Go to Wait LONG
                ref_time_long = TIMER1_get_value(); /// Reset the timer-long
            }

        }else{/// ====== NO INT0 pulse present ====================================
            /// Check the timer for the "TIMEOUT"
            if( (TIMER1_get_value()-ref_time) >= (T_SLAVE_RX_RDY + T_TX_RX + T_SLAVE_RX_LATE) ){
                ref_time = TIMER1_get_value(); /// Reset the ref_time
                //ref_time_long = timer_cnt;
                /// Announce lost packet ( for the LED signalization):
                system_flag &= ~TRX_PKTREC; /// CLEAR the ACK-bit in system flag register

                pkt_lost_cnt++;
                if(pkt_lost_cnt >= PKT_LOST_NUM){ /// Synchronization was LOST...
                    #ifdef UART_TERM_DEBUG/// Announce start of ST-B
                        USART_TX_STRING_WAIT("ST-B SYNC LOST\n");
                    #endif // UART_TERM
                    pkt_lost_cnt = PKT_LOST_NUM; /// Freeze the counter
                    /// Go to SYNC-SEARCH mode where the CC1101 remains in RX mode.
                    mode_cnt = 1;
                    //wait_period = T_SLAVE_SYNCSEARCH;
                    //mode_cnt = 4; /// WAIT mode
                   // mode_stack = 0; /// After Wait, go to INIT mode (need to execute SRX command strobe)

                }else{
                    CC1101_ByteCommand(0x36,0); /// SIDLE
                    #ifdef UART_TERM_DEBUG/// Announce start of ST-B
                        USART_TX_STRING_WAIT("ST-B PKT LOST\n");
                    #endif // UART_TERM
                    wait_period = T_SYNC-(T_SLAVE_RX_RDY + T_TX_RX + T_SLAVE_RX_LATE);
                    mode_cnt = 4; /// WAIT mode
                    mode_stack = 0; /// After Wait, go to INIT mode (need to execute SRX command strobe)
                    /// Go to WAIT for STB_PER_REACT (reaction period specific for STATION B):
                }

            }// end if
            /// Otherwise just skip...
        }// end if (int0_flag_pulse)

    }else if(mode_cnt== 2){ /// RX - DONE mode Station-B ================================================================
    /// =========================================================================================================
        /// TRX module is in IDLE state. A packet has been received for sure.
        /// Read data from RXFIFO (if any...). Determine whether it's the Empty packet or the Data Packet.
        /// In case of Data Packet, Set the trx_flag |= 0x02 and copy the data into TRX_message[33]
        /// In case of Empty packet, set the trx_flag |= 0x04 and read data into TRX_info[5];
        /// Then move into Wait mode for the rest of the T_react period. (which started by INT0 event)

        /// READ RX FIFO STATE:
        byte_num = SPI_TRX_STATUSREG_RD(0x3B); ///Read RXFIFO STATUS register
        if(byte_num>0){ /// It received something:
            #ifdef UART_TERM_DEBUG
                USART_TX_STRING_WAIT("\n Payload has ");
                USART_TX_STRING_WAIT(itoa(byte_num,buffer,10));
                USART_TX_STRING_WAIT("bytes\n");
            #endif // UART_TERM
            memset(rx_array_a,'\0',65); /// Clear the RX array
            /// Read the content of the RX FIFO:
            CC1101_ByteCommand(0xFF,0x01); /// Stream RX FIFO READ HEADER
            for(i=0;i<(byte_num-1); i=i+1){
                rx_array_a[i] =CC1101_ByteCommand(0x00,0x01);/// CSB - LOW
            }
            rx_array_a[(byte_num-1)] = CC1101_ByteCommand(0x00,0x00); /// CSB - HIGH
            if(rx_array_a[0] > 2){
                /// ========= Data byte received ==========================
                if((system_flag&TRX_RXEVNT) == 0x0000){ /// RX bit is Cleared
                    /// Start RX EVENT:
                    *trx_flag |= 0x02; /// Set the RX bit
                    #ifdef UART_TERM_DEBUG
                        USART_TX_STRING_WAIT("Data from MASTER\n");
                    #endif // UART_TERM
                    for(i=3;i<(byte_num-2); i=i+1){ /// Just the pure DATA, no appended bytes...
                        TRX_message[i-3] = rx_array_a[i];
                    }// end for

                }else{ /// The previous message is NOT processed yet:
                    /// IGNORE this message:
                }

            }else{ /// Empty Packet received:
                TRX_info[0] = rx_array_a[1]; ///His-Station ID
                TRX_info[1] = rx_array_a[2]; ///His-RSSI
                TRX_info[2] = rx_array_a[(byte_num-2)]; /// My-RSSI
                TRX_info[3] = rx_array_a[(byte_num-1)]; /// CRC-OK + LQI
                #ifdef UART_TERM_DEBUG
                    USART_TX_STRING_WAIT("TRX info\n");
                #endif // UART_TERM
                *trx_flag |= 0x04; /// SET the INFO bit
            }//end if
            /// move into WAIT mode for the rest of the T_react period.
            mode_cnt = 4;  /// WAIT
            mode_stack = 3; /// After WAIT go to TX mode.

        }else{ /// IDLE state but RX FIFO EMPTY (IMPROBABLE...)
            #ifdef UART_TERM_DEBUG
                USART_TX_STRING_WAIT("ST-B RXFIFO EMPTY\n");
            #endif // UART_TERM
            /// Clear buffers and transmit the same word again (after WAIT-LONG...)
            CC1101_ByteCommand(0x3A,0); /// SFRX
            CC1101_ByteCommand(0x3B,0); /// SFTX
            mode_cnt = 4; /// WAIT
            mode_stack = 3; /// After WAIT go to TX mode.
        }// end if byte_num > 0


    }else if(mode_cnt==3){ /// Mode 3 TX Station-B==============================================================
    /// ========================================================================================================
        /// The TRX module is in IDLE state. In this mode, the TXFIFO is erased and filled with data
        /// (Empty or Data packet).

        /// Check the message buffer for any data:
        byte_num = strlen(TRX_message);
        /// Flush both FIFOs:
        CC1101_ByteCommand(0x3A,0); /// SFRX
        CC1101_ByteCommand(0x3B,0); /// SFTX
        /// Fill the TX FIFO
        if(byte_num>0){ /// TX flag set and data is available. Send Data packet:
            if( system_flag&TRX_TXEVNT){ /// TX event is started by the System.
                /// Fill the TX FIFO with data:
                CC1101_ByteCommand(0x7F,0x01); /// Burst TX FIFO WRITE HEADER
                CC1101_ByteCommand((byte_num+2),0x01); /// Length-byte (Station ID + data)
                CC1101_ByteCommand(SLAVE_1_ID,0x01); /// Lower Byte second, CSB - stays LOW
                CC1101_ByteCommand(TRX_info[2],0x01); /// Send him my RSSI
                for(i=0; i<(byte_num-1); i=i+1){
                    CC1101_ByteCommand(TRX_message[i],0x01);
                }//end for
                CC1101_ByteCommand(TRX_message[(byte_num-1)],0x00);/// CSB-LOW

                /// Erase the Buffer:
                memset(TRX_message,'\0',33);
                /// End the TX event:
                system_flag &= ~TRX_TXEVNT; /// CLEAR the TX bit - SLAVE doesn't wait for ACK...

            }else{  /// Data available, but TX flag is CLEARED.. Send Empty packet:
                CC1101_ByteCommand(0x7F,0x01); /// Burst TX FIFO WRITE HEADER CSB - LOW
                CC1101_ByteCommand(2,0x01); /// Length-byte goes first   CSB - LOW
                CC1101_ByteCommand(SLAVE_1_ID|((system_flag&(LCD_TYPMSG))>>5),0x01); /// Lower Byte second, CSB - LOW
                CC1101_ByteCommand(TRX_info[2],0x00); /// Send him my RSSI  CSB - HIGH
            }

        }else{ /// Data NOT available.. Send Empty Packet:
            CC1101_ByteCommand(0x7F,0x01); /// Burst TX FIFO WRITE HEADER, CSB - LOW
            CC1101_ByteCommand(2,0x01); /// Length-byte goes first, CSB - LOW
            CC1101_ByteCommand(SLAVE_1_ID|((system_flag&(LCD_TYPMSG))>>5),0x01); /// Lower Byte second, CSB - LOW
            CC1101_ByteCommand(TRX_info[2],0x00); /// Send him my RSSI  CSB - HIGH
        }

        /// Put Device into TX state:
        CC1101_ByteCommand(0x35,0x00); /// STX CMD strobe
        /// now CC1101 goes from TX straight to IDLE... see config. register MCSM1 bits [1:0]
        /// The function now has to wait for the rest of the T_sync period.

        mode_cnt = 5; /// Wait-Long mode
        *trx_flag &= ~0x02; /// RESET the RX bit
        *trx_flag &= ~0x04; /// RESET the INFO bit

    }else if(mode_cnt==4){ /// WAIT mode Station-B ==============================================================
    /// =========================================================================================================
        /// Skip until WAIT period is elapsed...
        *trx_flag &= ~0x02; /// RESET the RX bit
        *trx_flag &= ~0x04; /// RESET the INFO bit
        if((TIMER1_get_value() - ref_time) >= wait_period){
            ref_time = TIMER1_get_value();
            mode_cnt = mode_stack; /// Go back where you came from...
        }
    }else if(mode_cnt==5){ /// WAIT-LONG mode Station-B =========================================================
    /// =========================================================================================================
        *trx_flag &= ~0x02; /// RESET the RX bit
        *trx_flag &= ~0x04; /// RESET the INFO bit
        /// Skip until WAIT period is elapsed...
        if((TIMER1_get_value() - ref_time_long) >= (T_SYNC-T_SLAVE_RX_RDY-T_TX_RX) ){
            ref_time_long = TIMER1_get_value(); /// LONG-Timer reset
            ref_time = TIMER1_get_value(); /// Timer reset
            mode_cnt = 0; /// Go back to mode 1 (RX-Ready mode)
        }

    }else{ /// invalid mode =====================================================================================
    /// =========================================================================================================
        /// go to INIT mode
        mode_cnt = 0;
    }///end if mode_cnt

}
