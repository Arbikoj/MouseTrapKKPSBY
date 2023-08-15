#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager

#include <SoftwareSerial.h>
SoftwareSerial serial(D1, D2);
String data;

WiFiServer server(80);

static const int buttonPin = 12;
int buttonStatePrevious = LOW;

unsigned long minButtonLongPressDuration = 3000;
unsigned long buttonLongPressMillis;
bool buttonStateLongPress = false;

const int intervalButton = 50;
unsigned long previousButtonMillis;

unsigned long buttonPressDuration;
unsigned long currentMillis;

void setup() {
  Serial.begin(115200);
  serial.begin(115200);
  WiFiManager wifiManager;

  wifiManager.autoConnect("Mouse Trap KKP");
  Serial.println("Connected.");

  server.begin();
}


void readButtonState() {
  if (currentMillis - previousButtonMillis > intervalButton) {
    int buttonState = digitalRead(buttonPin);

    buttonPressDuration = currentMillis - buttonLongPressMillis;
    if (buttonState == HIGH && !buttonStateLongPress && buttonPressDuration >= minButtonLongPressDuration) {
      buttonStateLongPress = true;
      Serial.println("Reset WiFi...");
      WiFiManager wifiManager;
      wifiManager.resetSettings();
      wifiManager.autoConnect("Mouse Trap KKP");
    }
    previousButtonMillis = currentMillis;
  }
}

void loop() {

  currentMillis = millis();

  readButtonState();

  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

  client->setInsecure();

  //create an HTTPClient instance
  HTTPClient https;

  while (serial.available() > 0) {
    data = serial.readString();
    // data = "bro";
    String dataku = "https://capt-api.kkpsurabaya.id/input_mousetrap_get/"+ data;
    char charBuf[100];
    dataku.toCharArray(charBuf, 100);
    char *hh = charBuf;

    
    // // char* input = toCharArray(data);
    char *params = "https://capt-api.kkpsurabaya.id/input_mousetrap_get/";
    // *params += *charBuf;
    // char buffer[100];
    // sprintf(buffer, "https://capt-api.kkpsurabaya.id/input_mousetrap_get/%s", data);
    Serial.println(hh);
    if (https.begin(*client, hh)) {  // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
         serial.printf("GET..code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
        //  serial.print("Data Sukses..");
        //  Serial.print("Data Sukses..");
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  if (data.length() > 0) {
    // sent data request http
    
  }
}