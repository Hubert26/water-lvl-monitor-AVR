# PMIK

## Opis projektu
Projekt urządzenia kontrolującego minimalny i maksymalny poziom wody w zbiorniku wodnym (np. oczko wodne). System umożliwia monitorowanie poziomu wody, przesyłanie ostrzeżeń do użytkownika oraz automatyczne dopompowanie wody, gdy poziom wody spadnie poniżej ustawionego minimum.

## Autor
- **Hubert Szewczyk**
- **Data utworzenia:** 26 stycznia 2020

---

## Realizowane funkcje
1. **Sygnalizacja przekroczenia poziomu wody**:
   - Minimalny i maksymalny poziom wody jest ustalany przez użytkownika za pomocą odpowiedniego zamocowania dwóch czujników poziomu wody.

2. **Przesyłanie ostrzeżeń do użytkownika**:
   - Ostrzeżenia są przesyłane przez USB (wykorzystana przejściówka UART-USB).
   - Ostrzeżenie zawiera datę i godzinę zdarzenia.
   - Możliwość odczytania dat wcześniejszych zdarzeń.

3. **Zapis zdarzeń na kartę SD**:
   - Dane (data i godzina zdarzenia) są zapisywane na karcie SD za pomocą interfejsu SPI.

4. **Automatyczne dopompowanie wody**:
   - Gdy poziom wody spadnie poniżej minimum, system automatycznie uruchamia dopompowanie wody.

---

## Użyte biblioteki
### 1. FatFS
- Uniwersalna biblioteka do obsługi systemów plików FAT, wykorzystywana do zapisu zdarzeń na karcie SD.

---

## Wymagane komponenty
- Mikrokontroler z rodziny AVR.
- Czujniki poziomu wody.
- Moduł RTC (DS1307) do obsługi daty i czasu.
- Moduł SD obsługujący interfejs SPI.
- Przejściówka UART-USB.
- Pompa wodna sterowana mikrokontrolerem.

---

## Struktura projektu
- **Pliki główne**:
  - `main.c`: Główny plik projektu.

- **Biblioteki użyte w projekcie**:
  - `common/common.h`: Funkcje i definicje wspólne dla projektu.
  - `DS1307/ds1307.h`: Obsługa modułu RTC (DS1307) za pomocą interfejsu I2C.
  - `I2C_TWI/i2c_twi.h`: Funkcje komunikacji I2C.
  - `UART/mkuart.h`: Obsługa komunikacji UART.
  - `MK_TERM/mk_term.h`: Terminal do komunikacji z użytkownikiem.
  - `SENSORS/water_sens.h`: Obsługa czujników poziomu wody.
  - `FatFS/diskio.h`, `FatFS/ff.h`: Obsługa systemu plików FAT na karcie SD.
  - `MENU/menu.h`: Interfejs użytkownika.
  - `global.h`: Zmienne globalne projektu.
