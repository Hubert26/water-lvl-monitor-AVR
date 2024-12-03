/**
* @file ds1307.c
* @author Hubert Szewczyk
* @date Wrzesien 19, 2019
* @brief Ten plik zawiera obsluge RTC modulu DS1307
*/ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>


#include "ds1307.h"
#include "../I2C_TWI/i2c_twi.h"
#include "../common/common.h"

/**
* @brief Flaga sygnalizujaca zgloszenie przerwania INT2.
*
* Flaga ustawiana co  jedna sekunde w przerwaniu INT2 i sprawdzana w funkcji RTC_EVENT.
*/
volatile uint8_t int2_flag=1;

/**
* @brief Tablica nazw dni tygodnia.
*
* Tablica zawiera trzy literowe skroty nazw dni tygodnia oddzielone od siebie znakami "\x00".
* Tablica znajduje sie w pamieci programu.
*/
const char dni[] PROGMEM = {
	"pon""\x00""wto""\x00""sro""\x00""czw""\x00""pia""\x00""sob""\x00""nie"
};

/**
* @brief Zmienna zawierajaca aktualny czas i date.
*
* Zmienna aktualizuje sie co 1 sekunde w funkcji RTC_EVENT().
*/
TDATETIME datetime;

/**
* @brief Wsakznik do funkcji collback dla zdarzenia RTC_EVENT().
*
*/
static void (*rtc_callback)( TDATETIME * dt );



/**
* @brief Funkcja sluzaca do rejestracji funkcji zwrotnej wywolywanej w funkcji RTC_EVENT().
* @param[in]  callback Wskaznik na funkcje collback ktorej parametrem jest wskaznik na strukture TDATETIME w ktorej przechowywany jest czas i data.
* @return VOID
*/
void register_rtc_event_callback(void(*callback)(TDATETIME * dt)) {
	rtc_callback = callback;
	}



/**
* @brief Funkcja inicjalizujaca komunikacje z modu³em DS1307
*
* Funkcja ustawia wyzwalanie odpowiednim zboczem, odblokowanie przerwania INT2 w mikrokontrolerze, szybkosc transmisji.
* Funkcja ustawia wyjscie OUT modulu DS1307, aby generowalo sygnal PWM o wypelnieniu 50% i czestotliwosci 1Hz.
* @return VOID
*/
void init_rtc( void ){
		// Przerwanie INT2
	MCUCR |= (1<<ISC21);	// wyzwalanie zboczem opadaj¹cym
	EIMSK |= (1<<INT2);		// odblokowanie przerwania nie potrzebne poniewa¿ korzystamy z flagi sprzêtowej
	PORTD |= (1<<PD2);		// podci¹gniêcie pinu INT2 do VCC
	
	i2cSetBitrate(100);
	
	uint8_t control = 0b10010000;
	TWI_write_buf(DS1307_ADDR, 0x07, 1, &control);
	TWI_write_buf(DS1307_ADDR, 0x00, 1, 0); //zerowanie bitu CH
}


/**
* @brief Funkcja zapisujaca czas w rejestrach DS1307.
* Funkcja zapisujaca godziny, minuty i sekundy w rejestrach DS1307.
* @param[in]  dt Wskaznik na strukture TDATETIME w ktorej przechowywany jest czas i data.
* @param[in]  hh Godzina w postaci dziesietnej.
* @param[in]  mm Minuty w postaci dziesietnej.
* @param[in]  ss Sekundy w postaci dziesietnej.
* @return VOID
*/
void set_rtc_time(TDATETIME * dt, uint8_t hh, uint8_t mm, uint8_t ss) {
	dt->rtc_buf[0] = dec2bcd(ss);
	dt->rtc_buf[1] = dec2bcd(mm);
	dt->rtc_buf[2] = dec2bcd(hh);
	TWI_write_buf(DS1307_ADDR, 0x00, 3, dt->rtc_buf);
}

/**
* @brief Funkcja zapisujaca date w rejestrach DS1307.
* Funkcja zapisujaca rok, miesiac i dzien w rejestrach DS1307.
* @param[in]  dt Wskaznik na strukture TDATETIME w ktorej przechowywany jest czas i data.
* @param[in]  YY Rok w postaci dziesietnej.
* @param[in]  MM Miesiac w postaci dziesietnej.
* @param[in]  DD Dzien w postaci dziesietnej.
* @return VOID
*/
void set_rtc_date(TDATETIME * dt, uint16_t YY, uint8_t MM, uint8_t DD) {
	dt->rtc_buf[3] = oblicz_week_day(DD, MM, YY);
	dt->rtc_buf[4] = dec2bcd(DD);
	dt->rtc_buf[5] = dec2bcd(MM);
	//dt->rtc_buf[6] = YY & 0xff;
	dt->rtc_buf[6] = yeartobcd(YY);
	
	TWI_write_buf(DS1307_ADDR, 0x03, 4, &dt->rtc_buf[3]);
	TWI_write_buf(DS1307_ADDR, 0x08, 2, (uint8_t*)&YY ); //zapis 2 bajtów do RAM rtc
}

/**
* @brief Funkcja zapisujaca czas i date w rejestrach DS1307.
* Funkcja zapisuj¹ca rok, miesiac, dzien, godziny, minuty i sekundy w rejestrach DS1307.
* @param[in]  dt Wskaznik na strukture TDATETIME w ktorej przechowywany jest czas i data.
* @param[in]  YY Rok w postaci dziesietnej.
* @param[in]  MM Miesiac w postaci dziesietnej.
* @param[in]  DD Dzien w postaci dziesietnej.
* @param[in]  hh Godzina w postaci dziesietnej.
* @param[in]  mm Minuty w postaci dziesietnej.
* @param[in]  ss Sekundy w postaci dziesietnej.
* @return VOID
*/
void set_rtc_datetime(TDATETIME * dt, uint16_t YY, uint8_t MM, uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss) {
	set_rtc_date(dt, YY, MM, DD);
	set_rtc_time(dt, hh, mm, ss);
}

/**
* @brief Funkcja zawierajaca odpowiedz na zdarzenie zgloszone przez modul DS1307.
* Funkcja zawiera funkcje callback oraz zeruje flage int2_flag ustawiana w przerwaniu INT2.
* @return VOID
*/
void RTC_EVENT( void )  { 
	if(int2_flag)//if (EIFR & (1<<INTF2)) //spr czy sprzêtowa flaga zosta³a ustawiona
	{
		
		//PORTB ^= (1<<PB7); //???Dlaczego czerwony LED œwieci siê krócej? 
	
		get_rtc_datetime(&datetime);
		
		if( rtc_callback ) rtc_callback( &datetime );
		int2_flag=0;
		//EIFR |= (1<<INTF2); //kasowanie flagi sprzêtowej (w AVR sie ustawia 1)
	}
}



/**
* @brief Funkcja odczytujaca czas i date z rejestrow DS1307.
* Funkcja odczytuje rok, miesiac, dzien, godziny, minuty i sekundy z rejestrow DS1307.
* Funkcja zapisuje odczytane dane w strukturze TDATETIME w postaci liczbowej jak i znakow ASCII.
* @param[in]  dt Wskaznik na strukture TDATETIME w ktorej przechowywany jest czas i data.
* @return VOID
*/
void get_rtc_datetime(TDATETIME * dt) {
	//odczyt 4 bajtów do bufora od adresu 0x01 z pamiêci RAM naszego RTC
	TWI_read_buf( DS1307_ADDR, 0x00, 7, dt->rtc_buf );
	TWI_read_buf( DS1307_ADDR, 0x08, 2, (uint8_t*)&dt->YY );//wczytujemy 2 bajty roku z pierwszego adresu RAM rtc
	
	int8_t i;
	uint8_t * wsk = dt->rtc_buf; //wskaŸnik na pierwszy bajt bufora
#if DATETIME_AS_STRING
	char * znak = dt->time; 
#endif
	for (i=2; i>-1; i--)
	{
#if DATETIME_AS_STRING
		*(znak++) = ( (*(wsk+i) & (2 == i ? 0x3f : 0x7f) ) >>4) + '0'; //zapisuje starsz¹ czêœæ bajtu np dziesi¹tki godzin i odp maskuje
		*(znak++) = (*(wsk+i) & 0x0f ) + '0'; //zapisuje m³odsz¹ czêœæ bajtu np jednostki godzin
		*(znak++) = i?':':0; //jeœli to sekundy to koñczy string 0
#endif
		*( (uint8_t*)dt+2-i ) = bcd2dec( *(wsk+1) );//zapisuje dziesiêtnie do hh mm ss
	}
	
	dt->DD = bcd2dec(*(wsk+4) & 0x3f); //zapis daty
	
	uint8_t yr =bcd2dec(*(wsk+6));//Czy napewno zamieniaæ na dec? -> bez dec nie dziala
	//uint8_t yr =wsk[6]; dt->YY jest bin a yr nie

	
	
	/*//Sylwestrowa procedura NOPE - dlatego ¿e ca³y bajt porównujesz?
	if ( (dt->YY & 0xff) <= yr)//!= - zapisuje sie z³y rok, ale siê zmienia w sylwestra /<- dobry rok ale sie nie zmienia w sylwestra
	{
		while((dt->YY & 0xff) != yr) dt->YY++;
		TWI_write_buf(DS1307_ADDR, 0x08, 2, (uint8_t*)&dt->YY); //zapis do RAM rtc
	}*/
	
	//Sylwestrowa procedura OK
	if ( (dt->YY & 0x03) != (yr & 0x03 ))
	{
		while((dt->YY & 0x03) != (yr & 0x03) ) dt->YY++;
		TWI_write_buf(DS1307_ADDR, 0x08, 2, (uint8_t*)&dt->YY); //zapis do RAM rtc
	}
	
	
	dt->MM = bcd2dec(*(wsk+5) & 0x1f);//zapis miesi¹ca
	dt->weekday = wsk[3];//zapis dnia tyg
	
#if DATETIME_AS_STRING == 1
	znak = dt->date; //pod wsk znak podk³adamy adres bufora date 
	if( dt->YY < 10) *(znak++) = '0';
	if( dt->YY < 100) *(znak++) = '0';
	if( dt->YY < 1000) *(znak++) = '0';
	itoa(dt->YY, znak, 10);
	
	znak = dt->date; //pod wsk znak podk³adamy adres bufora date 
	znak+=4;//wskaŸnik zwiêkszamy o 4 bo tyle znaków bufora zajmuje rok
	
	*(znak++) = DATE_SEPARATOR;
	
	*(znak++) = ( ( wsk[5] & 0x1f ) >> 4) + '0';//dziesi¹tki miesiêcy
	*(znak++) = ( ( wsk[5] & 0x0f )) + '0';//jednostki miesiêcy
	
	*(znak++) = DATE_SEPARATOR;
	
	*(znak++) = ( ( wsk[4] & 0x3f ) >> 4) + '0';//dziesi¹tki dni
	*(znak++) = ( ( wsk[4] & 0x0f )) + '0';//jednoœci dni
	
	*znak = 0;
#endif
}

/**
* @brief Funkcja oblicza dzien tygodnia.
* Funkcja oblicza dzien tygodnia na podstawie dnia, miesiaca i roku.
* @param[in]  dzien Dzien w postaci dziesietnej.
* @param[in]  miesiac Miesiac w postaci dziesietnej.
* @param[in]  year Rok w postaci dziesietnej.
* @return VOID
*/
uint8_t oblicz_week_day(uint8_t dzien, uint8_t miesiac, uint16_t year) {
	int Y,C,M,N,D;
	M=1+(9+miesiac)%12;
	Y=year-(M>10);
	C=Y/100;
	D=Y%100;
	N=((13*M-1)/5+D+D/4+6*C+dzien+5)%7;
	return (7+N)%7;
}

/**
* @brief Funkcja zamieniajaca rok w warotsci dziesietnej na wartoec w kodzie binarnym .
* 
* @param[in]  year  Rok w postaci dziesietnej.
* @return Wartosc roku w kodzie binarnym.
*/
uint8_t yeartobcd(uint16_t year)
{
	if(year>100)	
	return dec2bcd(year%100);
	else
	return dec2bcd(year);
}

/**
* @brief Procedura obslugii przerwania INT2.
* Ustawia flage int2_flag.
*/
ISR( INT2_vect ) {
	int2_flag = 1;
}