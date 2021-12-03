#include <LiquidCrystal.h>

/*****************************************
   variabelen die je gebruikt
 *****************************************/

// initialize het display
// de helderheid van het display regel je met de potmeter op de auto,
// daarvoor is geen code nodig
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// gebruikte pinnen
const int pinAfstandTrigM = A4; // afstandssensor midden
const int pinAfstandEchoM = A5; // afstandssensor midden
const int pinAfstandTrigR = A2; // afstandssensor rechts
const int pinAfstandEchoR = A3; // afstandssensor rechts
const int pinAfstandTrigL = A0; // afstandssensor links
const int pinAfstandEchoL = A1; // afstandssensor links
const int pinMotorSnelheidR = 11; // motor rechts
const int pinMotorSnelheidL = 10; // motor links

// variabelen om waarden van sensoren en actuatoren te onthouden
long afstandR = 0;
long afstandL = 0;
long afstandM = 0;
int snelheidR = 2;
int  snelheidL = 2;
String regelBoven = "";
String regelOnder = "";

// variabelen voor de toestanden maken
// toestanden:
const int RECHTSAF = 1;
const int LINKSAF = 2;
const int VOORUIT = 3;
const int WACHT = 4;
int toestand = RECHTSAF;
unsigned long toestandStartTijd = 0;

/*****************************************
   functies die je gebruikt
 *****************************************/

// functie om afstandssensor uit te lezen
long readDistance(int triggerPin, int echoPin)
{
  long echoTime = 0;
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  // timeout after 30.000 microseconds (around 5 meters)
  echoTime = pulseIn(echoPin, HIGH, 30000);
  if (echoTime == 0) {
    echoTime = 30000;
  }
  return echoTime;
}



/*****************************************
   setup() en loop()
 *****************************************/

void setup() {
  // pinnen voor afstandssensor worden
  // voor elke meting in readDistance()
  // in de goede mode gezet

  // zet pinmode voor motor aansturing via PWM
  pinMode(pinMotorSnelheidL, OUTPUT);
  pinMode(pinMotorSnelheidR, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // enable console
  Serial.begin(9600);

  // opstart bericht op console en seriële monitor
  lcd.setCursor(0, 0); // zet cursor op het begin van de bovenste regel
  lcd.print("Prototype");
  lcd.setCursor(0, 1); // zet cursor op het begin van de onderste regel
  lcd.print("1");
  delay(2000); // wachttijd om het display te lezen en de auto neer te zetten

  // zet toestanden en in beginstand
  toestand = VOORUIT;
  toestandStartTijd = millis();
}

void loop() {
  // lees afstandssensoren uit
  // dit is nodig voor alle test toestanden
  // omrekenen naar centimeters = milliseconden / 29 / 2
  afstandR = readDistance(pinAfstandTrigR, pinAfstandEchoR) / 29 / 2;
  afstandL = readDistance(pinAfstandTrigL, pinAfstandEchoL) / 29 / 2;
  afstandM = readDistance(pinAfstandTrigM, pinAfstandEchoM) / 29 / 2;

  // bepaal toestand

if (toestand == VOORUIT) {
    if (afstandM < 20 && afstandR < 20 && afstandL > 20) { // Turn left
      toestandStartTijd = millis();
    snelheidR = 255;
    snelheidL = 0;
    } else if (afstandM < 20 && afstandL < 20 && afstandR > 20) {  // Turn Right
      toestandStartTijd = millis();
          snelheidR = 0;
    snelheidL = 255;
  } else if (afstandM > 30) { // Naar voren
      toestandStartTijd = millis();
    snelheidR = 128;
    snelheidL = 128;
    }else if (afstandM > 30) {
      toestandStartTijd = millis();
    snelheidR = 128;
    snelheidL = 128;
    } else if (afstandL < 20 && afstandR < 20 && afstandM < 20) {
      toestandStartTijd = millis();
     snelheidR = 0;
    snelheidL = 0;
    } else {
      toestand = VOORUIT;
    }
  }


  // zet waarden voor acturatoren, voor alle toestanden
  // zet motorsnelheid
  analogWrite(pinMotorSnelheidR, snelheidR);
  analogWrite(pinMotorSnelheidL, snelheidL);
  // zet tekst op display
  regelBoven = String(afstandL) + "   " +
               String(afstandM) + "   " +
               String(afstandR) + "   ";
  regelOnder = String(snelheidL) +
               " TEST" + String(toestand) + " " +
               String(snelheidR) + "      ";
  lcd.setCursor(0, 0); // zet cursor op het begin van de bovenste regel
  lcd.print(regelBoven);
  lcd.setCursor(0, 1); // zet cursor op het begin van de onderste regel
  lcd.print(regelOnder);
  // zet debug info op de seriële monitor
  Serial.print(regelBoven);
  Serial.print("--");
  Serial.println(regelOnder);

  // vertraging om te zorgen dat de seriële monitor de berichten bijhoudt
  delay(100);
}
