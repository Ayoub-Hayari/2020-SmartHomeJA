(version non finale du code)

Code Arduino Mega 2 gérant l'application Android (lumières et arrosage automatique), le système de gestion d'humidité et de température et l'arlarme connectée

```C
// On inclut les biblithèques nécessaires
 
// Bibliothèques pour le KeyPad (pavé numérique)
#include <Key.h>
#include <Keypad.h>
 
// Bibliothèque pour les écrans LCD
#include <LiquidCrystal.h> 
 
// Bibliothèque pour le capteur de température et d'humidité
#include <SimpleDHT.h>
  
// Bibliothèque pour la communication avec l'Arduino et le HC-06
#include <SoftwareSerial.h>

// On définit les pins des leds de l'alarme
#define redLED 15 
#define greenLED 16
#define ledPin 17

// On définit les constantes pour le KeyPad
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
  
// On définit les pins pour les touches (ligne et colonne)
byte colPins[ROWS] = {5, 4, 3, 2}; 
byte rowPins[COLS] = {9, 8, 7, 6}; 
  
// On définit le mot de passe de l'alarme connectée
String password = "1234";
  
// Position initiale quand on tape sur des touches du KeyPad
int posKeypad = 0;
String test = ""; 
 
// Par défaut l'alarme est désactivée
bool alarmeActivated = false;
 
// Définition des pins des différents capteurs et des leds
int pirPin = 13;   
int pinDHT11 = 10;
int pinDcMotor = 51;
const byte buzzer = 53;
const char LedGarage = 22;
const char LedCuisine = 23;
const char LedJardin = 24;
const char LedChambre = 25;
const char LedSalon = 26;
const char pinArrosage = 50;
  
String messageRecu;
  
// Initialisation du HC-06
SoftwareSerial HC06(11,12);
// Initialisation du capteur de température/humidité
SimpleDHT11 dht11;
// Initialisation du keyPad
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
// Initialisation des modules LCD
LiquidCrystal lcdAlarme (A0, A1, A2, A3, A4, A5); // LCD Alarme
LiquidCrystal lcdTemHum (A6, A7, A8, A9, A10, A11); // LCD Température/Humidité

void setup(){
  Serial.begin(9600);
  HC06.begin(9600);  
  // Les écrans LCD sont des modules 16 * 2 (pour l'écriture)
  lcdAlarme.begin(16, 2);
  lcdTemHum.begin(16, 2);
  // On définit les différents pins en sortie en sortie ou en entrée
  pinMode(pinDHT11, OUTPUT);
  pinMode(pinDcMotor,OUTPUT);
  pinMode(redLED, OUTPUT);  
  pinMode(greenLED, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(LedGarage, OUTPUT);
  pinMode(LedCuisine, OUTPUT);
  pinMode(LedJardin, OUTPUT);
  pinMode(LedChambre, OUTPUT);
  pinMode(LedSalon, OUTPUT);
  pinMode(pinArrosage,OUTPUT);
   
  // Par défaut les pins sont "éteints"
  digitalWrite(LedGarage, LOW);
  digitalWrite(LedJardin, LOW);
  digitalWrite(LedCuisine, LOW);
  digitalWrite(LedChambre, LOW);
  digitalWrite(LedSalon, LOW);
  digitalWrite(pinArrosage, LOW);
  digitalWrite(ledPin, LOW);
  digitalWrite(pinDcMotor, LOW);
  analogWrite(pinDHT11, LOW); 
}
  
void loop(){
  // On lit ce qui a été tapé sur le KeyPad
  char key = myKeypad.getKey();  
  //Par défaut la témpérature et l'humidité valent 0
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  // Lecture de l'instruction venant de l'application via le HC-06
  while(HC06.available())
  {
    delay(3);
    char c = HC06.read();
    Serial.println("+" + c);
    messageRecu += c;
  }
  // En fonction de l'instruction reçu on allume/éteint les lumières des différentes zones ou de l'arrosage
  if (messageRecu.length() >0){
    Serial.println(messageRecu);
    if (messageRecu == "LED:1"){digitalWrite(LedGarage, HIGH);}
    if (messageRecu == "LED:0"){digitalWrite(LedGarage, LOW);}
    if (messageRecu == "LED:3"){digitalWrite(LedCuisine, HIGH);}
    if (messageRecu == "LED:2"){digitalWrite(LedCuisine, LOW);}
    if (messageRecu == "LED:5"){digitalWrite(LedJardin, HIGH);}
    if (messageRecu == "LED:4"){digitalWrite(LedJardin, LOW);}
    if (messageRecu == "LED:7"){digitalWrite(LedChambre, HIGH);}
    if (messageRecu == "LED:6"){digitalWrite(LedChambre, LOW);}
    if (messageRecu == "LED:9"){digitalWrite(LedSalon, HIGH);}
    if (messageRecu == "LED:8"){digitalWrite(LedSalon, LOW);}
    if (messageRecu.endsWith("ARRO:1")){digitalWrite(pinArrosage, HIGH);}
    if (messageRecu.endsWith("ARRO:0")){digitalWrite(pinArrosage, LOW);}
    if (messageRecu.endsWith("LED:ALL")){
      digitalWrite(LedGarage, LOW);
      digitalWrite(LedCuisine, LOW);
      digitalWrite(LedJardin, LOW);
      digitalWrite(LedChambre, LOW);
      digitalWrite(LedSalon, LOW);
      digitalWrite(pinArrosage, LOW);
    }
    messageRecu="";
  }
  // Affichage de la température et de lumidité sur le LCD
  if (!(err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess){
    //On affiche sur la 1 ere ligne à partir de la première position la température
    lcdTemHum.setCursor(0, 0); 
    lcdTemHum.print("Temp : ");
    lcdTemHum.print((float)temperature);
    lcdTemHum.print(" C");
    //On affiche sur la 2 eme ligne à partir de la première position l'humidité
    lcdTemHum.setCursor(0, 1);
    lcdTemHum.print("Humi : ");
    lcdTemHum.print((float)humidity);
    lcdTemHum.print(" %");
    HC06.print(temperature);
    HC06.print(",");
    HC06.print(humidity);
  }
  // Si l'humidité ou la température sont supérieures à un certain seuil, on active la ventilation
  if((float)humidity > 65 || (float)temperature > 30){
      digitalWrite(pinDcMotor, HIGH);
  }
  // Si l'humidité ou la température sont inférieurs à un certain seuil, on désactive la ventilation
  else if(((float)humidity <= 65 && (float)humidity > 10) && ((float)temperature <= 30 && (float)temperature > 10)){
      digitalWrite(pinDcMotor,LOW);
  }
  // Si l'alarme est activée
  if(alarmeActivated){
    // On affiche sur le LCD son état
    lcdAlarme.setCursor(0, 0);
    lcdAlarme.print("Alarme   ");
    lcdAlarme.setCursor(0, 1);
    lcdAlarme.print("activee   ");
    // On allume la led rouge et on éteint les autres
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(ledPin, LOW);
    // Si le capteur de mouvement détecte un mouvement,on fait sonner le buzze et on allume la led jaune 
    if(digitalRead(pirPin) == HIGH){
      digitalWrite(ledPin, HIGH);   
      tone(buzzer,800);
    }
    // On éteint la led jaune si aucun mouvement n'est détecté. Le buzzer reste activé néanmoins
    if(digitalRead(pirPin) == LOW){      
      digitalWrite(ledPin, LOW);
     }
  }
  else{
    // Si l'alarme est désactivée on affiche son état sur le LCD
    lcdAlarme.setCursor(0, 0);
    lcdAlarme.print("Alarme   ");
    lcdAlarme.setCursor(0, 1);
    lcdAlarme.print("desactivee   ");
    // On allume la led verte 
    digitalWrite(greenLED, HIGH);    
    digitalWrite(redLED, LOW);
  }
  if(key){
    // A chaque fois qu'un utilisateur appuie le keypad la touche est sauvegardé et la position incrémentée
    Serial.println(key);
    test += key;
    posKeypad ++;
  }
  // Si l'utilisateur tape sur la touche A, l'alarme s'active
  if(test[0] == 'A' && key == 'A'){
    //On affiche sur le LCD l'état de l'alarme
    Serial.println("Activation");
    lcdAlarme.clear();
    lcdAlarme.setCursor(0, 0);
    lcdAlarme.print("Activation");
    digitalWrite(redLED, HIGH);
    posKeypad = 0;
    test = "";
    alarmeActivated = true;
    digitalWrite(ledPin, LOW);
    // On fait sonner le buzzer pour signifier qu'on active l'alarme
    for (int i = 0;i<10;i++){
      if(i % 3 == 0){
        tone(buzzer,600);
        delay(500);
        noTone(buzzer);
        delay(200);
      }
    }
    noTone(buzzer);
    delay(1000);
    lcdAlarme.clear();
  }
  // Si l'utilisateur tape sur la touche C, cela a pour effet d'effacer son entrée en cas d'erreur de saisie par exemple
  if(key == 'C'){
    //Clear
    test = "";
    posKeypad = 0;
  }
  //Si l'utilisateur à entré 4 caractères et que son entrée correspond au mot de passe
  if (posKeypad == 4 && test == password){
    //On désactive l'alarme
    alarmeActivated = false;
    noTone(buzzer);
    // On remet à 0 les variables de saisie et de position
    test = "";
    posKeypad = 0;
    // On affiche sur le LCD l'état de l'alarme
    lcdAlarme.clear();
    lcdAlarme.setCursor(0, 0);
    lcdAlarme.print("Desactivation");
    
    // On allume la led verte et on éteint la rouge
    Serial.println("Alarme desactivee");
    digitalWrite(greenLED, HIGH);
    digitalWrite(ledPin, LOW);
    delay(2000);
    lcdAlarme.clear();
  }
  // Si l'utilisateur a entré une chaine de 4 caractères mais qui ne correspond pas au bon mot de passe
  else if(posKeypad == 4){
    // On remet à 0 les variables permettant la saisie
    posKeypad=0;
    test = "";
    //On affiche sur le module LCD que la saisie est erronée
    lcdAlarme.clear();
    lcdAlarme.setCursor(0, 0);
    lcdAlarme.print("Code errone");
    Serial.println("Code errone");
    digitalWrite(redLED, HIGH);
    delay(2000);
    lcdAlarme.clear();
  }
}
```
