Control a LED Brightness with a variable resistance.

Matériels utilisés : 
- Un led
- Une plaquette
- Un micro contrôleur
- Une résistance 220 Ohms
- Deux câbles

a) Without a microcontroller

<img src="./img/exercice1_1_schema.png" alt="Schéma électrique exercice1_1"/>
<img src="./img/exercice1_1.jpg" alt="Photo exercice1_1"/>

b) With a microcontroller

Soit le code utilisé : 

void setup() {
	pinMode(13,OUTPUT);
}

void loop() {
 	digitalWrite(13,HIGH);
}

<img src="./img/exercice1_2_schema.png" alt="Schéma électrique exercice1_2"/>
<img src="./img/exercice1_2.jpg" alt="Photo exercice1_2"/>
