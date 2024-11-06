#include <FS.h>
#include <SPIFFS.h>
#include <SPI.h>
#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"

// Include library for gyroscope
#include "LSM9DS1TR-SOLDERED.h"
#include "Wire.h"
#include <FastLED.h>

#define LED_PIN     26
#define NUM_LEDS    5
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

#define TFT_DC 21 // Data/Command of VIDI X display is connected to PIN 21
#define TFT_CS 5  // Chip select of VIDI X display is connected to PIN 5

#define speed 25  // Lower value is faster

#define CELL 6    // Define the size of one cell in pixels
#define RAW 32    // Number of rows in our game
#define COL 45    // Define the number of columns in our game
#define ssf 7     // 320 pixels / COL

#define Player 9  // Our player
#define wall 1    // Wall
#define empty 0   // Empty passage

#define key 2     
#define door 3    
#define garlic 4  
#define vampire 5 
#define end 8

int PlayerKeys = 0; // Number of keys available to the player 
int PlayerGarlic = 0;
#define ILI9341_NAVYBLUE 0x006E //https://www.barth-dev.de/online/rgb565-color-picker/ 

int startX = 3;
int startY = 6;

const int col = 25;    // Initial color of cells

const int savePin = 27;
const int loadPin = 39;

const int mapToSerialButton = 13;  // Button for mapToSerial

// Definition of pins for movement buttons
const int btn_lir = 34; // Button for left and right
const int btn_uid = 35; // Button for up and down

// Definition of I2C pins for gyroscope
const int I2C_SDA = 33;
const int I2C_SCL = 32;

// Initialization of gyroscope
LSM9DS1TR imu;

// Add this variable to control vampire movement speed
unsigned long lastVampireMove = 0;
const unsigned long vampireMoveInterval = 1500; // Vampire moves every 1500 ms

// Here we need to define an array and populate it with the initial state of the world
uint8_t world[RAW][COL] ={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                          1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 
                          1, 1, 1, 0, 0, 0, 9, 0, 1, 0, 0, 0, 0, 0, 4, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1,  
                          1, 1, 1, 0, 2, 2, 0, 0, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1,   
                          1, 1, 1, 2, 2, 0, 0, 0, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1,  
                          1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                          1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 5, 0, 0, 
                          1, 1, 0, 3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 4, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 1, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 4, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 4, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                          1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          1, 1, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
                          1, 1, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
                          1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 
                          1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 8, 8, 0, 1, 1, 1, 1, 1, 1, 
                          1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 8, 8, 0, 1, 1, 1, 1, 1, 1, 
                          1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1,
                          1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 
                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
                          };

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

bool saveButtonState = false;
bool loadButtonState = false;

#define NOTA_C4 262
#define NOTA_D4 294
#define NOTA_E4 330
#define NOTA_F4 349
#define NOTA_G4 392
#define NOTA_A4 440
#define NOTA_B4 466
#define NOTA_H4 494
#define NOTA_C5 523

int GPIOSpeaker = 25;
int NoteDuration = 200;

void fill_world_edges() {
  for (int i = 0; i < RAW; i++) {
    for (int j = 0; j < COL; j++) {
      if (
         i == 0 ||
         i == 1 ||
         i == RAW - 1 ||
         i == RAW - 2 ||

         j == 0 ||
         j == 1 ||
         j == COL - 1 ||
         j == COL - 2
         ) {
        world[i][j] = wall;
      }
      if ( ssf == 5) {
        if (
          i == 47 ||
          i == 46 ||
          j == 62 ||
          j == 63 
          ) {
          world[i][j] = wall;
        }
      }
      if ( ssf == 6) {
        if (
          i == 38 ||
          i == 39 ||
          j == 51 ||
          j == 52 ||
          j == 53 
          ) {
          world[i][j] = wall;
        }
      }
      if ( ssf == 7) {
        if (
          i == 32 ||
          i == 33 ||
          j == 44 ||
          j == 45 
          ) {
          world[i][j] = wall;
        }
      }
    }
  }
}

void generateRandomPattern() {
  for (int i = 1; i < RAW - 1; i = i + 3) {
    for (int j = 1; j < COL - 1; j = j + 3) {
      int tmp = random(3);
      world[i][j] = tmp;
      if (tmp == empty) {
        world[i - 1][j] = empty;
        world[i + 1][j] = empty;
        world[i][j - 1] = empty;
        world[i][j + 1] = empty;
        world[i - 1][j - 1] = empty;
        world[i + 1][j + 1] = empty;
        world[i + 1][j - 1] = empty;
        world[i - 1][j + 1] = empty;
      } else if (tmp == wall) {
        world[i - 1][j] = wall;
        world[i + 1][j] = wall;
        world[i][j - 1] = wall;
        world[i][j + 1] = wall;
        world[i - 1][j - 1] = wall;
        world[i + 1][j + 1] = wall;
        world[i + 1][j - 1] = wall;
        world[i - 1][j + 1] = wall;
      } else {
        world[i - 1][j] = empty;
        world[i + 1][j] = empty;
        world[i][j - 1] = empty;
        world[i][j + 1] = empty;
        world[i - 1][j - 1] = empty;
        world[i + 1][j + 1] = empty;
        world[i + 1][j - 1] = empty;
        world[i - 1][j + 1] = empty;
        world[i][j] = empty;
      }
    }
  }
}

void drawWorld() {
  for (int y = 0; y < RAW; y++) {
    for (int x = 0; x < COL; x++) {
      if (world[y][x] == wall) {
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, col);
      } else if (world[y][x] == empty) {
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_BLACK);
      } else if (world[y][x] == Player) {
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_RED);
      } else if (world[y][x] == key) { 
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_GREEN);
      } else if (world[y][x] == door) { 
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_NAVYBLUE); 
      } else if (world[y][x] == vampire) { 
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_CYAN); 
      } else if (world[y][x] == garlic) { 
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_YELLOW); 
      } else if (world[y][x] == end) { 
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_MAGENTA); 
      }
    }
  }
}

void mapToSerial() {

  bool mapToSerialButtonState = digitalRead(mapToSerialButton);

  if (mapToSerialButtonState == LOW) {
    Serial.print("uint8_t world[RAW][COL] = {");
    for (int y = 0; y < RAW; y++) {
      for (int x = 0; x < COL; x++) {
        if (world[y][x] == wall) {
          Serial.print("1"); Serial.print(",");
        } else if (world[y][x] == empty) {
          Serial.print("0"); Serial.print(",");
        } else if (world[y][x] == Player) {
          Serial.print("9"); Serial.print(",");
        }
      }
      Serial.println("");
    }
    Serial.print("};");
    delay(500);  // debounce delay
  }
}

void YouWon() {
  uint8_t world[RAW][COL] ={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 2, 1, 1, 2, 1, 2, 2, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 1, 2, 1, 2, 1, 2, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 2, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 2, 1, 2, 2, 1, 2, 1, 2, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 2, 1, 2, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 2, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 2, 1, 1, 2, 1, 2, 2, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 0, 5, 0, 5, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 0, 0, 0, 0, 0, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 0, 9, 0, 0, 0, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 0, 0, 9, 9, 0, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 0, 1, 1, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 8, 8, 8, 8, 8, 0, 0, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 0, 8, 8, 8, 8, 8, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 8, 8, 8, 8, 8, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 8, 8, 8, 8, 8, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 8, 8, 8, 8, 8, 8, 8, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 8, 8, 8, 8, 8, 8, 8, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 8, 8, 8, 8, 8, 8, 8, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 8, 8, 8, 8, 8, 8, 8, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 8, 8, 8, 8, 8, 8, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 8, 8, 8, 8, 8, 8, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 8, 8, 8, 8, 8, 8, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
                            };
  //drawWorld();
    for (int y = 0; y < RAW; y++) {
    for (int x = 0; x < COL; x++) {
      if (world[y][x] == wall) {
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, col);
      } else if (world[y][x] == empty) {
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_BLACK);
      } else if (world[y][x] == Player) {
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_RED);
      } else if (world[y][x] == key) { 
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_GREEN);
      } else if (world[y][x] == door) { 
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_NAVYBLUE); 
      } else if (world[y][x] == vampire) { 
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_CYAN); 
      } else if (world[y][x] == garlic) { 
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_YELLOW); 
      } else if (world[y][x] == end) { 
        tft.fillRect(x * ssf, y * ssf, CELL, CELL, ILI9341_MAGENTA); 
      }
    }
  }
  WinnerSound();
  delay(10000);
}


void YouDied() { 
  RGBRed();
  for (int n = 0; n < 9; n++) { 
    generateRandomPattern(); 
    drawWorld(); 
    RGBRed();
    delay(speed * n); 
  } 
  fill_world_edges(); 
  drawWorld(); 
  DiedSound();
  loadWorld(); 
} 
void RGBOff() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;  // Set each LED to black (off)
  }
  FastLED.show();  // Apply changes
}

void RGBGreen() {
  for (int n = 0; n < 4; n++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Green;  // Set the color of LEDs to green
    }
    FastLED.show();
    delay(100);
    RGBOff();
    delay(100);
  }
}

void RGBRed() {
  for (int n = 0; n < 4; n++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Red;  // Set the color of LEDs to red
    }
    FastLED.show();
    delay(100);
    RGBOff();
    delay(100);
  }
}

void RGBBlue() {
  for (int n = 0; n < 4; n++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Blue;  // Set the color of LEDs to blue
    }
    FastLED.show();
    delay(100);
    RGBOff();
    delay(100);
  }
}

void printPlayerKeys() {
  tft.setCursor(0, 224);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK); // Green text on black background
  tft.setTextSize(1);
  tft.print("PlayerKeys: "); tft.print(PlayerKeys);tft.print("   ");
  RGBGreen();
}

void printPlayerGarlic() {
  tft.setCursor(0, 232);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); // Yellow text on black background
  tft.setTextSize(1);
  tft.print("PlayerGarlic: "); tft.print(PlayerGarlic);
  RGBRed();
}

void go_right() {
  // Loop through each row
  for (int y = 0; y < RAW; y++) {
    // Loop from the last column to the first column
    for (int x = COL - 1; x > 0; x--) {
      
      // Check if the current cell is empty and the cell to the left has the player
      if (world[y][x] == empty && world[y][x - 1] == Player) {
        // Move the player one cell to the right
        world[y][x] = Player;
        world[y][x - 1] = empty;
      }
      
      // Check if the current cell is empty and the cell to the left has the END Game Portal
      if (world[y][x] == end && world[y][x - 1] == Player) {
        // Call the YouWon() function, indicating the player has reached the END
        YouWon(); // You are the Winner
      }

      // Check if the current cell contains a key and the cell to the left has the player
      if (world[y][x] == key && world[y][x - 1] == Player) { 
        // Move the player to the cell with the key
        world[y][x] = Player; 
        world[y][x - 1] = empty;
        // Increment player's key count and print it
        PlayerKeys++;
printPlayerKeys();
      }

      // Check if the current cell contains garlic and the cell to the left has the player
      if (world[y][x] == garlic && world[y][x - 1] == Player) { 
        // Move the player to the cell with the garlic
        world[y][x] = Player; 
        world[y][x - 1] = empty;
        // Increment player's garlic count and print it
        PlayerGarlic++; 
printPlayerGarlic();
      } 

      // Check if the current cell contains a door, the cell to the left has the player, and the player has at least one key
      if (world[y][x] == door && world[y][x - 1] == Player && PlayerKeys > 0) { 
        // Move the player to the cell with the door
        world[y][x] = Player; 
        world[y][x - 1] = empty;
        // Decrease player's key count by 1 and print it
        PlayerKeys--; 
printPlayerKeys();
      } 

      // Check if the current cell contains a vampire, the cell to the left has the player, and the player has at least one garlic
      if (world[y][x] == vampire && world[y][x - 1] == Player && PlayerGarlic > 0) { 
        // Move the player to the cell with the vampire
        world[y][x] = Player; 
        world[y][x - 1] = empty;
        // Decrease player's garlic count by 1 and print it
        PlayerGarlic--; 
printPlayerGarlic();
      } 

      // Check if the current cell contains a vampire, the cell to the left has the player, and the player has no garlic
      if (world[y][x] == vampire && world[y][x - 1] == Player && PlayerGarlic < 1) { 
        // Call the YouDied() function, indicating the player has been defeated
        YouDied(); 
      } 
    }
  }
}

void go_down() {
  // Loop from the last row to the first row
  for (int y = RAW - 1; y > 0; y--) {
    // Loop through each column
    for (int x = 0; x < COL; x++) {
      
      // Move player if empty space is below
      if (world[y][x] == empty && world[y - 1][x] == Player) {
        world[y][x] = Player;
        world[y - 1][x] = empty;
      }

      // You are the Winner
      if (world[y][x] == end && world[y - 1][x] == Player) {
        YouWon(); 
      }

      // Pick up key
      if (world[y][x] == key && world[y - 1][x] == Player) { 
        world[y][x] = Player; 
        world[y - 1][x] = empty;
        PlayerKeys++; 
printPlayerKeys();
      }

      // Pick up garlic
      if (world[y][x] == garlic && world[y - 1][x] == Player) { 
        world[y][x] = Player; 
        world[y - 1][x] = empty;
        PlayerGarlic++; 
printPlayerGarlic();
      }

      // Open door if player has keys
      if (world[y][x] == door && world[y - 1][x] == Player && PlayerKeys > 0) { 
        world[y][x] = Player; 
        world[y - 1][x] = empty;
        PlayerKeys--; 
printPlayerKeys();
      }

      // Fight vampire if player has garlic
      if (world[y][x] == vampire && world[y - 1][x] == Player && PlayerGarlic > 0) { 
        world[y][x] = Player; 
        world[y - 1][x] = empty;
        PlayerGarlic--; 
printPlayerGarlic();
      }

      // Player dies if vampire encountered and no garlic
      if (world[y][x] == vampire && world[y - 1][x] == Player && PlayerGarlic < 1) { 
        YouDied(); 
      } 
    }
  }
}

void go_up() {
  // Loop from the first row to the second-to-last row
  for (int y = 0; y < RAW - 1; y++) {
    // Loop through each column
    for (int x = 0; x < COL; x++) {
      
      // Move player if empty space is above
      if (world[y][x] == empty && world[y + 1][x] == Player) {
        world[y][x] = Player;
        world[y + 1][x] = empty;
      }

      // You are the Winner
      if (world[y][x] == end && world[y + 1][x] == Player) {
        YouWon(); 
      }

      // Pick up key
      if (world[y][x] == key && world[y + 1][x] == Player) { 
        world[y][x] = Player; 
        world[y + 1][x] = empty;
        PlayerKeys++; 
printPlayerKeys();
      }

      // Pick up garlic
      if (world[y][x] == garlic && world[y + 1][x] == Player) { 
        world[y][x] = Player; 
        world[y + 1][x] = empty;
        PlayerGarlic++; 
printPlayerGarlic();
      }

      // Open door if player has keys
      if (world[y][x] == door && world[y + 1][x] == Player && PlayerKeys > 0) { 
        world[y][x] = Player; 
        world[y + 1][x] = empty;
        PlayerKeys--; 
printPlayerKeys();
      }

      // Fight vampire if player has garlic
      if (world[y][x] == vampire && world[y + 1][x] == Player && PlayerGarlic > 0) { 
        world[y][x] = Player; 
        world[y + 1][x] = empty;
        PlayerGarlic--; 
printPlayerGarlic();
      }

      // Player dies if vampire encountered and no garlic
      if (world[y][x] == vampire && world[y + 1][x] == Player && PlayerGarlic < 1) { 
        YouDied(); 
      } 
    }
  }
}

void go_left() {
  // Loop through each row
  for (int y = 0; y < RAW; y++) {
    // Loop from the first column to the second-to-last column
    for (int x = 0; x < COL - 1; x++) {
      
      // Move player if empty space is to the left
      if (world[y][x] == empty && world[y][x + 1] == Player) {
        world[y][x] = Player;
        world[y][x + 1] = empty;
      }

      // You are the Winner
      if (world[y][x] == end && world[y][x + 1] == Player) {
        YouWon(); 
      }

      // Pick up key
      if (world[y][x] == key && world[y][x + 1] == Player) { 
        world[y][x] = Player; 
        world[y][x + 1] = empty;
        PlayerKeys++; 
printPlayerKeys();
      }

      // Pick up garlic
      if (world[y][x] == garlic && world[y][x + 1] == Player) { 
        world[y][x] = Player; 
        world[y][x + 1] = empty;
        PlayerGarlic++; 
printPlayerGarlic();
      }

      // Open door if player has keys
      if (world[y][x] == door && world[y][x + 1] == Player && PlayerKeys > 0) { 
        world[y][x] = Player; 
        world[y][x + 1] = empty;
        PlayerKeys--; 
printPlayerKeys();
      }

      // Fight vampire if player has garlic
      if (world[y][x] == vampire && world[y][x + 1] == Player && PlayerGarlic > 0) { 
        world[y][x] = Player; 
        world[y][x + 1] = empty;
        PlayerGarlic--; 
printPlayerGarlic();
      }

      // Player dies if vampire encountered and no garlic
      if (world[y][x] == vampire && world[y][x + 1] == Player && PlayerGarlic < 1) { 
        YouDied(); 
      } 
    }
  }
}

void saveWorld() {
  File file = SPIFFS.open("/world.bin", FILE_WRITE);
  if (!file) {
    Serial.println("Failed to create the file");
    return;
  }

  for (int i = 0; i < RAW; i++) {
    file.write(world[i], COL * sizeof(uint8_t));
  }

  file.close();
  Serial.println("World saved successfully");
}

void loadWorld() {
  File file = SPIFFS.open("/world.bin", FILE_READ);
  if (!file) {
    Serial.println("Failed to open the file");
    return;
  }

  for (int i = 0; i < RAW; i++) {
    file.read(world[i], COL * sizeof(uint8_t));
  }

  file.close();
  Serial.println("World loaded successfully");
}

void loadSave() {
  saveButtonState = digitalRead(savePin);
  loadButtonState = digitalRead(loadPin);

  if (saveButtonState == LOW) {
    saveWorld();
    delay(500);  // debounce delay
  }

  if (loadButtonState == LOW) {
    loadWorld();
    //drawWorld();
    delay(500);   // debounce delay
  }
}

void FastLEDsetup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void SoundSetup() {
  ledcSetup(0, 10000, 12);
  ledcAttachPin(GPIOSpeaker, 0);
}
void setup() {
  Serial.begin(115200);

  // Setting the initial state of the world
  // If you have saving and loading the world, you can use loadWorld();
  world[startX][startY] = Player;

  // Define input pins, i.e., control buttons
  pinMode(btn_lir, INPUT_PULLUP);
  pinMode(btn_uid, INPUT_PULLUP);

  SPIFFS.begin(true);
  pinMode(savePin, INPUT_PULLUP);
  pinMode(loadPin, INPUT_PULLUP);

  // Initialize the display
  tft.begin();
  // Set the screen orientation
  tft.setRotation(3);
  // Define the background color (black consumes the least power)
  tft.fillScreen(ILI9341_BLACK);
  // Draw the world
  fill_world_edges();
  drawWorld();
  delay(2000);

  // Initialize I2C communication for the gyroscope
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize the gyroscope
  while (imu.begin() == false) {
    tft.setCursor(0, 224);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK); // Red text on black background
    tft.setTextSize(1);
    tft.println("Failed to communicate with LSM9DS1.");
    tft.println("Double-check the wiring!");
    delay(1000);
  }
  tft.setCursor(0, 224);
  tft.println("                                   ");
  tft.println("                        ");

  FastLEDsetup();
  RGBloop();
  SoundSetup();
}

void DiedSound() {
  // Start - Falling melody
  ledcWriteTone(0, NOTA_C5);
  delay(NoteDuration * 2);
  ledcWriteTone(0, NOTA_B4);
  delay(NoteDuration * 2);
  ledcWriteTone(0, NOTA_A4);
  delay(NoteDuration * 2);
  ledcWriteTone(0, NOTA_G4);
  delay(NoteDuration * 2);

  // End - Low notes
  ledcWriteTone(0, NOTA_E4);
  delay(NoteDuration * 2);
  ledcWriteTone(0, NOTA_C4);
  delay(NoteDuration * 4);

  // Pause for ending
  ledcWriteTone(0, 0);  // Turn off sound
  delay(3000);
}

void WinnerSound() {
  // Part 1
  ledcWriteTone(0, NOTA_C5);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_G4);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_A4);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_G4);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_B4);
  delay(NoteDuration * 2);

  // Part 2
  ledcWriteTone(0, NOTA_C5);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_G4);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_A4);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_B4);
  delay(NoteDuration * 2);

  // Part 3 - UP-Beat
  ledcWriteTone(0, NOTA_C5);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_C5);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_C5);
  delay(NoteDuration * 2);
  ledcWriteTone(0, NOTA_G4);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_E4);
  delay(NoteDuration);

  // Part 4 - Ending
  ledcWriteTone(0, NOTA_C5);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_C5);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_H4);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_G4);
  delay(NoteDuration);
  ledcWriteTone(0, NOTA_A4);
  delay(NoteDuration * 3);

  // Pause
  ledcWriteTone(0, 0);  // Mute Sound
  delay(3000);
}

void RGBloop() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;  // Set the color of LEDs to red
  }
  FastLED.show();
  delay(100);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green;  // Set the color of LEDs to green
  }
  FastLED.show();
  delay(100);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;  // Set the color of LEDs to blue
  }
  FastLED.show();
  delay(100);
}

// Vampire movement function for multiple vampires
// There is a bug somewhere - Sometimes Vampires moves multiple steps
void moveVampires() {
  int playerX = -1, playerY = -1;

  // Find player position
  for (int y = 0; y < RAW; y++) {
    for (int x = 0; x < COL; x++) {
      if (world[y][x] == Player) {
        playerY = y;
        playerX = x;
        break;
      }
    }
  }

  // Exit if player not found
  if (playerX == -1 || playerY == -1) {
    return;
  }

  // Iterate through the map to find each vampire
  for (int y = 0; y < RAW; y++) {
    for (int x = 0; x < COL; x++) {
      if (world[y][x] == vampire) {
        
        int vampireX = x;
        int vampireY = y;

        // Determine direction to move towards the player
        int deltaX = playerX - vampireX;
        int deltaY = playerY - vampireY;

        int moveX = 0;
        int moveY = 0;

        if (abs(deltaX) > abs(deltaY)) {
          // Move in the x direction
          if (deltaX > 0) {
            moveX = 1; // Right
          } else if (deltaX < 0) {
            moveX = -1; // Left
          }
        } else {
          // Move in the y direction
          if (deltaY > 0) {
            moveY = 1; // Down
          } else if (deltaY < 0) {
            moveY = -1; // Up
          }
        }

        // Calculate the next position
        int nextX = vampireX + moveX;
        int nextY = vampireY + moveY;

        // Check boundaries
        if (nextX < 0 || nextX >= COL || nextY < 0 || nextY >= RAW) {
          continue; // Skip this move if out of bounds
        }

        // Check for wall or door
        if (world[nextY][nextX] == wall || world[nextY][nextX] == door) {
          continue; // Vampire cannot move into a wall or door
        }

        // Check for garlic
        if (world[nextY][nextX] == garlic) {
          // Vampire dies on garlic
          world[vampireY][vampireX] = empty;
          world[nextY][nextX] = empty;
          Serial.println("A vampire died encountering garlic!");
          return;
        }

        // Check for player
        if (world[nextY][nextX] == Player) {
          if (PlayerGarlic > 0) {
            // Vampire dies if player has garlic
            world[vampireY][vampireX] = empty;
            world[nextY][nextX] = Player;
            PlayerGarlic--;
            printPlayerGarlic();
            Serial.println("A vampire was killed by the player!");
          } else {
            // Player dies if no garlic
            YouDied();
          }
          continue;
        }

        // Move vampire to the next position
        world[nextY][nextX] = vampire;
        world[vampireY][vampireX] = empty;
      }
    }
  }
  drawWorld();
}
void loop() {
  loadSave();
  mapToSerial();

  // Update sensor values
  if (imu.gyroAvailable()) {
    imu.readGyro();
  }

  if (imu.accelAvailable()) {
    imu.readAccel();
  }

  if (imu.magAvailable()) {
    imu.readMag();
  }

  // Calculate pitch and roll
  float roll = atan2(imu.ay, imu.az);
  float pitch = atan2(-imu.ax, sqrt(imu.ay * imu.ay + imu.az * imu.az));

  // Conversion from radians to degrees
  roll *= 180.0 / PI;
  pitch *= 180.0 / PI;

  // Movement based on pitch and roll values
  if (roll > 10) {
    go_up(); // go_left();
    delay(speed);
  }

  if (roll < -10) {
    go_down(); // go_right();
    delay(speed);
  }

  if (pitch > 10) {
    go_right(); // go_up();
    delay(speed);
  }

  if (pitch < -10) {
    go_left(); // go_down();
    delay(speed);
  }

  // Movement using buttons
  if (analogRead(btn_lir) > 4000) {
    go_left();
    delay(speed);
  }

  if (analogRead(btn_lir) > 1800 && analogRead(btn_lir) < 2200) {
    go_right();
    delay(speed);
  }

  if (analogRead(btn_uid) > 4000) {
    go_up();
    delay(speed);
  }

  if (analogRead(btn_uid) > 1800 && analogRead(btn_uid) < 2200) {
    go_down();
    delay(speed);
  }

  // Call moveVampires() function at regular intervals
  if (millis() - lastVampireMove > vampireMoveInterval) {
    moveVampires();
    lastVampireMove = millis();
  }
  drawWorld();
}
