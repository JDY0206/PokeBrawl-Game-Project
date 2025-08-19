#include "PokemonMechanics.h"

bool CheckDeadZoneJ1(); // Returns true if the joystick is in the deadzone and false if it isn't 

bool CheckDeadZoneJ2(); // Returns true if the joystick is in the deadzone and false if it isn't 
 
void J1VConvert(Pokemon* player); // Returns the velocity of the player based on the joystick 1

void J2VConvert(Pokemon* player); // Returns the velocity of the player based on the joystick 2

int8_t J1VOConvert(Outliner* player);

int8_t J2VOConvert(Outliner* player);

void J1VGConvert(Pokemon* player,  uint16_t Jx, uint16_t Jy); // Returns the velocity of the player based on the joystick 1

void J2VGConvert(Pokemon* player, uint16_t Jx, uint16_t Jy); // Returns the velocity of the player based on the joystick 2
