/*
    Movement Mechanics 
*/

#include <iostream>
#include <math.h>
#include "MovementMechanics.h"
#include "OurJoystick.h"
#include "PokemonMechanics.h"

#define middle 2675 // The average middle value of the joysticks 
#define deadspace 75 // Dead values (middle values) go from 2600 to 2750 with this number
#define deadspace1 150
#define deadspace2 300
#define deadspace3 450


// Assumes Joystick Init has already been called 
// Limit the pixel redraw to be between -2 and 2 so that is automatically deletes old images with the 2 pixels of black around it 

// 16-direction mapping (corrected)

const int8_t dx[16] = { 0,   1,   1,   2,  2,  2,  1,  1, 0, -1, -1, -2, -2, -2, -1, -1};
const int8_t dy[16] = {-2,  -2,  -1,  -1,  0,  1,  1,  2, 2,  2,  1,  1,  0, -1, -1, -2};

const int8_t dx1[4] = {-1,  0, 1,  0};
const int8_t dy1[4] = {0,   1, 0, -1};

bool CheckDeadZoneJ1 (){
    int16_t rawx = 0;
    int16_t rawy = 0;

    rawx = Joystick1_ReadX();
    rawy = Joystick1_ReadY();

    rawx -= middle; // Want to see if the joystick is in the deadspace so subtract the middle number for when it's at rest
    rawy -= middle;

    if (((rawx * rawx) + (rawy * rawy)) <= (deadspace * deadspace)) return true;
    else return false;
}

bool CheckDeadZoneJ2 (){

    int16_t rawx = 0;
    int16_t rawy = 0;

    rawx = Joystick2_ReadX();
    rawy = Joystick2_ReadY();

    rawx -= middle; // Want to see if the joystick is in the deadspace so subtract the middle number for when it's at rest
    rawy -= middle;

    if (((rawx * rawx) + (rawy * rawy)) <= (deadspace * deadspace)) return true; // A radius of dead space will hopefully make the diagonals smoother
    else return false;

}

// If the input is between certain values, returns a velocity either 1 or -1 in the X or Y direction 

void J1VConvert(Pokemon* player){

    bool checker = CheckDeadZoneJ1(); 
    if (checker == true) {
        player->Changevx(0);
        player->Changevy(0);
        return;
    }

    int16_t rawX = Joystick1_ReadX();
    int16_t rawY = Joystick1_ReadY();

    rawX = rawX - middle; // Account for the dead space by subtracting it
    rawY = rawY - middle; // Same here 

    float angle = ((atan2(rawY, rawX)) * (180.0f / M_PI)); // atan returns the angle of coordinates from -pi to +pi, then we convert it to degrees by multiplying by 180/pi
    if (angle < 0) angle += 360.0f; // Add 360 if the angle goes negative

    int direction = (int)((angle + 11.25f) / 22.5f) % 16; // Want to map the angles to 16 different directions

    int8_t moveY = dx[direction];
    int8_t moveX = dy[direction];

    moveX = (moveX * -1);
    moveY = (moveY * -1);

    player->Changevx(0);
    player->Changevy(0);

    player->Changevx(moveX);  // Joystick Y controls vertical (up/down)
    player->Changevy(moveY);  // Joystick X controls horizontal (left/right)

}

void J2VConvert(Pokemon* player){

    bool checker = CheckDeadZoneJ2(); 
    if (checker == true) {
        player->Changevx(0);
        player->Changevy(0);
        return;
    }

    int16_t rawX = Joystick2_ReadX();
    int16_t rawY = Joystick2_ReadY();

    rawX = rawX - middle; // Account for the dead space by subtracting it
    rawY = rawY - middle; // Same here 

    float angle = ((atan2(rawY, rawX)) * (180.0f / M_PI)); // atan returns the angle of coordinates from -pi to +pi, then we convert it to degrees by multiplying by 180/pi
    if (angle < 0) angle += 360.0f; // Add 360 if the angle goes negative

    int direction = (int)((angle + 11.25f) / 22.5f) % 16; // Want to map the angles to 16 different directions

    int8_t moveX = dx[direction];
    int8_t moveY = dy[direction];

    player->Changevx(0);
    player->Changevy(0);

    player->Changevx(moveX);  // Joystick Y controls vertical (up/down)
    player->Changevy(moveY);  // Joystick X controls horizontal (left/right)

}

 bool CheckDeadZoneJV1 (){
    int16_t rawx = 0;
    int16_t rawy = 0;

    rawx = Joystick1_ReadX();
    rawy = Joystick1_ReadY();

    rawx -= middle; // Want to see if the joystick is in the deadspace so subtract the middle number for when it's at rest
    rawy -= middle;

    if (((rawx * rawx) + (rawy * rawy)) <= (deadspace3 * deadspace3)) return false;
    else return true;
}

 bool CheckDeadZoneJV2 (){

    int16_t rawx = 0;
    int16_t rawy = 0;

    rawx = Joystick2_ReadX();
    rawy = Joystick2_ReadY();

    rawx -= middle; // Want to see if the joystick is in the deadspace so subtract the middle number for when it's at rest
    rawy -= middle;

    if (((rawx * rawx) + (rawy * rawy)) <= (deadspace3 * deadspace3)) return false; // A radius of dead space will hopefully make the diagonals smoother
    else return true;

}

int8_t J1VOConvert(Outliner* player){

    bool checker = CheckDeadZoneJV1(); 
    if (checker == false) {
        return -1;
    }

    int16_t rawX = Joystick1_ReadX();
    int16_t rawY = Joystick1_ReadY();

    rawX = rawX - middle; // Account for the dead space by subtracting it
    rawY = rawY - middle; // Same here 

    float angle = ((atan2(rawY, rawX)) * (180.0f / M_PI)); // atan returns the angle of coordinates from -pi to +pi, then we convert it to degrees by multiplying by 180/pi
    if (angle < 0) angle += 360.0f; // Add 360 if the angle goes negative

    int direction = (int)((angle + 45.0f) / 90.0f) % 4; // Want to map the angles to 16 different directions

    return direction;


}

int8_t J2VOConvert(Outliner* player){

    bool checker = CheckDeadZoneJV2(); 
    if (checker == false) {
        return -1;
    }

    int16_t rawX = Joystick2_ReadX();
    int16_t rawY = Joystick2_ReadY();

    rawX = rawX - middle; // Account for the dead space by subtracting it
    rawY = rawY - middle; // Same here 

    float angle = ((atan2(rawY, rawX)) * (180.0f / M_PI)); // atan returns the angle of coordinates from -pi to +pi, then we convert it to degrees by multiplying by 180/pi
    if (angle < 0) angle += 360.0f; // Add 360 if the angle goes negative

    int direction = (int)((angle + 45.0f) / 90.0f) % 4; // Want to map the angles to 16 different directions

    if (direction == 0) direction = 1;
    else if (direction == 1) direction = 0;
    else if (direction == 2) direction = 3;
    else if (direction == 3) direction = 2;


    return direction;
}

void J1VGConvert(Pokemon* player, uint16_t Jx, uint16_t Jy){
    int16_t rawX = Jx;
    int16_t rawY = Jy;

    rawX = rawX - middle; // Account for the dead space by subtracting it
    rawY = rawY - middle; // Same here 

    if (((rawX * rawX) + (rawY * rawY)) <= (deadspace1 * deadspace1) && Jx > 15 && Jy > 15){             
        player->Changevx(0);
        player->Changevy(0);
        return;
    }; // A radius of dead space will hopefully make the diagonals smoother


    float angle = ((atan2(rawY, rawX)) * (180.0f / M_PI)); // atan returns the angle of coordinates from -pi to +pi, then we convert it to degrees by multiplying by 180/pi
    if (angle < 0) angle += 360.0f; // Add 360 if the angle goes negative

    int direction = (int)((angle + 11.25f) / 22.5f) % 16; // Want to map the angles to 16 different directions

    int8_t moveY = dx[direction];
    int8_t moveX = dy[direction];

    moveX = (moveX * -1);
    moveY = (moveY * -1);

    player->Changevx(0);
    player->Changevy(0);

    player->Changevx(moveX);  // Joystick Y controls vertical (up/down)
    player->Changevy(moveY);  // Joystick X controls horizontal (left/right)

}

void J2VGConvert(Pokemon* player, uint16_t Jx, uint16_t Jy){
    int16_t rawX = Jx;
    int16_t rawY = Jy;

    rawX = rawX - middle; // Account for the dead space by subtracting it
    rawY = rawY - middle; // Same here 

    if (((rawX * rawX) + (rawY * rawY)) <= (deadspace1 * deadspace1) && Jx > 15 && Jy > 15){             
        player->Changevx(0);
        player->Changevy(0);
        return;
    }; // A radius of dead space will hopefully make the diagonals smoother


    float angle = ((atan2(rawY, rawX)) * (180.0f / M_PI)); // atan returns the angle of coordinates from -pi to +pi, then we convert it to degrees by multiplying by 180/pi
    if (angle < 0) angle += 360.0f; // Add 360 if the angle goes negative

    int direction = (int)((angle + 11.25f) / 22.5f) % 16; // Want to map the angles to 16 different directions

    if (direction == 0) direction = 12;
    else if (direction == 1) direction = 13;
    else if (direction == 2) direction = 14;
    else if (direction == 3) direction = 15;
    else if (direction == 4) direction = 16;
    else {direction = direction - 4;}

    int8_t moveY = dx[direction];
    int8_t moveX = dy[direction];

    moveX = (moveX * -1);

    player->Changevx(0);
    player->Changevy(0);

    player->Changevx(moveX);  // Joystick Y controls vertical (up/down)
    player->Changevy(moveY);  // Joystick X controls horizontal (left/right)
}
    /*
        int16_t rawX = Jx;
        int16_t rawY = Jy;

        rawX = rawX - middle; // Account for the dead space by subtracting it
        rawY = rawY - middle; // Same here 

        if (((rawX * rawX) + (rawY * rawY)) <= (deadspace1 * deadspace1) && Jx > 15 && Jy > 15)

        float angler = ((atan2(rawY, rawX)) * (180.0f / M_PI)); // atan returns the angle of coordinates from -pi to +pi, then we convert it to degrees by multiplying by 180/pi
        
        if (angler < 0) angler += 360.0f; // Add 360 if the angle goes negative

        int direction = (int)((angler + 11.25f) / 22.5f) % 16; // Want to map the angles to 16 different directions

        int8_t moveX = dx[direction];
        int8_t moveY = dy[direction];*/

