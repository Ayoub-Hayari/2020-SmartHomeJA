Code de l'Arduino Mega 1 qui va gérer toutes les ouvertures de la maison (version non finale du code)

```C
// Inclusion des bibliothèques nécessaires
// IRremote pour la communiation infrarouge
#include <IRremote.h>
// Stepper pour les moteurs pas à pas
#include <Stepper.h>
// SPI et MFRC522 pour le RFID
#include <SPI.h>
#include <MFRC522.h>

// Initialisation des Pins et des constantes
constexpr uint8_t RST_PIN = 5;         
constexpr uint8_t SS_PIN = 53;   
const char DIN_RECEPTEUR_INFRAROUGE = 2;
const int LedPortail= 14;
const int LedPorteEntreeVerte = 3;
const int LedPorteEntreeRouge = 4;
const byte buzzer = 9;
int nombreDePas = 48*64;
int sensorPin = A0; //LDR luminosité
int sensorValue = 0;

// Initialisation des moteurs pas à pas avec leurs pins
Stepper moteurPorteEntree(nombreDePas,33,37,35,31);
Stepper moteurPortail(nombreDePas,41,45,43,39);
Stepper moteurVoletChambre(nombreDePas,24,28,26,22);
Stepper moteurGarage(nombreDePas,32,36,34,30);
Stepper moteurVoletCuisine(nombreDePas,25,29,27,23);

// Initialisation du RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Initialisation du recepteur infrarouge
IRrecv monRecepteurInfraRouge (DIN_RECEPTEUR_INFRAROUGE);

// Au début de notre programme toutes les ouvertures sont fermées
bool portailOuvert = false;
bool porteGarageOuvert = false;
bool voletCuisineOuvert = false;
bool voletChambreOuvert = false;
bool porteEntreeOuvert = false;

decode_results messageRecu;
unsigned long Depart;
void setup(){ 
  Serial.begin(9600);
  //Initialisation de la vitesse des moteurs
  moteurPortail.setSpeed(8);
  moteurGarage.setSpeed(8);
  moteurVoletCuisine.setSpeed(8);
  moteurVoletChambre.setSpeed(8);
  moteurPorteEntree.setSpeed(8);
  monRecepteurInfraRouge.enableIRIn();
  monRecepteurInfraRouge.blink13(true);

  //Initialisation du RFID
  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();

  //Configuration des pins pour qu'elles se comporte en sortie
  pinMode(buzzer, OUTPUT);
  pinMode(LedPortail, OUTPUT);   
  pinMode(LedPorteEntreeRouge, OUTPUT);
  pinMode(LedPorteEntreeVerte, OUTPUT);
  
  //Les leds sont éteintes au début du programme
  digitalWrite( LedPortail ,LOW );
  digitalWrite(LedPorteEntreeRouge, LOW);
  digitalWrite(LedPorteEntreeVerte, LOW); 
}

void loop(){
  // On lit la valeur du capteur de luminosité
  sensorValue = analogRead(sensorPin); 

  // On définit une variable de temps
  Depart = millis();

  // Si on présente une carte devant le RFID
  if(mfrc522.PICC_IsNewCardPresent()){
    // Lecture de la carte
    if (mfrc522.PICC_ReadCardSerial()) {
    // Récupération de l'UID de la carte 
      String UID = "";
      for(byte i = 0; i< mfrc522.uid.size;i++){
        UID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        UID.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      UID.toUpperCase();
      // Si la carte est la bonne carte
      if(UID.substring(1) == "B9 84 BA 99"){
        // On vérifie si la port est fermée
        if( porteEntreeOuvert == false){
          // On allume la led verte
          digitalWrite(LedPorteEntreeVerte, HIGH);
          Serial.println("Ouverture porte entree");
          // On ouvre la porte
          while((millis()-Depart)<=2500){
            moteurPorteEntree.step(100);
          }
          // La porte devient ouverte
          porteEntreeOuvert = true; 
          // On éteint la led et les pins associés au moteur
          digitalWrite(LedPorteEntreeVerte, LOW);
          digitalWrite( 31,LOW );
          digitalWrite( 33,LOW );
          digitalWrite( 35,LOW );
          digitalWrite( 37,LOW );
        }
        else{
          // Si elle était ouverte
          // On allume la led verte
          digitalWrite(LedPorteEntreeVerte, HIGH);
          // On ferme la porte
          Serial.println("Fermeture porte entree");
          while((millis()-Depart)<=2500){
            moteurPorteEntree.step(-100);
          }
          // On éteint les pins et on met la porte comme étant fermée
          porteEntreeOuvert = false; 
          digitalWrite(LedPorteEntreeVerte, LOW);
          digitalWrite( 31,LOW );
          digitalWrite( 33,LOW );
          digitalWrite( 35,LOW );
          digitalWrite( 37,LOW );
        }
      }                    
      else{
        // Si ce n'est pas la bonne carte
        // On allume la led rouge
        digitalWrite(LedPorteEntreeRouge, HIGH);
        // On fait sonner le buzzer pendant 500 ms
        tone(buzzer,800);
        delay(500);
        noTone(buzzer);
        // On éteint la led rouge
        digitalWrite(LedPorteEntreeRouge, LOW);
      }
    }
  }
  // Si on appuie sur une touche de la télécommande
  else if (monRecepteurInfraRouge.decode(&messageRecu)){ 
    if(messageRecu.decode_type == NEC){
      // Si c'est la flèche de droite et que le portail est fermé
      if(messageRecu.value == 16734885 && portailOuvert == false){
        // On ouvre le portail en faisant clignoter la led jaune
        Serial.println("Ouverture portail");
        digitalWrite( LedPortail ,HIGH );
        while((millis()-Depart)<=11000){
          moteurPortail.step(-100);
          digitalWrite( LedPortail , millis() / 400 % 2 );
        }
        // On met le portail comme étant ouvert et on éteint les pins du moteur pas à pas et de la led
        portailOuvert = true;
        digitalWrite( 39,LOW );
        digitalWrite( 41,LOW );
        digitalWrite( 43,LOW );
        digitalWrite( 45,LOW );
        digitalWrite( LedPortail ,LOW );
      }
      // Si c'est la flèche de gauche et que le portail est puvert 
      else if(messageRecu.value == 16716015 && portailOuvert == true){
        // On ferme le portail en faisant clignoter la led 
        Serial.println("Fermeture portail");   
        digitalWrite( LedPortail ,HIGH );
        while((millis()-Depart)<=11000){
          moteurPortail.step(100);
          digitalWrite( LedPortail , millis() / 400 % 2 );
        }
        portailOuvert = false;
        digitalWrite( 39,LOW );
        digitalWrite( 41,LOW );
        digitalWrite( 43,LOW );
        digitalWrite( 45,LOW );
        digitalWrite( LedPortail ,LOW );
      }
      // Si on appuie sur la flèche du haut et que la porte de garage est fermée
      else if(messageRecu.value == 16718055 && porteGarageOuvert == false){
        // On ouvre la porte de garage
        Serial.println("Ouverture porte garage");
        while((millis()-Depart)<=1300){
          moteurGarage.step(-100);
        }
        porteGarageOuvert = true; 
        digitalWrite( 30,LOW );
        digitalWrite( 32,LOW );
        digitalWrite( 34,LOW );
        digitalWrite( 36,LOW );
      }
      // Si on appuie sur la flèche du bas et que la porte de garage est ouverte
      else if(messageRecu.value == 16730805 && porteGarageOuvert == true){
        // On ferme la porte de garage
        Serial.println("Fermeture porte garage");   
        while((millis()-Depart)<=1300){
          moteurGarage.step(100);
        }
        porteGarageOuvert = false;
        digitalWrite( 30,LOW );
        digitalWrite( 32,LOW );
        digitalWrite( 34,LOW );
        digitalWrite( 36,LOW );
      }
      // Ouverture du volet de la cuisine (commentaire similaire aux précédents)
      else if(messageRecu.value == 16769565 && voletCuisineOuvert == false){
        Serial.println("Ouverture volet cuisine");
        while((millis()-Depart)<=3000){
          moteurVoletCuisine.step(100);
        }
        voletCuisineOuvert = true; 
        digitalWrite( 23,LOW );
        digitalWrite( 25,LOW );
        digitalWrite( 27,LOW );
        digitalWrite( 29,LOW );
      }
      // Fermeture du volet de la cuisine (commentaire similaire aux précédents)
      else if(messageRecu.value == 16748655 && voletCuisineOuvert == true){
        Serial.println("Fermeture volet cuisine");   
        while((millis()-Depart)<=3000){
          moteurVoletCuisine.step(-100);
        }
        voletCuisineOuvert = false;
        digitalWrite( 23,LOW );
        digitalWrite( 25,LOW );
        digitalWrite( 27,LOW );
        digitalWrite( 29,LOW );
      }
      // Ouverture du volet de la chambre (commentaire similaire aux précédents)
      else if(messageRecu.value == 16753245 && voletChambreOuvert == false){
        Serial.println("Ouverture volet chambre");
        while((millis()-Depart)<=3000){
          moteurVoletChambre.step(100);
        }
        voletChambreOuvert = true; 
        digitalWrite( 22,LOW );
        digitalWrite( 24,LOW );
        digitalWrite( 26,LOW );
        digitalWrite( 28,LOW );
      }
      // Fermeture du volet de la chambre (commentaire similaire aux précédents)
      else if(messageRecu.value == 16769055 && voletChambreOuvert == true){
        Serial.println("Fermeture volet chambre");   
        while((millis()-Depart)<=3000){
          moteurVoletChambre.step(-100);
        }
        voletChambreOuvert = false;
        digitalWrite( 22,LOW );
        digitalWrite( 24,LOW );
        digitalWrite( 26,LOW );
        digitalWrite( 28,LOW );
      }
    }
    // On met à zéro la variable du message reçu et on se prépare à en recevoir un autre
    messageRecu.value = NULL;
    monRecepteurInfraRouge.resume();
  }
  // Si la valeur retournée par le capteur de luminosité est < 25
  else if(sensorValue < 25){
    // On ferme les volets de la chambre et de la cuisine s'ils sont ouverts
    if(voletChambreOuvert == true){
      Serial.println("Fermeture volet chambre"); 
      while((millis()-Depart)<=3000){
        moteurVoletChambre.step(-1);
      }
      voletChambreOuvert = false;
      digitalWrite( 22,LOW );
      digitalWrite( 24,LOW );
      digitalWrite( 26,LOW );
      digitalWrite( 28,LOW );
    }
    if(voletCuisineOuvert == true){
      Serial.println("Fermeture volet cuisine");   
      Depart = millis() ;
      while((millis()-Depart)<=3000){
        moteurVoletCuisine.step(-1);
      }
      voletCuisineOuvert = false;
      digitalWrite( 23,LOW );
      digitalWrite( 25,LOW );
      digitalWrite( 27,LOW );
      digitalWrite( 29,LOW );
    }
  }
}
 ```
