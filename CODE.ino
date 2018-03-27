#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information, as described below

int a=0;
//Photorésistance 
int photoMin = 0;        //   pour le
int photoMax = 255;      //   calibrage 
int photoPin;
int photoValue;

//TMP36
int tmpPin;
int tmpValue;

//LED
int ledvertPin;
int ledvertBrightness;
int ledrougePin;
int ledrougeBrightness;


int calls = 1;   // Execution count, so this doesn't run forever
int maxCalls = 10;   // Maximum number of times the Choreo should be executed

void setup() {
  Serial.begin(9600);
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);
  Bridge.begin();

  //Pins
  photoPin = A0;
  tmpPin = A1;
  ledvertPin = 11; //ATTENTION, ceci fontionne seulement avec quelques pin (ceux qui ont le ~) 
  ledrougePin = 10;

  pinMode(ledvertPin,OUTPUT);
  pinMode(ledrougePin,OUTPUT);
  
  //Calibrage
  while (millis()<5000)
  {
    photoValue = analogRead(photoPin);
    if (photoValue > photoMax) { photoMax = photoValue; }
    if (photoValue < photoMin) { photoMin = photoValue; }
  }

}

void loop() {
  photoValue = analogRead(photoPin);
  photoValue = map (photoValue, photoMin, photoMax, 0, 255);
  photoValue = constrain(photoValue, 0, 255); 

  tmpValue = analogRead(tmpPin);
  float volt = tmpValue * 5000.0 / 1024.0;
  float tmp = (volt - 500) / 10;

  String tmpSt=String(tmp);
  String message = String("I have something to tell you: " + tmpSt + " °C");
  
  //ledvertBrightness = photoValue;
  //ledrougeBrightness = map(tmpValue, -50, 150, 0, 255); //tmp: couverture de -50 à 150°C)

  digitalWrite(ledvertPin, HIGH);
  //digitalWrite(ledrougePin, ledrougeBrightness);
  //analogWrite(ledvertPin,ledvertBrightness);
  //analogWrite(ledrougePin, ledrougeBrightness);
  
  Serial.print("Valeur de la photorésistance: ");
  Serial.println(photoValue);
  Serial.print("Valeur de la température: ");
  Serial.print(tmp); 
  Serial.println("°C");
  delay(1000);
  if (a==1)
  {
  if (calls <= maxCalls) {
    Serial.println("Running SendEmail - Run #" + String(calls++));
    
    TembooChoreo SendEmailChoreo;

    // Invoke the Temboo client
    SendEmailChoreo.begin();

    // Set Temboo account credentials
    SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);
    
    // Set Choreo inputs
    SendEmailChoreo.addInput("FromAddress", "arduinoyun@gmail.com");
    SendEmailChoreo.addInput("Username", "arduinoyun@gmail.com");
    SendEmailChoreo.addInput("Subject", "I HAVE A TEMPERATURE TO TELL YOU");
    SendEmailChoreo.addInput("ToAddress", "destinationmail");
    SendEmailChoreo.addInput("MessageBody", message );
    SendEmailChoreo.addInput("Password", "password");
    
    // Identify the Choreo to run
    SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail");
    
    // Run the Choreo; when results are available, print them to serial
    SendEmailChoreo.run();
    
    while(SendEmailChoreo.available()) {
      char c = SendEmailChoreo.read();
      Serial.print(c);
    }
    SendEmailChoreo.close();
  }

  Serial.println("Waiting...");
  }
  delay(30000); // wait 30 seconds between SendEmail calls
}
