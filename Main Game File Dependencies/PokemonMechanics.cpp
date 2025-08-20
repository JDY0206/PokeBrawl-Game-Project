/* 

PokemonMechanics

 */

#include <iostream>
#include "Characters.h"
#include "../inc/ST7735.h"
#include "Screens.h"
#include "PokemonMechanics.h"
#include "MovementMechanics.h"
#include <math.h>
#include "Projectiles.h"
#include "Switch.h"

#define Maxshots 5
#define COLLISION_BOX_WIDTH  28
#define COLLISION_BOX_HEIGHT 28

extern bool p1canAttack;
extern bool p2canAttack;

extern uint32_t p1AttackTimer; // Global time in milliseconds
extern uint32_t p2AttackTimer; // Global time in milliseconds


const int8_t dx3[16] = { 0,   1,   1,   2,  2,  2,  1,  1, 0, -1, -1, -2, -2, -2, -1, -1};
const int8_t dy3[16] = {-2,  -2,  -1,  -1,  0,  1,  1,  2, 2,  2,  1,  1,  0, -1, -1, -2};
const int8_t proj_dx[4] = { -2,  0,  2,  0 };
const int8_t proj_dy[4] = {  0, -2,  0,  2 };

Projectile shots[Maxshots];

int8_t shotspeeddx[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int8_t shotspeeddy[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//  ST7735_DrawBitmap(115, 49, Health, 7, 44); // 48 = too far left
//  ST7735_DrawBitmap(115, 155, Health, 7, 44);

// Health bars at 115,49 and 115, 155
/*static bool overlaps(int sx, int sy, int sw, int sh,
                     int bx, int by, int bw, int bh)
{
    return (sx < bx + bw) && (sx + sw > bx) &&
           (sy < by + bh) && (sy + sh > by);
}*/

void Pokemon::Move(uint16_t Jx, uint16_t Jy, Pokemon* opponent){
  int speedBoost = 1;
  if (ID == 1) J1VGConvert(this,  Jx,  Jy);
  if (ID == 2) J2VGConvert(this, Jx, Jy);
  if (vx == 0 && vy == 0) return; 
  if(this->specialUsed == true){
    speedBoost = 3;
  }
  vx *= 2 * speedBoost;
  vy *= 2 * speedBoost;
  int nextX = x + vx;
  int nextY = y + vy;
  
  // Boundary check
  if (width[direction] + nextX >= 103 || nextX <= 0 || (nextY - height[direction]) <= 0 || nextY >=160) return;
        
  // Check collision with opponent
  if (IsOverlapping(opponent, nextX, nextY)){
    if(this->health - 25 <= 0){
      this->health = 0;
      }else{
        this->health = this->health - 25;
      }

    nextX = x - vx;
    nextY = y - vy;
    if (width[direction] + nextX >= 103 || nextX <= 0 || (nextY - height[direction]) <= 0 || nextY >=160) return;
    if (this->ID == 1) {
      // P1 resets to position (60, 120)
      this->Delete();
      opponent->Delete();
      opponent->x = 70;
      opponent->y = 35;
      opponent->Draw();
      nextX = 70;
      nextY = 130;
    } else {
      // P2 resets to position (60, 35)
      this->Delete();
      opponent->Delete();
      opponent->x = 70;
      opponent->y = 130;
      opponent->Draw();
      nextX = 70;
      nextY = 35;
    }
    animationCounter++;

    if (abs(vx) > abs(vy)) {
      direction = (vx > 0) ? 3 : 0;
    } else {
      direction = (vy > 0) ? 2 : 1;
    }

    x = nextX;
    y = nextY;
      
    Draw();
    return;
  }
  animationCounter++;
  Delete();

  if (abs(vx) > abs(vy)) {
    direction = (vx > 0) ? 3 : 0;
  } else {
    direction = (vy > 0) ? 2 : 1;
  }

  x = nextX;
  y = nextY;

  Draw();
}
bool Pokemon::IsOverlapping(Pokemon* other, int nextX, int nextY) {
    if (this == other || other == nullptr) return false;

    const int separation = 2;  // Allow up to 1 pixel of edge contact
    // Center of this sprite
    int thisCenterX = nextX + width[direction] / 2;
    int thisCenterY = nextY - height[direction] / 2;
    int thisLeft    = thisCenterX - COLLISION_BOX_WIDTH / 2;
    int thisRight   = thisCenterX + COLLISION_BOX_WIDTH / 2;
    int thisTop     = thisCenterY - COLLISION_BOX_HEIGHT / 2;
    int thisBottom  = thisCenterY + COLLISION_BOX_HEIGHT / 2;

    // Center of the other sprite
    int otherCenterX = other->x + other->width[other->direction] / 2;
    int otherCenterY = other->y - other->height[other->direction] / 2;
    int otherLeft    = otherCenterX - COLLISION_BOX_WIDTH / 2;
    int otherRight   = otherCenterX + COLLISION_BOX_WIDTH / 2;
    int otherTop     = otherCenterY - COLLISION_BOX_HEIGHT / 2;
    int otherBottom  = otherCenterY + COLLISION_BOX_HEIGHT / 2;

    // Collision check with 1-pixel grace
    return !(thisRight - separation <= otherLeft ||
            thisLeft + separation >= otherRight ||
            thisBottom - separation <= otherTop ||
            thisTop + separation >= otherBottom);
}

void Pokemon::Draw(){
    int frame = (animationCounter) % 2;
    ST7735_DrawBitmap(x, y, images[direction][frame], width[direction], height[direction]);
}

void Pokemon::Special(){

}

void Pokemon::Dead(){

}

void Pokemon::Delete(){
    unsigned short backgroundCopy[width[direction] * height[direction]];
        for (int j = 0; j < height[direction]; j++) {
            for (int i = 0; i < width[direction]; i++) {
                backgroundCopy[j * width[direction] + i] = 0xFFFF;
            }
        }
        ST7735_DrawBitmap(x, y, backgroundCopy, width[direction], height[direction]);
}

void Pokemon::Changevx(int8_t newvx){
    vx = newvx;
}

void Pokemon::Changevy(int8_t newvy){
    vy = newvy;
}


void Pokemon::Attack(const unsigned short* attackSprite){ 
    // Leftmost Switch = 1
    // OuterLeft Switch = 2
    // Rightmost Switch = 8
    // InnerRight = 4
    // Have the global direction 

    int spawnX = 0;
    int spawnY = 0;
    int switcher = Switch_In();
    if (switcher != attackSwitch && switcher != 6) return;
    if (ID == 1) J1VConvert(this);
    if (ID == 2) J2VConvert(this);

    // find an inactive shot
    
    for(int i = 0; i < Maxshots; i ++) {
        if(this->canAttack == true){
            if (!shots[i].active) {
            // compute spawn point 4px away from the sprite’s center
                int x = 0;
                int y = 0;
                int dx = 0;
                int dy = 0;
                x = this->x + width[direction]/2;
                y = this->y + height[direction]/2;
                if (direction == 2) { // right
                    dx = 0;
                    dy = 4;
                    spawnX = x;
                    spawnY = y + 4;
                } else if (direction == 0) { // down
                    dx = -4;
                    dy = 0;
                    spawnX = x - width[direction] + width[direction]/4;
                    spawnY = y - height[direction]/4 - height[direction]/4;
                } else if (direction == 3) { // up
                    dx = 4;
                    dy = 0;
                    spawnX = x + width[direction] - width[direction]/4;
                    spawnY = y - height[direction]/4 - height[direction]/4;
                } else if (direction == 1) { // left
                    dx = 0;
                    dy = -4;
                    spawnX = x;
                    spawnY = y - height[direction] - height[direction]/2;
                }
                shots[i].init(spawnX, spawnY, vx, vy, attackSprite);
                shotspeeddx[i] = dx;
                shotspeeddy[i] = dy;       
            }
            this->canAttack = false;
            if(this->ID == 2){
                p2AttackTimer = 0;
            }
            if(this->ID == 1){
                p1AttackTimer = 0;
            }
        }
    }
}

    

Pokemon::Pokemon(int32_t posx, int32_t posy, int32_t Awidth[4], int32_t Aheight[4], const unsigned short* imageSet[4][2], const unsigned short *attack, const unsigned short *Abackground, int32_t Ahealth, int8_t vx, int8_t vy, int8_t AID, uint8_t AattackSwitch, uint8_t aType){

    x = posx;
    y = posy;
    direction = 0;
    for(int i = 0; i < 4; i++){
        width[i] = Awidth[i];
        height[i] = Aheight[i];
    }
    front = true;
    for (int d = 0; d < 4; d++) {
        for (int f = 0; f < 2; f++) {
            images[d][f] = imageSet[d][f];
        }
    }
    attacks = (unsigned short*)attack;
    animationCounter = 0;
    background = (unsigned short*)Abackground;
    health = Ahealth;
    ID = AID;
    attackSwitch = AattackSwitch;
    specialUsed = false;
    type = aType;
}

void Outliner::Move(){
    int thing = 0;
    int holder = 0;

    if (ID == 1){
        holder = J1VOConvert(this);
        if (holder != -1){
            thing = holder;
        }
    }

    if (ID == 2){
        holder = J2VOConvert(this);
        if (holder != -1){
            thing = holder;
        }
    }

    if (holder != -1){
        State_t* previousState = currentState;
        currentState = currentState->Next[thing];

        if (previousState != currentState){
            Delete();

            x = currentState->x;
            y = currentState->y;

            Draw(); 

            ST7735_DrawBitmap(15, 115, CFront1, 27, 17); // ID = 3
            ST7735_DrawBitmap(15, 65, SFront1, 26, 18); // ID = 2
            ST7735_DrawBitmap(65, 115, PFront1, 27, 23); // ID = 1
            ST7735_DrawBitmap(65, 65, BFront1, 26, 24); // ID = 0
        }
    }     
}

void Pokemon::updateProjectiles(Pokemon* opponent, Pokemon* itself){
    int8_t tempdx = 0;
    int8_t tempdy = 0;
    for (int i = 0; i < Maxshots; ++i){
        tempdx = shotspeeddx[i];
        tempdy = shotspeeddy[i];
        shots[i].update( tempdx,  tempdy, opponent, itself);  // <- this calls the Projectile::update() function
    }
}

void Outliner::Draw(){
    ST7735_DrawBitmap(x, y, image, width, height);
}

void Outliner::Delete(){
    ST7735_DrawBitmap(x, y, black, width, height);
}

Outliner::Outliner(int32_t posx, int32_t posy, int32_t Awidth, int32_t Aheight, const unsigned short *Aimage, const unsigned short *Ablack, uint8_t AID, const State_t *ScurrentState){
    x = posx;
    y = posy;
    width = Awidth;
    height = Aheight;
    image = (unsigned short*)Aimage;
    black = (unsigned short*)Ablack;
    ID = AID;
    currentState = ScurrentState;
}

void Projectile::init(int16_t startX, int16_t startY, int8_t vX, int8_t vY, const unsigned short *bmp){
    x  = startX - Sizer/2;
    y  = startY - Sizer/2;
    dx = vX;
    dy = vY;
    sprite = bmp;
    active = true;
}

void Projectile::update(int8_t dxx, int8_t dyy, Pokemon* opponent, Pokemon* itself) {
    // Erase current position
    ST7735_DrawBitmap(x, y, backgroundShot, Sizer, Sizer);

    // Move
    x += dxx;
    y += dyy;

    // Off-screen check
    if (x < 0 || x + Sizer >= 103 || y < 0 || y + Sizer >= 160) {
        active = false;
        return;
    }

    // Collision check with opponent
    if (opponent != nullptr && opponent->health > 0) {
        int projLeft   = x;
        int projRight  = x + Sizer;
        int projTop    = y - Sizer;
        int projBottom = y;

        int oppLeft   = opponent->x;
        int oppRight  = opponent->x + opponent->width[opponent->direction];
        int oppTop    = opponent->y - opponent->height[opponent->direction];
        int oppBottom = opponent->y;
        int healthLoss = 10;
        if(itself->specialUsed == true){
            healthLoss = 20;
        }
        if (!(projRight <= oppLeft || projLeft >= oppRight ||
            projBottom <= oppTop || projTop >= oppBottom)) {
            // Collision detected
            if(active == true){
                
                if(opponent->health - healthLoss <= 0){
                    opponent->health = 0;
                }else{
                    opponent->health -= healthLoss; // or more damage

                }
            }
            
            active = false;

            for (int i = 0; i < Maxshots; ++i) {
                shots[i].active = false;  // Deactivate all projectiles
                ST7735_DrawBitmap(shots[i].x, shots[i].y, backgroundShot, Sizer, Sizer);  // Clear projectiles 
            }
            opponent->Delete();
            if(opponent->ID == 2){
                opponent->x = 60;
                opponent->y = 35;
            }else{
                opponent->x = 60;
                opponent->y = 120;
            }
            opponent->Draw();
        }
    }

    // Draw new position
    if(active){
        ST7735_DrawBitmap(x, y, sprite, Sizer, Sizer);
        itself->canAttack = false; 
    }
}
uint32_t R=1;
uint32_t Random32again(void){
  R = 1664525*R+1013904223;
  return R;
}
uint32_t Randomagain(uint32_t n){
  return (Random32again()>>16)%n;
}



bool isBerryOverlappingPokemon(int bx, int by, Pokemon* poke) {
    if (!poke) return false;

    int berryLeft = bx;
    int berryRight = bx + 7;
    int berryTop = by - 7;
    int berryBottom = by;

    int pokeCenterX = poke->x + poke->width[poke->direction] / 2;
    int pokeCenterY = poke->y - poke->height[poke->direction] / 2;
    int pokeLeft = pokeCenterX - COLLISION_BOX_WIDTH / 2;
    int pokeRight = pokeCenterX + COLLISION_BOX_WIDTH / 2;
    int pokeTop = pokeCenterY - COLLISION_BOX_HEIGHT / 2;
    int pokeBottom = pokeCenterY + COLLISION_BOX_HEIGHT / 2;

    return !(berryRight <= pokeLeft ||
             berryLeft >= pokeRight ||
             berryBottom <= pokeTop ||
             berryTop >= pokeBottom);
}
void Berry::spawnBerries(Pokemon* p1, Pokemon* p2) {
    bool overlap = true;

    while (overlap) {
        // Generate random coordinates
        this->x = Randomagain(91)+5;   // 0 to 100
        this->y = Randomagain(151)+5;   // 0 to 160

        // Check overlap using helper function
        overlap = isBerryOverlappingPokemon(this->x, this->y, p1) ||
                  isBerryOverlappingPokemon(this->x, this->y, p2);
    }

    // Draw the berry at a safe location
    this->color = Randomagain(10); 
    if(this->color <= 2){
        ST7735_FillRect(x, y, 5, 5, ST7735_BLUE);
    }else if(this->color <= 4){
        ST7735_FillRect(x, y, 5, 5, ST7735_GREEN);
    }else if(this->color <= 6){
        ST7735_FillRect(x, y, 5, 5, ST7735_RED);
    }else if(this->color == 8){
        ST7735_FillRect(x, y, 5, 5, ST7735_PURPLE);
    }else{
        ST7735_FillRect(x, y, 5, 5, ST7735_BLACK); 
    }
}

void Berry::updateBerry(Pokemon* p1, Pokemon* p2) {
    if (eaten) return; // already consumed

    int berryLeft = x;
    int berryRight = x + 7;
    int berryTop = y - 7;
    int berryBottom = y;

    bool hitP1 = isBerryOverlappingPokemon(x, y, p1);
    bool hitP2 = isBerryOverlappingPokemon(x, y, p2);

    if (hitP1 || hitP2) {
        ST7735_FillRect(x, y, 5, 5, ST7735_WHITE);
        eaten = true;

        int healthEffect = 10;  // fixed health penalty

        // Apply berry effect: damage the player who touched it
        if(this->color == 0){
            healthEffect = 50;
        }
        else if(this->color <= 2) {
            healthEffect = 5;
        } else if (this->color <= 4) {
            healthEffect = 10;
        } else if (this->color <= 6) {
            healthEffect = -10;
        } else if (this->color == 8) {
            healthEffect =  15;
        } else {
            healthEffect = -20;
        }

        if (hitP1) {
            if (p1->health > healthEffect) p1->health += healthEffect;
            else p1->health = 0;
        }
        if (hitP2) {
            if (p2->health > healthEffect) p2->health += healthEffect;
            else p2->health = 0;
        }

        // Respawn berry at safe location
        bool overlap = true;
        while (overlap) {
            x = Randomagain(91) + 5;   // ensures berry stays inside bounds (x: 5–95)
            y = Randomagain(151) + 5;  // ensures y: 5–155

            berryLeft = x;
            berryRight = x + 5;
            berryTop = y - 5;
            berryBottom = y;

            overlap = isBerryOverlappingPokemon(x, y, p1) || isBerryOverlappingPokemon(x, y, p2);
        }

        // Assign random color and redraw berry
        this->color = Randomagain(10); 
        if(this->color == 0){
            ST7735_FillRect(x, y, 5, 5, ST7735_YELLOW);
        }
        else if(this->color <= 2) {
            ST7735_FillRect(x, y, 5, 5, ST7735_BLUE);
        } else if (this->color <= 4) {
            ST7735_FillRect(x, y, 5, 5, ST7735_GREEN);
        } else if (this->color <= 6) {
            ST7735_FillRect(x, y, 5, 5, ST7735_RED);
        } else if (this->color == 8) {
            ST7735_FillRect(x, y, 5, 5, ST7735_PURPLE);
        } else {
            ST7735_FillRect(x, y, 5, 5, ST7735_BLACK);
        }

        eaten = false;  // berry becomes active again
    }
}




Berry::Berry(int32_t xVal, int32_t yVal, bool isEaten) {
    x = xVal;
    y = yVal;
    eaten = isEaten;
}