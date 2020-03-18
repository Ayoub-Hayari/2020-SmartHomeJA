(version non finale du code)

Code Arduino Mega 2 gérant l'application Android (lumières et arrosage automatique), le système de gestion d'humidité et de température et l'arlarme connectée

```C
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal.h> //include LCD library (standard library)
#include <SimpleDHT.h>
#include <SoftwareSerial.h>


#define redLED 15 //define the LED pins
#define greenLED 16
#define ledPin 17//LED mouvement

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte colPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte rowPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad
String password = "1234";
int posKeypad = 0;
String test = ""; 
bool alarmeActivated = false;
int pirPin = 13;    //the digital pin connected to the PIR sensor's output
int pinDHT11 = 10;
int pinDcMotor = 27;
const byte buzzer = 53;
const char LedGarage = 22;
const char LedCuisine = 23;
const char LedJardin = 24;
const char LedChambre = 25;
const char LedSalon = 26;
const char pinArrosage = 50; // Mettre un relais avec PIN 27

String messageRecu;

SoftwareSerial HC06(11,12);
SimpleDHT11 dht11;
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
LiquidCrystal lcdAlarme (A0, A1, A2, A3, A4, A5); // LCD Alarme
LiquidCrystal lcdTemHum (A6, A7, A8, A9, A10, A11); // LCD Température/Humidité

void setup(){
   Serial.begin(9600);
   HC06.begin(9600);  

   lcdAlarme.begin(16, 2);
   lcdTemHum.begin(16, 2);
   pinMode(pinDHT11, OUTPUT);
   pinMode(pinDcMotor,OUTPUT);
   pinMode(redLED, OUTPUT);  //set the LED as an output
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
  char key = myKeypad.getKey();
       //  Serial.println(key);

  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  while(HC06.available())
  {
    delay(3);
    char c = HC06.read();
    Serial.println("+" + c);
    messageRecu += c;
  }
   if (messageRecu.length() >0){
    Serial.println(messageRecu);
    if (messageRecu == "LED:1")     
      {digitalWrite(LedGarage, HIGH);}
    if (messageRecu == "LED:0")
      {digitalWrite(LedGarage, LOW);}
    if (messageRecu == "LED:3")     
      {digitalWrite(LedCuisine, HIGH);}
    if (messageRecu == "LED:2")
      {digitalWrite(LedCuisine, LOW);}
    if (messageRecu == "LED:5")     
      {digitalWrite(LedJardin, HIGH);}
    if (messageRecu == "LED:4")
      {digitalWrite(LedJardin, LOW);}
    if (messageRecu == "LED:7")     
      {digitalWrite(LedChambre, HIGH);}
    if (messageRecu == "LED:6")
      {digitalWrite(LedChambre, LOW);}
    if (messageRecu == "LED:9")     
      {digitalWrite(LedSalon, HIGH);}
    if (messageRecu == "LED:8")
      {digitalWrite(LedSalon, LOW);}
    if (messageRecu.endsWith("ARRO:1"))     
      {digitalWrite(pinArrosage, HIGH);}
    if (messageRecu.endsWith("ARRO:0"))
      {digitalWrite(pinArrosage, LOW);}
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
  if (!(err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess){
    lcdTemHum.setCursor(0, 0); 
    lcdTemHum.print("Temp : ");
    lcdTemHum.print((float)temperature);
    lcdTemHum.print(" C");
    lcdTemHum.setCursor(0, 1);
    lcdTemHum.print("Humi : ");
    lcdTemHum.print((float)humidity);
    lcdTemHum.print(" %");
    HC06.print(temperature);
    HC06.print(",");
    HC06.print(humidity);
  }
  if((float)humidity > 65 || (float)temperature > 30){
      digitalWrite(pinDcMotor, HIGH);

  }
  else if(((float)humidity <= 65 && (float)humidity > 10) && ((float)temperature <= 30 && (float)temperature > 10)){
      digitalWrite(pinDcMotor,LOW);
  }
  
  if(alarmeActivated){
      lcdAlarme.setCursor(0, 0);
      lcdAlarme.print("Alarme   ");
      lcdAlarme.setCursor(0, 1);
      lcdAlarme.print("activee   ");
      digitalWrite(redLED, HIGH);
      digitalWrite(greenLED, LOW);
      digitalWrite(ledPin, LOW);
      if(digitalRead(pirPin) == HIGH){
        digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin 
        tone(buzzer,800);
      }
      if(digitalRead(pirPin) == LOW){      
        digitalWrite(ledPin, LOW);
      }
  }
  else{
      lcdAlarme.setCursor(0, 0);
      lcdAlarme.print("Alarme   ");
      lcdAlarme.setCursor(0, 1);
      lcdAlarme.print("desactivee   ");
      digitalWrite(greenLED, HIGH);    
      digitalWrite(redLED, LOW);
  }
  if(key){
       Serial.println(key);
       test += key;
       posKeypad ++;
  }
  if(test[0] == 'A' && key == 'A'){
    //Activation alarme
     Serial.println("Activation");
     lcdAlarme.clear();
     lcdAlarme.setCursor(0, 0);
     lcdAlarme.print("Activation");
     digitalWrite(redLED, HIGH);
     posKeypad = 0;
     test = "";
     alarmeActivated = true;
     digitalWrite(ledPin, LOW);
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
  if(key == 'C'){
    //Clear
    test = "";
    posKeypad = 0;
  }
  if (posKeypad == 4 && test == password){
    alarmeActivated = false;
    noTone(buzzer);
    test = "";
    posKeypad = 0;
    lcdAlarme.clear();
    lcdAlarme.setCursor(0, 0);
    lcdAlarme.print("Desactivation");
    Serial.println("Alarme desactivee");
    digitalWrite(greenLED, HIGH);
    digitalWrite(ledPin, LOW);
    delay(2000);
    lcdAlarme.clear();
  }
  else if(posKeypad == 4){
    posKeypad=0;
    test = "";
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
