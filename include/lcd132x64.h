/******************************************************************//**
 * @file	lcd132x64.h
 * @author  Arkadiusz Hudzikowski (Modyfikacja dla ST7565 128x64)
 * @version 1.6
 * @date	24.05.2026
 * @brief Plik naglowkowy obslugi wyswietlacza 128x64.
 *********************************************************************/
 
#define SCREEN_WIDTH 	128  // ZMIENIONO Z 132 NA 128
#define SCREEN_HEIGHT	64
#define PIXELS_PER_PAGE	8

// KOMPENSACJA OFFSETU PAMI?CI RAM:
// Wy?wietlacze 128x64 czasami mapuj? piksele od kolumny 0, a czasami od kolumny 4.
// Je?li po wgraniu obraz b?dzie przesuni?ty w lewo o 4 piksele, zmie? poni?sz? warto?? na 4.
#define LCD_COLUMN_OFFSET 0  

#include<avr/pgmspace.h>

#define SPLC501C_DISPLAY_ON 		0xAF
#define SPLC501C_DISPLAY_OFF		0xAE

#define SPLC501C_START_LINE			0x40

#define SPLC501C_PAGE_ADDRESS		0xB0

#define SPLC501C_COLUMN_ADDRESS_HI	0x10
#define SPLC501C_COLUMN_ADDRESS_LO	0x00

#define SPLC501C_ADC_NORMAL			0xA0
#define SPLC501C_ADC_REVERSE		0xA1

#define SPLC501C_DISPLAY_NORMAL		0xA6
#define SPLC501C_DISPLAY_REVERSE	0xA7

#define SPLC501C_DISPLAY_ALL_ON		0xA5
#define SPLC501C_DISPLAY_ALL_OFF	0xA4

#define SPLC501C_BIAS_19			0xA2
#define SPLC501C_BIAS_15			0xA3

#define SPLC501C_RMW_START			0xE0
#define SPLC501C_RMW_END			0xEE

#define SPLC501C_RESET				0xE2

#define SPLC501C_COM0				0xC0 // Normalny kierunek linii (SPLC501C)
#define SPLC501C_COM63				0xC8 // Odwrócony kierunek linii (Standard ST7565/TG12864)

#define SPLC501C_POWERON			0x2F

#define SPLC501C_VOLTAGE_RATIO		0x20

#define SPLC501C_VOLUME_MODE		0x81
#define SPLC501C_VOLUME_SET			0x00

#define SPLC501C_PAGE_BLINKING_MODE	0xD5
#define SPLC501C_PAGE_BLINKING_0	0x01
#define SPLC501C_PAGE_BLINKING_1	0x02
#define SPLC501C_PAGE_BLINKING_2	0x04
#define SPLC501C_PAGE_BLINKING_3	0x08
#define SPLC501C_PAGE_BLINKING_4	0x10
#define SPLC501C_PAGE_BLINKING_5	0x20
#define SPLC501C_PAGE_BLINKING_6	0x40
#define SPLC501C_PAGE_BLINKING_7	0x80

void LCDOff(void);
uint8_t LCDBright(int8_t step);
uint8_t LCDContrast(int8_t step);
void LCDWriteData(unsigned char dataToWrite);
void LCDGoTo(unsigned char, unsigned char);
void LCDWriteChar(uint8_t charCode);
void LCDWriteCharNeg(uint8_t charCode);
void LCDText_p(const char* string);
void LCDTextNeg_p(const char* string);
void LCDInit(void);
void LCDClearScreen(void);
void lcd_String_neg(uint8_t val);