# ARBALETE PARAMETER SETTINGS

With LCD Display, several features can be activated : 

# This Code contains UI for Arbalete Features Control
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

## SERIAL COMMUNICATION 
- Using 4 bits word for transmission 
- One Wire -> Half Duplex
- 1 xx 1 : xx are states

## MOTIONS
- Using MGI with absolute and relative coordinates
