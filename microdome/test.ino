#include "microdome.h"

// Define Your OutPut Pin Nb
#define PIN        6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 6

MicroDome dome(NUMPIXELS, PIN, RUL);



void setup() {
  dome.begin(); // INITIALIZE Dome object (REQUIRED)

}

void loop() {
  /*
      chenillard(1000);
      delay(1000);
      toutAllumer();
      delay(2500);
      dome.clear();
      chenillardPile(500);
      delay(2500);

  */

  dome.clear();

  dome.setTile(5, dome.setLedLevels(0, 50, 0));
  dome.show();
  delay(1000);
  dome.clear();

  dome.setTile(5, dome.setLedLevels(0, 0, 50));
  dome.show();
  delay(1000);
  dome.clear();

  dome.setTile(5, dome.setLedLevels(50, 0, 0));
  dome.show();
  delay(3000);
  dome.clear();


  Serial.println(" - " + String((int)dome.getLedLevels(5)));

  dome.setTile(5, dome.setLedLevels(0, 0, 0));
  dome.show();
  dome.clear();


}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// --------    FONCTIONS   ---------  //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/*
  void chenillard(int DELAYVAL) {

  // Pour réaliser un clignotement successif des LED
    //delay : Le délai de clignotement :: Time (in milliseconds) to pause between pixels

  dome.clear(); // Set all pixel colors to 'off'


  for (int i = 0; i < NUMPIXELS; i++)
  {

    dome.setPixelColor(i, dome.Color(50, 0, 0));

    dome.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop

    dome.setPixelColor(i, dome.Color(0, 50, 0));

    dome.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop

    dome.setPixelColor(i, dome.Color(0, 0, 50));

    dome.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop

    dome.clear();

  }
  dome.clear();
  }

  void toutAllumer() {

  dome.clear();
  for (int i = 0; i < NUMPIXELS; i++)
  {
    dome.setPixelColor(i, dome.Color(50, 50, 50));

    dome.show();
  }
  }


  void chenillardPile(int DELAYVAL) {
  // Pour réaliser un clignotement successif des LED
    // delay : Le délai de clignotement :: Time (in milliseconds) to pause between pixels

  dome.clear(); // Set all pixel colors to 'off'


  for (int i = 0; i < NUMPIXELS; i++)
  {

    dome.setPixelColor(i, dome.Color(0, 50, 0));
    //delay(DELAYVAL);
    dome.show();

    dome.setPixelColor(i, dome.Color(50, 0, 0));
    //delay(DELAYVAL); // Pause before next pass through loop
    dome.show();

    dome.setPixelColor(i, dome.Color(0, 0, 50));
    delay(DELAYVAL); // Pause before next pass through loop
    dome.show();

  }
  } */
