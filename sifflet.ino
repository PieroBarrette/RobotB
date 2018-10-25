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

/* ****************************************************************************
Variables globales et defines
**************************************************************************** */
//parametres pour tourner
const double circonferenceRoue = 24.19;

const double circonferenceCercleUneRoue = 116.43;  //116.43  116(trop vite)
const double circonferenceCercleDeuxRoues = 58.119; //58.119

#define Encodeur_roue 3200


/* ****************************************************************************
Vos propres fonctions sont creees ici
**************************************************************************** */



void avancertest(int secondes){
  long temps = secondes * 100;
  unsigned long temps1, temps2;
  temps1 = millis();
  temps2 = temps1 + temps;
  while (temps1 < temps2){
    temps1 = millis();
    MOTOR_SetSpeed(0,0.3);
    MOTOR_SetSpeed(1,0.3);
    sifflet();
  }
  MOTOR_SetSpeed(0,0);
  MOTOR_SetSpeed(1,0);
}

void Arretdurgence(){
    MOTOR_SetSpeed(0,0);
    MOTOR_SetSpeed(1,0);
}

void sifflet(){
  int j = 0;
  while (true){
    float sifflet = analogRead(12);
    float bruit = analogRead(13);
    Serial.print(bruit);
    Serial.print("//");
    Serial.print(sifflet);
    Serial.print("//");
    Serial.println(bruit-sifflet);
    if (bruit - sifflet <= 0){
      Serial.print("****");
      Serial.print("5KHz?");
      Serial.println("****");
      j += 1;
      if (j>=10){
      Serial.print("/////");
      Serial.print("SIFFLET");
      Serial.println("/////");
      MOTOR_SetSpeed(0,0);
      MOTOR_SetSpeed(1,0);
      delay(11000);
      MOTOR_SetSpeed(0,0.3);
      MOTOR_SetSpeed(1,0.3);
      }
    }
  }
}


/* ****************************************************************************
Fonctions d'initialisation (setup)
**************************************************************************** */

void setup(){
  BoardInit();
  Serial.begin(9600);
}


/* ****************************************************************************
Fonctions de boucle infini (loop())
**************************************************************************** */
// -> Se fait appeler perpetuellement suite au "setup"

void loop(){
  if (ROBUS_IsBumper(0)){
    avancertest(120);
    sifflet();
  } 
}
