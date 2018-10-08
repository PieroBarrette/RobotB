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
//parametres pour tourner
const double circonferenceRoueRB = 24.19;

const double circonferenceCercleUneRoue = 116.43;  //116.43  116(trop vite)
const double circonferenceCercleDeuxRoues = 58.119; //58.119

double virage_PpSec = 3000;

#define Nbre_Encodeur_roue 3200

//parametres pour avancer

/*#define kp 0.05
#define ki 0.0002*/
double vitesseVirageT = 0.30;

double v_enPulseParSec = 6000;
#define tempsDeCycle 100
#define kg 0.02
#define kd 0.02

/* ****************************************************************************
Vos propres fonctions sont creees ici
**************************************************************************** */

//id varie entre {0,1}
/*void TournerUneRoue(double degres,int id){
  double distanceAparcourirEnCm = degres/360 * circonferenceCercleUneRoue;
  double nbreDePulseTotal = distanceAparcourirEnCm/circonferenceRoueRB * Nbre_Encodeur_roue;
  Serial.println(nbreDePulseTotal);
  if (id == 0)
    AsservissementDesMoteurs2(nbreDePulseTotal,virage_PpSec,0,tempsDeCycle);
  else
    AsservissementDesMoteurs2(nbreDePulseTotal,0,virage_PpSec,tempsDeCycle);
}*/
void TournerSurPlace(double degres, int g_ou_d){ //gauche == -1, droite == 1
  double distanceAparcourirEnCm = degres/360 * circonferenceCercleDeuxRoues;
  double nbreDePulseTotal = distanceAparcourirEnCm/circonferenceRoueRB * 3200;
  AsservissementDesMoteurs2(nbreDePulseTotal,virage_PpSec*g_ou_d,virage_PpSec*-1*g_ou_d,tempsDeCycle);
}
void TournerUneRoue(double degres,int id){ 
  double distanceAparcourirEnCm = degres/360 * circonferenceCercleUneRoue;
  double nbreDePulseTotal = distanceAparcourirEnCm/circonferenceRoueRB * Nbre_Encodeur_roue;
  EncoderReset();
  if (id == 0)
    Motor2Speed(vitesseVirageT,0);
  else
    Motor2Speed(0,vitesseVirageT);
  bool activationDesRoues = true;
  while(activationDesRoues){
    //Serial.println(ENCODER_Read(id));
    if(ENCODER_Read(id) >= nbreDePulseTotal){ 
      activationDesRoues = false;
    }
    if(ROBUS_IsBumper(3)){
      Arretdurgence();
      activationDesRoues = false;
    }
  }
  MotorSpeed(0);
}
void Avancer(double distanceAparcourirEnCm){
  double nbreDePulseTotal = distanceAparcourirEnCm/circonferenceRoueRB * 3200;
  Serial.println(nbreDePulseTotal);
  AsservissementDesMoteurs2(nbreDePulseTotal,v_enPulseParSec,v_enPulseParSec,tempsDeCycle);
  MotorSpeed(0);
}
void AsservissementDesMoteurs2(int nbreDePulseTotal,double nbreDePulse_sec_G,double nbreDePulse_sec_D, double delay_){
  double vitesseDesireeG = CalculVitesse(delay_,(nbreDePulse_sec_G*delay_/1000));
  double vitesseDesireeD = CalculVitesse(delay_,(nbreDePulse_sec_D*delay_/1000));
  double vitesseG = vitesseDesireeG/100;
  double vitesseD = vitesseDesireeD/100;
  double erreurG,erreurD;
  bool activationDesRoues=true;
  double pulseParCycleGauche,pulseParCycleDroit;
  double pulseTotalG=0,pulseTotalD=0;
  EncoderReset();
  Motor2Speed(vitesseG,vitesseD);
  while(activationDesRoues){
    activationDesRoues = DelayAsservissement(delay_,nbreDePulseTotal);
    pulseParCycleGauche = ENCODER_Read(0) - pulseTotalG;
    pulseParCycleDroit = ENCODER_Read(1) - pulseTotalD;
    pulseTotalG=ENCODER_Read(0);
    pulseTotalD=ENCODER_Read(1);
    
    if(activationDesRoues){
      erreurG = vitesseDesireeG - CalculVitesse(delay_,pulseParCycleGauche);
      erreurD = vitesseDesireeD - CalculVitesse(delay_,pulseParCycleDroit);
      vitesseG += erreurG*kg;
      vitesseD += erreurD*kd;
      Motor2Speed(vitesseG,vitesseD);
    }
  }
}

/*void AsservissementDesMoteurs(int nbreDePulseTotal,double vitesseG,int delay_){
  bool activationDesRoues=true;
  double vitesseD=vitesseG;
  double vitesseCalculG,vitesseCalculD;
  int nbreDeCycle = 0;
  int pulse_par_cycle;
  int pulseParCycleGauche = 0;
  int pulseParCycleDroit = 0;
  EncoderReset();
  Motor2Speed(vitesseG,vitesseD);
  while(activationDesRoues){
    activationDesRoues = DelayAsservissement(delay_,nbreDePulseTotal);
    if(!activationDesRoues){
      MotorSpeed(0);
    }
    else{
      
      vitesseCalculG = CalculVitesse(delay_,ENCODER_Read(0)-pulseParCycleGauche);
      pulseParCycleGauche = ENCODER_Read(0);
      vitesseCalculD = CalculVitesse(delay_,(ENCODER_Read(1)-pulseParCycleDroit));
      pulseParCycleDroit = ENCODER_Read(1);

      if(++nbreDeCycle==1){
        pulse_par_cycle = pulseParCycleGauche;
      }
      vitesseD += (vitesseCalculG-vitesseCalculD)*kp;//+((pulseParCycleGauche)-pulseParCycleDroit)*ki;
      MOTOR_SetSpeed(1,vitesseD);
      
      if(ROBUS_IsBumper(3)){
        Arretdurgence();
        activationDesRoues = false;
      }
    }
  }
}*/
bool DelayAsservissement(int delay_, int nbreDePulseTotal){
  bool activationDesRoues = true;
  int i = 0;
  while (i<delay_){
    if (ENCODER_Read(0)>=nbreDePulseTotal||ENCODER_Read(1)>=nbreDePulseTotal){
      activationDesRoues=false;
      i += delay_;
      Serial.println("La distance totale a ete parcourue. ");
      Serial.println(ENCODER_Read(0));
    }
    else if (ROBUS_IsBumper(3)){
      Arretdurgence();
      i+=delay_;
    }
    else{
      i++;
      delay(1);
    }
  }
  return activationDesRoues;
}
double CalculVitesse(double delay_,double nbreDePulseTotal){
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
void DefiParcours(){
  Avancer(213.225);//213.225
  TournerUneRoue(90,1);
  Avancer(31.45);
  TournerUneRoue(90,0);
  Avancer(24.45);//26.45
  TournerUneRoue(90,0);
  Avancer(30.3235);//30.3235
  TournerUneRoue(90,1);
  Avancer(20.5765);//22.5765    //Pour bien s'alligner avec le virage dans la trajecoire diagonale
  TournerUneRoue(45,0);
  Avancer(30.0985);//32.0985
  TournerUneRoue(90,1);
  Avancer(53.225);//53.225
  TournerUneRoue(47,0);//45
  Avancer(32);
  TournerUneRoue(22,0);//12.5
  Avancer(77);
  MotorSpeed(0);
  TournerSurPlace(180,1);//**************************************************
  Avancer(77);
  TournerUneRoue(18,1);//12.5
  Avancer(32);
  TournerUneRoue(45,1);
  Avancer(53.225);//53.225
  TournerUneRoue(90,0);
  Avancer(30.0985);//32.0985
  TournerUneRoue(45,1);
  Avancer(22.5765);//22.5765 
  TournerUneRoue(90,0);
  Avancer(28.3235);//30.3235
  TournerUneRoue(90,1);
  Avancer(24.45);//26.45
  TournerUneRoue(95,1);
  Avancer(31.45);
  TournerUneRoue(90,0);//90

  v_enPulseParSec = 8000;
  Avancer(223.225);//213.225
  v_enPulseParSec = 6000;

  virage_PpSec = 7000;
  TournerSurPlace(360,-1);
  virage_PpSec = 3000;
  // ...
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
  if(ROBUS_IsBumper(0)){
    TournerUneRoue(90,0);
  }
  if(ROBUS_IsBumper(1)){
    TournerUneRoue(90,1);
  }
  if(ROBUS_IsBumper(2)){
    DefiParcours();
    //Avancer(70);
  } 

  delay(10);// Delais pour décharger le CPU
}