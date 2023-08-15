
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <SoftwareSerial.h>
SoftwareSerial serial(2, 3);

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

RF24 radio(7, 8);  // CE, CSN

const byte address[6] = "00001";
const int buzzer = 4;

String data = "";
//millis
unsigned long previousMillis1 = 0;
unsigned long previousMillisBuzzer = 0;
const long period1 = 500;
const long period_buzzer = 1000;
void setup() {
  while (!Serial)
    ;
  Serial.begin(9600);
  radio.begin();

  serial.begin(115200);
  lcd.init();
  //  pinMode(A3, OUTPUT);
  pinMode(buzzer, OUTPUT);
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.println("tes");
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long currentMillisBuzzer = millis();

  if (currentMillisBuzzer - previousMillisBuzzer >= period_buzzer) {
    previousMillisBuzzer = currentMillisBuzzer;
    //buzzer_aktif();
  }

  lcd.clear();



  if (currentMillis - previousMillis1 >= 50) {
    previousMillis1 = currentMillis;
    //



    if (radio.available()) {
      char text[32] = "";
      radio.read(&text, sizeof(text));

      lcd.backlight();
      lcd.setCursor(0, 1);
      lcd.print("Found =");
      lcd.setCursor(8, 1);
      lcd.print(text);
      // sent data to wemos
      serial.print(text);
      Serial.println(text);
      led_blink();
      buzzer_aktif();
      delay(300);
      lcd.noBacklight();
    }

    while (serial.available() > 0) {
      data = serial.readString();
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print(data);
      delay(300);
      lcd.noBacklight();
    }
  }
}

void led_blink() {
  analogWrite(A3, 150);
  delay(50);
  analogWrite(A3, 0);
  delay(200);
}

void buzzer_aktif() {
  for (int a = 0; a <= 3; a++) {
    for (int a = 0; a <= 5; a++) {
      tone(buzzer, 1000);  // Send 1KHz sound signal...
      delay(50);           // ...for 1 sec
      noTone(buzzer);
      delay(50);
    }
    delay(100);
  }
}
