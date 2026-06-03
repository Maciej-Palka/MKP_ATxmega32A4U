Do kompilacji i wgrywania kodu użyto odpowiednio MPLAB X IDE i MPLAB IPE <br>
W pobranym repo należy utworzyć projekt MPLAB i następnie dodać pliki źródłowe i nagłówkowe <br>
Choose project -> microchip embedded -> ATXMEGA32A4 (A4U, trzeba zportować niektóre rzeczy pod A4U) -> X8C (v3.10) <br>
Properties -> X8C Compiler -> define macros: F_CPU=32000000UL oraz additional options: -mconst-data-in-progmem <br>
<br>

Konwencja nazywania branchy np.: 
 - kiedy kilka osób pracuje nad problemem:
   - DAC_issue
 - kiedy jedna osoba pracuje nad problemem:
   - @maciekp/feature1
   - @maciekp/bugfix1
   - @maciekp/hotfix1

Konwencje projektowe:
- osobny branch na każdy feature
- do pull requesta ma być wypełniona templatka z informacjami
- main jest protected
- od dzisiaj każdy pisze testy jednostkowe dla swoich funkcji/klas

Templatka do pull requesta:
- co zostało dodane/zmienione
- czy jest zgodne z aktualnym mainem (ewentualny rebase)
- czy się kompiluje
- czy działa jak natura chciała
