/**
  //EDIT 1er Juin 2021
  //EDIT 23 SEPT 2021 <- debut opto 3 et 4
*/

/**
  //EDIT 1er Juin 2021
  //EDIT 23 SEPT 2021 <- debut opto 3 et 4
  
  // EDITED 4 OCT 2021 / SERIAL COMMUNICATION OPTIMIZED
*/

#include <VirtualWire.h>

/** Broches pour les LEDs */
const byte opto1 = 12;
const byte opto2 = 11;
const byte opto3 = 10;
const byte opto4 = 9;
//const byte RF = 6;
const byte Buzzer = 5;
const byte PIN_LED_R = 4;
const byte PIN_LED_G = 3;
const byte PIN_LED_B = 2;
const byte recep = 6;

/// added !
int data[4];
#define T 10
/** Différents messages de commande */

const char* CMD_CLICK = "clic";
const char* CMD_TEST = "test";


unsigned long previousMillis = 0;
long tpsattente = 1000;
int cpttps = 0;
int cpttpsmax = 60;
int cpt = 1;

void clearData(){
  for (int i=0; i<4; i++){
    data[i] = 0;
  }
}

void takephoto2cameras()
{

  zeroOpto();
  delay(200);
  digitalWrite(opto1, 1);
  digitalWrite(opto3, 1);
  delay(200);
  digitalWrite(opto2, 1);
  digitalWrite(opto4, 1);
  delay(100);
  zeroOpto();
  delay(20);

}

void takephoto1camera1()
{

  zeroOpto();
  digitalWrite(opto1, 1);

  delay(50);
  digitalWrite(opto2, 1);

  delay(100);
  zeroOpto();
}



void takephoto1camera2()
{

  zeroOpto();
  digitalWrite(opto3, 1);

  delay(50);
  digitalWrite(opto4, 1);

  delay(100);
  zeroOpto();

}

void zeroOpto() {
  digitalWrite(opto1, 0);
  digitalWrite(opto2, 0);
  digitalWrite(opto3, 0);
  digitalWrite(opto4, 0);
  delay(50);
}



void BEEP() {
  // Serial.println("YOUHOU YA QUELQUUN?");
  for (int i = 0; i < 5; i++)
  {
    digitalWrite(PIN_LED_R, HIGH);
    tone(Buzzer, 2400);
    delay(50);
    digitalWrite(PIN_LED_R, LOW);
    noTone(Buzzer);
    delay(50);
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
  delay(50);
  noTone(Buzzer);
}


void flashgreen()
{
  digitalWrite(PIN_LED_G, 1);
  delay(10);
  digitalWrite(PIN_LED_G, 0);
}

void mario2()
{
  //Serial.println("MARIOOOOO");
  tone(Buzzer, 2640, 100);
  delay(150);
  tone(Buzzer, 2640, 100);
  delay(300);
  tone(Buzzer, 2640, 100);
  delay(300);
  tone(Buzzer, 2040, 100);
  delay(100);
  tone(Buzzer, 2640, 100);
  delay(300);
  tone(Buzzer, 3080, 100);
  delay(550);
  tone(Buzzer, 1520, 100);
  delay(575);
}

float moySignCom() {
  uint8_t data[4];
  float sum = 0;

  for (int i = 0; i < 4; i++) {
    data[i] = digitalRead(recep);
    delay(9);
  }
  for (int i = 0; i < 4; i++) {
    sum += (float)data[i];
  }
  return sum / 4;
}


void setup() {
  Serial.begin(9600);
  //Serial.println("SALUT MA GUEULE");
  /* Met les broches des LEDs en sortie et à LOW */
  pinMode(opto1, OUTPUT);
  pinMode(opto2, OUTPUT);
  pinMode(opto3, OUTPUT);
  pinMode(opto4, OUTPUT);
  //  pinMode(RF, INPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(recep, INPUT);
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
  previousMillis = millis();
}

void loop() {
  Serial.println(digitalRead(recep));
  delay(10);
  // On attend de recevoir un message
  //vw_wait_rx();
  //vw_wait_rx_max(60000);
  if (millis() - previousMillis >= tpsattente)
  {
    previousMillis = millis();
    cpttps++;
    flashgreen();
    digitalWrite(PIN_LED_R, 0);
    digitalWrite(PIN_LED_G, 0);
    digitalWrite(PIN_LED_B, 0);
    if (cpttps >= cpttpsmax)
    {
      cpttps = 0;
      BEEP();
    }
  }

  if (digitalRead(recep) == 1){
     data[0] = 1;
     delay(11);
     for (int i = 0; i < 3; i++) {
      data[i+1] = digitalRead(recep);
      delay(11); // Pour cette fonction ça ira ! dès qu'on aura 10 valeurs ça va être changé (10*i + 5) <=> T*(i+1) + T/2
    }
  }

  if (data[0] == 1 && data[3] == 1) {

    if (data[1] == 0 && data[2] == 0) {
      Serial.println(String(data[1]) + " " + String(data[2]));
      mario2();
    }

    else if (data[1] == 0 && data[2] == 1) {
      Serial.println(String(data[1]) + " " + String(data[2]));
      takephoto1camera1();
      beep();
      
      digitalWrite(PIN_LED_R, 0);
      digitalWrite(PIN_LED_G, 0);
      digitalWrite(PIN_LED_B, 1);

    }

    else if (data[1] == 1 && data[2] == 0) {
      Serial.println(String(data[1]) + " " + String(data[2]));
      
      takephoto1camera2();
      beep();

      digitalWrite(PIN_LED_R, 1);
      digitalWrite(PIN_LED_G, 0);
      digitalWrite(PIN_LED_B, 0);
  

    }

    else if (data[1] == 1 && data[2] == 1) {
      Serial.println(String(data[1]) + " " + String(data[2]));

      takephoto2cameras();
      BEEP();
    }
  }

  clearData(); // Turn data to Zero !!! 

}
