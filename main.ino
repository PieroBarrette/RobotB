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


/* ****************************************************************************
Variables globales et defines
**************************************************************************** */
const double circonferenceRoueRB = 24.19; //Pour une meilleure précision, mettre une circonférence pour chaque roue et ajuster la fonction tourner

const double circonferenceCercleUneRoue = 116.43;//116.73
const double circonferenceCercleDeuxRoues = 58.119;

const double vitesseRoueGauche = 0.40;

double vitesseVirageG = 0.3;
double vitesseVirageD = 0.3;

double pourcentageAcceleration = 0.20;

#define Nbre_Encodeur_roue 3200

#define distAccMax 1000
#define pourcentageInitiale 0.35

#define kp 0.001
#define ki 0.0002
#define tempsDeCycle 250

/* ****************************************************************************
Vos propres fonctions sont creees ici
**************************************************************************** */

//roueGouD varie entre {0,1}
void TourneUneRoue(double degres,int roueGouD){ 
  double distanceAparcourirEnCm = degres/360 * circonferenceCercleUneRoue;
  double nbreDePulseTotal = distanceAparcourirEnCm/circonferenceRoueRB * Nbre_Encodeur_roue;
  EncoderReset();
  MOTOR_SetSpeed(roueGouD,0.3);
  bool activationDesRoues = true;
  while(activationDesRoues){
    Serial.println(ENCODER_Read(roueGouD));
    if(ENCODER_Read(roueGouD) >= nbreDePulseTotal){ 
      MOTOR_SetSpeed(roueGouD,0);
      activationDesRoues = false;
    }
    if(ROBUS_IsBumper(3)){
      Arretdurgence();
      activationDesRoues = false;
    }
  }
}
void TournerDeuxRoues(double degres, int gaucheOuDroite){ //gauche == -1, droite == 1
  double distanceAparcourirEnCm = degres/360 * circonferenceCercleDeuxRoues;
  double nbreDePulseTotal = distanceAparcourirEnCm/circonferenceRoueRB * 3200;
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  MOTOR_SetSpeed(0,vitesseVirageG * gaucheOuDroite);
  MOTOR_SetSpeed(1,vitesseVirageD * (-gaucheOuDroite));
  bool activationDesRoues = true;
  while(activationDesRoues){
    if(ENCODER_Read(0) >= nbreDePulseTotal || ENCODER_Read(1) >= nbreDePulseTotal){
      MotorSpeed(0);
      activationDesRoues = false;
    }
    if(ROBUS_IsBumper(3)){
      Arretdurgence();
      activationDesRoues = false;
    }
  }
}
/*void Avancer2(double distanceAparcourirEnCm){
  double distAcc = 0;
  double nbreDePulseTotal = distanceAparcourirEnCm/circonferenceRoueRB * 3200;
  if(nbreDePulseTotal>=2000){
    distAcc = distAccMax;
  }
  else{
    distAcc = nbreDePulseTotal/2;
  }
  FaireRoulerRobot((int)nbreDePulseTotal,(int)distAcc); 
  delay(50);
}
void FaireRoulerRobot(int nbreDePulseTotal, int distAcc){
  EncoderReset();
  Motor2Speed(vitesseRoueGauche,vitesseRoueDroite);
  bool activationDesRoues = true;
  while(activationDesRoues){
    if(ENCODER_Read(0) > nbreDePulseTotal - distAcc){
      if(ENCODER_Read(0) > nbreDePulseTotal - distAcc*pourcentageInitiale){
        Motor2Speed(vitesseRoueGauche*pourcentageInitiale,vitesseRoueDroite*pourcentageInitiale);
      }
      else{
        Motor2Speed(vitesseRoueGauche*(nbreDePulseTotal-ENCODER_Read(0))/distAcc, vitesseRoueDroite*(nbreDePulseTotal-ENCODER_Read(0))/distAcc);
      }
    }
    if(ENCODER_Read(0) >= nbreDePulseTotal){ //la roue a parcouru la bonne distance, donc elle s'arrete
      MotorSpeed(0);
      activationDesRoues = false;
      Serial.println("JE DEVRAIS ETRE ARRETER");
    }
    if(ROBUS_IsBumper(3)){ //arret d'urgence
      Arretdurgence();
      activationDesRoues = false;
    }
    Serial.println(ENCODER_Read(0));
  }
}*/
void Avancer(float distanceAparcourirEnCm){
  double nbreDePulseTotal = distanceAparcourirEnCm/circonferenceRoueRB * 3200;
  Serial.println(nbreDePulseTotal);
  AsservissementDesMoteurs(nbreDePulseTotal,vitesseRoueGauche, tempsDeCycle);
}
void AsservissementDesMoteurs(int nbreDePulseTotal,double vitesseG,int delay_){
  bool activationDesRoues=true;
  double vitesseD=vitesseG;
  double vitesseCalculG,vitesseCalculD;
  int nbreDeCycle = 0;
  int pulse_par_cycle;
  int pulseTotaleDroit = 0;
  EncoderReset();
  Motor2Speed(vitesseG,vitesseD);
  while(activationDesRoues){
    activationDesRoues = DelayAsservissement(delay_,nbreDePulseTotal);
    if(!activationDesRoues){
      MotorSpeed(0);
    }
    else{
      if(++nbreDeCycle==1){
        pulse_par_cycle = ENCODER_Read(0);
        Serial.println("le nombre de pulse par cycle est : ");
        Serial.println(pulse_par_cycle);
      }
      vitesseCalculG = CalculVitessse(delay_,pulse_par_cycle);
      vitesseCalculD = CalculVitessse(delay_,(ENCODER_Read(1)-pulseTotaleDroit));
      pulseTotaleDroit=ENCODER_Read(1);

      vitesseD += (vitesseCalculG-vitesseCalculD)*kp+(/*(nbreDeCycle*pulse_par_cycle)*/ENCODER_Read(0)-ENCODER_Read(1))*ki;
      MOTOR_SetSpeed(1,vitesseD);

      Serial.println("vitesseCalculD");
      Serial.println(vitesseCalculD);
      Serial.println("vitesseCalculG");
      Serial.println(vitesseCalculG);
      Serial.println("(nbreDeCycle*pulse_par_cycle)-ENCODER_Read(1)");
      Serial.println(ENCODER_Read(0)-ENCODER_Read(1));
      if(ROBUS_IsBumper(3)){
        Arretdurgence();
        activationDesRoues = false;
      }
    }
    
    /*else if(ENCODER_Read(0)>=nbreDePulseTotal){
      activationDesRoues = false;
      Serial.println(ENCODER_Read(0));
    }*/
  }
}
bool DelayAsservissement(int delay_, int nbreDePulseTotal){
  bool activationDesRoues = true;
  for (int i = 0;i<delay_;i+=10){
    if (ENCODER_Read(0)>=nbreDePulseTotal){
      activationDesRoues=false;
      i += delay_;
      Serial.println("La distance totale a ete parcourue. ");
      Serial.println(ENCODER_Read(0));
    }
    else
      delay(10);
  }
  Serial.println("Le robot a fait un cycle");
  return activationDesRoues;
}
double CalculVitessse(int delay_,int nbreDePulseTotal){
  double vitesse = nbreDePulseTotal/delay_;
  return vitesse;
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
  if(ROBUS_IsBumper(0)){
    TourneUneRoue(90,1);
  }
  if(ROBUS_IsBumper(1)){
    TourneUneRoue(90,0);
  }
  if(ROBUS_IsBumper(2)){
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