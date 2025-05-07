#include <TFT_eSPI.h>
#include "FluxGarage_RoboEyes_Single.h"
#include "rm67162.h"
#include "SDAnimation.h"
#include "esp_heap_caps.h"
#include "adc_bsp.h"

TFT_eSPI tft = TFT_eSPI();
roboEyes_Sprite eyes(&tft);

SDAnimation fireAnimation;

int currentMode = 0;
unsigned long modeChangeTimer = 0;
int modeChangeDuration = 8000;

float batteryVoltage = 0;
int batteryPercentage = 0;
unsigned long batteryCheckTimer = 0;
const int batteryCheckInterval = 60000; // Check every 60 seconds

uint16_t* fireFramePointers[30];

void setup() {
  Serial.begin(115200);

  //delay(5000);
  SD_card_Init();
  adc_bsp_init();
  //rm67162_init();
  //lcd_setRotation(1);
  
  // Initialize eyes with screen dimensions and framerate
  eyes.begin(536, 240, 90);

  eyes.setWidth(160, 160);
  eyes.setHeight(120, 120);

  // Configure eyes as needed
  //eyes.setMood(DEFAULT);
  eyes.setColors(TFT_BLACK, TFT_WHITE);

  //eyes.setPosition(9);
  eyes.setCuriosity(false);
  eyes.setCyclops(false);
  eyes.setHFlicker(false);
  eyes.setVFlicker(false);
  eyes.setIdleMode(true);
  //eyes.setBackground(true, frames, (uint16_t**)fireallArray);
  eyes.setAutoblinker(true, 3, 2);
  Serial.printf("Checking battery");
  checkBattery();
  //switchAnimation("/sd_card/sd_card/animations/charge", 350, 90, 25);

  //delay(8000);
  // Initialize our animation from SD card
}

void switchAnimation(const char* animPath, int x, int y, int buffers) {
  // First, fully delete and recreate the animation object
  fireAnimation.~SDAnimation(); // Call destructor manually
  new(&fireAnimation) SDAnimation(); // Placement new to reinitialize
  
  // Print available memory for debugging
  Serial.printf("Free heap before loading: %d bytes\n", 
                ESP.getFreeHeap());
  Serial.printf("Largest free block: %d bytes\n", 
                heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  
  // Now try to load the new animation with fewer buffers
  if (fireAnimation.begin(animPath, buffers)) {
    Serial.printf("Successfully loaded animation from %s\n", animPath);
    
    int frameCount = fireAnimation.getFrameCount();
    Serial.printf("Animation has %d frames\n", frameCount);
    
    // Load only as many frames as we have memory for
    int loadedFrames = 0;
    for (int i = 0; i < frameCount; i++) {
      fireFramePointers[i] = fireAnimation.getFrame(i);
      if (fireFramePointers[i] != NULL) {
        loadedFrames++;
      } else {
        Serial.printf("Failed to load frame %d\n", i);
        break;
      }
    }
    
    // Set the animation as background with the actual number of loaded frames
    eyes.setBackground(true, fireAnimation.getWidth(), 
                      fireAnimation.getHeight(), x, y, 
                      loadedFrames, (uint16_t**)fireFramePointers);
  } else {
    Serial.println("Failed to load animation from SD card");
    eyes.setBackground(false, 0, 0, 0, 0, 0, NULL);
  }
}

void checkBattery() {
  //Serial.printf("checkBattery Called");
  int adcData = 0;
    // Get battery voltage
  adc_get_value(&batteryVoltage, &adcData);
  
  // Convert to percentage (for standard 3.7V LiPo)
  batteryPercentage = map(constrain(batteryVoltage * 100, 300, 420), 300, 420, 0, 100);
  Serial.print(batteryPercentage);
  if (batteryPercentage < 10){
    eyes.setMood(TIRED);
    eyes.setPosition(5);
    switchAnimation("/sd_card/sd_card/animations/charge", 420, 20, 2);
    //Serial.printf("Battery checked, mood set to tired");
  }else if (batteryPercentage >= 10 && batteryPercentage < 65){
    eyes.setMood(DEFAULT);
    //Serial.printf("Battery checked, mood set to default");
  }else if (batteryPercentage >= 65 && batteryPercentage < 95){
    eyes.setMood(HAPPY);
    //Serial.printf("Battery checked, mood set to happy");
  }else {
    eyes.setMood(ANGRY);
    eyes.setIdleMode(false);
    eyes.setPosition(9);
    switchAnimation("/sd_card/sd_card/animations/fire", 0, 150, 15);
  }

  eyes.setBatteryPercentage(batteryPercentage);
  
  // Print for debugging
  //Serial.print("Battery voltage: ");
  //Serial.print(batteryVoltage);
  //Serial.print("V (");
  //Serial.print(batteryPercentage);
  //Serial.println("%)");
  
  batteryCheckTimer = millis();
}

void loop() {

  /*
  static unsigned long lastFrameTime = 0;
  if (millis() - lastFrameTime > 42) { // 42ms = ~24fps
    lastFrameTime = millis();
    
    // Get next frame
    uint16_t* nextFrame = fireAnimation.getNextFrame();
    if (nextFrame) {
      // Update the current frame pointer in our array
      int currentFrameIndex = fireAnimation.getCurrentFrameIndex();
      fireFramePointers[currentFrameIndex] = nextFrame;
    }
  }
  */
  eyes.update(); // This will draw and display the eyes
  //eyes.setPosition(9);
  if (millis() - batteryCheckTimer >= batteryCheckInterval) {   
    checkBattery();
  }
  /*
  if (millis() > modeChangeTimer + modeChangeDuration) {
    modeChangeTimer = millis();
    currentMode = (currentMode + 1) % 4;

    switch(currentMode){
      case 0:
        Serial.println("In case fire");
        //eyes.setBackground(false);
        switchAnimation("/sd_card/sd_card/animations/fire", 0, 150);
        break;
      case 1:
        //Serial.println("In case trip");
        eyes.setBackground(false);
        eyes.setPosition(9);
        //switchAnimation("/sd_card/sd_card/animations/trip", 0, 0);
        /*
        if (fireAnimation.begin("/sd_card/sd_card/animations/trip", 12)) {
          Serial.println("Successfully loaded fire animation from SD card");
          
          // Get frame count from animation
          int frameCount = fireAnimation.getFrameCount();
          // Prepare our frame pointer array for eyes.setBackground
          for (int i = 0; i < frameCount; i++) {
            fireFramePointers[i] = fireAnimation.getFrame(i);
          }
          
          // Set the animation as background
          eyes.setBackground(true, fireAnimation.getWidth(), fireAnimation.getHeight(), 0, 150, frameCount, (uint16_t**)fireFramePointers);
        } else {
          Serial.println("Failed to load animation from SD card");
        }
        
        break;
      case 2:
        eyes.setPosition(7);
        //eyes.setBackground(false);
        switchAnimation("/sd_card/sd_card/animations/battery", 320, 20);
        break;
      case 3:
        eyes.setBackground(false);
        eyes.setPosition(9);
        break;     
      case 4:
        eyes.setPosition(9);
        break;   
    }
  }
  */
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
