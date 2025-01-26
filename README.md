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

Plik main.py to aplikacja opartą na Bluetooth Low Energy (BLE) i MQTT, która umożliwia zbieranie danych z urządzeń BLE, przetwarzanie ich, a następnie wysyłanie do serwera ThingsBoard. Oto ogólny opis działania programu:

- Inicjalizacja i konfiguracja: Program inicjalizuje połączenie z serwerem MQTT (ThingsBoard), używając podanego tokenu dostępu. Skrypt wykorzystuje bibliotekę adafruit_ble do komunikacji z urządzeniami BLE, które przesyłają dane za pomocą usługi UART.

- Wyszukiwanie urządzeń BLE: Program skanuje dostępne urządzenia BLE, które oferują usługę UART. Sprawdza, czy urządzenie zostało już połączone, aby uniknąć wielokrotnego łączenia się z tym samym urządzeniem. Jeśli nowe urządzenie zostanie znalezione, jest automatycznie łączone i dodawane do listy aktywnych połączeń.

- Odczyt danych z urządzeń BLE: Program cyklicznie sprawdza dane z połączonych urządzeń. Gdy dane są dostępne w usłudze UART, są odczytywane i przetwarzane (w tym przypadku zawierają informacje o temperaturze, ciśnieniu i wilgotności).

- Przygotowanie i wysyłanie danych: Odczytane dane są formatowane w odpowiednią strukturę, a następnie wysyłane do serwera ThingsBoard jako dane telemetryczne (np. temperatura, ciśnienie, wilgotność) oraz atrybuty urządzenia (np. adres IP i MAC).

- Obsługa zapytań RPC: Program obsługuje zapytania RPC (Remote Procedure Call) wysyłane z serwera ThingsBoard. Na przykład, jeśli serwer zażąda telemetrii z urządzenia, aplikacja zbiera dane z czujników i wysyła je z powrotem na serwer.

- Aktualizacja interwału odczytu: Program może również odbierać zmiany w interwale odczytu (np. częstotliwości blinkingu) z serwera poprzez subskrypcję na atrybuty. Gdy nastąpi zmiana, interwał jest dostosowywany.

- Cykliczne przetwarzanie i wysyłanie danych: Aplikacja regularnie odczytuje dane z czujników (w pętli), przetwarza je i wysyła na serwer w określonym interwale czasowym (co 10 sekund).

Cały proces działa asynchronicznie, umożliwiając obsługę wielu urządzeń BLE jednocześnie i dynamiczne reagowanie na zmiany konfiguracyjne przesyłane z serwera. Program kończy działanie, gdy urządzenie przestanie łączyć się z serwerem.
![Bez tytułu](https://github.com/user-attachments/assets/c96d99e7-c63a-4c67-b6a1-0097036be20f)


