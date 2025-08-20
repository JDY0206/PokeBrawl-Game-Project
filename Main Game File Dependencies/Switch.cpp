/*
 * Switch.cpp
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "PokemonMechanics.h"
// LaunchPad.h defines all the indices into the PINCM table
void Switch_Init(void){
    // write this
  IOMUX->SECCFG.PINCM[PA25INDEX] = 0x00040081; // input, no pull
  IOMUX->SECCFG.PINCM[PA26INDEX] = 0x00040081; // input, no pull
  IOMUX->SECCFG.PINCM[PA27INDEX] = 0x00040081; // input, no pull
  IOMUX->SECCFG.PINCM[PA28INDEX] = 0x00040081; // input, no pull
}
// return current state of switches

uint32_t input = 0;
uint32_t Switch_In(void){
    uint32_t inputMask = (0xF << 25);  // Mask to focus on the 3 sensor inputs
    input = GPIOA->DIN31_0; // Read input from GPIOB
    input &= inputMask;              // Isolate the relevant bits for the sensors
    input >>= 25;                    // Shift the input to the lower bits (0-2)
    return input;
    }

    
uint32_t input2 = 0;
uint32_t Switch_In2(Pokemon* player){
    uint32_t inputMask = (0xF << 25);  // Mask to focus on the 3 sensor inputs
    input2 = GPIOA->DIN31_0; // Read input from GPIOB
    input2 &= inputMask;              // Isolate the relevant bits for the sensors
    input2 >>= 25;                    // Shift the input to the lower bits (0-2)

return input2;

  
}

  

