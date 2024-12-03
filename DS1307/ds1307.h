/**
* @file ds1307.h
* @author Hubert Szewczyk
* @date Wrzesien 19, 2019
* @brief Plik konfuguracyjny projektu.
*/


#ifndef DS1307_H_
#define DS1307_H_

#define DATETIME_AS_STRING 1
#define DATE_SEPARATOR '.'

#define DS1307_ADDR 0b11010000

/**
* @brief Rozne stany dni tygodnia 
* 
*/
typedef enum
{
	pon, /**< Poniedzia³ek */
	wto, /**< Wtorek */
	sro, /**< Sroda */
	czw, /**< Czwartek */
	pia, /**< Piatek */
	sob, /**< Sobota */
	nie /**< Niedziela */
} TDAYS;

/**
* @brief Struktura zawierajaca czas i date.
*
* Struktura przechowuje czas i date w postaci liczb jak i znakow ASCII.
*/	
typedef struct {
	uint8_t hh; /**< Godziny */
	uint8_t mm; /**< Minuty */
	uint8_t ss; /**< Sekundy */
	uint16_t YY; /**< Rok */
	uint8_t MM; /**< Miesiac */
	uint8_t DD; /**< Dzien */
	TDAYS weekday; /**< Dzien tygodnia */
	uint8_t dst; /**< Czas zimowy/letni */
#if DATETIME_AS_STRING == 1
	char time[9]; /**< Czas jako znaki ASCII */ //2zn godz, 2zn min, 2zn sek; 2zn : ,1zn 0
	char date[11]; /**< Data jako znaki ASCII */
#endif
	uint8_t rtc_buf[7]; /**< RTC bufor */
	} TDATETIME;

/**
* @brief Zmienna globalna zawierajaca aktualny czas i date.
*
* Zmienna aktualizuje sie co 1 sekundê w funkcji RTC_EVENT().
*/
extern TDATETIME datetime; //zmienna globalna widoczna równie¿ w main


/**
* @brief Tablica nazw dni tygodnia.
*
* Tablica zawiera trzy literowe skroty nazw dni tygodnia oddzielone od siebie znakami "\x00".
* Tablica znajduje sie w pamieci programu.
*/
extern const char dni[];





void init_rtc( void );
void register_rtc_event_callback(void(*callback)(TDATETIME * dt));
void RTC_EVENT( void ) ;
void get_rtc_datetime(TDATETIME * dt);

void set_rtc_time(TDATETIME * dt, uint8_t hh, uint8_t mm, uint8_t ss);
void set_rtc_date(TDATETIME * dt, uint16_t YY, uint8_t MM, uint8_t DD);
void set_rtc_datetime(TDATETIME * dt, uint16_t YY, uint8_t MM, uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss);

uint8_t oblicz_week_day(uint8_t dzien, uint8_t miesiac, uint16_t year);
uint8_t yeartobcd(uint16_t year);



#endif /* DS1307_H_ */