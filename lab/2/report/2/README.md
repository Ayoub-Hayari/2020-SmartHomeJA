- Communication Arduino pour faire varier la fréquence d'un Buzzer passif

Code dans le fichier Code1.ino

Matériels utilisés : 
- Un buzzer passif
- Une plaquette
- Un micro contrôleur
- Deux câbles

<img src="./img/schema2_2.png" alt="Schéma électrique Buzzer"/>
<img src="./img/branchement2_2.jpg" alt="branchement Buzzer"/>
<img src="./img/variationFrequence2_2.png" alt="Test Buzzer"/>

Le principe est de ce branchement est d'envoyer au buzze la fréquence qu'il doit jouer. Quand il reçoit une nouvelle fréquence, il la modifie et la joue en continue.

- LDR

Code dans le fichier Code2.ino

Matériels utilisés : 
- Une LDR
- Une plaquette
- Un micro contrôleur
- Trois câbles
- Une résistance 220 Ohms


<img src="./img/schemaLDR.png" alt="Schéma électrique LDR"/>
<img src="./img/branchementLDR.jpg" alt="Branchement LDR"/>
<img src="./img/TestLDR.png" alt="Test LDR"/>

Le principe est d'afficher la luminosité ambiante grâce au LDR. Le LDR capte la luminosité par un changement de potentiel et la retranscit par une valeur numérique. Dès que la luminosité augmente, l'indicateur augmente, dès qu'elle diminue, l'indicateur diminue.

Voici les shémas des deux exercices ensemble avec le plot du LDR afin de voir l'evolution de l'indicateur au fil du temps lorsqu'on alterne période de lumière et période d'obscurité.

<img src="./sketch.png" alt="Schéma électrique des deux expériences"/>
<img src="./plot.png" alt="Plot LDR"/>

