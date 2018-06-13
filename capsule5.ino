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

//variabelen client
String response;
int result;



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

  if (sensorState == HIGH) {
    Serial.println (F("Unbroken"));
  }
  if (sensorState == LOW) {
    Serial.println (F("Broken"));
  }



  //------------------------------------------------------------------------------------- login

  if (loginbool == false) {

    result = sendRequest(F("studenthome.hku.nl"),
                         F("/~sophie.vandersijp/kernmodule4/login.php?thingID=1&pass=12345"),
                         response);

    if (result == 1) {

      String login = getBody(response);
      Serial.println(F("login"));                                     // kan weg
      // Serial.println (login);                                            // kan weg

      //      if (login == 0) {
      //        loginbool = false;
      //        //Serial.println("login error");
      //      }

      if (login != F("0")) {
        loginbool = true;
      }



      if (loginbool == true) {


        //------------------------------------------------------------------------------------- is it go time

        // lezen van server
        result = sendRequest (F("studenthome.hku.nl"),
                              F("/~sophie.vandersijp/kernmodule4/dispensetime.php"),
                              response);

        if (result == 1) {
          Serial.println(F("is it go time?"));

          String goTime = getBody(response);
          // Serial.println(goTime);



          if (goTime == F("its GOTIME")) {

            goTimeFlag = true;
            Serial.println(F("Go!"));
           // Serial.println(client.available());

          }


          if (goTimeFlag) {

            //LED on
            digitalWrite(LEDPIN, HIGH);
            digitalWrite(LEDRED, 0);
            digitalWrite(LEDBLUE, 0);
            digitalWrite(LEDGREEN, 0);


            //------------------------------------------------------------------------------------- has pill been taken

            result = sendRequest (F("studenthome.hku.nl"),
                                  F("/~sophie.vandersijp/kernmodule4/worry.php"),
                                  response);

            if (result == 1) {

              String woTime = getBody(response);
              //Serial.println(woTime);

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
              result = sendRequest (F("studenthome.hku.nl"),
                                    F("/~sophie.vandersijp/kernmodule4/unbroken_insert.php?thingID=1&pillID=1"),
                                    response);

              if (result == 1) {


                //sensorState = LOW;

                if (sensorState == HIGH) {

                  digitalWrite(LEDRED, 255);
                  digitalWrite(LEDBLUE, 255);
                  digitalWrite(LEDGREEN, 255);

                }
                goTimeFlag = false;
                //-------------------------------------------------------------------------------------
              }
            } // if result == 1
          } // if gotime flag
        } // if result == 1 / login
      } // if result == 1 / gotime
    } // end goTime loop
  } // end login else loop




  delay(4000);


} // loop





