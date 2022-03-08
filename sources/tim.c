#include "tim.h"


void TIMER1_setup(void)
{

  /// CS12    CS11    CS10      N
  ///   0       0       0       No source
  ///   0       0       1       1
  ///   0       1       0       8
  ///   0       1       1       64
  ///   1       0       0       256
  ///   1       0       1       1024
  ///   1       1       0       Extern Falling
  ///   1       1       1       Extern Rising
    #ifdef F_CPU_16
    /// F_CPU = 16 MHz
  TCCR1A |= (0<<WGM11)|(0<<WGM10);
  TCCR1B |= (0<<WGM13)|(1<<WGM12)|(0<<CS12)|(1<<CS11)|(1<<CS10);
  /// CTC mode, N = 64
  TIMSK1 |= (0<<TOIE1)|(1<<OCIE1A);
  /// CTC interrupt enabled
  OCR1A = 249;
  /// T_ISR = 1.000 ms
  #endif // F_CPU_16
  /// ----------------------------------------------------------
  #ifdef F_CPU_4
    /// F_CPU = 4 MHz
  TCCR1A |= (0<<WGM11)|(0<<WGM10);
  TCCR1B |= (0<<WGM13)|(1<<WGM12)|(0<<CS12)|(1<<CS11)|(0<<CS10);
  /// CTC mode, N = 8
  TIMSK1 |= (0<<TOIE1)|(1<<OCIE1A);
  /// CTC interrupt enabled
  OCR1A = 499;
  /// T_ISR = 1.000 ms

  #endif // F_CPU_4

}

uint16_t TIMER1_get_value(void)
{
   uint16_t val = 0;
   val = timer1_cnt;
   return val;
}


/// ================== INTERRUPT SERVICE ROUTINE ===============================


ISR(TIMER1_COMPA_vect){
    /// Every 1 ms
    timer1_cnt++;
}





