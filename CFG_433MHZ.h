#ifndef CFG_433MHZ_H_INCLUDED
#define CFG_433MHZ_H_INCLUDED

#ifdef FREQ_433


#ifdef CFG_1200BAUD
   /// ============================ 433MHz CONFIG 2 ============================================
    ///  433 MHz, 1.2 kBaud, chanSpace = 199.9 kHz, Dev = 5.2 kHz,
    /// GFSK, RX_BW = 58 kHz, Optimized for current consumption,
    /// Whitening ON, Manchester OFF, PA Ramping OFF, Append Status ON
    /// DEFAULT Output power  = 8.5 dBm @ 868 MHz,  7.8 dBm @ 433 MHz
    /// Channel number - 0
    /// GDO0 Interrupt
uint8_t TRX_config_1[47] ={
    0x29,  // IOCFG2              GDO2 Output Pin Configuration
    0x2E,  // IOCFG1              GDO1 Output Pin Configuration
    0x07,  // IOCFG0              GDO0 - Asserts when a packet has been received with CRC OK.
    //De-asserts when the first byte is read from the RX FIFO.
    0x47,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
    0xD3,  // SYNC1               Sync Word, High Byte
    0x91,  // SYNC0               Sync Word, Low Byte
    0xFF,  // PKTLEN              Packet Length
    0x8C,  // PKTCTRL1            PQT = 4, CRC auto-flush ON, Append Status ON, Address check OFF.
    0x45,  // PKTCTRL0            Whitening ON, PKT_Format = NORMAL, CRC calc ON,  LENGTH_CONFIG = Variable packet length mode.

    0x00,  // ADDR                Device Address
    0x00,  // CHANNR              Channel Number
    0x06,  // FSCTRL1             Frequency Synthesizer Control
    0x00,  // FSCTRL0             Frequency Synthesizer Control
    0x10,  // FREQ2               Frequency Control Word, High Byte
    0xB1,  // FREQ1               Frequency Control Word, Middle Byte
    0x3A,  // FREQ0               Frequency Control Word, Low Byte
    0xF5,  // MDMCFG4             Modem Configuration
    0x83,  // MDMCFG3             Modem Configuration
    0x93,  // MDMCFG2             Modem Configuration
    0x22,  // MDMCFG1             Modem Configuration
    0xF8,  // MDMCFG0             Modem Configuration
    0x15,  // DEVIATN             Modem Deviation Setting
    0x07,  // MCSM2               Main Radio Control State Machine Configuration
    0x30,  // MCSM1               Main Radio Control State Machine Configuration * GOTO IDLE right from TX, or from RX
    0x18,  // MCSM0               Main Radio Control State Machine Configuration
    0x16,  // FOCCFG              Frequency Offset Compensation Configuration
    0x6C,  // BSCFG               Bit Synchronization Configuration
    0x03,  // AGCCTRL2            AGC Control
    0x40,  // AGCCTRL1            AGC Control
    0x91,  // AGCCTRL0            AGC Control
    0x87,  // WOREVT1             High Byte Event0 Timeout
    0x6B,  // WOREVT0             Low Byte Event0 Timeout
    0xFB,  // WORCTRL             Wake On Radio Control
    0x56,  // FREND1              Front End RX Configuration
    0x10,  // FREND0              Front End TX Configuration

    0xE9,  // FSCAL3              Frequency Synthesizer Calibration
    0x2A,  // FSCAL2              Frequency Synthesizer Calibration
    0x00,  // FSCAL1              Frequency Synthesizer Calibration
    0x1F,  // FSCAL0              Frequency Synthesizer Calibration

    0x41,  // RCCTRL1             RC Oscillator Configuration
    0x00,  // RCCTRL0             RC Oscillator Configuration
    0x59,  // FSTEST              Frequency Synthesizer Calibration Control
    0x7F,  // PTEST               Production Test
    0x3F,  // AGCTEST             AGC Test

    0x81,  // TEST2               Various Test Settings
    0x35,  // TEST1               Various Test Settings
    0x09  // TEST0               Various Test Settings

};

    #endif // CFG_1200BAUD

    #ifdef CFG_10000BAUD
    /// Optimized for current consumption,
uint8_t TRX_config_1[47] ={
    0x29,  // IOCFG2              GDO2 Output Pin Configuration
    0x2E,  // IOCFG1              GDO1 Output Pin Configuration
    0x07,  // IOCFG0              GDO0 - Asserts when a packet has been received with CRC OK.

    0x47,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
    0xD3,  // SYNC1               Sync Word, High Byte
    0x91,  // SYNC0               Sync Word, Low Byte
    0xFF,  // PKTLEN              Packet Length
    0x8C,  // PKTCTRL1            PQT = 4, CRC auto-flush ON, Append Status ON, Address check OFF.
    0x45,  // PKTCTRL0            Whitening ON, PKT_Format = NORMAL, CRC calc ON,  LENGTH_CONFIG = Variable packet length mode.
    0x00,  // ADDR                Device Address
    0x00,  // CHANNR              Channel Number
    0x08,  // FSCTRL1             Frequency Synthesizer Control
    0x00,  // FSCTRL0             Frequency Synthesizer Control
    0x10,  // FREQ2               Frequency Control Word, High Byte
    0xB1,  // FREQ1               Frequency Control Word, Middle Byte
    0x3A,  // FREQ0               Frequency Control Word, Low Byte
    0xC8,  // MDMCFG4             Modem Configuration
    0x93,  // MDMCFG3             Modem Configuration
    0x93,  // MDMCFG2             Modem Configuration
    0x22,  // MDMCFG1             Modem Configuration
    0xF8,  // MDMCFG0             Modem Configuration
    0x34,  // DEVIATN             Modem Deviation Setting
    0x07,  // MCSM2               Main Radio Control State Machine Configuration
    0x30,  // MCSM1               Main Radio Control State Machine Configuration * GOTO IDLE right from TX, or from RX
    0x18,  // MCSM0               Main Radio Control State Machine Configuration
    0x16,  // FOCCFG              Frequency Offset Compensation Configuration
    0x6C,  // BSCFG               Bit Synchronization Configuration
    0x43,  // AGCCTRL2            AGC Control
    0x40,  // AGCCTRL1            AGC Control
    0x91,  // AGCCTRL0            AGC Control
    0x87,  // WOREVT1             High Byte Event0 Timeout
    0x6B,  // WOREVT0             Low Byte Event0 Timeout
    0xFB,  // WORCTRL             Wake On Radio Control
    0x56,  // FREND1              Front End RX Configuration
    0x10,  // FREND0              Front End TX Configuration
    0xE9,  // FSCAL3              Frequency Synthesizer Calibration
    0x2A,  // FSCAL2              Frequency Synthesizer Calibration
    0x00,  // FSCAL1              Frequency Synthesizer Calibration
    0x1F,  // FSCAL0              Frequency Synthesizer Calibration
    0x41,  // RCCTRL1             RC Oscillator Configuration
    0x00,  // RCCTRL0             RC Oscillator Configuration
    0x59,  // FSTEST              Frequency Synthesizer Calibration Control
    0x7F,  // PTEST               Production Test
    0x3F,  // AGCTEST             AGC Test
    0x81,  // TEST2               Various Test Settings
    0x35,  // TEST1               Various Test Settings
    0x09  // TEST0               Various Test Settings
};
    #endif // CFG_10000BAUD

    #ifdef CFG_38400BAUD
    /// Optimized for current consumption
uint8_t TRX_config_1[47] ={
    0x29,  // IOCFG2              GDO2 Output Pin Configuration
    0x2E,  // IOCFG1              GDO1 Output Pin Configuration
    0x07,  // IOCFG0              GDO0 - Asserts when a packet has been received with CRC OK.
    0x47,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
    0xD3,  // SYNC1               Sync Word, High Byte
    0x91,  // SYNC0               Sync Word, Low Byte
    0xFF,  // PKTLEN              Packet Length
    0x8C,  // PKTCTRL1            PQT = 4, CRC auto-flush ON, Append Status ON, Address check OFF.
    0x45,  // PKTCTRL0            Whitening ON, PKT_Format = NORMAL, CRC calc ON,  LENGTH_CONFIG = Variable packet length mode.
    0x00,  // ADDR                Device Address
    0x00,  // CHANNR              Channel Number
    0x08,  // FSCTRL1             Frequency Synthesizer Control
    0x00,  // FSCTRL0             Frequency Synthesizer Control
    0x10,  // FREQ2               Frequency Control Word, High Byte
    0xB1,  // FREQ1               Frequency Control Word, Middle Byte
    0x3A,  // FREQ0               Frequency Control Word, Low Byte
    0xCA,  // MDMCFG4             Modem Configuration
    0x83,  // MDMCFG3             Modem Configuration
    0x93,  // MDMCFG2             Modem Configuration
    0x22,  // MDMCFG1             Modem Configuration
    0xF8,  // MDMCFG0             Modem Configuration
    0x35,  // DEVIATN             Modem Deviation Setting
    0x07,  // MCSM2               Main Radio Control State Machine Configuration
    0x30,  // MCSM1               Main Radio Control State Machine Configuration * GOTO IDLE right from TX, or from RX
    0x18,  // MCSM0               Main Radio Control State Machine Configuration
    0x16,  // FOCCFG              Frequency Offset Compensation Configuration
    0x6C,  // BSCFG               Bit Synchronization Configuration
    0x43,  // AGCCTRL2            AGC Control
    0x40,  // AGCCTRL1            AGC Control
    0x91,  // AGCCTRL0            AGC Control
    0x87,  // WOREVT1             High Byte Event0 Timeout
    0x6B,  // WOREVT0             Low Byte Event0 Timeout
    0xFB,  // WORCTRL             Wake On Radio Control
    0x56,  // FREND1              Front End RX Configuration
    0x10,  // FREND0              Front End TX Configuration
    0xE9,  // FSCAL3              Frequency Synthesizer Calibration
    0x2A,  // FSCAL2              Frequency Synthesizer Calibration
    0x00,  // FSCAL1              Frequency Synthesizer Calibration
    0x1F,  // FSCAL0              Frequency Synthesizer Calibration
    0x41,  // RCCTRL1             RC Oscillator Configuration
    0x00,  // RCCTRL0             RC Oscillator Configuration
    0x59,  // FSTEST              Frequency Synthesizer Calibration Control
    0x7F,  // PTEST               Production Test
    0x3F,  // AGCTEST             AGC Test
    0x81,  // TEST2               Various Test Settings
    0x35,  // TEST1               Various Test Settings
    0x09  // TEST0               Various Test Settings
};

    #endif // CFG_38400BAUD

    #ifdef CFG_76800BAUD
uint8_t TRX_config_1[47] ={
    0x29,  // IOCFG2              GDO2 Output Pin Configuration
    0x2E,  // IOCFG1              GDO1 Output Pin Configuration
    0x07,  // IOCFG0              GDO0 - Asserts when a packet has been received with CRC OK.
    0x47,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
    0xD3,  // SYNC1               Sync Word, High Byte
    0x91,  // SYNC0               Sync Word, Low Byte
    0xFF,  // PKTLEN              Packet Length
    0x8C,  // PKTCTRL1            PQT = 4, CRC auto-flush ON, Append Status ON, Address check OFF.
    0x45,  // PKTCTRL0            Whitening ON, PKT_Format = NORMAL, CRC calc ON,  LENGTH_CONFIG = Variable packet length mode.
    0x00,  // ADDR                Device Address
    0x00,  // CHANNR              Channel Number
    0x0B,  // FSCTRL1             Frequency Synthesizer Control
    0x00,  // FSCTRL0             Frequency Synthesizer Control
    0x10,  // FREQ2               Frequency Control Word, High Byte
    0xB1,  // FREQ1               Frequency Control Word, Middle Byte
    0x3A,  // FREQ0               Frequency Control Word, Low Byte
    0x7B,  // MDMCFG4             Modem Configuration
    0x83,  // MDMCFG3             Modem Configuration
    0x93,  // MDMCFG2             Modem Configuration
    0x22,  // MDMCFG1             Modem Configuration
    0xF8,  // MDMCFG0             Modem Configuration
    0x42,  // DEVIATN             Modem Deviation Setting
    0x07,  // MCSM2               Main Radio Control State Machine Configuration
    0x30,  // MCSM1               Main Radio Control State Machine Configuration * GOTO IDLE right from TX, or from RX
    0x18,  // MCSM0               Main Radio Control State Machine Configuration
    0x1D,  // FOCCFG              Frequency Offset Compensation Configuration
    0x1C,  // BSCFG               Bit Synchronization Configuration
    0xC7,  // AGCCTRL2            AGC Control
    0x00,  // AGCCTRL1            AGC Control
    0xB2,  // AGCCTRL0            AGC Control
    0x87,  // WOREVT1             High Byte Event0 Timeout
    0x6B,  // WOREVT0             Low Byte Event0 Timeout
    0xFB,  // WORCTRL             Wake On Radio Control
    0xB6,  // FREND1              Front End RX Configuration
    0x10,  // FREND0              Front End TX Configuration
    0xEA,  // FSCAL3              Frequency Synthesizer Calibration
    0x2A,  // FSCAL2              Frequency Synthesizer Calibration
    0x00,  // FSCAL1              Frequency Synthesizer Calibration
    0x1F,  // FSCAL0              Frequency Synthesizer Calibration
    0x41,  // RCCTRL1             RC Oscillator Configuration
    0x00,  // RCCTRL0             RC Oscillator Configuration
    0x59,  // FSTEST              Frequency Synthesizer Calibration Control
    0x7F,  // PTEST               Production Test
    0x3F,  // AGCTEST             AGC Test
    0x81,  // TEST2               Various Test Settings
    0x35,  // TEST1               Various Test Settings
    0x09  // TEST0               Various Test Settings
};
    #endif // CFG_76800BAUD
#endif // FREQ_433


#endif // CFG_433MHZ_H_INCLUDED
