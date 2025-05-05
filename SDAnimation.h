// SDAnimation.h
#ifndef _SD_ANIMATION_H
#define _SD_ANIMATION_H

#include <Arduino.h>
#include "sd_card_bsp.h"

class SDAnimation {
private:
    String _basePath;          // Base path to animation folder
    int _frameCount;           // Total number of frames
    int _width;                // Frame width
    int _height;               // Frame height
    int _currentFrame;         // Currently loaded frame index
    uint16_t** _frameBuffers;  // Array of frame buffer pointers
    int _frameBufferCount;     // Number of frame buffers to keep in memory
    bool _initialized;         // Flag to indicate if initialized

public:
    // Constructor
    SDAnimation() : _frameCount(0), _width(0), _height(0), _currentFrame(0), 
                   _frameBuffers(NULL), _frameBufferCount(2), _initialized(false) {}

    // Destructor
    ~SDAnimation() {
        freeBuffers();
    }

    // Free allocated memory
    void freeBuffers() {
        if (_frameBuffers) {
            for (int i = 0; i < _frameBufferCount; i++) {
                if (_frameBuffers[i]) {
                    free(_frameBuffers[i]);
                    _frameBuffers[i] = NULL;
                }
            }
            free(_frameBuffers);
            _frameBuffers = NULL;
        }
    }

    // Initialize animation from a directory on SD card
    bool begin(const char* animationPath, int buffersToKeep = 2) {
        Serial.printf("Initializing animation from %s\n", animationPath);
        
        // Make sure SD card is initialized - just call it, don't check return value
        SD_card_Init();
        
        _basePath = String(animationPath);
        _frameBufferCount = buffersToKeep;
        
        // Read metadata file
        char metadataPath[100];
        sprintf(metadataPath, "%s/metadata.txt", _basePath.c_str());
        
        FILE* metaFile = fopen(metadataPath, "r");
        if (!metaFile) {
            Serial.printf("Metadata file not found: %s\n", metadataPath);
            return false;
        }
        
        // Read frame count, width, and height
        fscanf(metaFile, "%d,%d,%d", &_frameCount, &_width, &_height);
        fclose(metaFile);
        
        Serial.printf("Animation has %d frames (%dx%d)\n", _frameCount, _width, _height);
        
        // Allocate frame buffers
        _frameBuffers = (uint16_t**)malloc(_frameBufferCount * sizeof(uint16_t*));
        if (!_frameBuffers) {
            Serial.println("Failed to allocate frame buffer pointers");
            return false;
        }
        
        // Initialize all buffers to NULL
        for (int i = 0; i < _frameBufferCount; i++) {
            _frameBuffers[i] = NULL;
        }
        
        // Allocate memory for each buffer
        for (int i = 0; i < _frameBufferCount; i++) {
            _frameBuffers[i] = (uint16_t*)malloc(_width * _height * sizeof(uint16_t));
            if (!_frameBuffers[i]) {
                Serial.printf("Failed to allocate memory for frame buffer %d\n", i);
                freeBuffers();
                return false;
            }
        }
        
        // Load initial frames
        for (int i = 0; i < _frameBufferCount && i < _frameCount; i++) {
            if (!loadFrameToBuffer(i, i % _frameBufferCount)) {
                Serial.printf("Failed to load initial frame %d\n", i);
                freeBuffers();
                return false;
            }
        }
        
        _initialized = true;
        return true;
    }
    
    // Load a specific frame into a buffer slot
    bool loadFrameToBuffer(int frameIndex, int bufferIndex) {
        if (frameIndex >= _frameCount || bufferIndex >= _frameBufferCount) {
            return false;
        }
        
        char framePath[100];
        sprintf(framePath, "%s/frame_%02d.bin", _basePath.c_str(), frameIndex);
        
        FILE* frameFile = fopen(framePath, "rb");
        if (!frameFile) {
            Serial.printf("Failed to open frame file: %s\n", framePath);
            return false;
        }
        
        size_t bytesRead = fread(_frameBuffers[bufferIndex], sizeof(uint16_t), _width * _height, frameFile);
        fclose(frameFile);
        
        return (bytesRead == _width * _height);
    }
    
    // Get the next frame, loading it if needed
    uint16_t* getNextFrame() {
        if (!_initialized || _frameCount == 0) {
            return NULL;
        }
        
        // Calculate which frame to return
        int nextFrame = (_currentFrame + 1) % _frameCount;
        
        // Calculate which buffer to use
        int bufferIndex = nextFrame % _frameBufferCount;
        
        // If the buffer doesn't have the frame we need, load it
        int expectedFrameInBuffer = (nextFrame / _frameBufferCount) * _frameBufferCount + bufferIndex;
        if (expectedFrameInBuffer != nextFrame) {
            if (!loadFrameToBuffer(nextFrame, bufferIndex)) {
                Serial.printf("Failed to load frame %d\n", nextFrame);
                return NULL;
            }
        }
        
        _currentFrame = nextFrame;
        return _frameBuffers[bufferIndex];
    }
    
    // Get a specific frame
    uint16_t* getFrame(int frameIndex) {
        if (!_initialized || frameIndex >= _frameCount) {
            return NULL;
        }
        
        // Calculate which buffer to use
        int bufferIndex = frameIndex % _frameBufferCount;
        
        // If the buffer doesn't have the frame we need, load it
        int expectedFrameInBuffer = (frameIndex / _frameBufferCount) * _frameBufferCount + bufferIndex;
        if (expectedFrameInBuffer != frameIndex) {
            if (!loadFrameToBuffer(frameIndex, bufferIndex)) {
                Serial.printf("Failed to load frame %d\n", frameIndex);
                return NULL;
            }
        }
        
        _currentFrame = frameIndex;
        return _frameBuffers[bufferIndex];
    }
    
    // Get current frame
    uint16_t* getCurrentFrame() {
        if (!_initialized) {
            return NULL;
        }
        
        int bufferIndex = _currentFrame % _frameBufferCount;
        return _frameBuffers[bufferIndex];
    }
    
    // Get frame count
    int getFrameCount() {
        return _frameCount;
    }
    
    // Get frame dimensions
    void getFrameDimensions(int* width, int* height) {
        if (width) *width = _width;
        if (height) *height = _height;
    }
    
    // Get current frame index
    int getCurrentFrameIndex() {
        return _currentFrame;
    }
};

#endif // _SD_ANIMATION_H