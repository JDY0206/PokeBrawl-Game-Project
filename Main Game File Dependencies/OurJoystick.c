#include "../inc/LaunchPad.h"
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#include "MyADC.h"

// ADC channel macros for clarity
#define JOY1_X_CHANNEL 1  // PA16, ADC1.1
#define JOY1_Y_CHANNEL 2  // PA17, ADC1.2
#define JOY2_X_CHANNEL 6  // PB19, ADC1.6
#define JOY2_Y_CHANNEL 4  // PB17, ADC1.4

#define JOY1_BUTTON_PIN 12 // PB12
#define JOY2_BUTTON_PIN 11 // PA11

void Joystick_Init(void) {
    // Initialize ADC1 once using one of the channels as a baseline.
    // (This call sets up the basic ADC parameters.)
    MyADC1_Init(JOY1_X_CHANNEL, ADCVREF_INT);

    ADC1->ULLMEM.CTL1 = 0x00010000;

    // Configure ADC1 for a multi-channel scan.
    // Set CTL2 so that the ADC scans from MEMCTL[0] to MEMCTL[3].
    // (The value 0x03000000 is taken from our ADC_InitTriple example;
    //  it sets ENDADD = 3 and may also set other needed parameters.)
    ADC1->ULLMEM.CTL2 = (1<<27);
    
    // Program each memory control register for the desired channel.
    // We assume here that the ADC requires the reference value OR'ed with the channel.
    // Adjust this operation if your hardware expects a different formula.
    ADC1->ULLMEM.MEMCTL[0] = ADCVREF_INT | JOY1_X_CHANNEL; // Joystick1 X (channel 1)
    ADC1->ULLMEM.MEMCTL[1] = ADCVREF_INT | JOY1_Y_CHANNEL; // Joystick1 Y (channel 2)
    ADC1->ULLMEM.MEMCTL[2] = ADCVREF_INT | JOY2_X_CHANNEL; // Joystick2 X (channel 6)
    ADC1->ULLMEM.MEMCTL[3] = ADCVREF_INT | JOY2_Y_CHANNEL; // Joystick2 Y (channel 4)
    
    // Configure button pins as GPIO inputs (your existing button configuration code).
    // PB12 (JOY1 button)
    GPIOB->DOE31_0 &= ~(1 << JOY1_BUTTON_PIN);  // Disable output (set to input)
    GPIOB->DOESET31_0 &= ~(1 << JOY1_BUTTON_PIN);
    GPIOB->DOUTSET31_0 |= (1 << JOY1_BUTTON_PIN);  // Enable input (pull-up/down control as required)
    // PA11 (JOY2 button)
    GPIOA->DOE31_0 &= ~(1 << JOY2_BUTTON_PIN);
    GPIOA->DOESET31_0 &= ~(1 << JOY2_BUTTON_PIN);
    GPIOA->DOUTSET31_0 |= (1 << JOY2_BUTTON_PIN);
}


uint16_t Joystick1_ReadX(void) { // Right joystick
    return MyADC1_In(0);
}

uint16_t Joystick1_ReadY(void) {
    return MyADC1_In(1);
}

bool Joystick1_ButtonPressed(void) {
    uint16_t val = GPIOB->DIN31_0 & (1 << JOY1_BUTTON_PIN);
    if (val == 1) return true;
    else return false;
}

uint16_t Joystick2_ReadX(void) { // Left joystick
    return MyADC1_In(2);
}

uint16_t Joystick2_ReadY(void) {
    return MyADC1_In(3);
}

bool Joystick2_ButtonPressed(void) {
    uint16_t val = GPIOA->DIN31_0 & (1 << JOY2_BUTTON_PIN); // Active low
    if (val == 1) return true;
    else return false;

}

