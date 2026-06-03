/******************************************************************//**
 * @file	lcd132x64.c
 * @author  Arkadiusz Hudzikowski (Modyfikacja dla ST7565 128x64)
 * @version 1.6
 * @date	24.05.2026
 * @brief Plik obslugi wyswietlacza ST7565 / ST7567.
 *********************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <inttypes.h>
#include "lcd132x64.h"
#include "font5x7.h"

#define SPLC501C_SET		IOSET0
#define SPLC501C_CLR		IOCLR0
#define SPLC501C_DIR		IODIR0
#define SPLC501C_PIN		IOPIN0

// rozklad wyprowadzen
#define SPLC501C_RD		(1 << 0) 	//pin PA0 -> E
#define SPLC501C_WR		(1 << 1) 	//pin PA1 -> RW
#define SPLC501C_A0		(1 << 2) 	//pin PA2 -> RS
#define SPLC501C_RES  	(1 << 3) 	//pin PA3 -> RST
#define SPLC501C_CS1 	(1 << 0) 	//pin PE0  -> CS1B
#define LCD_CTRL_PORT        PORTA
#define LCD_CTRL_PORT2        PORTE
#define VCC_PORT 			 PORTA
#define VCC_PIN				 (1<< 4)
#define DA_PIN   (1<<1)

// port danych
#define LCD_DATA_PORT   PORTC

// zmienne w pamieci EEPROM przechowujace ustawienia kontrastu i podswietlenia
EEMEM uint8_t e_contrast;
EEMEM int8_t e_bright;

/********************************************//**
 * @brief Funkcja konfigurujaca wyprowadzenia LCD
 * @return none
 ***********************************************/
void LCDInitPort(void)
{
volatile uint16_t i;

LCD_CTRL_PORT.OUTSET = (SPLC501C_RD | SPLC501C_WR | SPLC501C_A0 | SPLC501C_RES);
LCD_CTRL_PORT.DIRSET = (SPLC501C_RD | SPLC501C_WR | SPLC501C_A0 | SPLC501C_RES);
LCD_CTRL_PORT2.OUTSET = (SPLC501C_CS1 | DA_PIN);
LCD_CTRL_PORT2.DIRSET = (SPLC501C_CS1 | DA_PIN);
LCD_DATA_PORT.DIRSET = 0xFF;
LCD_DATA_PORT.OUT = 0xff;

VCC_PORT.DIRSET = VCC_PIN; // Wlacz zasilanie ukladow
VCC_PORT.OUTSET = VCC_PIN;
for(i = 0 ; i < 5000; i++);
LCD_CTRL_PORT.OUTCLR = SPLC501C_RES;
for(i = 0 ; i < 500; i++);
LCD_CTRL_PORT.OUTSET = SPLC501C_RES;

// Podswietlenie
TCE0.CTRLA = 0x06; // presc. 256x
TCE0.PER = 100; // period 100
LCDBright(0); // bright (compare value)
TCE0.CTRLB = 1<<5 | 3<<0; // CCBEN, single slope PWM
}

/********************************************//**
 * @brief Funkcja wysylajaca jeden bajt do wyswietlacza
 * @param data : bajt danych
 * @return none
 ***********************************************/
void LCDWriteData(uint8_t data)
{
LCD_DATA_PORT.DIRSET = 0xFF;
asm("nop");
asm("nop");
LCD_DATA_PORT.OUT = data;
LCD_CTRL_PORT.OUTCLR = (SPLC501C_WR);
LCD_CTRL_PORT2.OUTCLR = (SPLC501C_CS1);
asm("nop");
asm("nop");
LCD_CTRL_PORT.OUTSET = (SPLC501C_WR);
LCD_CTRL_PORT2.OUTSET = (SPLC501C_CS1);
asm("nop");
asm("nop");
}

/********************************************//**
 * @brief Funkcja wysylajaca komende do wyswietlacza
 * @param comm : komenda
 * @return none
 ***********************************************/
void LCDWriteComm(uint8_t comm)
{
LCD_DATA_PORT.DIRSET = 0xFF;
asm("nop");
asm("nop");
LCD_DATA_PORT.OUT = comm;
LCD_CTRL_PORT.OUTCLR = (SPLC501C_WR | SPLC501C_A0);
LCD_CTRL_PORT2.OUTCLR = (SPLC501C_CS1);
asm("nop");
asm("nop");
LCD_CTRL_PORT.OUTSET = (SPLC501C_WR | SPLC501C_A0);
LCD_CTRL_PORT2.OUTSET = (SPLC501C_CS1);
asm("nop");
asm("nop");
}

/********************************************//**
 * @brief Funkcja konfigurujaca wyswietlacz LCD ST7565
 * @return none
 ***********************************************/
void LCDInit(void)
{
LCDInitPort();
_delay_loop_2(0xffff);

LCDWriteComm(SPLC501C_RESET); // Wymuszenie sprz?towego resetu sterownika
_delay_loop_2(0x8000);

// 1. POPRAWKA LUSTRA (Zmieniono z SPLC501C_ADC_REVERSE na 0xA0)
LCDWriteComm(0xA0);                 // 0xA0 = Normalny kierunek kolumn (wy??cza lustro poziome)
LCDWriteComm(SPLC501C_COM63);       // Lustrzane odbicie pionowe werykalne (C8)

LCDWriteComm(SPLC501C_BIAS_19);     // Ustawienie napi?cia BIAS na 1/9 (A2)		

// Ustawienie stosunku wewn?trznych rezystorów (Voltage Regulator Ratio)
LCDWriteComm(SPLC501C_VOLTAGE_RATIO | 0x05); 						

// KROKOWE URUCHAMIANIA PRZETWORNICY:
LCDWriteComm(0x2C); // Krok 1: Booster
_delay_ms(20);

LCDWriteComm(0x2E); // Krok 2: Regulator
_delay_ms(20);

LCDWriteComm(0x2F); // Krok 3: Follower
_delay_ms(20);

// 2. POPRAWKA KONTRASTU (Zmniejszono warto?? startow? z 45 na 30)
LCDWriteComm(SPLC501C_VOLUME_MODE);	// Komenda 0x81
LCDWriteComm(SPLC501C_VOLUME_SET | 30); // Zmniejszony kontrast (zakres 0 - 63)

LCDWriteComm(0xA4); // Wy??czenie pod?wietlenia wszystkich punktów
LCDWriteComm(SPLC501C_DISPLAY_ON); // W??czenie matrycy (AF)		
LCDWriteComm(SPLC501C_DISPLAY_NORMAL); // Normalny tryb wy?wietlania (A6)

LCDClearScreen(); // Wyczyszczenie pami?ci RAM wy?wietlacza

LCDWriteComm(SPLC501C_PAGE_ADDRESS | 0);
LCDWriteComm(SPLC501C_COLUMN_ADDRESS_HI | 0);		
LCDWriteComm(SPLC501C_COLUMN_ADDRESS_LO | 0);
LCDWriteComm(SPLC501C_START_LINE | 0);

LCDContrast(0);
LCDBright(0);
}

/********************************************//**
 * @brief Funkcja wylaczajaca wyswietlacz
 * @return none
 ***********************************************/
void LCDOff(void)
{
	LCDWriteComm(SPLC501C_DISPLAY_OFF);
	LCD_DATA_PORT.OUT = 0;
	TCE0.CTRLB = 0; // PWM off
	TCE0.CTRLA = 0; // timer off
	LCD_CTRL_PORT.OUTCLR = (SPLC501C_RD | SPLC501C_WR | SPLC501C_A0 | SPLC501C_RES);
	LCD_CTRL_PORT2.OUTCLR = (SPLC501C_CS1 | DA_PIN);
	VCC_PORT.OUTCLR = VCC_PIN;
}

/********************************************//**
 * @brief Funkcja ustawiajaca podswietlenie wyswietlacza
 * @param step : wartosc zmiany podswietlenia (-100 - 100)
 * @return uint8_t : nowa wartosc podswietlenia (0 - 100)
 ***********************************************/
uint8_t LCDBright(int8_t step)
{
	uint8_t bright = eeprom_read_byte((uint8_t*)&e_bright);
    if(bright > 100 || bright == 0) bright = 89;
	bright+=step;
	if(bright>100)bright=100;
	else if(bright<0)bright=0;
	TCE0.CCB = bright;
	eeprom_write_byte((uint8_t*)&e_bright, bright);
	return bright;
}

/********************************************//**
 * @brief Funkcja ustawiajaca kontrast wyswietlacza dla ST7565
 * @param step : wartosc zmiany kontrastu (-64 - 64)
 * @return uint8_t : nowa wartosc kontrastu (0 - 63)
 ***********************************************/
uint8_t LCDContrast(int8_t step)
{
	uint8_t contrast = eeprom_read_byte(&e_contrast);
	
	// Bezpiecznik: je?li w EEPROM zapisana by?a stara warto?? (np. wygaszona), 
	// ustaw ?rodkow?, bezpieczn? warto?? pocz?tkow? dla ST7565 (32).
	if(contrast > 63 || contrast < 5) contrast = 19;
	
	int16_t calc_contrast = contrast + step;
	if(calc_contrast > 63) calc_contrast = 63;
	else if(calc_contrast < 0) calc_contrast = 0;
	contrast = (uint8_t)calc_contrast;
	
	LCDWriteComm(SPLC501C_VOLUME_MODE);	// Komenda 0x81
	LCDWriteComm(SPLC501C_VOLUME_SET | contrast); // Warto?? 0x00 - 0x3F
	eeprom_write_byte(&e_contrast, contrast);
	return contrast;
}

/********************************************//**
 * @brief Funkcja ustawiajaca kursor na wybranej pozycji
 * @param x : nr znaku w wierszu
 * @param y : nr wiersza
 * @return none
 ***********************************************/
void LCDGoTo(uint8_t x, uint8_t y)
{
x += LCD_COLUMN_OFFSET; // Dodanie offsetu mapowania kolumn RAM dla szkie? 128x64
LCDWriteComm(SPLC501C_COLUMN_ADDRESS_HI | (x >> 4));
LCDWriteComm(SPLC501C_COLUMN_ADDRESS_LO | (x & 0x0F));
LCDWriteComm(SPLC501C_PAGE_ADDRESS | y);
}

/********************************************//**
 * @brief Funkcja czyszczaca ekran wyswietlacza
 * @return none
 ***********************************************/
void LCDClearScreen(void)
{
unsigned char x = 0, y = 0;
for(y = 0; y < (SCREEN_HEIGHT/PIXELS_PER_PAGE); y++)
	{
	LCDGoTo(0,y);
	for(x = 0; x < SCREEN_WIDTH; x++)
		{
		LCDWriteData(0);
		}
	}
}		

/********************************************//**
 * @brief Funkcja wysylajaca znak do wyswietlacza
 * @param charCode : znak ASCII
 * @return none
 ***********************************************/
void LCDWriteChar(uint8_t charCode)
{
const uint8_t* wsk = &font5x7[(charCode-FONT_OFFSET)*FONT_WIDTH];
for (charCode=0; charCode<FONT_WIDTH; charCode++)
  LCDWriteData(pgm_read_byte(wsk++));
LCDWriteData(0);
}

/********************************************//**
 * @brief Funkcja wysylajaca znak do wyswietlacza z negacja kolorow
 * @param charCode : znak ASCII
 * @return none
 ***********************************************/
void LCDWriteCharNeg(uint8_t charCode)
{
const uint8_t* wsk = &font5x7[(charCode-FONT_OFFSET)*FONT_WIDTH];
for (charCode=0; charCode<FONT_WIDTH; charCode++)
  LCDWriteData(255-pgm_read_byte(wsk++));
LCDWriteData(255);
}

/********************************************//**
 * @brief Funkcja wysylajaca lancuch znakow z pamieci FLASH do wyswietlacza
 * @param *string : adres lancucha znakow z pamieci flash
 * @return none
 ***********************************************/
void LCDText_p(const char* string)
{
while(pgm_read_byte(string))
  LCDWriteChar(pgm_read_byte(string++));
}

/********************************************//**
 * @brief Funkcja wysylajaca lancuch znakow z pamieci FLASH do wyswietlacza z negacja kolorow
 * @param *string : adres lancucha znakow z pamieci flash
 * @return none
 ***********************************************/
void LCDTextNeg_p(const char* string)
{
while(pgm_read_byte(string))
  LCDWriteCharNeg(pgm_read_byte(string++));
}