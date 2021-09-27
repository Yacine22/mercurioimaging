/**
   Exemple de code pour la bibliothèque VirtualWire – Télécommande DIY (récepteur)
*/

#include <VirtualWire.h>
#include "mario.h"
/** Broches pour les LEDs */
const byte opto1 = 12;
const byte opto2 = 11;
const byte opto3 = 10;
const byte opto4 = 9;
const byte RF = 6;
const byte Buzzer = 5;
const byte PIN_LED_R = 4;
const byte PIN_LED_G = 3;
const byte PIN_LED_B = 2;
/** Différents messages de commande */

const char* CMD_CLICK = "clic";
const char* CMD_TEST = "test";


int cpt = 1;

void setup() {
  Serial.begin(9600);
  Serial.println("SALUT MA GUEULE");
  /* Met les broches des LEDs en sortie et à LOW */
  pinMode(opto1, OUTPUT);
  pinMode(opto2, OUTPUT);
  pinMode(opto3, OUTPUT);
  pinMode(opto4, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);

  digitalWrite(opto1, LOW);
  digitalWrite(opto2, LOW);
  digitalWrite(opto3, LOW);
  digitalWrite(opto4, LOW);
  digitalWrite(Buzzer, LOW);

  digitalWrite(PIN_LED_R, LOW);
  digitalWrite(PIN_LED_G, LOW);
  digitalWrite(PIN_LED_B, HIGH);

  // Initialisation de la bibliothèque VirtualWire
  // Vous pouvez changez les broches RX/TX/PTT avant vw_setup() si nécessaire
  vw_set_rx_pin(RF);
  vw_setup(2000);
  vw_rx_start(); // On peut maintenant recevoir des messages

}

void loop() {
  byte message[VW_MAX_MESSAGE_LEN];
  byte taille_message = VW_MAX_MESSAGE_LEN;

  digitalWrite(PIN_LED_R, 0);
  digitalWrite(PIN_LED_G, 0);
  digitalWrite(PIN_LED_B, 0);

  // On attend de recevoir un message
  vw_wait_rx();
  if (vw_get_message(message, &taille_message)) {
    // On copie le message, qu'il soit corrompu ou non
    String msg = (char*)message;
    Serial.print("Reçu : ");
    Serial.println(msg);

    if (strcmp((char*) message, CMD_CLICK) == 0) {
      digitalWrite(PIN_LED_R, 1);
      digitalWrite(PIN_LED_G, 1);
      digitalWrite(PIN_LED_B, 0);
      Serial.println("CLICK");
      beep();

    } else if (strcmp((char*) message, CMD_TEST) == 0) {
      if (cpt % 4 == 0) mario2();
      //if (cpt %4==0) sing();
      digitalWrite(PIN_LED_R, LOW);
      digitalWrite(PIN_LED_G, LOW);
      digitalWrite(PIN_LED_B, LOW);
      digitalWrite(PIN_LED_R, HIGH);
      Mario();
      Serial.println("TEST");
      cpt++;
      //digitalWrite(PIN_LED_B, !digitalRead(PIN_LED_B));
      //Serial.println("OFF sur pin 4");
    }
  }
}



void Mario() {
  tone(Buzzer, 1976);
  delay(100);
  tone(Buzzer, 2637);
  delay(500);
  noTone(Buzzer);
}
void beep()
{
  tone(Buzzer, 2000);
  delay(150);
  noTone(Buzzer);
}

void sing() {
  // iterate over the notes of the melody:
  int song = 1;
  Serial.println(" 'Mario Theme'");
  int size = sizeof(melody) / sizeof(int);
  for (int thisNote = 0; thisNote < size; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / tempo[thisNote];

    buzz(Buzzer, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    buzz(melodyPin, 0, noteDuration);

  }
}


void buzz(int targetPin, long frequency, long length) {
  digitalWrite(13, HIGH);
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
  digitalWrite(13, LOW);

}

void mario2()
{
  Serial.println("MARIOOOOO");
  tone(Buzzer, 660, 100);
  delay(150);
  tone(Buzzer, 660, 100);
  delay(300);
  tone(Buzzer, 660, 100);
  delay(300);
  tone(Buzzer, 510, 100);
  delay(100);
  tone(Buzzer, 660, 100);
  delay(300);
  tone(Buzzer, 770, 100);
  delay(550);
  tone(Buzzer, 380, 100);
  delay(575);
  tone(Buzzer, 510, 100);
  delay(450);
  tone(Buzzer, 380, 100);
  delay(400);
  tone(Buzzer, 320, 100);
  delay(500);
  tone(Buzzer, 440, 100);
  delay(300);
  tone(Buzzer, 480, 80);
  delay(330);
  tone(Buzzer, 450, 100);
  delay(150);
  tone(Buzzer, 430, 100);
  delay(300);
  tone(Buzzer, 380, 100);
  delay(200);
  tone(Buzzer, 660, 80);
  delay(200);
  tone(Buzzer, 760, 50);
  delay(150);
  tone(Buzzer, 860, 100);
  delay(300);
  tone(Buzzer, 700, 80);
  delay(150);
  tone(Buzzer, 760, 50);
  delay(350);
  tone(Buzzer, 660, 80);
  delay(300);
  tone(Buzzer, 520, 80);
  delay(150);
  tone(Buzzer, 580, 80);
  delay(150);
  tone(Buzzer, 480, 80);
  delay(500);

  tone(Buzzer, 510, 100);
  delay(450);
  tone(Buzzer, 380, 100);
  delay(400);
  tone(Buzzer, 320, 100);
  delay(500);
  tone(Buzzer, 440, 100);
  delay(300);
  tone(Buzzer, 480, 80);
  delay(330);
  tone(Buzzer, 450, 100);
  delay(150);
  tone(Buzzer, 430, 100);
  delay(300);
  tone(Buzzer, 380, 100);
  delay(200);
  tone(Buzzer, 660, 80);
  delay(200);
  tone(Buzzer, 760, 50);
  delay(150);
  tone(Buzzer, 860, 100);
  delay(300);
  tone(Buzzer, 700, 80);
  delay(150);
  tone(Buzzer, 760, 50);
  delay(350);
  tone(Buzzer, 660, 80);
  delay(300);
  tone(Buzzer, 520, 80);
  delay(150);
  tone(Buzzer, 580, 80);
  delay(150);
  tone(Buzzer, 480, 80);
  delay(500);

  tone(Buzzer, 500, 100);
  delay(300);

  tone(Buzzer, 760, 100);
  delay(100);
  tone(Buzzer, 720, 100);
  delay(150);
  tone(Buzzer, 680, 100);
  delay(150);
  tone(Buzzer, 620, 150);
  delay(300);

  tone(Buzzer, 650, 150);
  delay(300);
  tone(Buzzer, 380, 100);
  delay(150);
  tone(Buzzer, 430, 100);
  delay(150);

  tone(Buzzer, 500, 100);
  delay(300);
  tone(Buzzer, 430, 100);
  delay(150);
  tone(Buzzer, 500, 100);
  delay(100);
  tone(Buzzer, 570, 100);
  delay(220);

  tone(Buzzer, 500, 100);
  delay(300);

  tone(Buzzer, 760, 100);
  delay(100);
  tone(Buzzer, 720, 100);
  delay(150);
  tone(Buzzer, 680, 100);
  delay(150);
  tone(Buzzer, 620, 150);
  delay(300);

  tone(Buzzer, 650, 200);
  delay(300);

  tone(Buzzer, 1020, 80);
  delay(300);
  tone(Buzzer, 1020, 80);
  delay(150);
  tone(Buzzer, 1020, 80);
  delay(300);

  tone(Buzzer, 380, 100);
  delay(300);
  tone(Buzzer, 500, 100);
  delay(300);

  tone(Buzzer, 760, 100);
  delay(100);
  tone(Buzzer, 720, 100);
  delay(150);
  tone(Buzzer, 680, 100);
  delay(150);
  tone(Buzzer, 620, 150);
  delay(300);

  tone(Buzzer, 650, 150);
  delay(300);
  tone(Buzzer, 380, 100);
  delay(150);
  tone(Buzzer, 430, 100);
  delay(150);

  tone(Buzzer, 500, 100);
  delay(300);
  tone(Buzzer, 430, 100);
  delay(150);
  tone(Buzzer, 500, 100);
  delay(100);
  tone(Buzzer, 570, 100);
  delay(420);

  tone(Buzzer, 585, 100);
  delay(450);

  tone(Buzzer, 550, 100);
  delay(420);

  tone(Buzzer, 500, 100);
  delay(360);

  tone(Buzzer, 380, 100);
  delay(300);
  tone(Buzzer, 500, 100);
  delay(300);
  tone(Buzzer, 500, 100);
  delay(150);
  tone(Buzzer, 500, 100);
  delay(300);

  tone(Buzzer, 500, 100);
  delay(300);

  tone(Buzzer, 760, 100);
  delay(100);
  tone(Buzzer, 720, 100);
  delay(150);
  tone(Buzzer, 680, 100);
  delay(150);
  tone(Buzzer, 620, 150);
  delay(300);

  tone(Buzzer, 650, 150);
  delay(300);
  tone(Buzzer, 380, 100);
  delay(150);
  tone(Buzzer, 430, 100);
  delay(150);

  tone(Buzzer, 500, 100);
  delay(300);
  tone(Buzzer, 430, 100);
  delay(150);
  tone(Buzzer, 500, 100);
  delay(100);
  tone(Buzzer, 570, 100);
  delay(220);

  tone(Buzzer, 500, 100);
  delay(300);

  tone(Buzzer, 760, 100);
  delay(100);
  tone(Buzzer, 720, 100);
  delay(150);
  tone(Buzzer, 680, 100);
  delay(150);
  tone(Buzzer, 620, 150);
  delay(300);

  tone(Buzzer, 650, 200);
  delay(300);

  tone(Buzzer, 1020, 80);
  delay(300);
  tone(Buzzer, 1020, 80);
  delay(150);
  tone(Buzzer, 1020, 80);
  delay(300);

  tone(Buzzer, 380, 100);
  delay(300);
  tone(Buzzer, 500, 100);
  delay(300);

  tone(Buzzer, 760, 100);
  delay(100);
  tone(Buzzer, 720, 100);
  delay(150);
  tone(Buzzer, 680, 100);
  delay(150);
  tone(Buzzer, 620, 150);
  delay(300);

  tone(Buzzer, 650, 150);
  delay(300);
  tone(Buzzer, 380, 100);
  delay(150);
  tone(Buzzer, 430, 100);
  delay(150);

  tone(Buzzer, 500, 100);
  delay(300);
  tone(Buzzer, 430, 100);
  delay(150);
  tone(Buzzer, 500, 100);
  delay(100);
  tone(Buzzer, 570, 100);
  delay(420);

  tone(Buzzer, 585, 100);
  delay(450);

  tone(Buzzer, 550, 100);
  delay(420);

  tone(Buzzer, 500, 100);
  delay(360);

  tone(Buzzer, 380, 100);
  delay(300);
  tone(Buzzer, 500, 100);
  delay(300);
  tone(Buzzer, 500, 100);
  delay(150);
  tone(Buzzer, 500, 100);
  delay(300);

  tone(Buzzer, 500, 60);
  delay(150);
  tone(Buzzer, 500, 80);
  delay(300);
  tone(Buzzer, 500, 60);
  delay(350);
  tone(Buzzer, 500, 80);
  delay(150);
  tone(Buzzer, 580, 80);
  delay(350);
  tone(Buzzer, 660, 80);
  delay(150);
  tone(Buzzer, 500, 80);
  delay(300);
  tone(Buzzer, 430, 80);
  delay(150);
  tone(Buzzer, 380, 80);
  delay(600);

  tone(Buzzer, 500, 60);
  delay(150);
  tone(Buzzer, 500, 80);
  delay(300);
  tone(Buzzer, 500, 60);
  delay(350);
  tone(Buzzer, 500, 80);
  delay(150);
  tone(Buzzer, 580, 80);
  delay(150);
  tone(Buzzer, 660, 80);
  delay(550);

  tone(Buzzer, 870, 80);
  delay(325);
  tone(Buzzer, 760, 80);
  delay(600);

  tone(Buzzer, 500, 60);
  delay(150);
  tone(Buzzer, 500, 80);
  delay(300);
  tone(Buzzer, 500, 60);
  delay(350);
  tone(Buzzer, 500, 80);
  delay(150);
  tone(Buzzer, 580, 80);
  delay(350);
  tone(Buzzer, 660, 80);
  delay(150);
  tone(Buzzer, 500, 80);
  delay(300);
  tone(Buzzer, 430, 80);
  delay(150);
  tone(Buzzer, 380, 80);
  delay(600);

  tone(Buzzer, 660, 100);
  delay(150);
  tone(Buzzer, 660, 100);
  delay(300);
  tone(Buzzer, 660, 100);
  delay(300);
  tone(Buzzer, 510, 100);
  delay(100);
  tone(Buzzer, 660, 100);
  delay(300);
  tone(Buzzer, 770, 100);
  delay(550);
  tone(Buzzer, 380, 100);
  delay(575);

}
