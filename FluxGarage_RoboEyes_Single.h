/*
 * FluxGarage RoboEyes for TFT Displays using Sprites
 * Based on original FluxGarage RoboEyes but adapted for TFT_eSPI
 */

#ifndef _FLUXGARAGE_ROBOEYES_SPRITE_H
#define _FLUXGARAGE_ROBOEYES_SPRITE_H

#include <TFT_eSPI.h>
#include "rm67162.h"

// For mood type switch
#define DEFAULT 0
#define TIRED 1
#define ANGRY 2
#define HAPPY 3

// For turning things on or off
#define ON 1
#define OFF 0

// For switch "predefined positions"
#define N 1 // north, top center
#define NE 2 // north-east, top right
#define E 3 // east, middle right
#define SE 4 // south-east, bottom right
#define S 5 // south, bottom center
#define SW 6 // south-west, bottom left
#define W 7 // west, middle left
#define NW 8 // north-west, top left 
// for middle center set "DEFAULT"

#define BATTERY_X 460      // X position of battery indicator
#define BATTERY_Y 5      // Y position of battery indicator
#define BATTERY_WIDTH 70   // Width of battery indicator
#define BATTERY_HEIGHT 25  // Height of battery indicator

class roboEyes_Sprite
{
private:
    // The TFT_eSPI display object
    TFT_eSPI* _tft;
    
    // Single sprite for both eyes
    TFT_eSprite* _sprite;

    // Sprite for background 
    TFT_eSprite* _bgSprite;
    
    // Flag for sprite initialization
    bool _spriteInitialized = false;
    bool _bgSpriteInitialized = false;
    
    // Colors for eyes (moved from #define to class variables)
    uint16_t _bgColor = TFT_BLACK;    // Background color
    uint16_t _mainColor = TFT_CYAN;  // Main drawing color

public:
    // Constructor to pass TFT_eSPI reference
    roboEyes_Sprite(TFT_eSPI* tft) {
        _tft = tft;
        // Initialize sprite pointer to NULL
        _sprite = NULL;
        _bgSprite = NULL;
        Serial.println("RoboEyes_Sprite constructor called");
    }
    
    // Destructor to clean up sprites
    ~roboEyes_Sprite() {
        freeSprite();
    }
    
    // Free sprite memory
    void freeSprite() {
        Serial.println("Freeing sprite");
        if (_sprite) {
            _sprite->deleteSprite();
            delete _sprite;
            _sprite = NULL;
        }
        if (_bgSprite) {
            _bgSprite->deleteSprite();
            delete _bgSprite;
            _bgSprite = NULL;
        }
        _bgSpriteInitialized = false;
    }

    // For general setup - screen size and max. frame rate
    int screenWidth = 536; // TFT display width, in pixels
    int screenHeight = 240; // TFT display height, in pixels
    int frameInterval = 20; // default value for 50 frames per second (1000/50 = 20 milliseconds)
    unsigned long fpsTimer = 0; // for timing the frames per second
    bool background = 0;

    unsigned long gifTimer = 0; // Timer for GIF frame updates
    int gifInterval = 42; // Interval between GIF frames in milliseconds
    int i=0; //current background gif frame
    int frames = 0; //Total number of background gif frames
    uint16_t** backGif; //The gif
    int gifWidth = 0;
    int gifHeight = 0;
    int gifxpos = 0;
    int gifypos = 0;
    bool gifstatus = false;
    bool batIndStatus = false;

    // For controlling mood types and expressions
    bool tired = 0;
    bool angry = 0;
    bool happy = 0;
    bool curious = 0; // if true, draw the outer eye larger when looking left or right
    bool cyclops = 0; // if true, draw only one eye
    bool eyeL_open = 0; // left eye opened or closed?
    bool eyeR_open = 0; // right eye opened or closed?

    //For battery percentage
    int batteryPercentage = 0;


    //*********************************************************************************************
    //  Eyes Geometry - Made public for easier debugging
    //*********************************************************************************************

    // EYE LEFT - size and border radius
    int eyeLwidthDefault = 240;
    int eyeLheightDefault = 240;
    int eyeLwidthCurrent = eyeLwidthDefault;
    int eyeLheightCurrent = 1; // start with closed eye, otherwise set to eyeLheightDefault
    int eyeLwidthNext = eyeLwidthDefault;
    int eyeLheightNext = eyeLheightDefault;
    int eyeLheightOffset = 0;
    // Border Radius
    byte eyeLborderRadiusDefault = 8;
    byte eyeLborderRadiusCurrent = eyeLborderRadiusDefault;
    byte eyeLborderRadiusNext = eyeLborderRadiusDefault;

    // EYE RIGHT - size and border radius
    int eyeRwidthDefault = 240; // Changed to direct value rather than eyeLwidthDefault
    int eyeRheightDefault = 240; // Changed to direct value rather than eyeLheightDefault
    int eyeRwidthCurrent = eyeRwidthDefault;
    int eyeRheightCurrent = 1; // start with closed eye, otherwise set to eyeRheightDefault
    int eyeRwidthNext = eyeRwidthDefault;
    int eyeRheightNext = eyeRheightDefault;
    int eyeRheightOffset = 0;
    // Border Radius
    byte eyeRborderRadiusDefault = 8;
    byte eyeRborderRadiusCurrent = eyeRborderRadiusDefault;
    byte eyeRborderRadiusNext = eyeRborderRadiusDefault;

    // Space between eyes
    int spaceBetweenDefault = 10;
    int spaceBetweenCurrent = spaceBetweenDefault;
    int spaceBetweenNext = 10;

    // EYE LEFT - Coordinates
    int eyeLxDefault;
    int eyeLyDefault;
    int eyeLx;
    int eyeLy;
    int eyeLxNext;
    int eyeLyNext;

    // EYE RIGHT - Coordinates
    int eyeRxDefault;
    int eyeRyDefault;
    int eyeRx;
    int eyeRy;
    int eyeRxNext;
    int eyeRyNext;

    // BOTH EYES 
    // Eyelid top size
    byte eyelidsHeightMax; // top eyelids max height
    byte eyelidsTiredHeight = 0;
    byte eyelidsTiredHeightNext = eyelidsTiredHeight;
    byte eyelidsAngryHeight = 0;
    byte eyelidsAngryHeightNext = eyelidsAngryHeight;
    // Bottom happy eyelids offset
    byte eyelidsHappyBottomOffsetMax;
    byte eyelidsHappyBottomOffset = 0;
    byte eyelidsHappyBottomOffsetNext = 0;

    //*********************************************************************************************
    //  Macro Animations
    //*********************************************************************************************

    // Animation - horizontal flicker/shiver
    bool hFlicker = 0;
    bool hFlickerAlternate = 0;
    byte hFlickerAmplitude = 2;

    // Animation - vertical flicker/shiver
    bool vFlicker = 0;
    bool vFlickerAlternate = 0;
    byte vFlickerAmplitude = 2;

    // Animation - auto blinking
    bool autoblinker = 0; // activate auto blink animation
    int blinkInterval = 1; // basic interval between each blink in full seconds
    int blinkIntervalVariation = 4; // interval variaton range in full seconds, random number inside of given range will be add to the basic blinkInterval, set to 0 for no variation
    unsigned long blinktimer = 0; // for organising eyeblink timing

    // Animation - idle mode: eyes looking in random directions
    bool idle = 0;
    int idleInterval = 1; // basic interval between each eye repositioning in full seconds
    int idleIntervalVariation = 3; // interval variaton range in full seconds, random number inside of given range will be add to the basic idleInterval, set to 0 for no variation
    unsigned long idleAnimationTimer = 0; // for organising eyeblink timing

    // Animation - eyes confused: eyes shaking left and right
    bool confused = 0;
    unsigned long confusedAnimationTimer = 0;
    int confusedAnimationDuration = 500;
    bool confusedToggle = 1;

    // Animation - eyes laughing: eyes shaking up and down
    bool laugh = 0;
    unsigned long laughAnimationTimer = 0;
    int laughAnimationDuration = 500;
    bool laughToggle = 1;


    //*********************************************************************************************
    //  GENERAL METHODS
    //*********************************************************************************************

    // Initialize eye coordinates based on screen dimensions
    void initEyeCoordinates() {
        Serial.println("Initializing eye coordinates");
        // Calculate default eye positions
        eyeLxDefault = ((screenWidth)-(eyeLwidthDefault+spaceBetweenDefault+eyeRwidthDefault))/2;
        eyeLyDefault = ((screenHeight-eyeLheightDefault)/2);
        eyeLx = eyeLxDefault;
        eyeLy = eyeLyDefault;
        eyeLxNext = eyeLx;
        eyeLyNext = eyeLy;

        // Eye right default positions
        eyeRxDefault = eyeLx+eyeLwidthCurrent+spaceBetweenDefault;
        eyeRyDefault = eyeLy;
        eyeRx = eyeRxDefault;
        eyeRy = eyeRyDefault;
        eyeRxNext = eyeRx;
        eyeRyNext = eyeRy;

        // Set other size-dependent values
        eyelidsHeightMax = eyeLheightDefault/2;
        eyelidsHappyBottomOffsetMax = (eyeLheightDefault/2)+3;
        
        Serial.print("Eye positions: L(");
        Serial.print(eyeLx);
        Serial.print(",");
        Serial.print(eyeLy);
        Serial.print(") R(");
        Serial.print(eyeRx);
        Serial.print(",");
        Serial.print(eyeRy);
        Serial.println(")");
    }

    // Initialize single sprite for both eyes and sprite for background
    void initSprite() {
        Serial.println("Initializing sprite");
        
        // Clean up any existing sprite first
        freeSprite();
        
        // Allocate new sprite
        _sprite = new TFT_eSprite(_tft);
        _bgSprite = new TFT_eSprite(_tft);
        
        if (!_sprite) {
            Serial.println("ERROR: Failed to allocate sprite object!");
            return;
        }

        if (!_bgSprite) {
            Serial.println("ERROR: Failed to allocate bg sprite object!");
            return;
        }
        
        // Create sprite with the size of the screen - this will hold both eyes
        if (!_sprite->createSprite(screenWidth, screenHeight)) {
            Serial.println("ERROR: Failed to create sprite!");
            freeSprite();
            return;
        }

        if (!_bgSprite->createSprite(screenWidth, screenHeight)) {
            Serial.println("ERROR: Failed to create bg sprite!");
            freeSprite();
            return;
        }
        
        // Set color depth (8-bit is more memory efficient)
        _sprite->setColorDepth(16);
        _bgSprite->setColorDepth(16);
        _bgSprite->setSwapBytes(true);

        // Initialize sprite content
        _sprite->fillSprite(_bgColor);
        _bgSprite->fillSprite(_bgColor);
        
        Serial.print("Sprites created: ");
        Serial.print(screenWidth);
        Serial.print("x");
        Serial.print(screenHeight);
        Serial.println(" pixels");
        
        _spriteInitialized = true;
        _bgSpriteInitialized = true;
    }

    // Startup RoboEyes with defined screen-width, screen-height and max. frames per second
    void begin(int width, int height, byte frameRate) {
        Serial.println("Begin method called");
        screenWidth = width; // TFT display width, in pixels
        screenHeight = height; // TFT display height, in pixels
        
        Serial.print("Screen size: ");
        Serial.print(screenWidth);
        Serial.print("x");
        Serial.println(screenHeight);

        rm67162_init();
        lcd_setRotation(1);
        
        // Initialize coordinates after setting screen dimensions
        initEyeCoordinates();
        
        // Initialize sprites
        initSprite();
        
        // Clear the display 
        //_tft->fillScreen(_bgColor);
        lcd_fill(0, 0, 536, 240, TFT_BLACK);
        
        eyeLheightCurrent = 1; // start with closed eyes
        eyeRheightCurrent = 1; // start with closed eyes
        setFramerate(frameRate); // calculate frame interval based on defined frameRate
        
        Serial.println("Begin method completed");
    }

    void update() {
        // Limit drawing updates to defined max framerate
        if(millis()-fpsTimer >= frameInterval) {
            if (_spriteInitialized) {
                //Serial.println("Update eyes called");
                if(gifstatus){
                  background = true;
                }
                if (background && _bgSprite && gifstatus){
                    //_bgSprite->fillSprite(TFT_CYAN);
                      _bgSprite->fillSprite(_bgColor);

                    if(millis() - gifTimer >= gifInterval) {
                        i = (i + 1) % frames;  // Update frame index
                        gifTimer = millis();   // Reset timer
                    }

                    _bgSprite->pushImage(gifxpos,gifypos,gifWidth,gifHeight,(uint16_t*) backGif[i]);
                    //lcd_PushColors(0, 0, _bgSprite->width(), _bgSprite->height(), (uint16_t*)_bgSprite->getPointer());
                    //Serial.println("Background set");
                }

                if(batIndStatus){
                  if(!gifstatus){
                    _bgSprite->fillSprite(_bgColor);
                  }
                  drawBatteryIndicator();
                }

                drawEyes();
                
                //Push the sprite to the display
                if (_sprite && background) {
                    //_sprite->pushSprite(0, 0);
                    /*
                    Serial.println("pushing to lcd now with coordinates, Right y:");
                    Serial.println(eyeRy);
                    Serial.println(" x:");
                    Serial.println(eyeRx);
                    Serial.println("Left y:");
                    Serial.println(eyeLy);
                    Serial.println(" x:");
                    Serial.println(eyeLx);
                    */
                    //spriteWidth = _sprite->width();
                    //spriteHeight = _sprite->height();
                    //spritePointer = (uint16_t*)_sprite->getPointer();
                    //lcd_fill(0, 0, 536, 240, TFT_BLACK);
                    lcd_PushColors(0, 0, _bgSprite->width(), _bgSprite->height(), (uint16_t*)_bgSprite->getPointer());
                    //delay(5);
                    //lcd_PushColors((uint16_t*)_sprite->getPointer(), _sprite->width()*_sprite->height());
                }else if (_sprite){
                    lcd_PushColors(0, 0, _sprite->width(), _sprite->height(), (uint16_t*)_sprite->getPointer());
                }
            } else {
                Serial.println("WARNING: update called but sprite not initialized!");
                // Try to initialize again
                initSprite();
                if (!_spriteInitialized) {
                    // Draw a rectangle directly to show we're running
                    _tft->fillRect(10, 10, 20, 20, TFT_BLUE);
                }
            }            
            fpsTimer = millis();
            //i=(i+1)%frames;
        }
    }


    //*********************************************************************************************
    //  SETTERS METHODS
    //*********************************************************************************************

    // Calculate frame interval based on defined frameRate
    void setFramerate(byte fps) {
        frameInterval = 1000/fps;
    }

    void setWidth(byte leftEye, byte rightEye) {
        Serial.println("Setting eye widths");
        eyeLwidthNext = leftEye;
        eyeRwidthNext = rightEye;
        eyeLwidthDefault = leftEye;
        eyeRwidthDefault = rightEye;
    }

    void setHeight(byte leftEye, byte rightEye) {
        Serial.println("Setting eye heights");
        eyeLheightNext = leftEye;
        eyeRheightNext = rightEye;
        eyeLheightDefault = leftEye;
        eyeRheightDefault = rightEye;
    }

    // Set border radius for left and right eye
    void setBorderradius(byte leftEye, byte rightEye) {
        eyeLborderRadiusNext = leftEye;
        eyeRborderRadiusNext = rightEye;
        eyeLborderRadiusDefault = leftEye;
        eyeRborderRadiusDefault = rightEye;
    }

    // Set space between the eyes, can also be negative
    void setSpacebetween(int space) {
        spaceBetweenNext = space;
        spaceBetweenDefault = space;
    }

    // Set mood expression
    void setMood(unsigned char mood) {
        switch (mood) {
        case TIRED:
            tired=1; 
            angry=0; 
            happy=0;
            break;
        case ANGRY:
            tired=0; 
            angry=1; 
            happy=0;
            break;
        case HAPPY:
            tired=0; 
            angry=0; 
            happy=1;
            break;
        default:
            tired=0; 
            angry=0; 
            happy=0;
            break;
        }
    }

    // Set predefined position
    void setPosition(unsigned char position) {
        switch (position) {
        case N:
            // North, top center
            eyeLxNext = getScreenConstraint_X()/2;
            eyeLyNext = 0;
            break;
        case NE:
            // North-east, top right
            eyeLxNext = getScreenConstraint_X();
            eyeLyNext = 0;
            break;
        case E:
            // East, middle right
            eyeLxNext = getScreenConstraint_X();
            eyeLyNext = getScreenConstraint_Y()/2;
            break;
        case SE:
            // South-east, bottom right
            eyeLxNext = getScreenConstraint_X();
            eyeLyNext = getScreenConstraint_Y();
            break;
        case S:
            // South, bottom center
            eyeLxNext = getScreenConstraint_X()/2;
            eyeLyNext = getScreenConstraint_Y();
            break;
        case SW:
            // South-west, bottom left
            eyeLxNext = 0;
            eyeLyNext = getScreenConstraint_Y();
            break;
        case W:
            // West, middle left
            eyeLxNext = 0;
            eyeLyNext = getScreenConstraint_Y()/2;
            break;
        case NW:
            // North-west, top left
            eyeLxNext = 0;
            eyeLyNext = 0;
            break;
        default:
            // Middle center
            eyeLxNext = getScreenConstraint_X()/2;
            eyeLyNext = getScreenConstraint_Y()/2;
            break;
        }
    }

    // Set automated eye blinking, minimal blink interval in full seconds and blink interval variation range in full seconds
    void setAutoblinker(bool active, int interval, int variation) {
        autoblinker = active;
        blinkInterval = interval;
        blinkIntervalVariation = variation;
    }
    
    void setAutoblinker(bool active) {
        autoblinker = active;
    }

    // Set idle mode - automated eye repositioning, minimal time interval in full seconds and time interval variation range in full seconds
    void setIdleMode(bool active, int interval, int variation) {
        idle = active;
        idleInterval = interval;
        idleIntervalVariation = variation;
    }
    
    void setIdleMode(bool active) {
        idle = active;
    }

    // Set curious mode - the respectively outer eye gets larger when looking left or right
    void setCuriosity(bool curiousBit) {
        curious = curiousBit;
    }

    // Set cyclops mode - show only one eye 
    void setCyclops(bool cyclopsBit) {
        cyclops = cyclopsBit;
    }

    // Set horizontal flickering (displacing eyes left/right)
    void setHFlicker(bool flickerBit, byte Amplitude) {
        hFlicker = flickerBit; // turn flicker on or off
        hFlickerAmplitude = Amplitude; // define amplitude of flickering in pixels
    }
    
    void setHFlicker(bool flickerBit) {
        hFlicker = flickerBit; // turn flicker on or off
    }

    // Set vertical flickering (displacing eyes up/down)
    void setVFlicker(bool flickerBit, byte Amplitude) {
        vFlicker = flickerBit; // turn flicker on or off
        vFlickerAmplitude = Amplitude; // define amplitude of flickering in pixels
    }
    
    void setVFlicker(bool flickerBit) {
        vFlicker = flickerBit; // turn flicker on or off
    }

    // Set colors for eyes
    void setColors(uint16_t bgColor, uint16_t mainColor) {
        _bgColor = bgColor;
        _mainColor = mainColor;
    }
    
    // Method to get pointer to sprite
    TFT_eSprite* getSprite() {
        if (!_spriteInitialized || !_sprite) {
            Serial.println("WARNING: Trying to get sprite before initialization");
            return NULL;
        }
        return _sprite;
    }

    void setBackground (bool bg, int width = 0, int height = 0, int x = 0, int y = 0, int frameCount = 0, uint16_t** gif = NULL){
        if (bg){
          gifxpos = x;
          gifypos = y;
          gifWidth = width;
          gifHeight = height;
          background = bg;
          frames = frameCount;
          backGif = (uint16_t**)gif;
          gifstatus = true;
        }else {
          background = bg;
          gifstatus = bg;
          Serial.println("BG reset: Clearing screen");
          _bgSprite->fillSprite(_bgColor);
          lcd_PushColors(0, 0, _bgSprite->width(), _bgSprite->height(), (uint16_t*)_bgSprite->getPointer());
          //lcd_PushColors(0, 0, _sprite->width(), _sprite->height(), (uint16_t*)_sprite->getPointer());
        }
    }

    void setBatteryPercentage (int bat) {
        batteryPercentage = bat;
    }

    void setBattery(bool battery){
      batIndStatus = battery;
      background = false;
    }


    //*********************************************************************************************
    //  GETTERS METHODS
    //*********************************************************************************************

    // Returns the max x position for left eye
    int getScreenConstraint_X() {
        return screenWidth-eyeLwidthCurrent-spaceBetweenCurrent-eyeRwidthCurrent;
    } 

    // Returns the max y position for left eye
    int getScreenConstraint_Y() {
        return screenHeight-eyeLheightDefault; // using default height here, because height will vary when blinking and in curious mode
    }

    bool getGifStatus() {
      return gifstatus;
    }


    //*********************************************************************************************
    //  BASIC ANIMATION METHODS
    //*********************************************************************************************

    // BLINKING FOR BOTH EYES AT ONCE
    // Close both eyes
    void close() {
        eyeLheightNext = 1; // closing left eye
        eyeRheightNext = 1; // closing right eye
        eyeL_open = 0; // left eye not opened (=closed)
        eyeR_open = 0; // right eye not opened (=closed)
    }

    // Open both eyes
    void open() {
        eyeL_open = 1; // left eye opened - if true, drawEyes() will take care of opening eyes again
        eyeR_open = 1; // right eye opened
    }

    // Trigger eyeblink animation
    void blink() {
        Serial.println("blink called");
        close();
        open();
    }

    // BLINKING FOR SINGLE EYES, CONTROL EACH EYE SEPARATELY
    // Close eye(s)
    void close(bool left, bool right) {
        if(left) {
            eyeLheightNext = 1; // blinking left eye
            eyeL_open = 0; // left eye not opened (=closed)
        }
        if(right) {
            eyeRheightNext = 1; // blinking right eye
            eyeR_open = 0; // right eye not opened (=closed)
        }
    }

    // Open eye(s)
    void open(bool left, bool right) {
        if(left) {
            eyeL_open = 1; // left eye opened - if true, drawEyes() will take care of opening eyes again
        }
        if(right) {
            eyeR_open = 1; // right eye opened
        }
    }

    // Trigger eyeblink(s) animation
    void blink(bool left, bool right) {
        close(left, right);
        open(left, right);
    }


    //*********************************************************************************************
    //  MACRO ANIMATION METHODS
    //*********************************************************************************************

    // Play confused animation - one shot animation of eyes shaking left and right
    void anim_confused() {
        confused = 1;
    }

    // Play laugh animation - one shot animation of eyes shaking up and down
    void anim_laugh() {
        laugh = 1;
    }

    //*********************************************************************************************
    //  DRAWING METHODS
    //*********************************************************************************************


    void drawBatteryIndicator() {

        background = true;
        //gifstatus = false;
        // Draw battery outline
        _bgSprite->drawRect(BATTERY_X, BATTERY_Y, BATTERY_WIDTH, BATTERY_HEIGHT, TFT_WHITE);
        // Small battery terminal
        _bgSprite->fillRect(BATTERY_X + BATTERY_WIDTH, BATTERY_Y + 4, 4, BATTERY_HEIGHT - 8, TFT_WHITE);
        
        // Battery fill level
        int fillWidth = (BATTERY_WIDTH - 4) * batteryPercentage / 100;
        
        // Choose color based on level
        uint16_t fillColor;
        if (batteryPercentage < 15) {
            fillColor = TFT_RED;
        } else if (batteryPercentage < 30) {
            fillColor = TFT_ORANGE;
        } else {
            fillColor = TFT_GREEN;
        }
        
        // Set text properties
        _bgSprite->setTextFont(4);
        _bgSprite->setTextSize(1);           // Text size multiplier
        _bgSprite->setTextColor(TFT_PURPLE);  // Text color
        //_sprite->setTextDatum(MC_DATUM);   // Middle-Center text alignment

        // Format the battery percentage text
        char batteryText[10];
        sprintf(batteryText, "%d%%", batteryPercentage);
        
        // Draw fill level
        _bgSprite->fillRect(BATTERY_X + 2, BATTERY_Y + 2, fillWidth, BATTERY_HEIGHT - 4, fillColor);
        _bgSprite->drawString(batteryText, BATTERY_X + 4, BATTERY_Y + 2);
        //Serial.println("Drew battery");
    }

    void drawEyes() {
        // Ensure sprite is initialized
        if (!_spriteInitialized || !_sprite) {
            Serial.println("ERROR: drawEyes called but sprite not initialized!");
            _tft->fillScreen(TFT_RED); // Show error on screen
            delay(500);
            initSprite();
            return;
        }
        
        //// PRE-CALCULATIONS - EYE SIZES AND VALUES FOR ANIMATION TWEENINGS ////

        // Vertical size offset for larger eyes when looking left or right (curious gaze)
        if(curious) {
            if(eyeLxNext<=50) {
                eyeLheightOffset=20;
            } else if (eyeLxNext>=(getScreenConstraint_X()-10) && cyclops) {
                eyeLheightOffset=8;
            } else {
                eyeLheightOffset=0;
            } // left eye
            
            if(eyeRxNext>=screenWidth-eyeRwidthCurrent-50) {
                eyeRheightOffset=20;
            } else {
                eyeRheightOffset=0;
            } // right eye
        } else {
            eyeLheightOffset=0; // reset height offset for left eye
            eyeRheightOffset=0; // reset height offset for right eye
        }
        
        // Left eye height
        eyeLheightCurrent = (eyeLheightCurrent + eyeLheightNext + eyeLheightOffset)/2;
        eyeLy+= ((eyeLheightDefault-eyeLheightCurrent)/2); // vertical centering of eye when closing
        eyeLy-= eyeLheightOffset/2;
        // Right eye height
        eyeRheightCurrent = (eyeRheightCurrent + eyeRheightNext + eyeRheightOffset)/2;
        eyeRy+= (eyeRheightDefault-eyeRheightCurrent)/2; // vertical centering of eye when closing
        eyeRy-= eyeRheightOffset/2;
        
        // Open eyes again after closing them
        if(eyeL_open) {
            if(eyeLheightCurrent <= 1 + eyeLheightOffset) {
                eyeLheightNext = eyeLheightDefault;
            } 
        }
        if(eyeR_open) {
            if(eyeRheightCurrent <= 1 + eyeRheightOffset) {
                eyeRheightNext = eyeRheightDefault;
            } 
        }
        
        // Left eye width
        eyeLwidthCurrent = (eyeLwidthCurrent + eyeLwidthNext)/2;
        // Right eye width
        eyeRwidthCurrent = (eyeRwidthCurrent + eyeRwidthNext)/2;
        
        // Space between eyes
        spaceBetweenCurrent = (spaceBetweenCurrent + spaceBetweenNext)/2;

        // Left eye coordinates
        eyeLx = (eyeLx + eyeLxNext)/2;
        eyeLy = (eyeLy + eyeLyNext)/2;
        // Right eye coordinates
        eyeRxNext = eyeLxNext+eyeLwidthCurrent+spaceBetweenCurrent; // right eye's x position depends on left eyes position + the space between
        eyeRyNext = eyeLyNext; // right eye's y position should be the same as for the left eye
        eyeRx = (eyeRx + eyeRxNext)/2;
        eyeRy = (eyeRy + eyeRyNext)/2;
        
        // Left eye border radius
        eyeLborderRadiusCurrent = (eyeLborderRadiusCurrent + eyeLborderRadiusNext)/2;
        // Right eye border radius
        eyeRborderRadiusCurrent = (eyeRborderRadiusCurrent + eyeRborderRadiusNext)/2;
        
        //// APPLYING MACRO ANIMATIONS ////

        if(autoblinker) {
            if(millis() >= blinktimer) {
                blink();
                Serial.println("blink");
                blinktimer = millis()+(blinkInterval*1000)+(random(blinkIntervalVariation)*1000); // calculate next time for blinking
            }
        }

        // Laughing - eyes shaking up and down for the duration defined by laughAnimationDuration (default = 500ms)
        if(laugh) {
            if(laughToggle) {
                setVFlicker(1, 5);
                laughAnimationTimer = millis();
                laughToggle = 0;
            } else if(millis() >= laughAnimationTimer+laughAnimationDuration) {
                setVFlicker(0, 0);
                laughToggle = 1;
                laugh=0; 
            }
        }

        // Confused - eyes shaking left and right for the duration defined by confusedAnimationDuration (default = 500ms)
        if(confused) {
            if(confusedToggle) {
                setHFlicker(1, 20);
                confusedAnimationTimer = millis();
                confusedToggle = 0;
            } else if(millis() >= confusedAnimationTimer+confusedAnimationDuration) {
                setHFlicker(0, 0);
                confusedToggle = 1;
                confused=0; 
            }
        }

        // Idle - eyes moving to random positions on screen
        if(idle) {
            if(millis() >= idleAnimationTimer) {
                eyeLxNext = random(getScreenConstraint_X());
                eyeLyNext = random(getScreenConstraint_Y());
                idleAnimationTimer = millis()+(idleInterval*1000)+(random(idleIntervalVariation)*1000); // calculate next time for eyes repositioning
            }
        }

        // Adding offsets for horizontal flickering/shivering
        if(hFlicker) {
            if(hFlickerAlternate) {
                eyeLx += hFlickerAmplitude;
                eyeRx += hFlickerAmplitude;
            } else {
                eyeLx -= hFlickerAmplitude;
                eyeRx -= hFlickerAmplitude;
            }
            hFlickerAlternate = !hFlickerAlternate;
        }

        // Adding offsets for vertical flickering/shivering
        if(vFlicker) {
            if(vFlickerAlternate) {
                eyeLy += vFlickerAmplitude;
                eyeRy += vFlickerAmplitude;
            } else {
                eyeLy -= vFlickerAmplitude;
                eyeRy -= vFlickerAmplitude;
            }
            vFlickerAlternate = !vFlickerAlternate;
        }

        // Cyclops mode, set second eye's size and space between to 0
        if(cyclops) {
            eyeRwidthCurrent = 0;
            eyeRheightCurrent = 0;
            spaceBetweenCurrent = 0;
        }

        //// PREPARE MOOD TRANSITIONS ////
        
        // Prepare mood type transitions
        if (tired) {
            eyelidsTiredHeightNext = eyeLheightCurrent/2; 
            eyelidsAngryHeightNext = 0;
        } else {
            eyelidsTiredHeightNext = 0;
        }
        
        if (angry) {
            eyelidsAngryHeightNext = eyeLheightCurrent/2;
            eyelidsTiredHeightNext = 0;
        } else {
            eyelidsAngryHeightNext = 0;
        }
        
        if (happy) {
            eyelidsHappyBottomOffsetNext = eyeLheightCurrent/2;
        } else {
            eyelidsHappyBottomOffsetNext = 0;
        }
        
        // Calculate current mood transition values through easing
        eyelidsTiredHeight = (eyelidsTiredHeight + eyelidsTiredHeightNext)/2;
        eyelidsAngryHeight = (eyelidsAngryHeight + eyelidsAngryHeightNext)/2;
        eyelidsHappyBottomOffset = (eyelidsHappyBottomOffset + eyelidsHappyBottomOffsetNext)/2;
        //Serial.println("all calculations done, prepping for actual drawings");
        if (background && _bgSprite){
            try {
                //// ACTUAL DRAWINGS WITH SPRITE ////
                
                // Clear sprite for next frame
                //_sprite->fillSprite(_bgColor);
                
                // Draw basic eye rectangles
                _bgSprite->fillRoundRect(eyeLx, eyeLy, eyeLwidthCurrent, eyeLheightCurrent, 
                                    eyeLborderRadiusCurrent, _mainColor); // left eye
                
                if (!cyclops) {
                    _bgSprite->fillRoundRect(eyeRx, eyeRy, eyeRwidthCurrent, eyeRheightCurrent, 
                                        eyeRborderRadiusCurrent, _mainColor); // right eye
                }
                //Serial.println("drew basic eye");
                // Draw tired top eyelids
                if (tired) {
                    if (!cyclops) {
                        _bgSprite->fillTriangle(eyeLx, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, 
                                            eyeLx, eyeLy+eyelidsTiredHeight-1, _bgColor); // left eye 
                        _bgSprite->fillTriangle(eyeRx, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy-1, 
                                            eyeRx+eyeRwidthCurrent, eyeRy+eyelidsTiredHeight-1, _bgColor); // right eye
                    } else {
                        // Cyclops tired eyelids
                        _bgSprite->fillTriangle(eyeLx, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy-1, 
                                            eyeLx, eyeLy+eyelidsTiredHeight-1, _bgColor); // left eyelid half
                        _bgSprite->fillTriangle(eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, 
                                            eyeLx+eyeLwidthCurrent, eyeLy+eyelidsTiredHeight-1, _bgColor); // right eyelid half
                    }
                //Serial.println("drew tired eye");
                }
                // Draw angry top eyelids
                if (angry) {
                    if (!cyclops) { 
                        _bgSprite->fillTriangle(eyeLx, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, 
                                            eyeLx+eyeLwidthCurrent, eyeLy+eyelidsAngryHeight-1, _bgColor); // left eye
                        _bgSprite->fillTriangle(eyeRx, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy-1, 
                                            eyeRx, eyeRy+eyelidsAngryHeight-1, _bgColor); // right eye
                    } else {
                        // Cyclops angry eyelids
                        _bgSprite->fillTriangle(eyeLx, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy-1, 
                                            eyeLx+(eyeLwidthCurrent/2), eyeLy+eyelidsAngryHeight-1, _bgColor); // left eyelid half
                        _bgSprite->fillTriangle(eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, 
                                            eyeLx+(eyeLwidthCurrent/2), eyeLy+eyelidsAngryHeight-1, _bgColor); // right eyelid half
                    }
                //Serial.println("drew angry eye");
                }
                // Draw happy bottom eyelids
                if (happy) {
                    _bgSprite->fillRoundRect(eyeLx-1, (eyeLy+eyeLheightCurrent)-eyelidsHappyBottomOffset+1, 
                                        eyeLwidthCurrent+2, eyeLheightDefault, 
                                        eyeLborderRadiusCurrent+13, _bgColor); // left eye
                                        
                    if (!cyclops) { 
                        _bgSprite->fillRoundRect(eyeRx-1, (eyeRy+eyeRheightCurrent)-eyelidsHappyBottomOffset+1, 
                                            eyeRwidthCurrent+2, eyeRheightDefault, 
                                            eyeRborderRadiusCurrent+13, _bgColor); // right eye
                    }
                //Serial.println("drew happy eye");
                }
            } catch (...) {
            Serial.println("ERROR: Exception caught during sprite rendering");
            // Draw an error indicator
            //_tft->fillTriangle(10, 10, 30, 10, 20, 30, TFT_RED);
            }
        }
        else{
            try {
                //// ACTUAL DRAWINGS WITH SPRITE ////
                
                // Clear sprite for next frame
                _sprite->fillSprite(_bgColor);
                
                // Draw basic eye rectangles
                _sprite->fillRoundRect(eyeLx, eyeLy, eyeLwidthCurrent, eyeLheightCurrent, 
                                    eyeLborderRadiusCurrent, _mainColor); // left eye
                
                if (!cyclops) {
                    _sprite->fillRoundRect(eyeRx, eyeRy, eyeRwidthCurrent, eyeRheightCurrent, 
                                        eyeRborderRadiusCurrent, _mainColor); // right eye
                }
                //Serial.println("drew basic eye");
                // Draw tired top eyelids
                if (tired) {
                    if (!cyclops) {
                        _sprite->fillTriangle(eyeLx, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, 
                                            eyeLx, eyeLy+eyelidsTiredHeight-1, _bgColor); // left eye 
                        _sprite->fillTriangle(eyeRx, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy-1, 
                                            eyeRx+eyeRwidthCurrent, eyeRy+eyelidsTiredHeight-1, _bgColor); // right eye
                    } else {
                        // Cyclops tired eyelids
                        _sprite->fillTriangle(eyeLx, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy-1, 
                                            eyeLx, eyeLy+eyelidsTiredHeight-1, _bgColor); // left eyelid half
                        _sprite->fillTriangle(eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, 
                                            eyeLx+eyeLwidthCurrent, eyeLy+eyelidsTiredHeight-1, _bgColor); // right eyelid half
                    }
                //Serial.println("drew tired eye");
                }
                // Draw angry top eyelids
                if (angry) {
                    if (!cyclops) { 
                        _sprite->fillTriangle(eyeLx, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, 
                                            eyeLx+eyeLwidthCurrent, eyeLy+eyelidsAngryHeight-1, _bgColor); // left eye
                        _sprite->fillTriangle(eyeRx, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy-1, 
                                            eyeRx, eyeRy+eyelidsAngryHeight-1, _bgColor); // right eye
                    } else {
                        // Cyclops angry eyelids
                        _sprite->fillTriangle(eyeLx, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy-1, 
                                            eyeLx+(eyeLwidthCurrent/2), eyeLy+eyelidsAngryHeight-1, _bgColor); // left eyelid half
                        _sprite->fillTriangle(eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, 
                                            eyeLx+(eyeLwidthCurrent/2), eyeLy+eyelidsAngryHeight-1, _bgColor); // right eyelid half
                    }
                //Serial.println("drew angry eye");
                }
                // Draw happy bottom eyelids
                if (happy) {
                    _sprite->fillRoundRect(eyeLx-1, (eyeLy+eyeLheightCurrent)-eyelidsHappyBottomOffset+1, 
                                        eyeLwidthCurrent+2, eyeLheightDefault, 
                                        eyeLborderRadiusCurrent+13, _bgColor); // left eye
                                        
                    if (!cyclops) { 
                        _sprite->fillRoundRect(eyeRx-1, (eyeRy+eyeRheightCurrent)-eyelidsHappyBottomOffset+1, 
                                            eyeRwidthCurrent+2, eyeRheightDefault, 
                                            eyeRborderRadiusCurrent+13, _bgColor); // right eye
                    }
                //Serial.println("drew happy eye");
                }
            } catch (...) {
            Serial.println("ERROR: Exception caught during sprite rendering");
            // Draw an error indicator
            //_tft->fillTriangle(10, 10, 30, 10, 20, 30, TFT_RED);
            }
        }
    } // end of drawEyes method

}; // end of class roboEyes_Sprite

#endif
