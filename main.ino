/*
Projet: Le nom du script
Equipe: P-20 / b
Auteurs: 
Description: Défi du parcours
Date: Derniere date de modification
*/

/* ****************************************************************************
Inclure les librairies de functions que vous voulez utiliser
**************************************************************************** */

#include <LibRobus.h> // Essentielle pour utiliser RobUS
#include <math.h>
#include <arduino.h>

/* ****************************************************************************
Variables globales et defines
**************************************************************************** */
//parametres pour tourner
const double circonferenceRoue = 24.19;

const double circonferenceCercleUneRoue = 116.43;  //116.43  116(trop vite)
const double circonferenceCercleDeuxRoues = 58.119; //58.119

#define Encodeur_roue 3200

float vitesseVirageT = 0.3;

float vitesse  = 0.7;

/* ****************************************************************************
Vos propres fonctions sont creees ici
**************************************************************************** */

void tourner(float degre, int id){
  double distance = degre/360 * circonferenceCercleUneRoue;
  double pulses = distance/circonferenceRoue * Encodeur_roue;
  bool activationDesRoues=true;
  MOTOR_SetSpeed(id, vitesseVirageT);
  ENCODER_Reset(id);
  delay(10);
  while(activationDesRoues){
    long pulsesDG = ENCODER_Read(id);
    if(pulsesDG >= pulses){ 
      activationDesRoues=false;
    }
  }
  MOTOR_SetSpeed(0,0);
  MOTOR_SetSpeed(1,0);
}

void tourner2(float degre){
  double distance = degre/360 * circonferenceCercleDeuxRoues;
  double pulses = distance/circonferenceRoue * Encodeur_roue;
  ENCODER_Reset(1);
  ENCODER_Reset(0);
  MOTOR_SetSpeed(0, -vitesseVirageT);
  MOTOR_SetSpeed(1, vitesseVirageT);
  asservissement(pulses, vitesseVirageT);
}

void Avancer(float distance){
 double pulses = distance/circonferenceRoue * 3200;
 ENCODER_Reset(0);
 ENCODER_Reset(1);
  MOTOR_SetSpeed(0,vitesse);
  MOTOR_SetSpeed(1,vitesse);
 asservissement(pulses, vitesse); 
}

void asservissement(double pulses, float vitesse){
  bool activationDesRoues=true;
  delay(20);
  while (activationDesRoues){
    long pulsesG = abs(ENCODER_Read(0));
    long pulsesD = abs(ENCODER_Read(1));
    if (pulsesG >= pulses || pulsesD >= pulses){
      activationDesRoues = false;
    }
    float ratio = (pulsesG/pulsesD);
    float vitesseD = (vitesse*ratio);
    MOTOR_SetSpeed(1, vitesseD);
    }
  MOTOR_SetSpeed(1, 0);
  MOTOR_SetSpeed(0, 0);
 }



void Arretdurgence(){
    MOTOR_SetSpeed(0,0);
    MOTOR_SetSpeed(1,0);
}
void DefiParcours(){
  Avancer(220);//213.225
  tourner(90,1);
  Avancer(31.45);
  tourner(90,0);
  Avancer(24.45);//26.45
  tourner(90,0);
  Avancer(30.3235);//30.3235
  tourner(90,1);
  Avancer(20.5765);//22.5765    //Pour bien s'alligner avec le virage dans la trajecoire diagonale
  tourner(45,0);
  Avancer(30.0985);//32.0985
  tourner(90,1);
  Avancer(53.225);//53.225
  tourner(47,0);//45
  Avancer(32);
  tourner(22,0);//12.5
  Avancer(77);
  Arretdurgence();
  tourner2(180);//**************************************************
  Avancer(77);
  tourner(18,1);//12.5
  Avancer(32);
  tourner(45,1);
  Avancer(53.225);//53.225
  tourner(90,0);
  Avancer(30.0985);//32.0985
  tourner(45,1);
  Avancer(22.5765);//22.5765 
  tourner(90,0);
  Avancer(28.3235);//30.3235
  tourner(90,1);
  Avancer(24.45);//26.45
  tourner(95,1);
  Avancer(31.45);
  tourner(90,0);//90
  Avancer(223.225);//213.225

  Arretdurgence();
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

void loop(){
  if(ROBUS_IsBumper(3)){
   DefiParcours();
  }
}