#ifndef MICRO_DOME_H
#define MICRO_DOME_H

#ifdef ARDUINO
#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#include <pins_arduino.h>
#endif

#ifdef USE_TINYUSB // For Serial when selecting TinyUSB
#include <Adafruit_TinyUSB.h>
#endif

#endif

#ifdef TARGET_LPC1768
#include <Arduino.h>
#endif

// Blink Directions Right Left Up 


#define RUL ((2 << 6) | (2 << 4) | (0 << 2) | (1)) ///< Transmit In Direction Right -> Left -> Up 



#define NEO_KHZ800 0x0000 ///< 800 KHz data transmission
#ifndef __AVR_ATtiny85__
#define NEO_KHZ400 0x0100 ///< 400 KHz data transmission
#endif


#ifdef NEO_KHZ400
typedef uint16_t ledDirType; ///< 3rd arg to MicroDome constructor
#else
typedef uint8_t ledDirType; ///< 3rd arg to MicroDome constructor
#endif



class MicroDome {

public:
  // Constructor: number of LEDs, pin number, LED type of Rotation
  MicroDome(uint16_t index, int16_t pin = 6, ledDirType type = RUL);
  MicroDome(void);
  ~MicroDome();

  void begin(void);
  void show(void);
  void setPin(int16_t Pin);
  void setTile(uint16_t index, uint32_t levels); // 
  void setBrightness(uint8_t);
  void clear(void);
  void updateLength(uint16_t index);
  void updateType(ledDirType t);
  /*!
    @brief   Check whether a call to show() will start sending data
             immediately or will 'block' for a required interval. NeoPixels
             require a short quiet time (about 300 microseconds) after the
             last bit is received before the data 'latches' and new data can
             start being received. Usually one's sketch is implicitly using
             this time to generate a new frame of animation...but if it
             finishes very quickly, this function could be used to see if
             there's some idle time available for some low-priority
             concurrent task.
    @return  1 or true if show() will start sending immediately, 0 or false
             if show() would block (meaning some idle time is available).
  */
  bool canShow(void) {
    // It's normal and possible for endTime to exceed micros() if the
    // 32-bit clock counter has rolled over (about every 70 minutes).
    // Since both are uint32_t, a negative delta correctly maps back to
    // positive space, and it would seem like the subtraction below would
    // suffice. But a problem arises if code invokes show() very
    // infrequently...the micros() counter may roll over MULTIPLE times in
    // that interval, the delta calculation is no longer correct and the
    // next update may stall for a very long time. The check below resets
    // the latch counter if a rollover has occurred. This can cause an
    // extra delay of up to 300 microseconds in the rare case where a
    // show() call happens precisely around the rollover, but that's
    // neither likely nor especially harmful, vs. other code that might
    // stall for 30+ minutes, or having to document and frequently remind
    // and/or provide tech support explaining an unintuitive need for
    // show() calls at least once an hour.
    uint32_t now = micros();
    if (endTime > now) {
      endTime = now;
    }
    return (now - endTime) >= 300L;
  }
  
  uint8_t *getPixels(void) const { return pixels; };
  uint8_t getBrightness(void) const;
 
  int8_t getPin(void) const { return pin; };
 
  uint16_t numPixels(void) const { return (tilesNumber * numLEDs); }
  uint32_t getLedLevels(uint16_t index) const;
  

  static uint32_t setLedLevels(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
  }
  /*!
    @brief   Convert separate red, green, blue and white values into a
             single "packed" 32-bit WRGB color.
    @param   r  Red brightness, 0 to 255.
    @param   g  Green brightness, 0 to 255.
    @param   b  Blue brightness, 0 to 255.
    
    @return  32-bit packed value, which can then be assigned to a
             variable for later use.
  */
  
protected:
#ifdef NEO_KHZ400 // If 400 KHz NeoPixel support enabled...
  bool is800KHz; ///< true if 800 KHz pixels
#endif
  bool begun;         ///< true if begin() previously called
  uint16_t tilesNumber; ///< Number of all Tiles 
  uint16_t numLEDs ;   ///< Number of  LEDs in strip
  uint16_t numBytes;  ///< Size of 'pixels' buffer below
  int16_t pin;        ///< Output pin number (-1 if not yet set)
  uint8_t brightness; ///< Strip brightness 0-255 (stored as +1)
  uint8_t *pixels;    ///< Holds LED color values (3 or 4 bytes each)
  uint8_t rOffset;    ///< Red index within each 3- or 4-byte pixel
  uint8_t gOffset;    ///< Index of green byte
  uint8_t bOffset;    ///< Index of blue byte
  uint8_t wOffset;    ///< Index of white (==rOffset if no white)
  uint32_t endTime;   ///< Latch timing reference
#ifdef __AVR__
  volatile uint8_t *port; ///< Output PORT register
  uint8_t pinMask;        ///< Output PORT bitmask
#endif
#if defined(ARDUINO_ARCH_STM32) || defined(ARDUINO_ARCH_ARDUINO_CORE_STM32)
  GPIO_TypeDef *gpioPort; ///< Output GPIO PORT
  uint32_t gpioPin;       ///< Output GPIO PIN
#endif
};

#endif // ADAFRUIT_NEOPIXEL_H
