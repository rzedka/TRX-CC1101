#include "gpio.h"

void GPIO_setup(void)
{
    DDRLED0 |= (1<<LED0_PIN);   // output
    PORTLED &= ~(1<<LED0_PIN); // LED0 OFF

    DDRLED1 |= (1<<LED1_PIN);   // output
    PORTLED &= ~(1<<LED1_PIN); // LED1 OFF

    DDRBTN_H &= ~((1<<BTN_H0_PIN)|(1<<BTN_H1_PIN)|(1<<BTN_H2_PIN)|(1<<BTN_H3_PIN));
    DDRBTN_V &= ~((1<<BTN_V0_PIN)|(1<<BTN_V1_PIN)|(1<<BTN_V2_PIN));

    DDRSPI |= (1<<SS_PIN)|(1<<MOSI_PIN)|(1<<SCLK_PIN); // ALL SPI pins as INPUT PINS
    DDRSPI &= ~(1<<MISO_PIN);
    PORTSPI &= ~((1<<SCLK_PIN)|(1<<MOSI_PIN));

    PORTSPI |= (1<<SS_PIN);  // CSB starts HIGH

    /// External Interrupt inputs:
    DDR_EXTINT &= ~((1<<INT0_PIN)|(1<<INT1_PIN));

    /// DEBUG pins:
    DDRD |= (1<< PIND3); /// out
    PORTD &= ~(1<< PIND3); /// LOW
}


/*
void LED_toggle(uint8_t led_bit)
{   /// led_bit = 0x01, 0x02, 0x04, 0x08, ...

    uint8_t bitshift = 0;

    switch(led_bit){
    case 0x01: /// LED0
            bitshift = LED0_PIN; /// PINB0
        break;
    case 0x02: /// LED1
            bitshift = LED1_PIN; /// PINB1
        break;
    //case 0x04: /// IDLE process LED
           // bitshift = LEDx_PIN;
        break;
    default:   /// LED0
            bitshift = LED0_PIN;

    }// end switch

}
*/
