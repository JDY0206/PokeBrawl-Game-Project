// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// your name
// your data 
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"
#include "PokemonMechanics.h"

// Define sound frequencies for SysTick->LOAD (adjust as needed)
#define E0   7584  // 329.6 Hz
#define AF0  6020  // 415.3 Hz
#define B0   5062  // 493.9 Hz
#define DF   4510  // 554.4 Hz
#define DEFAULT_RATE 7273 // 11 kHz for 80 MHz clock

const uint8_t* soundPointer = nullptr;  // Pointer to the current sound array
uint32_t soundCount = 0;                // Number of samples left in the current sound
uint32_t soundIndex = 0;                // Index for accessing the current sound array

extern uint32_t currentInput;
extern uint32_t global_time_ms;

// Arm SysTick with given period and priority
void SysTick_IntArm(uint32_t period, uint32_t priority){
    SysTick->CTRL = 0;                 // Disable SysTick during setup
    SysTick->LOAD = period - 1;        // Set the period (SysTick interrupt frequency)
    SysTick->VAL = 0;                  // Clear the current value
    SCB->SHP[1] = (SCB->SHP[1] & ~0xFF) | (priority << 5);  // Set priority (highest priority = 0)
    SysTick->CTRL = 0x0007;            // Enable SysTick with core clock and interrupt
}

// Initialize the sound system and DAC
void Sound_Init(void){
    DAC5_Init();
    SysTick_IntArm(DEFAULT_RATE, 0); // 11 kHz
    soundPointer = nullptr;
    soundCount = 0;
    soundIndex = 0;
}

// SysTick interrupt handler: output one sample to DAC if sound is active
extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){
    if (soundPointer != nullptr && soundCount > 0) {
        DAC5_Out(soundPointer[soundIndex]);
        soundIndex++;
        soundCount--;
        if (soundCount == 0) {
            soundPointer = nullptr;
        }
    }
}

// Start playing a sound: set pointer, count, and SysTick period
void Sound_Start(const uint8_t *pt, uint32_t count, uint32_t whichSwitch){
    soundPointer = pt;
    soundCount = count;
    soundIndex = 0;

    // Set SysTick->LOAD for different pitches if needed
    switch (whichSwitch) {
        case 1:
            SysTick->LOAD = E0 - 1;
            break;
        case 2:
            SysTick->LOAD = AF0 - 1;
            break;
        case 4:
            SysTick->LOAD = B0 - 1;
            break;
        case 8:
            SysTick->LOAD = DF - 1;
            break;
        default:
            SysTick->LOAD = DEFAULT_RATE - 1;
            break;
    }
    SysTick->VAL = 0; // Clear current value to restart timing
}

// Stop sound output
void Sound_Stop(void){
    soundPointer = nullptr;
    soundCount = 0;
    SysTick->LOAD = 0;
}
