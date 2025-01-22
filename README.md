# Wstęp
Projekt jest realizowany w ramach zajęć z przedmiotu Internet Rzeczy.

# Założenia projektowe
Celem projektu jest stworzenie systemu, który umożliwia zbieranie danych środowiskowych za pomocą czujnika BME280 (mierzącego temperaturę, wilgotność i ciśnienie atmosferyczne) i przesyłanie ich do Raspberry Pi za pomocą technologii Bluetooth Low Energy (BLE). Płytki Adafruit Feather M0 Bluefruit LE pełnią rolę urządzeń zbierających dane z czujnika i transmitujących je do Raspberry Pi, które działa jako pośrednik w komunikacji. Raspberry Pi odbiera dane za pomocą BLE, a następnie przesyła je do platformy IoT ThingsBoard za pomocą protokołu MQTT, umożliwiając ich monitorowanie i dalszą analizę w chmurze. Projekt ma na celu zademonstrowanie wykorzystania BLE do transmisji danych w systemach IoT oraz integrację z popularną platformą IoT do zarządzania i wizualizacji danych.

# Struktura plików
## Adafruit Feather M0 Bluefruit LE
```
- BLEsendSensorData.cpp
- BluefruitConfig.h
```
### Opis funkcjonalności
Plik BluefruitConfig.h odpowiada za konfigurację działania BLE, UART oraz SPI na płytce. 
Plik BLEsendSensorData.cpp implementuje system zbierania, przesyłania i zapisywania danych środowiskowych z czujnika BME280 za pomocą technologii Bluetooth Low Energy (BLE) oraz karty SD. 
Program rozpoczyna od konfiguracji czujnika BME280, modułu RTC, karty SD oraz BLE. Jeśli którykolwiek komponent nie zostanie poprawnie zainicjalizowany, wyświetlane są komunikaty diagnostyczne w trybie debugowania.
- Pomiar danych:
Dane z czujnika BME280 są odczytywane i przypisywane do zmiennych przechowujących temperaturę, ciśnienie i wilgotność.

- Timer1 wywołuje funkcję pomiarową co 10 sekund, co umożliwia cykliczne odczytywanie i przesyłanie danych.
Przesyłanie danych przez BLE:

- Dane są przesyłane w formacie tekstowym za pomocą polecenia AT+BLEUARTTX. Komunikacja BLE jest monitorowana pod kątem poprawności, a ewentualne błędy są zgłaszane.

Tryb debugowania:
W trybie DEBUG_MODE szczegółowe informacje o stanie działania systemu i przesyłanych danych są wyświetlane w konsoli.


## Raspberry PI
```
- iot.py
- main.py
```
Plik iot.py implementuje system komunikacji z wieloma urządzeniami BLE (Bluetooth Low Energy) jednocześnie za pomocą protokołu UART (Universal Asynchronous Receiver-Transmitter).
Program umożliwia:
- Skanowanie i nawiązywanie połączeń z urządzeniami BLE, które obsługują usługę UART.
- Obsługę wielu połączeń jednocześnie (do maksymalnej liczby zdefiniowanej przez max_connections).
- Odczyt danych z każdego połączonego urządzenia BLE w sposób asynchroniczny.
- Automatyczne wykrywanie i usuwanie urządzeń, które się rozłączyły.


Plik main.py implementuje klienta MQTT do komunikacji z serwerem ThingsBoard, platformą IoT do monitorowania i zarządzania urządzeniami.
