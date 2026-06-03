Do kompilacji i wgrywania kodu użyto odpowiednio MPLAB X IDE i MPLAB IPE
W pobranym repo należy utworzyć projekt MPLAB i następnie dodać pliki źródłowe i nagłówkowe
Choose project -> microchip embedded -> ATXMEGA32A4 (A4U, trzeba zportować niektóre rzeczy pod A4U) -> X8C (v3.10)
Properties -> X8C Compiler -> define macros: F_CPU=32000000UL oraz additional options: -mconst-data-in-progmem
