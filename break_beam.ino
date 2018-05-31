#include <SoftwareSerial.h>
#include <SparkFunESP8266WiFi.h>

#define NETWORK_NAME "HKU"
#define NETWORK_PASSWORD "draadloos"

#define LEDPIN 13
#define SENSORPIN 7
 
// variabelen break beam sensor
int sensorState = 0;
int lastState = 0 ;         
 
void setup() {  
  Serial.begin(9600);

  //---------------------------------------------- sparkfun
  setupESP8266();

  //---------------------------------------------- break beam
  pinMode(LEDPIN, OUTPUT);      
  pinMode(SENSORPIN, INPUT);     
  //digitalWrite(SENSORPIN, HIGH); //turn on pull up - maar ik heb geen pull up, zelf resistor geplaatst

}
 
void loop(){
  //---------------------------------------------- break beam
  // staat ontvanger
  sensorState = digitalRead(SENSORPIN);
 
  // Als beam gebroken is = sensorState is LOW
  //led13 gaat aan als beam gebroken is
  if (sensorState == LOW) {     
    digitalWrite(LEDPIN, HIGH);  
  } 
  else {
    digitalWrite(LEDPIN, LOW); 
  }
  //printen of beam gebroken is of niet
  if (sensorState && !lastState) {
    Serial.println("Unbroken");
  } 
  if (!sensorState && lastState) {
    Serial.println("Broken");
  }
  lastState = sensorState;


  
  //---------------------------------------------- wifi shield
    ESP8266Client client;

  int result = client.connect("studenthome.hku.nl", 80);
  if (result < 0) {
    Serial.println("Failed to connect to server.");
  } else {
    

  //---------------------------------------------- insert  
  // lezen van server
       String beamRequest = "/~sophie.vandersijp/kernmodule4/";

       
    if (sensorState == HIGH) {
      String unbr =  "unbroken_insert.php?thingID=1&pill=12345&dispense=12";
      beamRequest += unbr;
    } else {
      String br = "broken_insert.php?thingID=1&pill=12345&dispense=12";
      beamRequest += br;
    }
       
  
  
  
    Serial.println("Send HTTP request...");
    client.println("GET " + beamRequest + " HTTP/1.1\n"
                   "Host: studenthome.hku.nl\n"
                   "Connection: close\n");

    Serial.println("Response from server");
    String response;
    Serial.println("-----");
    while (client.available()) {
      Serial.write(client.read());
    }
    Serial.println("-----");
  
} // end else loop

  
  //--------------------------------------------- 

   
if (client.connected()) {
    client.stop();
  }

delay(4000);

  
} // loop
