#ifndef CC1101_CONFIG_H_INCLUDED
#define CC1101_CONFIG_H_INCLUDED


#include "main.h"

/// CC1101 definitions ===========================================

///  CC1101 Device ID:
#ifdef FREQ_433
    #define CC1101_ID 0x04
#endif // FREQ_433

#ifdef FREQ_868
    #define CC1101_ID 0x14
#endif // FREQ_868

/// User layer device ID:
#define MASTER_ID 65 /// Station A ID = 'A'
#define SLAVE_1_ID 66 ///  ID = 'B'
#define SLAVE_2_ID 67 /// Station C ID = 'C'
/// The MSBit must be zero (reserved for the TYPING flag)

#ifdef CFG_1200BAUD
/// ========== MASTER time periods: ===========================
/// Considering 1.2 kBd, T_TX = T_RX =~ 506.7 ms (64-Byte FIFO)
/// I will only use 32 Bytes =~ 293.3 ms.
/// T_RX_RDY should be about 1/2 times the T_RX.
/// ===================================================
/// T_sync > T_TX_RX*2 + T_SLAVE_REACT + T_SLAVE_RX_RDY, if T_SLAVE_REACT > T_MASTER_RX_RDY.
/// ===================================================
/// T_SYNC > 1766 ms
#define T_SYNC 2000 /// [ms] Synchronization period (Repetition)
#define T_TX_RX 508   /// [ms] Approx. duration of TX event (and RX is the same).
#define T_MASTER_RX_RDY 250 /// [ms] Be in RX before the packet is expected.
#define T_MASTER_RX_LATE 250 /// [ms] Stay in RX a little longer if the packet doesn't come in time

/// ========== SLAVE time periods: ===========================
#define T_SLAVE_1_REACT 500 /// [ms] SLAVE waits after packet reception until it responds...
#define T_SLAVE_RX_LATE 250  /// [ms] A time margin of RX mode AFTER the packet is expected to come...
#define T_SLAVE_RX_RDY 250   /// [ms] A time margin of RX mode BEFORE the packet is expected to come...

#define PKT_LOST_NUM 3 /// After loosing this num. of packets (coming from MASTER), SLAVE goes to SYNC mode

#endif // CFG_1200BAUD


#ifdef CFG_10000BAUD
/// ========== MASTER time periods: ===========================
/// Considering 10.0 kBd, T_TX = T_RX =~ 60.8 ms (64-Byte FIFO)
/// I will only use 32 Bytes =~ 35.2 ms.
/// T_RX_RDY should be about 2-times the T_RX.
/// ===================================================
/// T_sync > T_TX_RX*2 + T_SLAVE_REACT + T_SLAVE_RX_RDY
/// ===================================================
/// T_SYNC > 344 ms
#define T_SYNC 600 /// [ms] Synchronization period (Repetition)
#define T_TX_RX 36   /// [ms] Approx. duration of TX event (and RX is the same).
#define T_MASTER_RX_RDY 72 /// [ms] Be in RX before the packet is expected.
#define T_MASTER_RX_LATE 36 /// [ms] Stay in RX a little longer if the packet doesn't come in time

/// ========== SLAVE time periods: ===========================
#define T_SLAVE_1_REACT 200 /// [ms] SLAVE waits after packet reception until it responds...
#define T_SLAVE_RX_LATE 36  /// [ms] A time margin of RX mode AFTER the packet is expected to come...
#define T_SLAVE_RX_RDY 72   /// [ms] A time margin of RX mode BEFORE the packet is expected to come...

#define PKT_LOST_NUM 3 /// After loosing this num. of packets (coming from MASTER), SLAVE goes to SYNC mode

#endif //CFG_10000BAUD

#ifdef CFG_38400BAUD
/// ========== MASTER time periods: ===========================
/// Considering 38.4 kBd, T_TX = T_RX =~ 15.8 ms (64-Byte FIFO)
/// I will only use 32 Bytes =~ 9.1 ms.
/// T_RX_RDY should be about 2-times the T_RX.
/// ===================================================
/// T_sync > T_TX_RX*2 + T_SLAVE_REACT + T_SLAVE_RX_RDY
/// ===================================================
/// T_SYNC > 264 ms
#define T_SYNC 500 /// [ms] Synchronization period (Repetition)
#define T_TX_RX 16   /// [ms] Approx. duration of TX event (and RX is the same).
#define T_MASTER_RX_RDY 32 /// [ms] Be in RX before the packet is expected.
#define T_MASTER_RX_LATE 16 /// [ms] Stay in RX a little longer if the packet doesn't come in time

/// ========== SLAVE time periods: ===========================
#define T_SLAVE_1_REACT 200 /// [ms] SLAVE waits after packet reception until it responds...
#define T_SLAVE_RX_LATE 16  /// [ms] A time margin of RX mode AFTER the packet is expected to come...
#define T_SLAVE_RX_RDY 32   /// [ms] A time margin of RX mode BEFORE the packet is expected to come...

#define PKT_LOST_NUM 3 /// After loosing this num. of packets (coming from MASTER), SLAVE goes to SYNC mode

#endif // CFG_38400BAUD

#ifdef CFG_76800BAUD
/// ========== MASTER time periods: ===========================
/// Considering 76.8 kBd, T_TX = T_RX =~ 7.9 ms (64-Byte FIFO)
/// I will only use 32 Bytes =~ 4.6 ms.
/// T_RX_RDY should be about 1/2-times the T_RX.
/// ===================================================
/// T_sync > T_TX_RX*2 + T_SLAVE_REACT + T_SLAVE_RX_RDY
/// ===================================================
/// T_SYNC >  223 ms
#define T_SYNC 500 /// [ms] Synchronization period (Repetition)
#define T_TX_RX 9   /// [ms] Approx. duration of TX event (and RX is the same).
#define T_MASTER_RX_RDY 5 /// [ms] Be in RX before the packet is expected.
#define T_MASTER_RX_LATE 5 /// [ms] Stay in RX a little longer if the packet doesn't come in time

/// ========== SLAVE time periods: ===========================
#define T_SLAVE_1_REACT 200 /// [ms] SLAVE waits after packet reception until it responds...
#define T_SLAVE_RX_LATE 5  /// [ms] A time margin of RX mode AFTER the packet is expected to come...
#define T_SLAVE_RX_RDY 5   /// [ms] A time margin of RX mode BEFORE the packet is expected to come...

#define PKT_LOST_NUM 3 /// After loosing this num. of packets (coming from MASTER), SLAVE goes to SYNC mode

#endif // CFG_76800BAUD

/// ======================== MASTER Status_flag definitions =================================================

#define LCD_INIT 0x8000   /// LCD INIT DONE
#define LCD_BCKLGT 0x4000 /// LCD backlight
#define LCD_CLRALL 0x2000 /// LCD Clear ALL
#define LCD_TYPMSG 0x1000 /// LCD's purpose is typing message
#define LCD_PRTWHL 0x0800 /// LCD print whole message (max all 32 characters)
#define LCD_TRXINF 0x0400 /// LCD's purpose is to display TRX info
#define LCD_USED   0x0200 /// LCD currently displays something
#define LCD_PRTONE 0x0100 /// LCD print ONE character
#define TRX_INIT   0x0080 /// TRX module INIT DONE
///
///
#define TRX_RXEVNT 0x0010 /// TRX RX Event
#define TRX_ACK    0x0008 /// TRX ACK received
#define TRX_INFO   0x0004 /// TRX INFO incoming
#define TRX_PKTSNT 0x0002 /// TRX Packet Sent
#define TRX_TXEVNT 0x0001 /// TRX TX Event

/// ======================== SLAVE Status_flag definitions =================================================

#define UART_INIT 0x8000   /// LCD INIT DONE
///
///
#define TRX_INIT   0x0080 /// TRX module INIT DONE
///
///
#define TRX_RXEVNT 0x0010 /// TRX RX Event
#define TRX_PKTREC 0x0008 /// TRX Packet Received
#define TRX_INFO   0x0004 /// TRX INFO

#define TRX_TXEVNT 0x0001 /// TRX TX Event

/// ============ RSSI calculation =================
#define RSSI_offset 74 /// dBm


#endif // CC1101_CONFIG_H_INCLUDED
