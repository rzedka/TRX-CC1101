#ifndef MATRIX_KEYPAD_H_INCLUDED
#define MATRIX_KEYPAD_H_INCLUDED

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>

#include "main.h"
#include "tim.h" // includes "timer1_cnt" global variable

extern uint16_t system_flag;

void Matrix_Keypad_Debounc_Edge(uint8_t *key_pressed, uint8_t *key_risedge );
    /// FCN must be placed inside the infinite LOOP. Any keyboard processing FCN must be
    /// located behind this FCN to react to "key_pressed" and "key_risedge" signals.
    /// Conditions: press only one key at the time.
    /// Vertical axis signals are driven one-at-the-time while other two are at HIGH impedance.

void Matrix_Keypad_Char_Generator(char *char_gen, uint8_t key_risedge);
    /// This function processes information from Matrix_Keypad_Debounc_Edge()
    /// It counts the rising edges of the keys. If a rising edge comes more than once per the "CHAR_TIMEOUT",
    /// different characters are generated (cellphone keypad)
    /// For example if the key "Number 2" is pressed:
    /// - once    = "A"
    /// - twice   = "B"
    /// - 3-times = "C"
    /// - 4-times = "2"
    /// The "Number 9" and "Number 7" keys are exceptions because they can be pressed 5-times { P, Q, R, S, 7},{ W, X, Y, Z, 9}
    /// The output - *char_gen is available only for One Loop Cycle (it is cleared by this FCN when it's called again)

void Matrix_Keypad_CMD_Proc(uint8_t *UI_flag, char CMD_word[33], char char_gen);
    /// the OUTPUT is "CMD_word" array, carrying the body of the message. "UI_flag" then gives instructions to TRX module and LCD display.
    /// Any bit set in *UI_flag will be cleared in the following appropriate function.

#endif // MATRIX_KEYPAD_H_INCLUDED
