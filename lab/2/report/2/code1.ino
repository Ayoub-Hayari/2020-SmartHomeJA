const byte buzzer = 13;

void setup() {
  // initialize serial
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT); 
}

    

void loop() {
  // if there's any serial available, read it:
  while (Serial.available() > 0) {
    //On lit l'entrée en temps que chaîne de caractères
     String b = Serial.readString()  ;
    Serial.println(b.toInt());
    //On fait sonner le buzzer avec l'entrée
    tone(buzzer, b.toInt());    
  }
}
