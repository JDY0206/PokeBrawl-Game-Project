// Lab9HMain.cpp
// Runs on MSPM0G3507
// Lab 9 ECE319H
// Your name
// Last Modified: 12/26/2024
#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "Sound.h"
#include "images/images.h"

#include "../inc/LaunchPad.h"
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#include "MyADC.h"
#include "MyADC.c"
#include "Screens.h"
#include "Characters.h"
#include "OurJoystick.c"
#include "MovementMechanics.h"
#include "UI.h"
#include "Projectiles.h"
#include "Sounder.h"

uint32_t Flag = 0; // 1 means new data
uint32_t G12Count = 0;

uint32_t p1AttackTimer = 0; // Global time in milliseconds
uint32_t p1AttackTicks = 0; // Global time in milliseconds
bool p1canAttack = true;

uint32_t p2AttackTimer = 0; // Global time in milliseconds
uint32_t p2AttackTicks = 0; // Global time in milliseconds
bool p2canAttack = true;

int32_t p1SpecialTimer = 0;
int32_t p2SpecialTimer = 0;
bool p1SpecialCountdown = false;
bool p2SpecialCountdown = false;

int32_t LEDtimer = 0;



uint32_t currentInput = 0; // Global time in milliseconds


const uint8_t SinWave[32] = {
  16,19,22,24,27,28,30,31,31,31,30,
  28,27,24,22,19,16,13,10,8,5,4,
  2,1,1,1,2,4,5,8,10,13};

const unsigned short* bulbaFrames[4][2] = {
  {BFront1, BFront3},
  {BLeft1, BLeft3},
  {BRight1, BRight3},
  {BBack1, BBack3}};
int32_t bulbaFramesWidth[4] = {26, 24, 24, 26};
int32_t bulbaFramesHeight[4] = {24, 32, 32, 22};

const unsigned short* squirtleFrames[4][2] = {
  {SFront1, SFront3},
  {SLeft1, SLeft3},
  {SRight1, SRight3},
  {SBack1, SBack3}};
int32_t squirtleFramesWidth[4] = {26, 25, 25, 26};
int32_t squirtleFramesHeight[4] = {18, 25, 25, 18};

const unsigned short* charmanderFrames[4][2] = {
  {CFront1, CFront3},
  {CLeft1, CLeft3},
  {CRight1, CRight3},
  {CBack1, CBack3}};
int32_t charmanderFramesWidth[4] = {27, 24, 24, 28};
int32_t charmanderFramesHeight[4] = {17, 28, 28, 16};

const unsigned short* pikachuFrames[4][2] = {
  {PFront1, PFront3},
  {PLeft1, PLeft3},
  {PRight1, PRight3},
  {PBack1, PBack3}};
int32_t pikachuFramesWidth[4] = {27, 25, 25, 26};
int32_t pikachuFramesHeight[4] = {23, 25, 25, 24};

Pokemon P1(0, 0, bulbaFramesWidth, bulbaFramesHeight, bulbaFrames, leaf1, TestBackground1, 100, 0, 0, 1, 0, 0);
Pokemon P2(0, 0, bulbaFramesWidth, bulbaFramesHeight, bulbaFrames, leaf1, TestBackground1, 100, 0, 0, 1, 0, 0);

Pokemon* P1P = &P1;
Pokemon* P2P = &P2;



uint16_t J1X = 0;
uint16_t J1Y = 0;
uint16_t J2X = 0;
uint16_t J2Y = 0;
uint8_t Switch = 0;

extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);

// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t M=1;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

SlidePot Sensor(1500,0); // copy calibration from Lab 7

// Initalize TimerG12 
// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    // game engine goes here
        // 1) sample slide pot
        // 2) read input switches
        // 3) move sprites
        // 4) start sounds
        // 5) set semaphore
        // NO LCD OUTPUT IN INTERRUPT SERVICE ROUTINES
    G12Count ++; // 30 for each second 

    // Read the joysticks and put the appropriate value into the object
    // Moves the sprites but does not actual printing 
    J1X = Joystick1_ReadX();
    J1Y = Joystick1_ReadY();
    J2X = Joystick2_ReadX();
    J2Y = Joystick2_ReadY();

    // Sample the switches 
    // Need to write a function that converts the switch values to a projectile or a special with each tied to the class 
    Switch = Switch_In();

    // Set the semaphore 
    Flag = 1;

    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)

    LEDtimer += 1;
    if(LEDtimer > 15){
      LEDtimer = 0;
    }
    p1AttackTicks++;
    p2AttackTicks++;
    if(p1SpecialCountdown == true){
      p1SpecialTimer--;
      if(p1SpecialTimer <= 0){
        p1SpecialTimer = 0;
      }
    }
    if(p2SpecialCountdown == true){
      p2SpecialTimer--;
      if(p2SpecialTimer <= 0){
        p2SpecialTimer = 0;
      }
    }
  }
}

uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};

int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speeds
  LaunchPad_Init();
  ST7735_InitPrintf();
  //note: if you colors are weird, see different options for
  // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  Sensor.Init(); // PB18 = ADC1 channel 5, slidepot
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
  Joystick_Init(); // Initialize Joysticks
  // initialize interrupts on TimerG12 at 30 Hz:: Going to initialize before the primary game 
  TimerG12_IntArm(2666666, 2);

  
  // initialize all data structures
  // Need to set up 4 copies of each available character in case all 4 players select the same character
  uint8_t CharmanderCount = 0;
  uint8_t SquirtleCount = 0;
  uint8_t PikachuCount = 0;
  uint8_t BulbasaurCount = 0;
  uint8_t Mover = 0;

  uint8_t language = 0;
  uint16_t xpos = 0;
  uint16_t ypos = 0;


  __enable_irq();

  // Pokemon( x,  y,  width,  height, *image, *black,  health,  vx,  vy,  ID);

  for (int i = 0; i < 8; i ++){
    for (int j = 0; j < 11; j ++){
      if (i == 0 || i == 4){
        ST7735_DrawBitmap(xpos, ypos, CFront1, 27, 17);
        ypos += 17;
      }
      if (i == 1 || i == 5){
        ST7735_DrawBitmap(xpos, ypos, SFront1, 26, 18);
        ypos += 18;
      }
      if (i == 2 || i == 6){
        ST7735_DrawBitmap(xpos, ypos, PFront1, 27, 23);
        ypos += 23;
      }
      if (i == 3 || i == 7){
        ST7735_DrawBitmap(xpos, ypos, BFront1, 26, 24);
        ypos += 24;
      }
    }
    ypos = 0;
    xpos += 26;
  }
  xpos = 0;
  ypos = 0;

  // Start Screen
  ST7735_DrawBitmap(40, 115, Sat, 60, 75);
  ST7735_DrawBitmap(0, 115, Press, 30, 75);
  while (Mover == 0){
    if (Switch_In() != 0) {
      Mover = 1;
    }
  }

// Need a language select screen
  ST7735_FillScreen(ST7735_BLACK);
  Mover = 0;
  ST7735_SetRotation(1);
  ST7735_OutString("ENGLISH OR SPANISH \n\r");
  ST7735_OutString("Left 2 buttons for \n\r");
  ST7735_OutString("English \n\r");
  ST7735_OutString("Dos botones derechos\n\r");
  ST7735_OutString("para Espanol \n\r");

  while (Mover == 0){
    if (Switch_In() == 1 || Switch_In() == 2){
      language = 0;
      Mover = 1;
    }
    if (Switch_In() == 4 || Switch_In() == 8){
      language = 1;
      Mover = 1;
    }
  }  

  ST7735_FillScreen(ST7735_BLACK);
  Mover = 0;
  if (language == 0){
    ST7735_SetRotation(1);
    ST7735_SetCursor(0, 0);
    ST7735_OutString("Instructions: ");
    ST7735_SetCursor(0, 1);
    ST7735_OutString("Joystick to move ");
    ST7735_SetCursor(0, 2);
    ST7735_OutString("Inner button to ");
    ST7735_SetCursor(0, 3);
    ST7735_OutString("attack (3s)");
    ST7735_SetCursor(0, 4);
    ST7735_OutString("Outer button for");
    ST7735_SetCursor(0, 5);
    ST7735_OutString("speed boost (1)");
    ST7735_SetCursor(0, 6);
    ST7735_OutString("Collisions cause");
    ST7735_SetCursor(0, 7);
    ST7735_OutString("Health loss");
    ST7735_SetCursor(0, 8);
    ST7735_OutString("Any button for next");
    ST7735_SetCursor(0, 9);

    Clock_Delay(80000000); // 1 seconds 

    while (Mover == 0){
      if (Switch_In() != 0) {
        Mover = 1;
      }
    }
    ST7735_FillScreen(ST7735_BLACK);

    Mover = 0;
    ST7735_SetCursor(0, 0);
    ST7735_OutString("Collecting a berry");
    ST7735_SetCursor(0, 1);
    ST7735_OutString("will do something");
    ST7735_SetCursor(0, 2);
    ST7735_OutString("based on its color");
    ST7735_SetCursor(0, 3);
    ST7735_OutString("Red: -10 HP");
    ST7735_SetCursor(0, 4);
    ST7735_OutString("Black: -20 HP");
    ST7735_SetCursor(0, 5);
    ST7735_OutString("Blue: +5 HP");
    ST7735_SetCursor(0, 6);
    ST7735_OutString("Green: +10 HP");
    ST7735_SetCursor(0, 7);
    ST7735_OutString("Purple: +15 HP");
    ST7735_SetCursor(0, 8);
    ST7735_OutString("Yellow: ???");
    ST7735_SetCursor(0, 9);
    ST7735_OutString("Any button for next");

    Clock_Delay(80000000); // 5 seconds 

    while (Mover == 0){
      if (Switch_In() != 0) {
        Mover = 1;
      }
    }
  }


  if (language == 1){
    ST7735_SetRotation(1);
    ST7735_SetCursor(0, 0);
    ST7735_OutString("Instrucciones:");
    ST7735_SetCursor(0, 1);
    ST7735_OutString("Palanca de mando ");
    ST7735_SetCursor(0, 2);
    ST7735_OutString(" para moverse");
    ST7735_SetCursor(0, 3);
    ST7735_OutString("Boton interior para ");
    ST7735_SetCursor(0, 4);
    ST7735_OutString("ataque (3s)");
    ST7735_SetCursor(0, 5);
    ST7735_OutString("Boton exterio para");
    ST7735_SetCursor(0, 6);
    ST7735_OutString("aumento develocidad");
    ST7735_SetCursor(0, 7);
    ST7735_OutString("Causa de colisiones");
    ST7735_SetCursor(0, 8);
    ST7735_OutString("Perdida de salud");
    ST7735_SetCursor(0, 9);

    Clock_Delay(80000000); // 5 seconds 

    while (Mover == 0){
      if (Switch_In() != 0) {
        Mover = 1;
      }
    }
    ST7735_FillScreen(ST7735_BLACK);

    Mover = 0;
    ST7735_SetCursor(0, 0);
    ST7735_OutString("Recogiendo una baya");
    ST7735_SetCursor(0, 1);
    ST7735_OutString("hara algo");
    ST7735_SetCursor(0, 2);
    ST7735_OutString("segun su color");
    ST7735_SetCursor(0, 3);
    ST7735_OutString("Roja: -10 HP");
    ST7735_SetCursor(0, 4);
    ST7735_OutString("Negro: -20 HP");
    ST7735_SetCursor(0, 5);
    ST7735_OutString("Azul: +5 HP");
    ST7735_SetCursor(0, 6);
    ST7735_OutString("Verde: +10 HP");
    ST7735_SetCursor(0, 7);
    ST7735_OutString("Purpura: +15 HP");
    ST7735_SetCursor(0, 8);
    ST7735_OutString("Cualquer boton para");
    ST7735_SetCursor(0, 9);
    ST7735_OutString("el siguiente");

    Clock_Delay(80000000); // 5 seconds 

    while (Mover == 0){
      if (Switch_In() != 0) {
        Mover = 1;
      }
    }
  }



  // Selection Screen
  ST7735_SetRotation(0);
  ST7735_FillScreen(ST7735_BLACK);
  Mover = 0;

  for (int i = 0; i < 8; i ++){
    for (int j = 0; j < 11; j ++){
      ST7735_DrawBitmap(xpos, ypos, Select, 16, 16);
      ypos += 16;
     }
     ypos = 0;
     xpos += 16;
  }
  if (language == 1){
    ST7735_SetRotation(1);
    ST7735_SetCursor(2, 1);
    ST7735_OutString("Elige tu personaje\n\r");
    ST7735_SetRotation(0);
  }

  // Wait for players to select a character

  bool p1select = false;
  bool p2select = false;

  uint8_t p1char = 0;
  uint8_t p2char = 0;

  Outliner p1(13, 118, 29, 26, Outline, Boutline, 1, Botright);
  Outliner p2(63, 68, 29, 26, Outline2, Boutline, 2, Topleft);
  p1.Draw();
  p2.Draw();

  if (language == 0){
    ST7735_DrawBitmap(100, 120, Choose, 30, 85);
  }
  ST7735_DrawBitmap(15, 115, CFront1, 27, 17);
  ST7735_DrawBitmap(15, 65, SFront1, 26, 18);
  ST7735_DrawBitmap(65, 115, PFront1, 27, 23);
  ST7735_DrawBitmap(65, 65, BFront1, 26, 24);

  while (Mover == 0){
    if (p1select == false){
      Clock_Delay(8000000);
      p1.Move();
    }

    if (p2select == false){
      Clock_Delay(8000000);
      p2.Move();
    }
    if (p1select == true){
      p1.Delete();
      p1.Draw(); 
      ST7735_DrawBitmap(15, 115, CFront1, 27, 17);
      ST7735_DrawBitmap(15, 65, SFront1, 26, 18);
      ST7735_DrawBitmap(65, 115, PFront1, 27, 23);
      ST7735_DrawBitmap(65, 65, BFront1, 26, 24);
    }

    if (p2select == true){
      p2.Delete();
      p2.Draw(); 
      ST7735_DrawBitmap(15, 115, CFront1, 27, 17);
      ST7735_DrawBitmap(15, 65, SFront1, 26, 18);
      ST7735_DrawBitmap(65, 115, PFront1, 27, 23);
      ST7735_DrawBitmap(65, 65, BFront1, 26, 24);
    }

    Clock_Delay(8000000);

    if (Switch_In() == 1 || Switch_In() == 2){
      p2select = true;
    }
  
    Clock_Delay(8000000);

    if (Switch_In() == 4 || Switch_In() == 8){
      p1select = true;
    }

    if (p1select == true && p2select == true){
      Mover = 1;
      p1char = p1.currentState->ID;
      p2char = p2.currentState->ID;
      Clock_Delay(160000000);
    }
  }

  ST7735_FillScreen(ST7735_BLACK);


  // Initialize the proper pokemon: only difference between each are the image and the black image 

  if (p1char == 3){ // Charmander 
    P1 = Pokemon(60, 120, charmanderFramesWidth, charmanderFramesHeight, charmanderFrames, fireball1, TestBackground1, 100, 0, 0, 1, 4, 1);
  }
  if (p2char == 3){ // Charmander 
    P2 = Pokemon(60, 35, charmanderFramesWidth, charmanderFramesHeight, charmanderFrames, fireball1, TestBackground1, 100, 0, 0, 2, 2, 1);
  }

  if (p1char == 2){ // Squirtle
    P1 = Pokemon(60, 120, squirtleFramesWidth, squirtleFramesHeight, squirtleFrames, bubble1, TestBackground1, 100, 0, 0, 1, 4, 2);
  }
  if (p2char == 2){ // Squirtle
   P2 = Pokemon(60, 35, squirtleFramesWidth, squirtleFramesHeight, squirtleFrames, bubble1, TestBackground1, 100, 0, 0, 2, 2, 2);
  }

  if (p1char == 1){ // Pikachu
    P1 = Pokemon(60, 120, pikachuFramesWidth, pikachuFramesHeight, pikachuFrames, lightning1, TestBackground1, 100, 0, 0, 1, 4, 4);
  }
  if (p2char == 1){ // Pikachu
    P2 = Pokemon(60, 35, pikachuFramesWidth, pikachuFramesHeight, pikachuFrames, lightning1, TestBackground1, 100, 0, 0, 2, 2, 4);
  } 

  if (p1char == 0){ // Bulbasaur
    P1 = Pokemon(60, 120, bulbaFramesWidth, bulbaFramesHeight, bulbaFrames, leaf1, TestBackground1, 100, 0, 0, 1, 4, 3);
  }
  if (p2char == 0){ // Bulbasaur
    P2 = Pokemon(60, 35, bulbaFramesWidth, bulbaFramesHeight, bulbaFrames, leaf1, TestBackground1, 100, 0, 0, 2, 2, 3);
  }


  ST7735_FillScreen(ST7735_WHITE);

  // 1 is char, 2 is squir, 3 is bulb, 4 is pika
  //P1 = Pokemon(60, 120, pikachuFramesWidth, pikachuFramesHeight, pikachuFrames, lightning1, TestBackground1, 100, 0, 0, 1, 4, 4);
  //P2 = Pokemon(60, 35, bulbaFramesWidth, bulbaFramesHeight, bulbaFrames, leaf1, TestBackground1, 100, 0, 0, 2, 2, 3);

  P1.Draw();
  P2.Draw();

  // Timer G12 are the hardware checks 
  // ISR has the joysick reader and the switch reader
    // TimerG12_IntArm2(3,1); // Period = 3 = 26.67 MHz, Priority = 1 / 3 ( Lowest is highest priority)
  // Hardcode the sounds to each thing and just output to the DAC

  // Systick is really only the DAC. Use the Systick for sounds that are regularly emitted

  // Actual Gameplay
  // Turn on TimerG12 here 
  uint32_t currentInput = Switch_In();
  uint32_t lastInput = 0;
  uint8_t sw = 0;
  p1AttackTimer = 100;
  p2AttackTimer = 100;
  p1SpecialTimer = 300;
  p2SpecialTimer = 300;

  bool endScreen = false;
  // spawning berries
  int32_t randX = Random(101);   // 0 to 100 inclusive
  int32_t randY = Random(161);   // 0 to 160 inclusive
  Berry B1 = Berry(randX, randY, false);
  B1.spawnBerries(&P1, &P2);

  randX = Random(101);   // 0 to 100 inclusive
  randY = Random(161);   // 0 to 160 inclusive
  Berry B2 = Berry(randX, randY, false);
  B2.spawnBerries(&P1, &P2);

  randX = Random(101);   // 0 to 100 inclusive
  randY = Random(161);   // 0 to 160 inclusive
  Berry B3 = Berry(randX, randY, false);
  B3.spawnBerries(&P1, &P2);

  randX = Random(101);   // 0 to 100 inclusive
  randY = Random(161);   // 0 to 160 inclusive
  Berry B4 = Berry(randX, randY, false);
  B4.spawnBerries(&P1, &P2);

  randX = Random(101);   // 0 to 100 inclusive
  randY = Random(161);   // 0 to 160 inclusive
  Berry B5 = Berry(randX, randY, false);
  B5.spawnBerries(&P1, &P2);

  while(!endScreen){
    if (Flag == 1){
      Flag = 0;
      if (G12Count == 30){// Increment Special Bar 
      }

    
      ST7735_SetRotation(0);

      sw = Switch;
      P1.Attack(P1.attacks);             // may spawn a shot
      P1.updateProjectiles(&P2, &P1);    // moves & draws all active shots
      P1.Move(J1X, J1Y, &P2);  
      Clock_Delay(4000000);

      P2.Attack(P2.attacks);             // may spawn a shot
      P2.updateProjectiles(&P1, &P2);    // moves & draws all active shots
      P2.Move(J2X, J2Y, &P1);  
      Clock_Delay(4000000);

      if(P2.health == 0){
        ST7735_SetRotation(1);
        ST7735_FillScreen(ST7735_WHITE);
        ST7735_SetTextColor(ST7735_BLACK);
        if(language == 1){
          ST7735_SetCursor(2, 6);  // Next row down for P2
          ST7735_OutString("JUEGO TERMINADO\n");
          ST7735_SetCursor(2, 7);  // Next row down for P2
          ST7735_OutString("GANA P2");
          ST7735_SetCursor(2, 8);  // Next row down for P2
          ST7735_OutString("PUNTAJE: ");
          ST7735_OutUDec(P1.health-P2.health);
        }else{
          ST7735_SetCursor(6, 6);  // Next row down for P2
          ST7735_OutString("GAME OVER\n");
          ST7735_SetCursor(6, 7);  // Next row down for P2
          ST7735_OutString("P2 WINS");
          ST7735_SetCursor(6, 8);  // Next row down for P2
          ST7735_OutString("SCORE: ");
          ST7735_OutUDec(P1.health-P2.health);
        }
        endScreen = true;
        ST7735_SetRotation(0);
        P1.x = 50;
        P1.y = 130;
        P1.direction =0;
        ST7735_DrawBitmap(75, 130, crown, 19, 25);
        P1.Draw();
        LED_On(PB23INDEX);
        LED_On(PB24INDEX);
        LED_On(PA24INDEX); 
      }else if(P1.health == 0){
        ST7735_SetRotation(1);
        ST7735_FillScreen(ST7735_WHITE);
        ST7735_SetTextColor(ST7735_BLACK);
        if(language == 1){
          ST7735_SetCursor(2, 6);  // Next row down for P2
          ST7735_OutString("JUEGO TERMINADO\n");
          ST7735_SetCursor(2, 7);  // Next row down for P2
          ST7735_OutString("GANA P1");
          ST7735_SetCursor(2, 8);  // Next row down for P2
          ST7735_OutString("PUNTAJE: ");
          ST7735_OutUDec(P2.health - P1.health);
        }else{
          ST7735_SetCursor(6, 6);  // Next row down for P2
          ST7735_OutString("GAME OVER\n");
          ST7735_SetCursor(6, 7);  // Next row down for P2
          ST7735_OutString("P1 WINS");
          ST7735_SetCursor(6, 8);  // Next row down for P2
          ST7735_OutString("SCORE: ");
          ST7735_OutUDec(P2.health - P1.health);
        }
        endScreen = true;
        ST7735_SetRotation(0);
        P2.x = 50;
        P2.y = 130;
        P2.direction = 0;
        ST7735_DrawBitmap(75, 130, crown, 19, 25);
        P2.Draw();
        LED_On(PB23INDEX);
        LED_On(PB24INDEX);
        LED_On(PA24INDEX); 
      }

      
      B1.updateBerry(&P1, &P2);
      B2.updateBerry(&P1, &P2);
      B3.updateBerry(&P1, &P2);
      B4.updateBerry(&P1, &P2);
      B5.updateBerry(&P1, &P2);

      if(endScreen){ break;}

      currentInput = Switch_In();
      if(currentInput != lastInput){
        Sound_Stop();
        if (currentInput == 1) {
          Sound_Start(SinWave, 32, currentInput); 
        } 

        else if (currentInput == 2 && P2.type == 1) {
          Sound_Start(Sniper, 4145, currentInput);  
        } 
        else if (currentInput == 2 && P2.type == 2) {
          Sound_Start(shoot, 4074, currentInput);  
        } 
        else if (currentInput == 2 && P2.type == 3) {
          Sound_Start(slimeball, 7570, currentInput);  
        } 
        else if (currentInput == 2 && P2.type == 4) {
          Sound_Start(laser, 2223, currentInput);  
        } 
        
        else if (currentInput == 4 && P1.type == 1) {
          Sound_Start(Sniper, 4145, currentInput);  
        } 
        else if (currentInput == 4 && P1.type == 2) {
          Sound_Start(shoot, 4074, currentInput);  
        } 
        else if (currentInput == 4 && P1.type == 3) {
          Sound_Start(slimeball, 7570, currentInput);  
        } 
        else if (currentInput == 4 && P1.type == 4) {
          Sound_Start(laser, 2223, currentInput);  
        }   

        else if (currentInput == 8) {
          Sound_Start(SinWave, 32, currentInput);  
        }
        lastInput = currentInput;
      }

      if(currentInput == 1){
        P2.specialUsed = true;
        p2SpecialCountdown = true;
      }
      if(currentInput == 8){
        P1.specialUsed = true;
        p1SpecialCountdown = true;
      }
      if(P1.specialUsed == true){
        if(p1SpecialTimer <= 0){
          P1.specialUsed = false;
        }
      }
      if(P2.specialUsed == true){
        if(p2SpecialTimer <= 0){
          P2.specialUsed = false;
        }
      }
      Clock_Delay(800000);

      if(LEDtimer % 5 == 0){
        LED_Off(PB23INDEX);
        LED_Off(PB24INDEX);
        LED_On(PA24INDEX); 
      }
      if (LEDtimer % 10 == 0){
        LED_Off(PB23INDEX);
        LED_On(PB24INDEX);
        LED_Off(PA24INDEX); 
      }
      if (LEDtimer % 15 == 0){
        LED_On(PB23INDEX);
        LED_Off(PB24INDEX);
        LED_Off(PA24INDEX);
      }

      p1AttackTimer = p1AttackTimer + p1AttackTicks / (.3 * 4);
      if(p1AttackTimer >= 100){
        p1AttackTimer = 100;
        p1AttackTicks = 0;
        P1.canAttack = true;
      }
      p2AttackTimer = p2AttackTimer + p2AttackTicks / (.3 * 4);
      if(p2AttackTimer >= 100){
        p2AttackTimer = 100;
        p2AttackTicks = 0;
        P2.canAttack = true;
      }
      ST7735_SetRotation(1);
      ST7735_FillRect(0,0,160,25, ST7735_GREEN);
      ST7735_SetTextColor(ST7735_BLACK);


      ST7735_SetCursor(1, 0);  // Top left for P1
      ST7735_OutString("P1:");
      if(p2AttackTimer == 100){
        ST7735_OutString("GO!");
      }else{
        ST7735_OutUDec(p2AttackTimer); // swapped for some reason
      }

      ST7735_SetCursor(1, 1);  // Next row down for P2
      ST7735_OutString("P2:");
      if(p1AttackTimer == 100){
        ST7735_OutString("GO!");
      }else{
        ST7735_OutUDec(p1AttackTimer); // swapped for some reason
      }

      ST7735_SetCursor(8, 0);  // Next row down for P2
      ST7735_OutString("HP:");
      ST7735_OutUDec(P2.health); // swapped for some reason
      
      ST7735_SetCursor(8, 1);  // Next row down for P2
      ST7735_OutString("HP:");
      ST7735_OutUDec(P1.health); // swapped for some reason

      if(P1.specialUsed){
        ST7735_SetTextColor(ST7735_RED);
        ST7735_SetCursor(15, 0);  // Next row down for P2
        ST7735_OutUDec(p1SpecialTimer); // swapped for some reason    
      }
      if(P2.specialUsed){
        ST7735_SetTextColor(ST7735_RED);
        ST7735_SetCursor(15, 1);  // Next row down for P2
        ST7735_OutUDec(p2SpecialTimer); // swapped for some reason
      }
    }
  }
}