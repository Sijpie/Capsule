#include <SoftwareSerial.h>
#include <SparkFunESP8266WiFi.h>

//HKU
#define NETWORK_NAME "HKU"
#define NETWORK_PASSWORD "draadloos"

//HOTSPOT
//#define NETWORK_NAME "Xperia"
//#define NETWORK_PASSWORD "12345678"

#define LEDPIN 13
#define LEDRED 3
#define LEDBLUE 5
#define LEDGREEN 6
#define SENSORPIN 7

//variabelen login
bool loginbool = false;
bool goTimeFlag = false;
bool woTimeFlag = false;

// variabelen break beam sensor
int sensorState = 0;
//int lastState = 0 ;

//string maken van body om uit te lezen
//String getBody(const String & response) {
//  int bodyStart = response.indexOf("\r\n\r\n"); //zoek begin body op (na double space)
//  int bodyEnd = response.indexOf("\n", bodyStart + 4); //zoek einde body op, de laatste \n
//  return response.substring(bodyStart + 4, bodyEnd); //substring deel van string beginnend bij teken (n) eindigend bij teken (x)
//}

String getBody(const String & response) {
  int bodyStart = response.indexOf(F("its")); //zoek begin body op (na its)
  int bodyEnd = response.indexOf(F("<br>"), bodyStart + 14); //zoek einde body op, <br>
  String body = response.substring(bodyStart + 14, bodyEnd); //substring deel van string beginnend bij teken (n) eindigend bij teken (x)
  body.trim();
  return body;
}

void setup() {
  Serial.begin(9600);

  //---------------------------------------------- sparkfun
  setupESP8266();

  //---------------------------------------------- break beam
  pinMode(LEDPIN, OUTPUT);
  pinMode(SENSORPIN, INPUT);

  pinMode(LEDRED, OUTPUT);
  pinMode(LEDGREEN, OUTPUT);
  pinMode(LEDBLUE, OUTPUT);

  digitalWrite(LEDRED, 255);
  digitalWrite(LEDGREEN, 255);
  digitalWrite(LEDBLUE, 255);

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
  //  if (sensorState && !lastState) {
  //    Serial.println(F("Unbroken"));
  //  }
  //  if (!sensorState && lastState) {
  //    Serial.println(F("Broken"));
  //  }
  //  lastState = sensorState;

  if (sensorState == HIGH) {
    Serial.println (F("Unbroken"));
  }
  if (sensorState == LOW) {
    Serial.println (F("Broken"));
  }


  //------------------------------------------------------------------------------------- wifi shield
  ESP8266Client client;

  int result = client.connect(F("studenthome.hku.nl"), 80);
  if (result < 0) {
    Serial.println(F("Failed to connect to server."));
  } else {

    //------------------------------------------------------------------------------------- login

    if (loginbool == false) {

      String loginRequest = F("/~sophie.vandersijp/kernmodule4/login.php?thingID=1&pass=12345");

      Serial.println(F("Send HTTP request..."));
      client.println("GET " + loginRequest + " HTTP/1.1\n"
                     "Host: studenthome.hku.nl\n"
                     "Connection: close\n");

      String response;

      String  login = getBody(response);
      Serial.println(F("login = "));                                     // kan weg
      Serial.println (getBody(response));                                          // kan weg

      //      if (login == 0) {
      //        loginbool = false;
      //        //Serial.println("login error");
      //      }

      if (login != F("0")) {
        loginbool = true;
      }
    }

    if (loginbool == true) {


      //------------------------------------------------------------------------------------- is it go time

      // lezen van server
      String dataRequest = F("/~sophie.vandersijp/kernmodule4/dispensetime.php");


      Serial.println(F("is it go time?"));
      client.println("GET " + dataRequest + " HTTP/1.1\n"
                     "Host: studenthome.hku.nl\n"
                     "Connection: close\n");

      String response = "";
      while (client.available()) {
        response += (char)client.read();
      }
      response.trim();



      String goTime = getBody(response);
      Serial.println(getBody(response));



      if (goTime == F("its GOTIME")) {   

        goTimeFlag = true;
        Serial.println(F("Go!"));
        Serial.println(client.available());

      }

       
      if (goTimeFlag) {

        //LED on
        digitalWrite(LEDPIN, HIGH);
        digitalWrite(LEDRED, 0);
        digitalWrite(LEDBLUE, 0);
        digitalWrite(LEDGREEN, 0);


        //------------------------------------------------------------------------------------- has pill been taken

        String worryRequest = F("/~sophie.vandersijp/kernmodule4/worry.php");

        Serial.println(F("should I be worried?"));
        client.println("GET " + worryRequest + " HTTP/1.1\n"
                       "Host: studenthome.hku.nl\n"
                       "Connection: close\n");

        String response;


        while (client.available()) {
          response += (char)client.read();

          Serial.println(F("while works"));
        }
        response.trim();


        String woTime = getBody(response);
        Serial.println(getBody(response));

        if (woTime == F("its WOTIME")) {
          woTimeFlag = true;
          Serial.println(F("worry"));
        }

        if (woTimeFlag) {

          digitalWrite(LEDRED, 0);
          digitalWrite(LEDBLUE, 255);
          digitalWrite(LEDGREEN, 255);

          woTimeFlag = false;
        }



        //------------------------------------------------------------------------------------- pill has been taken
        String beamRequest = F("/~sophie.vandersijp/kernmodule4/");

        //sensorState = LOW;

        if (sensorState == HIGH) {

          digitalWrite(LEDRED, 255);
          digitalWrite(LEDBLUE, 255);
          digitalWrite(LEDGREEN, 255);
          
          String unbr =  F("unbroken_insert.php?thingID=1&pill=12345");
          beamRequest += unbr;

          

          //        } else {
          //          String br = "broken_insert.php?thingID=1&pill=12345";
          //          beamRequest += br;
          //        }



          Serial.println(F("to PHP: beam is unbroken"));
          client.println("GET " + beamRequest + " HTTP/1.1\n"
                         "Host: studenthome.hku.nl\n"
                         "Connection: close\n");

          Serial.println("Response from server");
          String beamResponse;
          Serial.println("-----");
          while (client.available()) {
            Serial.write(client.read());
          }
          Serial.println(F("beamResponse"));

        }
        goTimeFlag = false;
        //-------------------------------------------------------------------------------------
      } // end goTime loop
    } // end login else loop
  } // end else loop


    if (client.connected()) {
      Serial.println(F("Closed client connection"));
     // client.flush();
      client.stop();
    }

  delay(4000);


} // loop





