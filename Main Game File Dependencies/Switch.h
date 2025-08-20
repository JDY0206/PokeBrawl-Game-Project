/*
 * Switch.h
 *
 *  Created on: Nov 5, 2023
 *      Author: jonat
 */

#ifndef SWITCH_H_
#define SWITCH_H_

#include "PokemonMechanics.h"

// initialize your switches
void Switch_Init(void);

// return current state of switches
uint32_t Switch_In(void);

uint32_t Switch_In2(Pokemon* player);


#endif /* SWITCH_H_ */
