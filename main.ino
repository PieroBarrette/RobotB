/*
Projet: défi du parcours
Equipe: P-20
Auteurs: Loic
Description: Breve description du script
Date: 2018-09-24
*/

/* ****************************************************************************
Inclure les librairies de functions que vous voulez utiliser
**************************************************************************** */

#include <LibRobus.h> // Essentielle pour utiliser RobUS
#include <Arduino.h>


/* ****************************************************************************
Variables globales et defines
**************************************************************************** */
double circonferenceRoueRB = 24.19; //Pour une meilleure précision, mettre une circonférence pour chaque roue et ajuster la fonction tourner

double circonferenceCercleUneRoue = 116.73;
double circonferenceCercleDeuxRoues = 58.119;

double vitesseRoueGauche = 0.5;
double vitesseRoueDroite = 0.5;
double vitesseVirageG = 0.3;
double vitesseVirageD = 0.3;

double pourcentageAcceleration = 0.20;

#define Nbre_Encodeur_roue 3200
#define distAccMax 1000
#define vitesseInitiale 0.35


/* ****************************************************************************
Vos propres fonctions sont creees ici
**************************************************************************** */

//roueGouD varie entre {0,1}
void TourneUneRoue(double degres,int roueGouD){ 
  double distanceAparcourirEnCm = degres/360 * circonferenceCercleUneRoue;
  double nbreDePulse = distanceAparcourirEnCm/circonferenceRoueRB * Nbre_Encodeur_roue;
  Serial.println(nbreDePulse);
  ENCODER_Reset(roueGouD);
  MOTOR_SetSpeed(roueGouD,0.3);
  bool activationDesRoues = true;
  while(activationDesRoues){
    if(ENCODER_Read(roueGouD) >= nbreDePulse){ 
      MOTOR_SetSpeed(roueGouD,0);
      activationDesRoues = false;
    }
    if(AX_IsBumper(3)){
    Arretdurgence();
    activationDesRoues = false;
    }
  }
  delay(50);
}
void TournerDeuxRoues(double degres, int gaucheOuDroite){ //gauche == -1, droite == 1
  double distanceAparcourirEnCm = degres/360 * circonferenceCercleDeuxRoues;
  double nbreDePulse = distanceAparcourirEnCm/circonferenceRoueRB * 3200;
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  MOTOR_SetSpeed(0,vitesseVirageG * gaucheOuDroite);
  MOTOR_SetSpeed(1,vitesseVirageD * (-gaucheOuDroite));
  bool activationDesRoues = true;
  while(activationDesRoues){
    if(ENCODER_Read(0) >= nbreDePulse || ENCODER_Read(1) >= nbreDePulse){
      MotorSpeed(0);
      activationDesRoues = false;
    }
    if(AX_IsBumper(3)){
      Arretdurgence();
      activationDesRoues = false;
    }
  }
}
void Avancer(double distanceAparcourirEnCm){
  double distAcc = 0;
  double nbreDePulse = distanceAparcourirEnCm/circonferenceRoueRB * 3200;
  if(nbreDePulse>=2000){
    distAcc = distAccMax;
  }
  else{
    distAcc = nbreDePulse/2;
  }
  FaireRoulerRobot((int)nbreDePulse,(int)distAcc); 
  delay(50);
}
void FaireRoulerRobot(int nbreDePulse, int distAcc){
  EncoderReset();
  Motor2Speed(vitesseRoueGauche,vitesseRoueDroite);
  bool activationDesRoues = true;
  while(activationDesRoues){
    if(ENCODER_Read(0) > nbreDePulse - distAcc){
      if(ENCODER_Read(0) > nbreDePulse - distAcc*vitesseInitiale){
        Motor2Speed(vitesseRoueGauche*vitesseInitiale,vitesseRoueDroite*vitesseInitiale);
      }
      else{
        Motor2Speed(vitesseRoueGauche*(nbreDePulse-ENCODER_Read(0))/distAcc, vitesseRoueDroite*(nbreDePulse-ENCODER_Read(0))/distAcc);
      }
    }
    if(ENCODER_Read(0) >= nbreDePulse){ //la roue a parcouru la bonne distance, donc elle s'arrete
      MotorSpeed(0);
      activationDesRoues = false;
      Serial.println("JE DEVRAIS ETRE ARRETER");
    }
    if(AX_IsBumper(3)){ //arret d'urgence
      Arretdurgence();
      activationDesRoues = false;
    }
    Serial.println(ENCODER_Read(0));
  }
}
void Arretdurgence(){
  MotorSpeed(0);
  Serial.println("Arret d'urgence");
}
void MotorSpeed(double vitesse){
  MOTOR_SetSpeed(0,vitesse);
  MOTOR_SetSpeed(1,vitesse);
}
void Motor2Speed(double vitesseG, double vitesseD){
  MOTOR_SetSpeed(0,vitesseG);
  MOTOR_SetSpeed(1,vitesseD);
}
void EncoderReset(){
  ENCODER_Reset(0);
  ENCODER_Reset(1);
}
void Avancer2(float distanceAparcourirEnCm){
  double nbreDePulse = distanceAparcourirEnCm/circonferenceRoueRB * 3200;
  EncoderReset();
  bool activationDesRoues = true;
  Motor2Speed(vitesseRoueGauche,vitesseRoueDroite);
  while(activationDesRoues){
    if(ENCODER_Read(0)>=nbreDePulse || ENCODER_Read(1)>=nbreDePulse){
      activationDesRoues = false ;
    }
  }
  MotorSpeed(0);
}
/* ****************************************************************************
Fonctions d'initialisation (setup)
**************************************************************************** */

void setup(){
  BoardInit();
}


/* ****************************************************************************
Fonctions de boucle infini (loop())
**************************************************************************** */
// -> Se fait appeler perpetuellement suite au "setup"

void loop() {
  // SOFT_TIMER_Update(); // A decommenter pour utiliser des compteurs logiciels


  if(AX_IsBumper(0)){
    TourneUneRoue(90,1);
  }
  if(AX_IsBumper(1)){
    TourneUneRoue(90,0);
  }
  if(AX_IsBumper(2)){
    Avancer(213.25);
    TourneUneRoue(90,1);
    Avancer(31.5);
    TourneUneRoue(90,0);
    Avancer(26.5);
    TourneUneRoue(90,0);
    Avancer(26.5);
    TourneUneRoue(90,1);
    Avancer(18.55);//Pour bien s'alligner avec le virage dans la trajecoire diagonale
    TourneUneRoue(21.8,0);
    // ... 
    MotorSpeed(0);
    
  }

  delay(10);// Delais pour décharger le CPU
}