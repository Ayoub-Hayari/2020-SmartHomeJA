Code de l'Arduino Mega 1 qui va gérer toutes les ouvertures de la maison (version non finale du code)

    #include <IRremote.h>
    #include <Stepper.h>
    #include <SPI.h>
    #include <MFRC522.h>

    constexpr uint8_t RST_PIN = 5;          // Configurable, see typical pin layout above
    constexpr uint8_t SS_PIN = 53;   
    const char DIN_RECEPTEUR_INFRAROUGE = 2;
    const int LedPortail= 14;
    const int LedPorteEntreeVerte = 3;
    const int LedPorteEntreeRouge = 4;
    const byte buzzer = 9;

    
    int nombreDePas = 48*64;

    int sensorPin = A0; //LDR luminosité
    int sensorValue = 0;
    
    Stepper moteurPorteEntree(nombreDePas,41,45,43,39);
    Stepper moteurPortail(nombreDePas,25,29,27,23);
    Stepper moteurGarage(nombreDePas,24,28,26,22);
    Stepper moteurVoletCuisine(nombreDePas,32,36,34,30);
    Stepper moteurVoletChambre(nombreDePas,40,44,42,38);

    MFRC522 mfrc522(SS_PIN, RST_PIN);
    IRrecv monRecepteurInfraRouge (DIN_RECEPTEUR_INFRAROUGE);
    
    bool portailOuvert = false;
    bool porteGarageOuvert = false;
    bool voletCuisineOuvert = false;
    bool voletChambreOuvert = false;
    bool porteEntreeOuvert = false;
    
    decode_results messageRecu;
    unsigned long Depart;
    void setup()
    { 
      Serial.begin(9600);
      //Initialisation de la vitesse des moteurs
      moteurPortail.setSpeed(8);
      moteurGarage.setSpeed(8);
      moteurVoletCuisine.setSpeed(8);
      moteurVoletChambre.setSpeed(8);
      moteurPorteEntree.setSpeed(8);
      monRecepteurInfraRouge.enableIRIn();
      monRecepteurInfraRouge.blink13(true);
      while (!Serial);
      SPI.begin();
      mfrc522.PCD_Init();
      mfrc522.PCD_DumpVersionToSerial();
      pinMode(buzzer, OUTPUT);
      pinMode(LedPortail, OUTPUT);   
      pinMode(LedPorteEntreeRouge, OUTPUT);
      pinMode(LedPorteEntreeVerte, OUTPUT);
      digitalWrite( LedPortail ,LOW );
      digitalWrite(LedPorteEntreeRouge, LOW);
      digitalWrite(LedPorteEntreeVerte, LOW); 
    }
    

    void loop()
    {
      sensorValue = analogRead(sensorPin); // read the value from the sensor
      Depart = millis();
      if(mfrc522.PICC_IsNewCardPresent()){
         if (mfrc522.PICC_ReadCardSerial()) {
            String UID = "";
            for(byte i = 0; i< mfrc522.uid.size;i++){
                UID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
                UID.concat(String(mfrc522.uid.uidByte[i], HEX));
            }
            UID.toUpperCase();
            if(UID.substring(1) == "B9 84 BA 99"){
              if( porteEntreeOuvert == false){
                digitalWrite(LedPorteEntreeVerte, HIGH);
                Serial.println("Ouverture porte entree");
                while((millis()-Depart)<=3000){
                     moteurPorteEntree.step(100);
                }
                porteEntreeOuvert = true; 
                digitalWrite(LedPorteEntreeVerte, LOW);
              }
              else{
                digitalWrite(LedPorteEntreeVerte, HIGH);
                Serial.println("Fermeture porte entree");
                while((millis()-Depart)<=3000){
                     moteurPorteEntree.step(-100);
                }
                porteEntreeOuvert = false; 
                digitalWrite(LedPorteEntreeVerte, LOW);
              }
            }                    
            else{
              digitalWrite(LedPorteEntreeRouge, HIGH);
              tone(buzzer,800);
              delay(500);
              noTone(buzzer);
              digitalWrite(LedPorteEntreeRouge, LOW);
            }
        }
      }
      else if (monRecepteurInfraRouge.decode(&messageRecu)){ 
        if(messageRecu.decode_type == NEC){
           if(messageRecu.value == 16734885 && portailOuvert == false){
              Serial.println("Ouverture portail");
              digitalWrite( LedPortail ,HIGH );
              while((millis()-Depart)<=3000){
                  moteurPortail.step(100);
                  digitalWrite( LedPortail , millis() / 400 % 2 );
              }
              portailOuvert = true;
          }
          else if(messageRecu.value == 16716015 && portailOuvert == true){
              Serial.println("Fermeture portail");   
              digitalWrite( LedPortail ,HIGH );
              while((millis()-Depart)<=3000){
                moteurPortail.step(-100);
                digitalWrite( LedPortail , millis() / 400 % 2 );
              }
              portailOuvert = false;
          }
          else if(messageRecu.value == 16718055 && porteGarageOuvert == false){
              Serial.println("Ouverture porte garage");
              while((millis()-Depart)<=3000){
                  moteurGarage.step(100);
              }
              porteGarageOuvert = true; 
          }
          else if(messageRecu.value == 16730805 && porteGarageOuvert == true){
              Serial.println("Fermeture porte garage");   
              while((millis()-Depart)<=3000){
                moteurGarage.step(-100);
              }
              porteGarageOuvert = false;
          }
          else if(messageRecu.value == 16769565 && voletCuisineOuvert == false){
              Serial.println("Ouverture volet cuisine");
              while((millis()-Depart)<=3000){
                  moteurVoletCuisine.step(100);
              }
              voletCuisineOuvert = true; 
          }
          else if(messageRecu.value == 16748655 && voletCuisineOuvert == true){
              Serial.println("Fermeture volet cuisine");   
              while((millis()-Depart)<=3000){
                moteurVoletCuisine.step(-100);
              }
              voletCuisineOuvert = false;
          }
          else if(messageRecu.value == 16753245 && voletChambreOuvert == false){
              Serial.println("Ouverture volet chambre");
              while((millis()-Depart)<=3000){
                  moteurVoletChambre.step(100);
              }
              voletChambreOuvert = true; 
          }
          else if(messageRecu.value == 16769055 && voletChambreOuvert == true){
              Serial.println("Fermeture volet chambre");   
              while((millis()-Depart)<=3000){
                moteurVoletChambre.step(-100);
              }
              voletChambreOuvert = false;
          }
        }
        digitalWrite( LedPortail ,LOW );
        messageRecu.value = NULL;
        monRecepteurInfraRouge.resume();
      }
       else if(sensorValue < 25){
          if(voletChambreOuvert == true){
              Serial.println("Fermeture volet chambre"); 
              while((millis()-Depart)<=3000){
                moteurVoletChambre.step(-100);
              }
              voletChambreOuvert = false;
          }
          if(voletCuisineOuvert == true){
              Serial.println("Fermeture volet cuisine");   
              Depart = millis() ;
              while((millis()-Depart)<=3000){
                                   Serial.println(millis());   

                moteurVoletCuisine.step(-100);
              }
              voletCuisineOuvert = false;
          }
        }
}
