/**
 * Timelapse Motion Controller Arduino Program v2.0
 * Autor: Mrinabh Dutta
 * Version: 2.0
 * Date: June 26, 2015
 * 
 * Copyrights (c) 2015, Mrinabh Dutta
 * All rights reserved.
 * DISCLAIMER
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation and/or
 *    other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include<stdlib.h>
#include <stdint.h>
#include <TFTv2.h>
#include <SPI.h>
#include <SeeedTouchScreen.h>
#include<Stepper.h>

#define TOUCH_DELAY 50
#define CAMERA_PIN 53
#define MOTOR_STEPS_PER_TURN 200
#define MOTOR_SPEED 200

int photographsTaken = 0;

//init TouchScreen port pins
TouchScreen ts = TouchScreen(XP, YP, XM, YM);

// initialize the stepper library on pins 8 through 11:
Stepper motor(MOTOR_STEPS_PER_TURN, 8,11,12,13);

int currentMenuIdx = 0;
 
 void setup() {
  // Camera remote release init
  pinMode(CAMERA_PIN, OUTPUT);
  digitalWrite(CAMERA_PIN, LOW);

  // Stepper Motor init
  initMotor();

  // UI init
  TFT_BL_ON;
  Tft.TFTinit();
  initMenu();
}

void loop() {
  detectKeyPress();
}


// --------------------------------------------------------------------------------------
// GUI functions: Menus and Buttons
// --------------------------------------------------------------------------------------

#define DARK_BROWN 0x7000

char* menu[] = {
  "Home",
  "Direction",
  "Displacement",
  "Interval",
  "Camera Delay"
};

void initMenu() {
  Tft.fillScreen();

  // Menu Title
  Tft.fillRectangle(0, 0, 240, 30, DARK_BROWN);
  Tft.drawString(menu[currentMenuIdx], 5, 10, 2, WHITE);

  switch(currentMenuIdx) {
    case 0: // Home
    homeMenu();
    break;
    
    case 1: // Direction
    directionMenu();
    break;
    
    case 2: // Displacement
    displacementMenu();
    break;
    
    case 3: // Interval
    intervalMenu();
    break;
    
    case 4: // Camera Delay
    cameraDelayMenu();
    break;
  }
}

void homeMenu() {
  // Menu Buttons
  Tft.drawRectangle(0, 35, 240, 50, BLUE);
  Tft.drawString("Direction", 5, 55, 2, WHITE);

  Tft.drawRectangle(0, 90, 240, 50, BLUE);
  Tft.drawString("Displacement", 5, 110, 2, WHITE);

  Tft.drawRectangle(0, 145, 240, 50, BLUE);
  Tft.drawString("Interval", 5, 165, 2, WHITE);

  Tft.drawRectangle(0, 200, 240, 50, BLUE);
  Tft.drawString("Camera Delay", 5, 220, 2, WHITE);

  Tft.fillRectangle(0, 255, 240, 50, RED);
  Tft.drawString("Start Timelapse!", 25, 270, 2, WHITE);
}

char* dirVal[] = {
  "Forward",
  "Backward"
};

int currentDirSelIdx = 0;
void directionMenu() {
  Tft.fillRectangle(0, 35, 240, 50, YELLOW);
  Tft.drawString("Select Direction", 5, 55, 2, BLACK);

  Tft.drawRectangle(0, 90, 240, 50, BLUE);
  Tft.drawString(dirVal[currentDirSelIdx], 5, 110, 2, WHITE);

  Tft.drawRectangle(0, 145, 240, 50, BLUE);
  Tft.drawString("+", 100, 165, 2, WHITE);

  Tft.drawRectangle(0, 200, 240, 50, BLUE);
  Tft.drawString("-", 100, 220, 2, WHITE);

  Tft.drawRectangle(0, 255, 240, 50, RED);
  Tft.drawString("Back", 25, 270, 2, WHITE);
}

int displacement = 1;
void displacementMenu() {
  Tft.fillRectangle(0, 35, 240, 50, YELLOW);
  Tft.drawString("Select Displacement", 5, 55, 2, BLACK);

  Tft.drawRectangle(0, 90, 240, 50, BLUE);
  char buffer[7];
  Tft.drawString(itoa(displacement, buffer, 10), 5, 110, 2, WHITE);

  Tft.drawRectangle(0, 145, 240, 50, BLUE);
  Tft.drawString("+", 100, 165, 2, WHITE);

  Tft.drawRectangle(0, 200, 240, 50, BLUE);
  Tft.drawString("-", 100, 220, 2, WHITE);

  Tft.drawRectangle(0, 255, 240, 50, RED);
  Tft.drawString("Back", 25, 270, 2, WHITE);
}

char* intervalSelVal[] {
  "1 sec", "2 sec", "5 sec",
  "10 sec", "30 sec", "1 min",
  "3 min", "5 min", "10 min",
  "15 min", "20 min", "25 min",
  "30 min", "1 hr", "5 hr"
};

unsigned long intervalSeconds[] = {
  1, 2, 5,
  10, 30, 60,
  180, 300, 600,
  900, 1200, 1500,
  1800, 3600, 18000
};
int intervalIdx = 0;
void intervalMenu() {
  Tft.fillRectangle(0, 35, 240, 50, YELLOW);
  Tft.drawString("Select Interval", 5, 55, 2, BLACK);

  Tft.drawRectangle(0, 90, 240, 50, BLUE);
  Tft.drawString(intervalSelVal[intervalIdx], 5, 110, 2, WHITE);

  Tft.drawRectangle(0, 145, 240, 50, BLUE);
  Tft.drawString("+", 100, 165, 2, WHITE);

  Tft.drawRectangle(0, 200, 240, 50, BLUE);
  Tft.drawString("-", 100, 220, 2, WHITE);

  Tft.drawRectangle(0, 255, 240, 50, RED);
  Tft.drawString("Back", 25, 270, 2, WHITE);
}

char* shutterDelaySel[] {
  "1 sec", "5 sec", "10 sec",
  "30 sec", "1 min", "5 min",
  "10 min", "30 min", "1 hr"
};
unsigned long shutterDelaySeconds[] {
  1, 5, 10,
  30, 60, 300,
  600, 1800, 3600
};
int shutterDelayIdx = 0;
void cameraDelayMenu() {
  Tft.fillRectangle(0, 35, 240, 50, YELLOW);
  Tft.drawString("Shutter Delay", 5, 55, 2, BLACK);

  Tft.drawRectangle(0, 90, 240, 50, BLUE);
  Tft.drawString(shutterDelaySel[shutterDelayIdx], 5, 110, 2, WHITE);

  Tft.drawRectangle(0, 145, 240, 50, BLUE);
  Tft.drawString("+", 100, 165, 2, WHITE);

  Tft.drawRectangle(0, 200, 240, 50, BLUE);
  Tft.drawString("-", 100, 220, 2, WHITE);

  Tft.drawRectangle(0, 255, 240, 50, RED);
  Tft.drawString("Back", 25, 270, 2, WHITE);
}

// --------------------------------------------------------------------------------------
// GUI functions: User Inputs
// --------------------------------------------------------------------------------------

// Button ranges on y-axis
const int BTN1LOW  = 35;
const int BTN1HIGH = 85;

const int BTN2LOW  = 90;
const int BTN2HIGH = 140;

const int BTN3LOW  = 145;
const int BTN3HIGH = 195;

const int BTN4LOW  = 200;
const int BTN4HIGH = 250;

const int BTN5LOW  = 255;
const int BTN5HIGH = 305;

void detectKeyPress() {
  Point p = ts.getPoint();
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
  
  if (p.z > __PRESURE) {
    processKeyPress(p.x, p.y);
    delay(TOUCH_DELAY);
  }
}

void processKeyPress(int x, int y) {
  if(currentMenuIdx ==0) {
    // Home/Main Menu selections are simple redirects
    if(y > BTN1LOW && y < BTN1HIGH) { // Show 'Direction' Menu
      currentMenuIdx = 1;
    }

    if(y > BTN2LOW && y < BTN2HIGH) { // Show 'Displacement' Menu
      currentMenuIdx = 2;
    }

    if(y > BTN3LOW && y < BTN3HIGH) { // Show 'Interval' Menu
      currentMenuIdx = 3;
    }

    if(y > BTN4LOW && y < BTN4HIGH) { // Show 'Camera Delay' Menu
      currentMenuIdx = 4;
    }

    if(y > BTN5LOW && y < BTN5HIGH) { // Start Timelapse
      startTimeLapse();
    }

    initMenu();
    
    return;
  }
  
  // Sub-Menus: Determine which button is pressed
  if(y > BTN3LOW && y < BTN3HIGH) {   // '+' is pressed
    incrementSelectedValue();
  }

  if(y > BTN4LOW && y < BTN4HIGH) {   // '-' is pressed
    decrementSelectedValue();
  }

  if(y > BTN5LOW && y < BTN5HIGH) {   // 'Back' is pressed
    // Return to Home Menu
    currentMenuIdx = 0;
    initMenu();
  }
}

void incrementSelectedValue() {
  switch(currentMenuIdx) {
    case 1: // Direction
    if(currentDirSelIdx == 0) {
      currentDirSelIdx = 1;
    } else {
      currentDirSelIdx = 0;
    }

    Tft.fillRectangle(0, 90, 240, 50, BLACK);
    Tft.drawRectangle(0, 90, 240, 50, BLUE);
    Tft.drawString(dirVal[currentDirSelIdx], 5, 110, 2, WHITE);
    break;
    
    case 2: // Displacement
    if(displacement < 20) {
      displacement++;
    } else {
      displacement = 1;
    }
    
    Tft.fillRectangle(0, 90, 240, 50, BLACK);
    Tft.drawRectangle(0, 90, 240, 50, BLUE);
    char buffer[7];
    Tft.drawString(itoa(displacement, buffer, 10), 5, 110, 2, WHITE);
    break;
    
    case 3: // Interval
    if(intervalIdx < 14) {
      intervalIdx++;
    } else {
      intervalIdx = 0;
    }

    Tft.fillRectangle(0, 90, 240, 50, BLACK);
    Tft.drawRectangle(0, 90, 240, 50, BLUE);
    Tft.drawString(intervalSelVal[intervalIdx], 5, 110, 2, WHITE);
    break;
    
    case 4: // Camera Delay
    if(shutterDelayIdx < 9) {
      shutterDelayIdx++;
    } else {
      shutterDelayIdx = 0;
    }

    Tft.fillRectangle(0, 90, 240, 50, BLACK);
    Tft.drawRectangle(0, 90, 240, 50, BLUE);
    Tft.drawString(shutterDelaySel[shutterDelayIdx], 5, 110, 2, WHITE);
    break;
  }
}

void decrementSelectedValue() {
  switch(currentMenuIdx) {
    case 1: // Direction
    if(currentDirSelIdx == 0) {
      currentDirSelIdx = 1;
    } else {
      currentDirSelIdx = 0;
    }

    Tft.fillRectangle(0, 90, 240, 50, BLACK);
    Tft.drawRectangle(0, 90, 240, 50, BLUE);
    Tft.drawString(dirVal[currentDirSelIdx], 5, 110, 2, WHITE);
    break;
    
    case 2: // Displacement
    if(displacement > 1) {
      displacement--;
    } else {
      displacement = 20;
    }
    
    Tft.fillRectangle(0, 90, 240, 50, BLACK);
    Tft.drawRectangle(0, 90, 240, 50, BLUE);
    char buffer[7];
    Tft.drawString(itoa(displacement, buffer, 10), 5, 110, 2, WHITE);
    break;
    
    case 3: // Interval
    if(intervalIdx > 0) {
      intervalIdx--;
    } else {
      intervalIdx = 14;
    }

    Tft.fillRectangle(0, 90, 240, 50, BLACK);
    Tft.drawRectangle(0, 90, 240, 50, BLUE);
    Tft.drawString(intervalSelVal[intervalIdx], 5, 110, 2, WHITE);
    break;
    
    case 4: // Camera Delay
    if(shutterDelayIdx > 0) {
      shutterDelayIdx--;
    } else {
      shutterDelayIdx = 8;
    }

    Tft.fillRectangle(0, 90, 240, 50, BLACK);
    Tft.drawRectangle(0, 90, 240, 50, BLUE);
    Tft.drawString(shutterDelaySel[shutterDelayIdx], 5, 110, 2, WHITE);
    break;
  }
}

void showParameters() {
    Tft.fillScreen();

  // Menu Title
  Tft.drawRectangle(0, 0, 240, 30, BLUE);
  Tft.drawString("TimeLapse Recording", 5, 10, 2, RED);

  Tft.drawRectangle(0, 35, 240, 50, BLUE);
  Tft.drawString("Direction", 5, 55, 2, GREEN);
  Tft.drawString(dirVal[currentDirSelIdx], 180, 55, 1, RED);

  Tft.drawRectangle(0, 90, 240, 50, BLUE);
  Tft.drawString("Displacement", 5, 110, 2, GREEN);
  char buffer[7];
  Tft.drawString(itoa(displacement, buffer, 10), 180, 110, 1, RED);

  Tft.drawRectangle(0, 145, 240, 50, BLUE);
  Tft.drawString("Interval", 5, 165, 2, GREEN);
  Tft.drawString(intervalSelVal[intervalIdx], 180, 165, 1, RED);

  Tft.drawRectangle(0, 200, 240, 50, BLUE);
  Tft.drawString("Camera Delay", 5, 220, 2, GREEN);
  Tft.drawString(shutterDelaySel[shutterDelayIdx], 180, 220, 1, RED);

  Tft.drawRectangle(0, 255, 240, 50, BLUE);
  Tft.drawString("Photograph", 5, 270, 2, GREEN);
  Tft.drawString(itoa(photographsTaken, buffer, 10), 160, 270, 1, RED);
}

void updatePhotosTaken() {
  Tft.fillRectangle(0, 255, 240, 50, BLACK);
  Tft.drawRectangle(0, 255, 240, 50, BLUE);
  Tft.drawString("Photograph", 5, 270, 2, GREEN);
  char buffer[7];
  Tft.drawString(itoa(photographsTaken, buffer, 10), 160, 270, 1, RED);
}

// --------------------------------------------------------------------------------------
// Stepper Motor functions
// --------------------------------------------------------------------------------------

const int CONTROL_PIN_A = 9;
const int CONTROL_PIN_B = 10;

const int TOTAL_STEPS = 22500;

void initMotor() {
  motor.setSpeed(MOTOR_SPEED);
  pinMode(CONTROL_PIN_A, OUTPUT);
  pinMode(CONTROL_PIN_B, OUTPUT);
}

// Start timelapse recording!
// We need the following parameters:
//  - dir
//  - steps_per_turn
//  - interval
//  - shutterDelay
void startTimeLapse() {
  showParameters();
  photographsTaken = 0;
  int dir = currentDirSelIdx; // {0:Forward, 1:Backward}
  int steps_per_turn = displacement * 20;
  unsigned long interval = intervalSeconds[intervalIdx];
  unsigned long shutterDelay = shutterDelaySeconds[shutterDelayIdx];

  int total_stops = TOTAL_STEPS / steps_per_turn;
  for(int i=0; i<total_stops; i++) {

    delay((interval * 1000L) / 2);
    
    triggerShutterRelease();
    delay(shutterDelay * 1000L);
    
    moveMotor(dir, steps_per_turn);
    delay((interval * 1000L) / 2);
  }
}

void moveMotor(int dir, int steps) {
  if(dir == 1) {
    steps = 0 - steps;
  }
  digitalWrite(CONTROL_PIN_A, HIGH);
  digitalWrite(CONTROL_PIN_B, HIGH);

  motor.step(steps);
  
  digitalWrite(CONTROL_PIN_A, LOW);
  digitalWrite(CONTROL_PIN_B, LOW);
}

void triggerShutterRelease() {
  digitalWrite(CAMERA_PIN, HIGH);
  delay(50);
  digitalWrite(CAMERA_PIN, LOW);
  photographsTaken++;
  updatePhotosTaken();
}

