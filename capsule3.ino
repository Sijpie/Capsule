#include <SoftwareSerial.h>
#include <SparkFunESP8266WiFi.h>

//HKU
//#define NETWORK_NAME "HKU"
//#define NETWORK_PASSWORD "draadloos"

//HOTSPOT
#define NETWORK_NAME "Xperia"
#define NETWORK_PASSWORD "12345678"

#define LEDPIN 13
#define SENSORPIN 7

//variabelen login
bool loginbool = false;

// variabelen break beam sensor
int sensorState = 0;
int lastState = 0 ;

//string maken van body om uit te lezen
String getBody(const String & response) {
  int bodyStart = response.indexOf("\r\n\r\n"); //zoek begin body op (na double space)
  int bodyEnd = response.indexOf("\n", bodyStart + 4); //zoek einde body op, de laatste \n
  return response.substring(bodyStart + 4, bodyEnd); //substring deel van string beginnend bij teken (n) eindigend bij teken (x)
}

void setup() {
  Serial.begin(9600);

  //---------------------------------------------- sparkfun
  setupESP8266();

  //---------------------------------------------- break beam
  pinMode(LEDPIN, OUTPUT);
  pinMode(SENSORPIN, INPUT);

  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(3, OUTPUT);

  analogWrite(6, 255);
  analogWrite(5, 255);
  analogWrite(3, 255);

}

void loop() {

  //------------------------------------------------------------------------------------ break beam
  // staat ontvanger
  sensorState = digitalRead(SENSORPIN);

  // Als beam gebroken is = sensorState is LOW
  //led13 gaat aan als beam gebroken is
  // if (sensorState == LOW) {
  //    digitalWrite(LEDPIN, HIGH);
  // }
  //  else {
  //    digitalWrite(LEDPIN, LOW);
  //  }

  //printen of beam gebroken is of niet
  if (sensorState && !lastState) {
    Serial.println("Unbroken");
  }
  if (!sensorState && lastState) {
    Serial.println("Broken");
  }
  lastState = sensorState;



  //------------------------------------------------------------------------------------- wifi shield
  ESP8266Client client;

  int result = client.connect("studenthome.hku.nl", 80);
  if (result < 0) {
    Serial.println("Failed to connect to server.");
  } else {

    //------------------------------------------------------------------------------------- login

    if (loginbool == false) {

      String loginRequest = "/~sophie.vandersijp/kernmodule4/login.php?thingID=1&pass=12345";

      Serial.println("Send HTTP request...");
      client.println("GET " + loginRequest + " HTTP/1.1\n"
                     "Host: studenthome.hku.nl\n"
                     "Connection: close\n");

      String response;

      String  login = getBody(response);
      Serial.println("login = ");                                     // kan weg
      Serial.println(login);                                          // kan weg

      //      if (login == 0) {
      //        loginbool = false;
      //        //Serial.println("login error");
      //      }

      if (login != "0") {
        loginbool = true;
      }
    }

    if (loginbool == true) {


      //------------------------------------------------------------------------------------- is it go time

      // lezen van server
      String dataRequest = "/~sophie.vandersijp/kernmodule4/dispensetime.php";


      Serial.println("Send HTTP request...");
      client.println("GET " + dataRequest + " HTTP/1.1\n"
                     "Host: studenthome.hku.nl\n"
                     "Connection: close\n");

      String response;
      String goTime = getBody(response);
      Serial.println(goTime);


      if (goTime == "GOGOGO") {

        //LED on
        digitalWrite(LEDPIN, HIGH);
        analogWrite(3, 255);
        analogWrite(5, 255);
        analogWrite(6, 255);

        //------------------------------------------------------------------------------------- has pill been taken

        String worryRequest = "/~sophie.vandersijp/kernmodule4/worry.php";

        Serial.println("Send HTTP request...");
        client.println("GET " + worryRequest + " HTTP/1.1\n"
                       "Host: studenthome.hku.nl\n"
                       "Connection: close\n");

        String response;
        String worryTime = getBody(response);
        Serial.println(worryTime);

        if (worryTime == "worry") {
          analogWrite(3, 255);
          analogWrite(5, 0);
          analogWrite(6, 0);
        }



        //------------------------------------------------------------------------------------- pill has been taken
        String beamRequest = "/~sophie.vandersijp/kernmodule4/";

        if (sensorState && !lastState) {
          String unbr =  "unbroken_insert.php?thingID=1&pill=12345";
          beamRequest += unbr;

          analogWrite(3, 0);
          analogWrite(5, 0);
          analogWrite(6, 0);
        }
        //        } else {
        //          String br = "broken_insert.php?thingID=1&pill=12345";
        //          beamRequest += br;
        //        }



        Serial.println("Send HTTP request...");
        client.println("GET " + beamRequest + " HTTP/1.1\n"
                       "Host: studenthome.hku.nl\n"
                       "Connection: close\n");

        //   Serial.println("Response from server");
        String beamResponse;
        //    Serial.println("-----");
        //    while (client.available()) {
        //      Serial.write(client.read());
        //    }
        Serial.println("beamResponse");




        //-------------------------------------------------------------------------------------
      } // end goTime loop
    } // end login else loop
  } // end else loop


  if (client.connected()) {
    client.stop();
  }

  delay(4000);


} // loop





