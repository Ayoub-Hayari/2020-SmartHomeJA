# Utilisation de Fritzing et Github

Ce rapport permet de présenter un exemple d'utilisation de Fritzing et de Github.

Nous allons présenter dans ce rapport : 

- Un exemple de sketch avec fritzing 
- La construction avec les composants de cette esquissse

Nous allons prendre l'exemple du moteur pas à pas. En effet, comme nous allons l'utiliser dans notre projet, c'est l'occasion d'avancer.

Dans notre projet, nous allons utiliser plusieurs moteurs pas à pas. Notamment un pour l'overture et la fermeture du portail. 
Ainsi, notre moteur fonctionnera avec une télécommande infrarouge, un récepteur infrarouge et une led afin d'ouvrir et de fermer le portail comme pour un portail élétrique classique. Afin de faciliter les branchements, nous allons utiliser un Arduino Mega, une breadboard et des câbles.

Le principe est simple quand nous appuyons sur la touche '>' de la télécommande le moteur s'actionne et tourne dans le sens horaire pendant 3 secondes simulant l'ouverture du portail. Quand nous appuyons sur '<' le moteur s'actionne et tourne dans le sens anti-horaire pendant 3 secondes simulant la fermeutre du portail. Pendant l'ouverture et la fermeture du portail, une led clignote afin d'avertir d'un mouvement du portail.

Notons que deux ouvertures ou deux fermetures successives ne peuvent pas se faire, pour eviter l'ouverture ou la fermueture du portail dans une zone où il ne devrait pas se trouver.

![Branchement moteur pas à pas](./schematic.png?raw=true)

Code :

```C
    #include <IRremote.h>
    #include <Stepper.h>


    const char DIN_RECEPTEUR_INFRAROUGE = 2;
    const int ledpin = 14;
    int nombreDePas = 48*64;

    Stepper monMoteur(nombreDePas,9,11,10,6);
    IRrecv monRecepteurInfraRouge (DIN_RECEPTEUR_INFRAROUGE);
    bool portailOuvert = false;
    
    decode_results messageRecu;
    unsigned long Depart;
    void setup()
    {
      monMoteur.setSpeed(8);
      Serial.begin(9600);
      monRecepteurInfraRouge.enableIRIn();
      monRecepteurInfraRouge.blink13(true);
      pinMode(ledpin, OUTPUT);    
    }
    

    void loop()
    {
      digitalWrite( ledpin ,LOW );

      if (monRecepteurInfraRouge.decode(&messageRecu))
      {
        Depart = millis();
        if(messageRecu.decode_type == NEC){
           if(messageRecu.value == 16734885 && portailOuvert == false){
              Serial.println("Ouverture");
              digitalWrite( ledpin ,HIGH );
              while((millis()-Depart)<=3000){
                  monMoteur.step(100);
                  digitalWrite( ledpin , millis() / 400 % 2 );
              }
              portailOuvert = true;
          }
          else if(messageRecu.value == 16716015 && portailOuvert == true){
              Serial.println("Fermeture");   
              digitalWrite( ledpin ,HIGH );
              while((millis()-Depart)<=3000){
                monMoteur.step(-100);
                digitalWrite( ledpin , millis() / 400 % 2 );
              }
              portailOuvert = false;
          }
        }
        digitalWrite( ledpin ,LOW );
        monRecepteurInfraRouge.resume();
      }
}
```



