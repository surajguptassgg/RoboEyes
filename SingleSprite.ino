#include <TFT_eSPI.h>
#include "FluxGarage_RoboEyes_Single.h"
#include "rm67162.h"

TFT_eSPI tft = TFT_eSPI();
roboEyes_Sprite eyes(&tft);

int currentMode = 0;
unsigned long modeChangeTimer = 0;
int modeChangeDuration = 3000;

void setup() {
  Serial.begin(115200);
  //rm67162_init();
  //lcd_setRotation(1);
  
  // Initialize eyes with screen dimensions and framerate
  eyes.begin(536, 240, 90);

  //eyes.setWidth(160, 160);
  //eyes.setHeight(120, 120);
  eyes.setWidth(100, 100);
  eyes.setHeight(60, 60);

  // Configure eyes as needed
  eyes.setMood(DEFAULT);
  eyes.setColors(TFT_RED, TFT_WHITE);

  eyes.setPosition(E);
  eyes.setCuriosity(false);
  eyes.setCyclops(false);
  eyes.setHFlicker(false);
  eyes.setVFlicker(false);
  eyes.setIdleMode(true);
  eyes.setBackground(true);
  eyes.setAutoblinker(true, 3, 2);
}

void loop() {
  eyes.update(); // This will draw and display the eyes
  
  /*
  if (millis() > modeChangeTimer + modeChangeDuration) {
    modeChangeTimer = millis();
    currentMode = (currentMode + 1) % 8; // Cycle through different modes
    //currentMode = 0;

    Serial.print("Changing to mode: ");
    Serial.println(currentMode);
    
    // Reset all modes first
    eyes.setMood(DEFAULT);
    eyes.setCuriosity(false);
    eyes.setCyclops(false);
    eyes.setHFlicker(false);
    eyes.setVFlicker(false);
    eyes.setIdleMode(false);
    
    // Apply new mode
    switch(currentMode) {
      case 0: // Default eyes
        Serial.println("Default eyes");
        // Just normal eyes
        break;
      
      case 1: // Happy eyes
        Serial.println("Happy eyes");
        eyes.setMood(HAPPY);
        break;
      
      case 2: // Angry eyes
        Serial.println("Angry eyes");
        eyes.setMood(ANGRY);
        break;
      
      case 3: // Tired eyes
        Serial.println("Tired eyes");
        eyes.setMood(TIRED);
        break;
      
      case 4: // Curious eyes
        Serial.println("Curious eyes");
        eyes.setCuriosity(true);
        eyes.setPosition(E); // Look to the right
        delay(1000);
        eyes.setPosition(W); // Look to the left
        break;
      
      case 5: // Cyclops mode
        Serial.println("Cyclops mode");
        eyes.setCyclops(true);
        break;
      
      case 6: // Laugh animation
        Serial.println("Laugh animation");
        eyes.anim_laugh();
        break;
      
      case 7: // Confused animation
        Serial.println("Confused animation");
        eyes.anim_confused();
        break;
    }
  }
  */
  //Serial.println("pushing to lcd now");
  //lcd_PushColors(0, 0, eyes.getSprite()->width(), eyes.getSprite()->height(), (uint16_t*)eyes.getSprite()->getPointer());
  //delay(40);
}
