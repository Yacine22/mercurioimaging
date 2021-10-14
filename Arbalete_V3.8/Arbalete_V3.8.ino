/* 
 ---------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------- /////////////   //////////  ///     //  ////////      //        ////////       //         //     //     //    //
                                  //         //  //      //  // //   //  //            //        //    //        //       //      //     //    //
                                  /          //  //////////  //  //  //    /////       //        ////////         //     //       //     //    //
                                  /          //  //      //  //   // //        //      //        //    //          //   //        /////////    //
                                  //         //  //      //  //    ///        //       //        //    //           // //                //
                                 ////////////    //      //  //     //   //////        ////////  //    //            //                  //    //

----------------------------------------------------------------------------------------------------------------------------------------------------
This Code contains UI for Arbalete Features Control
It Counts those Menus :
    - REGLAGES: Defines all Parameters to set before excuting any function, like number of pictures to take and which camera to use !
      -> TPS DE PDV : Temps de Prise de Vues
      -> TPS STAB : Temps de stabilisation
      -> NB PDV HORI : Nombre de Prise de vues en Déplacement Horizontal
      -> NB PDV VERT : Nombre de Prise de vues en Déplacement Vertical
      -> DEPLAC HOR : DEPLACEMENT Horizontal, 
      -> DEPLAC VER : DEPLACEMENT Vertical,
      -> CAM SYNCHRO : Activation ou non des Coméras Synchro, Si Non il faut choisir un décalage entre les 2 prises
      -> TPS ENTRE CAM : Le temps de décalage entre les deux caméra, si CAM SYNCHRO égal à 0 
      
    - DEPLACEMENTS: For this one, we introduce motion parameters in 2D Plan
      -> Deplacement horizontal : Choix de la distance selon l'axe horizontal
      -> Deplacement vertical : Choix de la distance selon l'axe vertical
      -> Réglages deplacement : permets de définir les vitesses et les accéleration des moteurs

    - GOTO ZERO: Permet de faire un reset de positions, et revenir aux positions initiales par défaut
    
    - SET ZERO: Permet de définir la position actuelle comme position initial

    - TEST RADIO : Transmission du signal binaire à la rélécommande
      -> Here We Go: Emmission d'un son Mario Tunes
      -> Cam1: Déclenchment de la caméra 1 
      -> Cam2: Déclenchment de la caméra 2
      -> Cam 1 & 2: Déclenchement des 2 Caméras simultanément

    - PDV PHOTO: Permet de prendre des séries de photos via les paramètres initialisés dans le menu Réglages
 -------------------------------------------------------------------------------------------------------------------------------------   
COMMING SOON : ADDING SHUT DOWN MENU §§
*/

// Import Useful Libraries
#include <LiquidCrystal595.h>  // For LCD UI
#include <AccelStepper.h>  // Motors
#include <Rotary.h>  // Rotator & Bouton
#include <TimerOne.h>
#include <EEPROM.h> // To save Defaults Parameters 
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

/* Transmission Periode --- For Serial Communication*/
#define periodeSerCom 10

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
byte adr_cam_synchro = 22;
byte adr_tps_entre_cam = 24;

byte adr_end_stop = 26;

int tps_pause = 500;
int tps_stab = 100;
int nb_photos = 1;
byte nb_h = 5;
byte nb_v = 4;
int nb_total;
int dist_h = 30;
int dist_v = 20;
bool cam_synchro = 1;
int tps_entre_cam = 1000;

bool end_stop = 1; // End Stop Parameter !

//Parametres_deplacements
int stepspercmX = 383;
int stepspercmY = 1430;

long stepsX = 0;
long stepsY = 0;
int speedX = 2000;
int accX = 2000;
int speedY = 2000;
int accY = 2000;

bool StopPDV = false;


// parameter_ parametre(type, val, step, min, max, name)
parameter_ p_tps_pause("int", tps_pause, 100, 0, 30000, "TPS DE PDV");
parameter_ p_tps_stab("int", tps_stab, 100, 0, 30000, "TPS STAB");
parameter_ p_nb_photos("int", nb_photos, 1, 1, 6, "NB IMG/PDV");
parameter_ p_nb_h("int", nb_h, 1, 1, 500, "NB PDV HORI");
parameter_ p_nb_v("int", nb_v, 1, 1, 500, "NB PDV VERT");
parameter_ p_dist_h("int", dist_h, 1, 1, 500, "DEPLAC HOR");
parameter_ p_dist_v("int", dist_v, 1, 1, 500, "DEPLAC VER");
parameter_ p_cam_synchro("bool", cam_synchro, 1, 0, 1, "CAM SYNCHRO");
parameter_ p_tps_entre_cam("int", tps_entre_cam, 100, 0, 30000, "TPS ENTRE CAM");

// FIN De Course !!
parameter_ p_end_stop("bool", end_stop, 1, 0, 1, "FIN DE COURSE");


//Parametres moteur

parameter_ p_speedX("int", speedX, 100, 0, 5000, "VIT MOT X");
parameter_ p_speedY("int", speedX, 100, 0, 5000, "VIT MOT Y");
parameter_ p_accX("int", accX, 250, 0, 10000, "ACC MOT X");
parameter_ p_accY("int", accY, 250, 0, 10000, "ACC MOT Y");

parameter_* Liste_Reglages[] = {&p_tps_pause, &p_tps_stab, &p_nb_photos, &p_nb_h, &p_dist_h, &p_nb_v, &p_dist_v, &p_cam_synchro, &p_tps_entre_cam, &p_end_stop};

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

/**  CARACTERE SPECIAUX **/
byte warning[8]{
    B11111,
    B10001,
    B10101,
    B10101,
    B10001,
    B10101,
    B10001,
    B11111
  }; 

void setup() {


  //En cas de modification des réglages, pour réécrire les paramètres par défaut, décommenter SaveParam()
  //SaveParameters();
  lcd.createChar(0, warning); // creating caractere 
  ReadParameters();
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
  lcd.print(centerElement("ARBALETE V4"));
  lcd.setCursor(0, 1);
  lcd.print(centerElement("Encore mieux!"));
  beep();
  delay(500);
  lcd.clear();


  //vw_set_tx_pin(RF);
  //vw_setup(2000);

  digitalWrite(RF, 0);


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
        lcd.setCursor(0, 1);
        lcd.print(centerElement("PosX"+String(StepperX.currentPosition()/stepspercmX)+"  PosY"+String(StepperY.currentPosition()/stepspercmY)));
        break;

      case 2:
        lcd.print("<3  SET ZERO   >");
        lcd.setCursor(5, 1);
        lcd.print("CLICK");
        break;

      case 3:
        lcd.print("<4  GOTO ZERO  >");
        lcd.setCursor(5, 1);
        lcd.print("CLICK");
        break;

      case 4:
        lcd.print("<5 TEST RADIO  >");
        lcd.setCursor(6, 1);
        lcd.print("CLICK");
        break;

      case 5:
        lcd.print("<6 PDV PHOTO   >");
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
    if (Menu > 5) {
      Menu = 0;
    };
    updatelcdMenu = true;

  }
  else if (result == DIR_CCW)
  {
    Menu --;
    if (Menu < 0) {
      Menu = 5;
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
        setZero();
        break;
      case 3:
        gotoZero();
        break;
      case 4:
        envoiMsg();
        //RF_test();
        //Mario();
        break;
      case 5:
        pdv();
        break;
    }
  }
}


void setZero() {
  /* Set Current Postions as Zero  */
  StepperX.setCurrentPosition(0);
  StepperY.setCurrentPosition(0);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(centerElement("POS INIT OK"));
  delay(1000);
}

void gotoZero() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(centerElement("GOTO ZERO"));
  delay(1000);
  Deplace2Moteurs(&StepperX, &StepperY, 0, 0, Xmax, Ymax);
  delay(500);
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


// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ###################################### PRISE DE VUES #################################
// ######################################               #################################
// ######################################               #################################
// ######################################################################################


// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ###################################### PRISE DE VUES #################################
// ######################################               #################################
// ######################################               #################################
// ######################################################################################

void alerteArbaletePasZero() { // Display alertes

  //char errorMsg[] = "ATTENTION ! ARBALETE PAS EN POSITION INIT";
  int positionCounter = 0;
  long timer = millis();
  bool sortie = false;
  bool retour = true;
  updatelcd = true;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  lcd.print(" Pos NOT ZerO ");
  lcd.write(byte(0));
  lcd.setCursor(0, 1);
  lcd.print(String(" <GOTO 0> SET 0 "));

  while (!sortie) {
    unsigned char result = rotary.process();

    if (result == DIR_CW) {

      lcd.setCursor(0, 1);
      lcd.print(String(" GOTO 0 <SET 0> "));
      retour = false;
    }

    if (result == DIR_CCW) {

      lcd.setCursor(0, 1);
      lcd.print(String("<GOTO 0> SET 0 "));
      retour = true;

    }
    if (digitalRead(Bouton) == 0)
    {
      delay(250);
      sortie = true;
      if (retour) gotoZero();
      else setZero();

    }
  }
}



void pdv()
{
  StopPDV = false;

  if (StepperX.currentPosition() != 0 || StepperY.currentPosition() != 0)
  {
    alerteArbaletePasZero(); // Alerte arbalete pas à 0
  }


  //////////////
  // SI ZERO NA PAS ETE REGLE, METTRE UN WARNING AVEC OPTION DE SORTIE
  //PAR EXEMPLE "ATTENTION, LE ZERO NEST PAS RÉGLÉ."
  //            "ARRETER PDV? <OUI> NON           "
  //SI L'UTILISATEUR CHOISIT NON, ON APPELLE SETZERO().

  stepsX = long(p_dist_h._val) * long(stepspercmX ); //Distance parcourue entre chaque photos horizontale en pas
  stepsY = long(p_dist_v._val) * long(stepspercmY); //Distance parcourue entre chaque photos verticale en pas
  int nb_total = int(p_nb_h._val) * int(p_nb_v._val); //nombre total d'images pour la pdv
  int cpt_photos = 1;
  bool dirhor = 1;    //Direction horizontale pour le déplacement (pas sur que ça serve)

  // LANCEMENT DE LA PRISE DE VUE
  digitalWrite(RF, 0); //Au cas ou, on reset la communication filaire
  digitalWrite(Enable, HIGH); //On met enable à high pour pouvoir faire fonctionner les moteurs

  lcd.clear();
  lcd.setCursor(0, 0);

  if (p_cam_synchro._val == 1) lcd.print(centerElement("CAM SYNCHRONES"));
  else lcd.print(centerElement("CAM ASYNCHRONES /!\\"));

  lcd.setCursor(0, 1);
  lcd.print(centerElement(String("...3..."))) ;
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print(centerElement(String("...2..."))) ;
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print(centerElement(String("...1..."))) ;
  delay(500);

  for (int i = 1; i <= int(p_nb_v._val); i++ ) //on boucle sur les pdv verticales
  {
    for (int j = 1; j < int(p_nb_h._val); j++ ) //on boucle sur les pdv horizontales
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


      //BOUCLE D'ATTENTE DE STABILISATION APRES DÉPLACEMENT
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

      // ######################################################################################

      //ON PREND LES PHOTOS

      DeclenchementCameras();
      if (StopPDV == true) goto FINPDV;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(centerElement(String("PDV EN COURS")));
      lcd.setCursor(0, 1);
      txt = "PHOTO ";
      txt = txt + cpt_photos;
      txt = txt + " / ";
      txt = txt + nb_total;
      lcd.print(centerElement(String(txt)));
      // ######################################################################################

      cpt_photos++; //on incrémente le compteur de "zones"

      //ON AVANCE HORIZONTALLEMENT

      if (dirhor == 1)
      {
        DeplaceMoteurAbs(&StepperX, j * dirhor * stepsX, Xmax);
        if (StopPDV == true) goto FINPDV;
      }
      if (dirhor == 0)
      {
        DeplaceMoteurAbs(&StepperX, (p_nb_h._val - j - 1) * stepsX, Xmax);
        if (StopPDV == true) goto FINPDV;
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

    // ######################################################################################

    //ON PREND LES PHOTOS
    DeclenchementCameras();
    if (StopPDV == true) goto FINPDV;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(centerElement(String("PDV EN COURS")));
    lcd.setCursor(0, 1);
    String txt = "PHOTO ";
    txt = txt + cpt_photos;
    txt = txt + " / ";
    txt = txt + nb_total;
    lcd.print(centerElement(String(txt)));
    // ######################################################################################

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
  digitalWrite(Enable, LOW);
}




void camera1et2()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(centerElement("PDV CAM 1 ET 2"));
  digitalWrite(RF, 1); // bit de start !!
  delay(periodeSerCom);
  digitalWrite(RF, 1);
  delay(periodeSerCom);
  digitalWrite(RF, 1);
  delay(periodeSerCom);
  digitalWrite(RF, 1);
  delay(periodeSerCom);

  digitalWrite(RF, 0);

}
void camera2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(centerElement("PDV CAM 2"));
  digitalWrite(RF, 1); // bit de start !!
  delay(periodeSerCom);
  digitalWrite(RF, 1);
  delay(periodeSerCom);
  digitalWrite(RF, 0);
  delay(periodeSerCom);
  digitalWrite(RF, 1);
  delay(periodeSerCom);
  digitalWrite(RF, 0);
}
void camera1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(centerElement("PDV CAM 1"));
  digitalWrite(RF, 1); // bit de start !!
  delay(periodeSerCom);
  digitalWrite(RF, 0);
  delay(periodeSerCom);
  digitalWrite(RF, 1);
  delay(periodeSerCom);
  digitalWrite(RF, 1);
  delay(periodeSerCom);
  digitalWrite(RF, 0);
}


void DeclenchementCameras()
{
  //PRISE DE PHOTOS
  if (p_cam_synchro._val == 1)
  {
    for (int k = 1; k <= int(p_nb_photos._val); k++)
    { camera1et2();
      unsigned long previousMillis = millis();
      while (millis() - previousMillis <= long(p_tps_pause._val) || StopPDV == true)
      {
        if (!digitalRead(Bouton))
        {
          digitalWrite(RF, 0);
          MenuPause();
        }
        if (StopPDV == true) goto FINDECLENCHEMENT;
      }
    }
  }
  else
  { //ON PREND TOUTES LES PHOTOS DE LA CAMERA 1
    for (int k = 1; k <= int(p_nb_photos._val); k++)
    { camera1();
      unsigned long previousMillis = millis();
      while (millis() - previousMillis <= long(p_tps_pause._val) || StopPDV == true)
      {
        if (!digitalRead(Bouton))
        {
          digitalWrite(RF, 0);
          MenuPause();
        }
        if (StopPDV == true) goto FINDECLENCHEMENT;
      }
    }
    //ON ATTEND LE TEMPS ENTRE CAM
    unsigned long previousMillis = millis();
    while (millis() - previousMillis <= long(p_tps_entre_cam._val) || StopPDV == true)
    {
      if (!digitalRead(Bouton))
      {
        digitalWrite(RF, 0);
        MenuPause();
      }
      if (StopPDV == true) goto FINDECLENCHEMENT;
    }
    for (int k = 1; k <= int(p_nb_photos._val); k++)
    { camera2();
      unsigned long previousMillis = millis();
      while (millis() - previousMillis <= long(p_tps_pause._val) || StopPDV == true)
      {
        if (!digitalRead(Bouton))
        {
          digitalWrite(RF, 0);
          MenuPause();
        }
        if (StopPDV == true) goto FINDECLENCHEMENT;
      }
    }
  }
FINDECLENCHEMENT :
  delay(0);
}

void FINPDV()
{

  digitalWrite(RF, 0);
  lcd.setLED1Pin(LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(centerElement(String("FIN DE LA PDV")));
  delay(500);
  MenuRetourZero();
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



/*Added !!!
  BY YM
*/


void envoiMsg() {


  boolean sortie = false;
  updatelcd = true;
  int SubMenu = 0;
  //ReadParameters();

  while (sortie == false)
  {
    unsigned char result = rotary.process();
    if (updatelcd == true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      // lcd.print("< PDV >");

      switch (SubMenu) {

        case 0:
          lcd.print(centerElement("Here we Go !")); // We can add a task here !
          // Since we have 4 possibles tasks with 2 bits Communications tram !
          break;

        case 1:
          lcd.setCursor(0, 0);
          lcd.print(centerElement("CAM 1")); // Switch on the CAMERA 1
          break;

        case 2:
          lcd.setCursor(0, 0);
          lcd.print(centerElement("CAM 2")); // Switch on the CAMERA 2
          break;

        case 3:
          lcd.setCursor(0, 0);
          lcd.print(centerElement("CAM 1 + 2")); // Switch on the CAMERA 1 and 2
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

    if (digitalRead(Bouton) == 0 )
    {
      switch (SubMenu) {

        case 0: // *** Here We Go *** Nothing will happen

          digitalWrite(RF, 1);
          delay(periodeSerCom);

          digitalWrite(RF, 0);
          delay(2 * periodeSerCom);
          digitalWrite(RF, 1);
          delay(periodeSerCom);
          break;

        case 1: // **** Cam 1 = 1  cam 2 = 0
          camera1();
          break;


        case 2:    // **** Cam 1 = 0  cam 2 = 1
          camera2();
          break;


        case 3:  // **** cam 1 = cam 2 = 1
          camera1et2();
          break;


        case 4:
          digitalWrite(RF, 0);
          sortie = true;
          break;
      }
      delay(200);
      lcd.setCursor(0, 1);
      lcd.print(centerElement("DONE!"));
      digitalWrite(RF, 0);
    }
  }
}



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


void MenuChoixStep(AccelStepper* Moteur, int PasParCm, byte* EndStop)
{

  boolean sortie = false;
  updatelcd = true;
  int SubMenu = 0;
  long PositionEnCm = Moteur->currentPosition() / PasParCm;
  long destination = PositionEnCm;

  while (!sortie)
  {

    unsigned char result = rotary.process();
    if (updatelcd) {
      lcd.clear();
      lcd.setCursor(0, 0);
      PositionEnCm = Moteur->currentPosition() / PasParCm;
      lcd.setCursor(0, 0);
      lcd.print("MOVE TO : ");
      lcd.print(String(destination));
      lcd.setCursor(0, 1);
      lcd.print("POSTITION : ");
      lcd.print(String(PositionEnCm));
      updatelcd = false;
    }

    if (result == DIR_CW) {
      destination++;
      if (destination > 1000) destination = 1000;
      updatelcd = true;
    }
    if (result == DIR_CCW) {
      destination--;
      if (destination < -1000)  destination = -1000;
      updatelcd = true;
    }


    if (!digitalRead(Bouton)) { //Si on appuie sur le bouton
      delay(200); //Delay de debounce
      if ( destination == PositionEnCm) sortie = 1;
      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MOVING TO:");
        lcd.setCursor(0, 1);
        lcd.print(centerElement(String(destination)));
        long destinationenpas = destination * PasParCm;
        DeplaceMoteurAbs(Moteur, destinationenpas, EndStop);
        updatelcd = true;
        sortie = 1;
      }
    }
  }
}



void ArretMoteurs(AccelStepper * Moteur)
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


void DeplaceMoteurAbs(AccelStepper * Moteur, long Pos, int EndStop) // why this was as long Depl..
{

  digitalWrite(Enable, LOW);
  bool sortie = false;
  Moteur->moveTo(Pos);

  while (sortie == false)
  {
    if (Moteur->distanceToGo() == 0) sortie = true;

    if (p_end_stop._val == 1) {

      if (digitalRead(EndStop) == 0) {
        beep();
        Touch_EndStop(Moteur, EndStop );
        sortie = true;
      }
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


void Touch_EndStop(AccelStepper * Moteur, int EndStop)
{

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FIN DE COURSE");
  lcd.setCursor(0, 1);
  lcd.print("EN CONTACT");
  digitalWrite(Enable, LOW);
  int HomingPos = 1;
  Moteur->setCurrentPosition(0);
  Moteur->setSpeed(1000);
  while (!digitalRead(EndStop))
  { Moteur->moveTo(HomingPos);
    Moteur->run();
    HomingPos += 10;
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
    if (p_end_stop._val == 1) {
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
  //Ajout synchro cam
  EEPROMWriteInt(adr_cam_synchro, p_cam_synchro._val);
  EEPROMWriteInt(adr_tps_entre_cam, p_tps_entre_cam._val);

  // Ajout End Stop !!
  EEPROMWriteInt(adr_end_stop, p_end_stop._val);


  tone(Buzzer, 2000);
  delay(100);
  noTone(Buzzer);
  delay(50);
  tone(Buzzer, 1000);
  delay(100);
  noTone(Buzzer);

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
  //Ajout synchro

  p_cam_synchro._val = EEPROMReadInt(adr_cam_synchro);
  p_tps_entre_cam._val = EEPROMReadInt(adr_tps_entre_cam);

  p_end_stop._val = EEPROMReadInt(adr_end_stop);

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
