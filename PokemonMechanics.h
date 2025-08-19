// Need to outline the characteristics and actions of each pokemon
// Copy and paste to make multiple 
#ifndef POKEMON_MECHANICS_H
#define POKEMON_MECHANICS_H

#define Topleft &Selectin[0]
#define Topright &Selectin[1]
#define Botleft &Selectin[2]
#define Botright &Selectin[3]

#define TopLeft 0
#define TopRight 1
#define BottomLeft 2
#define BottomRight 3

typedef enum {deader, aliver} pokestatus_t; // Make status a variable of either alive or dead 

struct State {
public:
  uint8_t ID;
  uint8_t x;
  uint8_t y; 
  const struct State *Next[4];};

  typedef const struct State State_t;

  State_t Selectin [4] = {  // -vy, vy, -vx, vx
    {TopLeft, 63, 68, {Topleft, Topleft, Botleft, Topright}}, // Topleft, Topleft, Topright, Botleft
    {TopRight, 63, 118, {Topright, Topleft, Botright, Topright}}, //Topleft, Topright, Topright, Botright
    {BottomLeft, 13, 68, {Topleft, Botleft, Botleft, Botright}}, //Botleft, Topleft, Botright, Botleft
    {BottomRight, 13, 118, {Topright, Botleft, Botright, Botright}} //Botleft, Topright, Botright, Botright
};

class Pokemon{
  public:
    int32_t x; // X coordinate
    int32_t y; // Y coordinate
    int8_t vx; // Velocity in the x direction in pixels/30Hz
    int8_t vy; // Velocity in the y direction in pixels/30Hz

    const unsigned short* images[4][2];
    int32_t width[4]; // The physical width of the pokemon itself 
    int32_t height[4]; // The physical height of the pokemon itself
    int32_t animationCounter;
    int8_t direction;

    bool front;
    unsigned short *background; // Pointer to the current background
    pokestatus_t life; // Dead or Alive

    int32_t needDraw; // True if need to draw 
    int32_t ID;
    int8_t ThisCount;
    bool Fire;
    bool specialUsed = false;
    bool canAttack;
    uint8_t type;

    const unsigned short *attacks;
    int32_t health;
    uint8_t attackSwitch;

    void PokeInit();
    void Attack(const unsigned short* attackSprite);
    void Move(uint16_t Jx, uint16_t Jy, Pokemon* opponent);
    bool IsOverlapping(Pokemon* opponent, int nextX, int nextY);
    void Dead();
    void Delete();
    void Special();
    void Draw();
    void Changevx(int8_t newvx);
    void Changevy(int8_t newvy);
    void updateProjectiles(Pokemon* opponent, Pokemon* itself);
    Pokemon(int32_t x, int32_t y, int32_t width[4], int32_t height[4], const unsigned short* imageSet[4][2], const unsigned short *attack, const unsigned short *background, int32_t health, int8_t vx, int8_t vy, int8_t ID, uint8_t attackSwitch, uint8_t type);
};

// const int8_t dx[16] = {0,  1,  1,  2,  2,   2,   1,   1,  0, -1, -1, -2, -2, -2, -1, -1};
// const int8_t dy[16] = {-2,  -2,  -1,  -1,  0,  1,  1,  2, 2, 2, 1, 1,  0,  -1,  -1,  -2};

class Outliner {
  public:
    int32_t x; // X coordinate
    int32_t y; // Y coordinate
    unsigned short *image; // Pointer to the image of the pokemon
    unsigned short *black; // Pointer to the blacked out image of the pokemon 
    int32_t width; // The physical width of the pokemon itself 
    int32_t height; // The physical height of the pokemon itself
    int8_t ID;
    const State_t *currentState;

    void Move();
    void Draw();
    void Delete();
    Outliner(int32_t x, int32_t y, int32_t width, int32_t height, const unsigned short *image, const unsigned short *black, uint8_t ID, const State_t *currentState);
};

class Projectile {
  public:
    int16_t x;
    int16_t y;        
    int8_t dx;
    int8_t dy;         
    bool active;
    static const int Sizer = 12;
    const unsigned short *sprite;

    void init(int16_t startX, int16_t startY, int8_t vX, int8_t vY, const unsigned short *bmp);
    void update(int8_t dxx, int8_t dyy, Pokemon* opponent, Pokemon* itself);
};

class Berry {
  public:
    int16_t x;
    int16_t y;        
    int16_t color;
       
    bool eaten;

    
    void spawnBerries(Pokemon* p1, Pokemon* p2);
    void updateBerry(Pokemon* itself, Pokemon* opponent);
    Berry(int32_t x, int32_t y, bool eaten);

};

#endif