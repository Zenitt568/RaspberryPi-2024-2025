#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#include <RTClib.h>
#include <Fat16.h>

#include <TimerOne.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

float temperature = 0;
float pressure = 0;
float humidity = 0;
byte start = 0;
byte fail = 0;
bool DEBUG_MODE = true;
bool sendNumbers = false;
int iter = 0;

#if SOFTWARE_SERIAL_AVAILABLE
#include <SoftwareSerial.h>
#endif

#define FACTORYRESET_ENABLE         1
//#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
//#define MODE_LED_BEHAVIOUR          "MODE"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

RTC_PCF8523 rtc;
SdCard card;
Fat16 file;
void measure(void);
void dataSend(float temperature, float pressure, float humidity);
void numbersSend (int);
void check_response(void);
void dataLog(float temperature, float pressure, float humidity);

#define error(s) error_P(PSTR(s))

void error_P(const char* str) {
  if (DEBUG_MODE) {
    Serial.print("Blad: ");
    Serial.println(str);
  }
  if (card.errorCode) {
    if (DEBUG_MODE) {
      Serial.print("Blad SD: ");
      Serial.println(card.errorCode, HEX);
    }
  }

}
void setup(void) {
if (DEBUG_MODE) {
  Serial.begin(115200);
}
delay(5000);
  // BME280
  if (!bme.begin()) {
    sendNumbers = true;
    if (DEBUG_MODE) {
        Serial.println(F("BME280 configuration fail"));
    }
  } else {
    sendNumbers = false;
    if (DEBUG_MODE) {
        Serial.println(F("BME280- OK"));
    }
  }

  // RTC
  if (!rtc.begin()) {
    if (DEBUG_MODE) {
        Serial.println(F("Couldn't find RTC"));
    }
  } else {
    if (DEBUG_MODE) {
        Serial.println(F("RTC- OK"));
    }
  }
delay(5000);
  // SD
    if (!card.init(10)) 
    {
      error("card.begin");
    }

  // Inicjalizacja systemu plików FAT16
  if (!Fat16::init(&card)) error("Fat16::init");

    if (DEBUG_MODE) {
      Serial.print(F("Initialising the Bluefruit LE module: "));
    }

  if (!ble.begin(VERBOSE_MODE)) {
    if (DEBUG_MODE) {
        error("Couldn't find Bluefruit");
    }
  }

  if (DEBUG_MODE) {
    Serial.println(F("OK!"));
  }

  ble.echo(false);
  ble.verbose(false);

  // Timer interrupt
  Timer1.initialize(10000000); // 10 s
  Timer1.attachInterrupt(measure);
}

void loop(void) {
  noInterrupts();
  interrupts();

  if (start) {
    if (sendNumbers) {
      iter = iter + 1;
      numbersSend(iter);
      if (iter >= 10) {
        iter = 0;
      }
      start = 0;
    } else {
      temperature = bme.readTemperature();
      pressure = bme.readPressure() / 100.0F;
      humidity = bme.readHumidity();

      dataSend(temperature, pressure, humidity);
      dataLog(temperature, pressure, humidity);
      start = 0;
    }
  }
}

void numbersSend(int iter) {
  ble.print("AT+BLEUARTTX=");
  ble.println(String(iter));
  check_response();
}


void dataSend(float temperature, float pressure, float humidity) {
  ble.print("AT+BLEUARTTX=");
  ble.println("#2t" + String(temperature) + "p" + String(pressure) + "h" + String(humidity));
  check_response();

  if (DEBUG_MODE) {
    if (fail != 1) {
      Serial.print("[Send] Temperature: ");
      Serial.println(temperature);
      Serial.print("[Send] Pressure: ");
      Serial.println(pressure);
      Serial.print("[Send] Humidity: ");
      Serial.println(humidity);
      Serial.println("");
    }
  }
}

// Funkcja zapisująca liczbę zmiennoprzecinkową do pliku (z dwoma miejscami po przecinku)
void writeFloat(float value) {
  char buffer[10];
  dtostrf(value, 4, 2, buffer); // Konwertuj float na string (4 znaki szerokości, 2 miejsca po przecinku)
  file.write(buffer); // Zapisz ciąg znaków do pliku
}

void dataLog(float temperature, float pressure, float humidity) {
  DateTime now = rtc.now();

  // Sprawdzenie, czy karta SD została zainicjalizowana
  if (!card.begin(10)) {
    if (DEBUG_MODE) {
        Serial.println("Karta SD nie zostala zainicjalizowana");
    }
  }

  // Otwieranie pliku do zapisu (lub tworzenie go, jeśli nie istnieje)
  if (!file.open("datalog.txt", O_RDWR | O_CREAT | O_APPEND)) {
    if (DEBUG_MODE) {
        Serial.println("Blad podczas otwierania pliku datalog.txt");
    }
  }

  // Formatowanie daty i godziny
  char buf[] = "DD-MM-YYYY hh:mm";
  String timestamp = now.toString(buf);

  // Zapisywanie danych do pliku
  file.println(timestamp);
  if (DEBUG_MODE) {
    Serial.println(timestamp);
  }

  file.print("Temperature: ");
  file.print(temperature);
  file.println("°C");
  if (DEBUG_MODE) {
    Serial.print("Saved temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
  }

  file.print("Pressure: ");
  file.print(pressure);
  file.println("hPa");
  if (DEBUG_MODE) {
    Serial.print("Saved pressure: ");
    Serial.print(pressure);
    Serial.println("hPa");
  }

  file.print("Humidity: ");
  file.print(humidity);
  file.println("%");
  if (DEBUG_MODE) {
    Serial.print("Saved humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  }

  file.println();
  if (DEBUG_MODE) {
    Serial.println();
  }

  // Zamknięcie pliku
  file.close();
}
/*
void dataLog(float temperature, float pressure, float humidity) {
  DateTime now = rtc.now();

  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    char buf[] = "DD-MM-YYYY hh:mm";

    dataFile.println(now.toString(buf));
if (DEBUG_MODE) {
    Serial.println(now.toString(buf));
}

    dataFile.println("Temperature: " + String(temperature) + "°C");
if (DEBUG_MODE) {
    Serial.println("Temperature: " + String(temperature) + "°C");
}

    dataFile.println("Pressure: " + String(pressure) + "hPa");
if (DEBUG_MODE) {
    Serial.println("Pressure: " + String(pressure) + "hPa");
}

    dataFile.println("Humidity: " + String(humidity) + "%");
if (DEBUG_MODE) {
    Serial.println("Humidity: " + String(humidity) + "%");
}

    dataFile.println("");
if (DEBUG_MODE) {
    Serial.println("");
}

    dataFile.close();
  } else {
if (DEBUG_MODE) {
    Serial.println("error opening datalog.txt");
}
  }
}*/

void measure(void) {
  start = 1;
}

void check_response(void) {
  if (!ble.waitForOK()) {
    if (DEBUG_MODE) {
        Serial.println(F("Failed to send?"));
    }
    fail = 1;
  } else {
    fail = 0;
  }
}
