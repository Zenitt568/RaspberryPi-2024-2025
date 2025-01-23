Projekt jest realizowany w ramach zajęć z przedmiotu Internet Rzeczy.

# Założenia projektowe
Celem projektu jest stworzenie systemu, który umożliwia zbieranie danych środowiskowych za pomocą czujnika BME280 (mierzącego temperaturę, wilgotność i ciśnienie atmosferyczne) i przesyłanie ich do Raspberry Pi za pomocą technologii Bluetooth Low Energy (BLE). Płytki Adafruit Feather M0 Bluefruit LE pełnią rolę urządzeń zbierających dane z czujnika i transmitujących je do Raspberry Pi, które działa jako pośrednik w komunikacji. Raspberry Pi odbiera dane za pomocą BLE, a następnie przesyła je do platformy IoT ThingsBoard za pomocą protokołu MQTT, umożliwiając ich monitorowanie i dalszą analizę w chmurze. Projekt ma na celu zademonstrowanie wykorzystania BLE do transmisji danych w systemach IoT oraz integrację z popularną platformą IoT do zarządzania i wizualizacji danych. Układ przedstawiono poniżej:

![472776695_973621041351978_8155405428183044361_n](https://github.com/user-attachments/assets/dd81927d-4d83-4f3d-a32c-c4294b5b76fc)


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

Plik main.py odczytuje dane z Adafruit Feather przetwarza je i przesyła do platformy Thingsboard z wykorzystaniem protokołu MQTT. Na poniższym rysunku pokazano wyniki z platformy ThingsBoard:

![Bez tytułu](https://github.com/user-attachments/assets/c96d99e7-c63a-4c67-b6a1-0097036be20f)


