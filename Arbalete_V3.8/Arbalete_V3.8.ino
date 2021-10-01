
#include <LiquidCrystal595.h>    // include the library
#include <AccelStepper.h>
#include <Rotary.h>
#include <TimerOne.h>
#include <EEPROM.h>
#include "param.h"

/****** Define System parameters ******/
#define X_dir 7
#define Y_dir 12
#define X_step 8
#define Y_step 13
#define Bouton A5
#define OutA A4
#define OutB 2
#define Buzzer 6
#define Batterie A0
#define RF 11
#define Enable A1

/* Constantes système */
#define LCDwidth 16
#define LCDheight 2

int Xmax = 9;
int Ymax = 10;

// Parametres_Arbalete
// Parametres_PDV
byte adr_tps_pause = 0;
byte adr_nb_photos = 2;
byte adr_nb_h = 4;
byte adr_nb_v = 6;
byte adr_dist_h = 8;
byte adr_dist_v = 10;
byte adr_speedX = 12;
byte adr_accX = 14;
byte adr_speedY = 16;
byte adr_accY = 18;
byte adr_tps_stab = 20;
byte nb_h = 0;
byte nb_v = 0;

int tps_pause = 0;
int nb_photos = 0;
int nb_total;
int dist_h = 0;
int dist_v = 0;
int tps_stab = 500;

//Parametres_deplacements
int stepspercmX = 383;
int stepspercmY = 1430;

long stepsX = 0;
long stepsY = 0;
int speedX = 1000;
int accX = 10000;
int speedY = 10000;
int accY = 10000;

bool StopPDV = false;


// parameter_ parametre(type, val, step, min, max, name)
parameter_ p_tps_pause("int", tps_pause, 250, 0, 30000, "TPS DE PDV");
parameter_ p_tps_stab("int", tps_stab, 250, 0, 20000, "TPS STAB");
parameter_ p_nb_photos("int", nb_photos, 1, 1, 5, "NB IMG/PDV");
parameter_ p_nb_h("int", nb_h, 1, 1, 500, "NB PDV HORI");
parameter_ p_nb_v("int", nb_v, 1, 1, 500, "NB PDV VERT");
parameter_ p_dist_h("int", dist_h, 1, 1, 500, "DEPLAC HOR");
parameter_ p_dist_v("int", dist_v, 1, 1, 500, "DEPLAC VER");

//Parametres moteur

parameter_ p_speedX("int", speedX, 100, 0, 5000, "VIT MOT X");
parameter_ p_speedY("int", speedX, 100, 0, 5000, "VIT MOT Y");
parameter_ p_accX("int", accX, 250, 0, 10000, "ACC MOT X");
parameter_ p_accY("int", accY, 250, 0, 10000, "ACC MOT Y");

parameter_* Liste_Reglages[] = {&p_tps_pause, &p_tps_stab, &p_nb_photos, &p_nb_h, &p_dist_h, &p_nb_v, &p_dist_v};

parameter_* Liste_Reglages_Moteurs[] = {&p_speedX, &p_speedY, &p_accX, &p_accY};
//Creation des outils (moteurs, ecran, molette, RF)

AccelStepper StepperX(1, X_step, X_dir); //use pin 12 and 13 for dir and step, 1 is the "external driver" mode (A4988)
AccelStepper StepperY(1, Y_step, Y_dir);
Rotary rotary = Rotary(OutB, OutA);
LiquidCrystal595 lcd(5, 3, 4);   // datapin, latchpin, clockpin



//Paramètres internes de gestion des menus
int counter = 0;
int aLastState = 1;
int bLastState = 1;
int boutonLastState = 0;
int Menu = 0;
int SubMenu = 0;
bool updatelcdMenu = true;
bool updatelcd = true;

const char* CMD_CLICK = "clic";
const char* CMD_TEST = "test";
//

void setup() {
  pinMode(OutA, INPUT);
  pinMode(OutB, INPUT);
  pinMode(Bouton, INPUT);
  pinMode(RF, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Enable, OUTPUT);
  pinMode(X_dir, OUTPUT);
  pinMode(Y_dir, OUTPUT);

  digitalWrite(Enable, HIGH);

  lcd.begin(16, 2);
  lcd.setLED2Pin(HIGH);
  lcd.setLED1Pin(HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(centerElement("ARBALETE V3.7"));
  lcd.setCursor(0, 1);
  lcd.print(centerElement("HOTFIX!!"));
  beep();
  delay(500);
  lcd.clear();


  //vw_set_tx_pin(RF);
  //vw_setup(2000);

  digitalWrite(RF, 0);

  ReadParameters();
  StepperX.setMaxSpeed(p_speedX._val); //set max speed the motor will turn (steps/second)
  StepperY.setMaxSpeed(p_speedY._val); //set max speed the motor will turn (steps/second)
  StepperX.setAcceleration(p_accX._val); //set accX (steps/second^2)
  StepperY.setAcceleration(p_accY._val); //set accX (steps/second^2)
  StepperX.setSpeed(0);
  StepperX.setSpeed(0);
  StepperX.setCurrentPosition(0);
  StepperY.setCurrentPosition(0);
  StepperX.moveTo(0);
  StepperY.moveTo(0);
}

void loop() {

  digitalWrite(Enable, HIGH);

  unsigned char result = rotary.process();
  int boutonState = digitalRead(Bouton);
  if (updatelcdMenu == true)
  { nb_total = int(p_nb_h._val) * int(p_nb_v._val);
    lcd.clear();
    lcd.setCursor(0, 0);
    switch (Menu)
    {
      case 0:
        lcd.print("<1  REGLAGES   >");
        lcd.setCursor(5, 1);
        lcd.print("ENTRER");
        break;
      case 1:
        lcd.print("<2 DEPLACEMENTS>");
        lcd.setCursor(5, 1);
        lcd.print("ENTRER");
        break;
      case 2:
        lcd.print("<3 TEST RADIO  >");
        lcd.setCursor(6, 1);
        lcd.print("CLICK");
        break;
      case 3:
        lcd.print("<4 PDV PHOTO   >");
        lcd.setCursor(0, 1);
        lcd.print(Liste_Reglages[0]->_val);
        lcd.print("ms | ");
        lcd.print(String(nb_total));
        lcd.print("imgs");
        break;
    }

    updatelcdMenu = false;
  }

  if (result == DIR_CW) {
    Menu ++;
    if (Menu > 3) {
      Menu = 0;
    };
    updatelcdMenu = true;

  }
  else if (result == DIR_CCW)
  {
    Menu --;
    if (Menu < 0) {
      Menu = 3;
    };
    updatelcdMenu = true;

  }

  if (boutonState == 0)
  {
    delay(250);
    updatelcdMenu = true;
    switch (Menu)
    {
      case 0:
        menureglages();
        nb_total = int(p_nb_h._val) * int(p_nb_v._val);
        break;
      case 1:
        MenuChoixAxe();
        break;
      case 2:
        PDV();
        //RF_test();
        //Mario();
        break;
      case 3:
        pdv();
        break;
    }
  }
}




void MenuChoixStep(AccelStepper* Moteur, int PasParCm, byte* EndStop)
{

  boolean sortie = false;
  updatelcd = true;
  int SubMenu = 0;

  int PositionEnCm = Moteur->currentPosition() / PasParCm;
  int destination = PositionEnCm;

  while (!sortie)
  {

    unsigned char result = rotary.process();

    // int boutonState = digitalRead(Bouton);
    if (updatelcd) {
      lcd.clear();
      lcd.setCursor(0, 0);
      PositionEnCm = Moteur->currentPosition() / PasParCm;

      lcd.setCursor(0, 0);
      lcd.print("MOVE TO : ");
      lcd.print(String(destination));
      lcd.setCursor(0, 1);
      lcd.print("POSITION : ");
      lcd.print(String(Moteur->currentPosition() / PasParCm));
      updatelcd = false;
    }

    if (result == DIR_CW) {
      destination++;
      if (destination > 100) destination = 100;
      updatelcd = true;
    }
    if (result == DIR_CCW) {
      destination--;
      if (destination < -100)  destination = -100;
      updatelcd = true;
    }

    unsigned int count = 0; // pressed time !!

    if ( digitalRead(Bouton) == 0) {

      delay(200);

      updatelcd = true;
      // sortie = true;

      // ControlleurDeplacement(Moteur, PasParCm, destination, EndStop);
      // updatelcd = true;

    }

  }
  sortie = false;
  DeplaceMoteurAbs(Moteur, PasParCm * destination, EndStop);
}




void menureglages()
{
  boolean sortie = false;
  updatelcd = true;
  int SubMenu = 0;
  int numElements = (sizeof(Liste_Reglages) / sizeof(Liste_Reglages[0]));
  ReadParameters();
  while (sortie == false)
  {
    unsigned char result = rotary.process();
    int boutonState = digitalRead(Bouton);

    if (updatelcd == true) {
      if (SubMenu == numElements) {
        Menu_Retour();
      }
      else {
        Liste_Reglages[SubMenu]->afficher(lcd);
      }
      updatelcd = false;
    }

    if (result == DIR_CW) {
      SubMenu++;
      if (SubMenu > numElements) {
        SubMenu = 0;
      }
      updatelcd = true;
    }
    if (result == DIR_CCW) {
      SubMenu--;
      if (SubMenu < 0) {
        SubMenu = numElements;
      };
      updatelcd = true;
    }

    if (boutonState == 0) {
      delay(200);
      if (SubMenu == numElements) {
        SaveParameters();
        sortie = true;
      }
      else {
        Liste_Reglages[SubMenu]->ctrl(lcd, rotary);
      }
      updatelcd = true;
    }
  }
}

// ######################################    REGLAGES MOTEURS  #################################

void menureglagesmoteurs()
{
  boolean sortie = false;
  boolean updatelcd = true;
  int SubMenu = 0;
  int numElements = (sizeof(Liste_Reglages_Moteurs) / sizeof(Liste_Reglages_Moteurs[0]));
  ReadParameters();
  while (sortie == false)
  {
    unsigned char result = rotary.process();
    int boutonState = digitalRead(Bouton);

    if (updatelcd == true) {
      if (SubMenu == numElements) {
        Menu_Retour();
      }
      else {
        Liste_Reglages_Moteurs[SubMenu]->afficher(lcd);
      }
      updatelcd = false;
    }

    if (result == DIR_CW) {
      SubMenu++;
      if (SubMenu > numElements) {
        SubMenu = 0;
      }
      updatelcd = true;
    }
    if (result == DIR_CCW) {
      SubMenu--;
      if (SubMenu < 0) {
        SubMenu = numElements;
      };
      updatelcd = true;
    }

    if (boutonState == 0) {
      delay(200);
      if (SubMenu == numElements) {
        SaveParameters();
        sortie = true;
      }
      else {
        Liste_Reglages_Moteurs[SubMenu]->ctrl(lcd, rotary);
      }
      updatelcd = true;
    }
  }
}

// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ###################################### PRISE DE VUES #################################
// ######################################               #################################
// ######################################               #################################
// ######################################################################################


void pdv()
{
  StopPDV = false;
  //A PASSER DANS LA FONCTION SET ZERO()
  StepperX.setCurrentPosition(0);
  StepperY.setCurrentPosition(0);
  //////////////

  // SI ZERO NA PAS ETE REGLE, METTRE UN WARNING AVEC OPTION DE SORTIE
  //PAR EXEMPLE "ATTENTION, LE ZERO NEST PAS RÉGLÉ."
  //            "ARRETER PDV? <OUI> NON           "
  //SI L'UTILISATEUR CHOISIT NON, ON APPELLE SETZERO().

  stepsX = long(p_dist_h._val) * long(stepspercmX );
  stepsY = long(p_dist_v._val) * long(stepspercmY);
  //Serial.println("Début de la prise de vue");
  int nb_total = int(p_nb_h._val) * int(p_nb_v._val);
  int cpt_photos = 1;
  bool dirhor = 1;
  // LANCEMENT DE LA PRISE DE VUE
  digitalWrite(RF, 0);
  digitalWrite(Enable, HIGH);

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(centerElement(String("...3..."))) ;
  delay(333);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(centerElement(String("...2..."))) ;
  delay(333);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(centerElement(String("...1..."))) ;
  delay(333);
  //Serial.print("Nombre d'images horizontales :  ");
  //Serial.println(int(p_nb_h._val));
  //Serial.print("Nombre d'images verticales : ");
  //Serial.println(int(p_nb_v._val));

  for (int i = 1; i <= int(p_nb_v._val); i++ )
  {
    for (int j = 1; j < int(p_nb_h._val); j++ )
    { //On affiche ou on en est
      lcd.setLED1Pin(HIGH);
      lcd.setCursor(0, 0);
      lcd.print(centerElement(String("PDV EN COURS")));
      lcd.setCursor(0, 1);
      String txt = "PHOTO ";
      txt = txt + cpt_photos;
      txt = txt + " / ";
      txt = txt + nb_total;
      lcd.print(centerElement(String(txt)));
      //ON ATTEND LA STABILISATION
      unsigned long previousMillis = millis();
      while (millis() - previousMillis <= long(p_tps_stab._val) || StopPDV == true)
      {
        if (!digitalRead(Bouton))
        {
          MenuPause();
        }
        if (StopPDV == true) goto FINPDV;
      }

      //FIN DE L'ATTENTE DE STABILISATION
      lcd.setLED1Pin(HIGH);
      lcd.setCursor(0, 0);
      lcd.print(centerElement(String("PDV EN COURS")));
      lcd.setCursor(0, 1);
      txt = "PHOTO ";
      txt = txt + cpt_photos;
      txt = txt + " / ";
      txt = txt + nb_total;
      lcd.print(centerElement(String(txt)));

      //PRISE DE PHOTOS
      for (int k = 1; k <= int(p_nb_photos._val); k++)
      {
        //REMPLACER PAR LA FONCTION PDV A PDV B OU LES DEUX
        digitalWrite(RF, 1);
        //PDV();
        //RF_click();
        //ON ATTEND LE TEMPS DE PAUSE
        unsigned long previousMillis = millis();
        while (millis() - previousMillis <= long(p_tps_pause._val) || StopPDV == true)
        {
          if (!digitalRead(Bouton))
          {
            digitalWrite(RF, 0);
            MenuPause();
          }
          if (StopPDV == true) goto FINPDV;
        }
        digitalWrite(RF, 0);
        lcd.setLED1Pin(LOW);
        //FIN DE LATTENTE TPS DE PAUSE
      }

      cpt_photos++;

      //ON AVANCE HORIZONTALLEMENT
      //DeplaceMoteur(&StepperX, i & 1 ? stepsX : -stepsX, stepspercmX, Xmax);
      //DeplaceMoteur(&StepperX, stepsX,  i & 1 ? true : false, stepspercmX, Xmax);
      //DeplaceMoteurAbs(AccelStepper* Moteur, long Pos, int EndStop)
      if (dirhor == 1)
      {
        DeplaceMoteurAbs(&StepperX, j * dirhor * stepsX, Xmax);
        if (StopPDV == true) goto FINPDV;
      }
      if (dirhor == 0)
      {
        DeplaceMoteurAbs(&StepperX, (p_nb_h._val - j - 1) * stepsX, Xmax);
        if (StopPDV == true) goto FINPDV;
        lcd.setLED1Pin(HIGH);
        lcd.setCursor(0, 0);
        lcd.print(centerElement(String("PDV EN COURS")));
        lcd.setCursor(0, 1);
        txt = "PHOTO ";
        txt = txt + cpt_photos;
        txt = txt + " / ";
        txt = txt + nb_total;
        lcd.print(centerElement(String(txt)));
      }
    }
    //ON AVANCE VERTICALEMENT
    dirhor = !dirhor;
    //ON ATTEND LA STABILISATION
    unsigned long previousMillis = millis();
    while (millis() - previousMillis <= long(p_tps_stab._val))
    {
      if (!digitalRead(Bouton))
      {
        MenuPause();
      }
      if (StopPDV == true) goto FINPDV;
    }

    //PRISE DE PHOTOS
    for (int k = 1; k <= int(p_nb_photos._val); k++)
    {
      digitalWrite(RF, 1);
      //PDV();
      //RF_click();
      //ON ATTEND LE TEMPS DE PAUSE
      unsigned long previousMillis = millis();
      while (millis() - previousMillis <= long(p_tps_pause._val))
      {
        if (!digitalRead(Bouton))
        {
          MenuPause();
          digitalWrite(RF, 0);
        }
        if (StopPDV == true) goto FINPDV;
      }
      lcd.setLED1Pin(LOW);
      digitalWrite(RF, 0);
      //FIN DE LATTENTE TPS DE PAUSE
    }
    cpt_photos++;
    if (i < int(p_nb_v._val))
    {

      DeplaceMoteurAbs(&StepperY, i * stepsY, Ymax);
      if (StopPDV == true) goto FINPDV;
    }
  }

  digitalWrite(RF, 0);
  //DeplaceMoteur(&StepperY, stepsY, stepspercmY, 1, Ymax);


  tone(Buzzer, 2000);
  delay(100);
  noTone(Buzzer);
  delay(50);
  tone(Buzzer, 1000);
  delay(100);
  noTone(Buzzer);

FINPDV :
  digitalWrite(RF, 0);
  lcd.setLED1Pin(LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(centerElement(String("FIN DE LA PDV")));
  delay(500);
  MenuRetourZero();

  //TODO: RETOURNER A POSITION INITIALE?
  digitalWrite(Enable, LOW);
}


void MenuRetourZero()
{
  bool sortie = false;
  bool retour = true;
  updatelcd = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(String("RETOUR POS INIT?"));
  lcd.setCursor(0, 1);
  lcd.print(String(" <OUI>     NON  "));
  while (!sortie)
  {
    unsigned char result = rotary.process();
    int boutonState = digitalRead(Bouton);

    if (result == DIR_CW) {
      lcd.setCursor(0, 1);
      lcd.print(String("  OUI     <NON> "));
      retour = false;
      //Serial.println(retour);

    }

    if (result == DIR_CCW) {
      lcd.setCursor(0, 1);
      lcd.print(String(" <OUI>     NON  "));
      retour = true;
      //Serial.println(retour);
    }


    if (digitalRead(Bouton) == 0)
    { //beep();
      delay(250);
      sortie = true;
      if (retour) Deplace2Moteurs(&StepperX, &StepperY, 0, 0, Xmax, Ymax);  // FONCTION GOTO ZERO

      //Serial.println("On sort!");
    }
  }
}


bool MenuPause()
{
  //delay(200);
  Arret2Moteurs(&StepperX, &StepperY);
  bool sortie = false;
  bool retour = true;
  updatelcd = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(String("PAUSE-REPRENDRE?"));
  lcd.setCursor(0, 1);
  lcd.print(String(" <OUI>     NON  "));
  while (!sortie)
  {
    unsigned char result = rotary.process();
    int boutonState = digitalRead(Bouton);

    if (result == DIR_CW) {
      lcd.setCursor(0, 1);
      lcd.print(String("  OUI     <NON> "));
      retour = false;
      StopPDV = true;
      //Serial.println(retour);

    }

    if (result == DIR_CCW) {
      lcd.setCursor(0, 1);
      lcd.print(String(" <OUI>     NON  "));
      retour = true;
      StopPDV = false;
      //Serial.println(retour);
    }


    if (digitalRead(Bouton) == 0)
    { //beep();
      delay(250);
      sortie = true;
      return retour;

      //Serial.println("On sort!");
    }
  }
}
// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ###################################### ENVOI DE MSG  #################################
// ######################################               #################################
// ######################################               #################################
// ######################################################################################

void  PDV()
{
  /*
    digitalWrite(RF, 1);
    delay(250);
    digitalWrite(RF, 0);
  */
  int state = 0;
  envoiMsg(state);

}

/*Added !!!
  BY YM
*/
void envoiMsg(int state) {


  boolean sortie = false;
  updatelcd = true;
  int SubMenu = 0;
  ReadParameters();

  while (sortie == false)
  {
    unsigned char result = rotary.process();
    int state = digitalRead(Bouton);
    if (updatelcd == true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      // lcd.print("< PDV >");

      switch (SubMenu) {
        case 0:
          lcd.print(centerElement("Here we Go"));
          break;

        case 1:
          lcd.setCursor(0, 0);
          lcd.print(centerElement("CAM 1"));
          break;

        case 2:
          lcd.setCursor(0, 0);
          lcd.print(centerElement("CAM 2"));
          break;

        case 3:
          lcd.setCursor(0, 0);
          lcd.print(centerElement("CAM 1 & 2"));
          break;

        case 4:
          lcd.setCursor(0, 0);
          lcd.print(centerElement("RETOUR"));
          break;

      }


      lcd.setCursor(0, 0);
      lcd.print("<");
      lcd.setCursor(15, 0);
      lcd.print(">");
      updatelcd = false;
    }

    if (result == DIR_CW) {
      SubMenu++;
      if (SubMenu > 4) {
        SubMenu = 0;
      }
      updatelcd = true;
    }
    if (result == DIR_CCW) {
      SubMenu--;
      if (SubMenu < 0) {
        SubMenu = 4;
      };
      updatelcd = true;
    }

    if (digitalRead(Bouton) == 0)
    {
      delay(50);
      digitalWrite(RF, 1);
      delay(10);
      switch (SubMenu) {

        case 0:
          digitalWrite(RF, 0);
          delay(10);
          digitalWrite(RF, 0);
          delay(10);
          break;

        case 1:
          digitalWrite(RF, 0);
          delay(10);
          digitalWrite(RF, 1);
          delay(10);
          break;

        case 2:
          digitalWrite(RF, 1);
          delay(10);
          digitalWrite(RF, 0);
          delay(10);
          break;

        case 3:
          digitalWrite(RF, 1);
          delay(10);
          digitalWrite(RF, 1);
          delay(10);
          break;

        case 4:
          sortie = true;
          break;

      }

      lcd.setCursor(0, 1);
      lcd.print(centerElement("DONE!"));
      digitalWrite(RF, 0);
    }
  }
}

/*
  void RF_click()
  {
  byte message[VW_MAX_MESSAGE_LEN];
  vw_send((byte*) CMD_CLICK, strlen(CMD_CLICK) + 1); // On envoie le message
  vw_wait_tx(); // On attend la fin de l'envoi
  //Serial.println("CLICK!");
  beep();
  }
  void RF_test()
  {
  byte message[VW_MAX_MESSAGE_LEN];
  vw_send((byte*) CMD_TEST, strlen(CMD_TEST) + 1); // On envoie le message
  vw_wait_tx(); // On attend la fin de l'envoi
  //Serial.println("TEST!");
  }
*/
// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ###################################### DEPLACEMENTS  #################################
// ######################################               #################################
// ######################################               #################################
// ######################################################################################

void MenuChoixAxe() {

  digitalWrite(Enable, LOW);

  boolean sortie = false;
  updatelcd = true;
  int SubMenu = 0;
  ReadParameters();
  while (sortie == false)
  {
    unsigned char result = rotary.process();
    int boutonState = digitalRead(Bouton);
    if (updatelcd == true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("< DEPLACEMENTS >");
      switch (SubMenu)
      {
        case 0:
          lcd.setCursor(0, 1);
          lcd.print(centerElement(String("HORIZONTAL")));
          break;
        case 1:
          lcd.setCursor(0, 1);
          lcd.print(centerElement(String("VERTICAL")));
          break;
        case 2:
          lcd.setCursor(0, 1);
          lcd.print(centerElement(String("REGLAGES")));
          break;
        case 3:
          Menu_Retour();
          break;
      }
      lcd.setCursor(0, 0);
      lcd.print("<");
      lcd.setCursor(15, 0);
      lcd.print(">");
      updatelcd = false;
    }

    if (result == DIR_CW) {
      SubMenu++;
      if (SubMenu > 3) {
        SubMenu = 0;
      }
      updatelcd = true;
    }
    if (result == DIR_CCW) {
      SubMenu--;
      if (SubMenu < 0) {
        SubMenu = 3;
      };
      updatelcd = true;
    }
    if (boutonState == 0) {
      delay(200);
      switch (SubMenu)
      {
        case 0:
          MenuChoixStep(&StepperX, stepspercmX, Xmax);
          break;
        case 1:
          MenuChoixStep(&StepperY, stepspercmY, Ymax);
          break;
        case 2:
          menureglagesmoteurs();
          break;
        case 3:
          sortie = true;
          break;
      }
      updatelcd = true;
    }
  }
  digitalWrite(Enable, HIGH);
}



void ArretMoteurs(AccelStepper* Moteur)
{
  Moteur->stop();
  while (Moteur->distanceToGo()) Moteur->run();
}
void Arret2Moteurs (AccelStepper * Moteur1, AccelStepper * Moteur2)
{
  bool sortie = false;
  Moteur1->stop();
  Moteur2->stop();
  while (!sortie)
  {
    if ( Moteur1->distanceToGo() == 0 && Moteur2->distanceToGo() == 0) sortie = true;
    Moteur1->run();
    Moteur2->run();
  }

}

void ControlleurDeplacement(AccelStepper* Moteur, int PasParCm, int cm, int EndStop)
{
  boolean sortie = false;
  bool deplacementencours = true;
  int SubMenu = 0;
  long PositionEnCm;
  //PositionEnCm = (Moteur->currentPosition() +1) / PasParCm;
  long Steps = 0;
  Steps = long(PasParCm) * long(cm);
  while (!sortie)
  {
    if (updatelcd)
    {
      PositionEnCm = Moteur->currentPosition() / PasParCm;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(centerElement("<<<<<>>>>>"));
      lcd.setCursor(0, 1);
      lcd.print(centerElement(String(PositionEnCm)));
      updatelcd = false;
    }

    if (rotary.process() == DIR_CW)
    { //Serial.println("DIR_CW");
      lcd.setCursor(0, 0);
      lcd.print(centerElement(">>>>>>>>>>"));
      lcd.setCursor(0, 1);
      lcd.print(centerElement("^^^^^^^^^^"));
      updatelcd = true;

      Moteur->move(Steps);
      deplacementencours = true;
      while (deplacementencours == true)
      {
        if ( !Moteur->distanceToGo()) deplacementencours = false;
        if (digitalRead(Bouton) == 0 )//|| digitalRead(EndStop) == 0)
        { Moteur->stop();
          while (Moteur->distanceToGo()) Moteur->run();
          deplacementencours = false;
        }
        Moteur->run();
      }
      //DeplaceMoteur(Moteur, Steps, true, PasParCm, EndStop);
    }
    bool STOP = digitalRead(EndStop);

    if (rotary.process() == DIR_CCW)
    { //Serial.println("DIR_CCW");
      lcd.setCursor(0, 0);
      lcd.print(centerElement("<<<<<<<<<<"));
      lcd.setCursor(0, 1);
      lcd.print(centerElement("VVVVVVVVVV"));
      updatelcd = true;
      Moteur->move(-Steps);
      deplacementencours = true;
      while (deplacementencours == true)
      {
        if (!Moteur->distanceToGo()) deplacementencours = false;
        if (!digitalRead(EndStop)) {
          beep();
          Touch_EndStop(Moteur, EndStop);
        }
        if (digitalRead(Bouton) == 0)
        { Moteur->stop();
          while (Moteur->distanceToGo()) Moteur->run();
          deplacementencours = false;
        }
        Moteur->run();
      }
      //DeplaceMoteur(Moteur, Steps, false, PasParCm, EndStop);
    }
    if (!digitalRead(Bouton))
    {
      sortie = true;
      delay(200);
    }
  }
}

long DeplaceMoteur(AccelStepper* Moteur, long Steps, bool Direction, int PasParCm, int EndStop)
{ digitalWrite(Enable, LOW);
  bool sortie = false;
  int PositionEnCm ;
  if (Direction) Moteur->move(Steps);
  else Moteur->move(-Steps);

  while (sortie == false)
  {
    if ( Moteur->distanceToGo() == 0) sortie = true;

    if (digitalRead(EndStop) == 0) {
      beep();
      Touch_EndStop(Moteur, EndStop );
      sortie = true;
    }

    if (digitalRead(Bouton) == 0)
    { // MODIFICATION 3.4 TEST SORTIE IMMEDIATE
      //Moteur->stop();
      //while (Moteur->distanceToGo()) Moteur->run();
      sortie = true;
    }
    Moteur->run();
  }
  digitalWrite(Enable, HIGH);
}


bool DeplaceMoteurAbs(AccelStepper* Moteur, long Pos, int EndStop)
{
  digitalWrite(Enable, LOW);
  bool sortie = false;
  Moteur->moveTo(Pos);

  while (sortie == false)
  {
    if ( Moteur->distanceToGo() == 0) sortie = true;
    if (digitalRead(EndStop) == 0) {
      beep();
      Touch_EndStop(Moteur, EndStop );
      sortie = true;
    }
    if (digitalRead(Bouton) == 0)
    {

      Moteur->stop();
      while (Moteur->distanceToGo()) Moteur->run();
      if (MenuPause() == false)
      { StopPDV = true;
        sortie = true;
      }
      else(Moteur->moveTo(Pos));
    }
    Moteur->run();
  }
  digitalWrite(Enable, HIGH);
}

void Touch_EndStop(AccelStepper* Moteur, int EndStop)
{ digitalWrite(Enable, LOW);
  int HomingPos = 1;
  Moteur->setCurrentPosition(0);
  Moteur->setSpeed(1000);
  while (!digitalRead(EndStop))
  { Moteur->moveTo(HomingPos);
    Moteur->run();
    HomingPos++;
    delay(2);
  }
  Moteur->setCurrentPosition(0);
  digitalWrite(Enable, HIGH);
}
void Deplace2Moteurs(AccelStepper * Moteur1, AccelStepper * Moteur2, long Pos1, long Pos2, int EndStop1, int EndStop2)
{
  digitalWrite(Enable, LOW);
  bool sortie = false;
  Moteur1->moveTo(Pos1);
  Moteur2->moveTo(Pos2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(String("RETOUR POS INIT!"));
  lcd.setCursor(0, 1);
  while (!sortie)
  {
    if ( Moteur1->distanceToGo() == 0 && Moteur2->distanceToGo() == 0 ) sortie = true;
    if ( digitalRead(Bouton) == 0)
    {
      Moteur1->stop();
      Moteur2->stop();
      while ( Moteur1->distanceToGo() == 0 && Moteur2->distanceToGo() == 0 )
      {
        Moteur1->run();
        Moteur2->run();
      }
      if (MenuPause() == false)
      {
        sortie = true;
      }
      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(String("RETOUR POS INIT!"));
        lcd.setCursor(0, 1);
        Moteur1->moveTo(Pos1);
        Moteur2->moveTo(Pos2);
      }

    }
    if (digitalRead(EndStop1) == 0) {
      beep();
      Touch_EndStop(Moteur1, EndStop1);
      sortie = true;
    }
    if (digitalRead(EndStop2) == 0) {
      beep();
      Touch_EndStop(Moteur2, EndStop2 );
      sortie = true;
    }

    Moteur1->run();
    Moteur2->run();
  }

  delay(200);
  //tone(Buzzer, 1500);
  //delay(100);
  //noTone(Buzzer);

  ////Serial.print("Position: ");
  ////Serial.print(Moteur->currentPosition());
  ////Serial.println("(APRES)");
  digitalWrite(Enable, HIGH);
}


// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ###################################### AUTRES FCTS   #################################
// ######################################               #################################
// ######################################               #################################
//#########################################################################################


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
  delay(100);
  noTone(Buzzer);
}

String centerElement(String element) {
  int textLength = element.length() - 1;
  int paddingLeft;
  if (textLength < LCDwidth - 2) {
    int padding = LCDwidth - textLength;
    paddingLeft = padding / 2;
  }
  String ret = "";
  if (textLength % 2 == 0) {
    paddingLeft--;
  }
  for (int i = 0; i < paddingLeft; i++) {
    ret = ret + " ";
  }
  ret = ret + element;
  for (int i = 0; i < paddingLeft; i++) {
    ret = ret + " ";
  }
  return ret;
}


void Menu_Retour() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("<    RETOUR    >");
}


// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ###################################### EEPROM        #################################
// ######################################               #################################
// ######################################               #################################
//#######################################################################################

void SaveParameters()
{

  EEPROMWriteInt(adr_tps_pause, p_tps_pause._val);
  EEPROMWriteInt(adr_nb_photos, p_nb_photos._val);
  EEPROMWriteInt(adr_nb_h, p_nb_h._val );
  EEPROMWriteInt(adr_nb_v, p_nb_v._val);
  EEPROMWriteInt(adr_dist_h, p_dist_h._val );
  EEPROMWriteInt(adr_dist_v, p_dist_v._val );
  EEPROMWriteInt(adr_speedX, p_speedX._val );
  EEPROMWriteInt(adr_accX, p_accX._val);
  EEPROMWriteInt(adr_speedY, p_speedY._val );
  EEPROMWriteInt(adr_accY, p_accY._val );
  EEPROMWriteInt(adr_tps_stab, p_tps_stab._val  );
  /*
    tone(Buzzer, 2000);
    delay(100);
    noTone(Buzzer);
    delay(50);
    tone(Buzzer, 1000);
    delay(100);
    noTone(Buzzer);
  */
  StepperX.setMaxSpeed(p_speedX._val); //set max speed the motor will turn (steps/second)
  StepperY.setMaxSpeed(p_speedY._val); //set max speed the motor will turn (steps/second)
  StepperY.setAcceleration(p_accY._val); //set accX (steps/second^2)
  StepperX.setAcceleration(p_accX._val); //set accX (steps/second^2)


}



void ReadParameters()
{
  p_tps_stab._val = EEPROMReadInt(adr_tps_stab);
  p_tps_pause._val = EEPROMReadInt(adr_tps_pause);
  p_nb_photos._val = EEPROMReadInt(adr_nb_photos);
  p_nb_h._val = EEPROMReadInt(adr_nb_h);
  p_nb_v._val = EEPROMReadInt(adr_nb_v);
  p_dist_h._val = EEPROMReadInt(adr_dist_h);
  p_dist_v._val = EEPROMReadInt(adr_dist_v);
  p_speedX._val = EEPROMReadInt(adr_speedX);
  p_speedY._val = EEPROMReadInt(adr_speedY);
  p_accX._val = EEPROMReadInt(adr_accX);
  p_accY._val = EEPROMReadInt(adr_accY);

  /*
    tone(Buzzer, 1000);
    delay(100);
    noTone(Buzzer);
    delay(50);
    tone(Buzzer, 2000);
    delay(100);
    noTone(Buzzer);
  */
}

void EEPROMWriteInt(int address, int value)
{
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);

  EEPROM.update(address, two);
  EEPROM.update(address + 1, one);
}

int EEPROMReadInt(int address)
{
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);

  return ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
}
