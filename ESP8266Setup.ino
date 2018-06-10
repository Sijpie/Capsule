void setupESP8266() {
  Serial.begin(9600);

  int found = 0;
  while (found == 0) {
    Serial.println(F("Connecting to ESP8266 shield..."));
    found = esp8266.begin();
    if (found) {
      Serial.println(F("ESP8266 shield found!"));
    } else {
      Serial.println(F("Could not connect to ESP8266"));
      delay(3000);
    }
  }

  if (esp8266.status() < 0) {
    int result = -1;
    while (result < 0) {
      Serial.print(F("Connecting to network"));
      Serial.println(F(NETWORK_NAME));
      result = esp8266.connect(NETWORK_NAME, NETWORK_PASSWORD);
      if (result < 0) {
        Serial.print(F("Error connecting: result="));
        Serial.println(result);
        delay(3000);
      }
    }
  }

  Serial.print(F("Connected to network "));
  Serial.println(F(NETWORK_NAME " ....NOT"));

  IPAddress mijnIP = esp8266.localIP();
  Serial.print("Mijn IP: ");
  Serial.println(mijnIP);
}
