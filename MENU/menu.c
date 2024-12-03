/**
* @file menu.c
* @author Hubert Szewczyk
* @date Grudzien 27, 2019
* @brief Ten plik zawiera funkcje obslugujace menu uzytkownika.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "menu.h"
#include "../MK_TERM/mk_term.h"
#include "../UART/mkuart.h"
#include "../common/common.h"
#include "../global.h"
#include "../DS1307/ds1307.h"
//#include "../FatFS/diskio.h"
//#include "../FatFS/ff.h"

/**
* @brief Wskaznik do funkcji callback dla zdarzenia MENU().
*
*/
static void (*main_menu2_callback)( void);

/**
* @brief Funkcja do rejestracji funkcji zwrotnej w zdarzeniu MENU()
* @param[in]  callback Wskaznik na funkcje collback.
* @return VOID
*/
void register_main_menu2_callback(void(*callback)(void)) {
	main_menu2_callback = callback;
}

/**
* @brief Funkcja sprawdzajaca jaki poziom menu uzytkownika zostal otworzony przez uzytkownika.
* Funkcja ta sprawdza jaki poziom menu uzytkownika zostal otworzony, wywoluje funkcje odpowiedzialne za wyswietlanie menu, wywoluje funkcje collback.
* @return VOID
*/
void MENU(void)
{
	//TDATETIME setting_datetime;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t days;
	uint8_t months;
	uint16_t year;
	
	switch(main_menu_state)
	{
		case 1:
		show_state1_menu();
		if((menu1_state!=1)|(menu1_state!=2))
		{
			//while(!(menu1_state==1)|(menu1_state==2))
			menu1_state=get_menu_position();
			//tr_cls(0);
		}
			switch(menu1_state)
			{
				case 1:
				
				hours=type_hours();
				uart_putint(hours,10);
				_delay_ms(1000);
				
				minutes=type_minutes();
				uart_putint(minutes,10);
				_delay_ms(1000);
				
				seconds=type_seconds();
				uart_putint(seconds,10);
				_delay_ms(1000);
				
				set_rtc_time(&datetime,hours,minutes,seconds);
				
				main_menu_state=0;
				menu1_state=0;
				tr_cls(0);
				break;
				
				case 2:
				days=type_days();
				uart_putint(days,10);
				_delay_ms(1000);
				
				months=type_months();
				uart_putint(months,10);
				_delay_ms(1000);
				
				year=type_year();
				uart_putint(year,10);
				_delay_ms(1000);
				
				set_rtc_date(&datetime,year,months,days);
				
				main_menu_state=0;
				menu1_state=0;
				tr_cls(0);
				break;
				
				case 3:
				menu1_state=0;
				main_menu_state=0;
				tr_cls(0);
				break;
				
				default:
				//tr_cls(0);
				menu1_state=0;
				break;
			}	
		
		break;
		
		case 2:
		show_state2_menu();
		if( main_menu2_callback ) main_menu2_callback();
		
		if(3==get_menu_position())
		{
			main_menu_state=0;
		}
		
		
		break;
		
		default:
		show_main_menu();
		main_menu_state = get_menu_position();		
		break;
	}
}

/**
* @brief Funkcja wyswietlajaca menu glowne.
* 
* @return VOID
*/
void show_main_menu()
{
	tr_locate(3,1);
	tr_cursor_hide(1);//??????????????????
	tr_attr(1,BLUE,BLACK);
	uart_puts("MAIN MENU:");
	tr_locate(4,1);
	uart_puts("1. Ustaw czas");
	tr_locate(5,1);
	uart_puts("2. Wyswietl poprzednie zdarzenia");
}

/**
* @brief Funkcja wyswietlajaca menu dotyczace ustawienia czasu i daty.
*
* @return VOID
*/
void show_state1_menu()
{
	//tr_cls(0);
	tr_locate(2,1);
	tr_cursor_hide(1);//???????????
	tr_attr(1,BLUE,BLACK);
	uart_puts("Ustaw date i czas\r\n");
	uart_puts("1. Ustaw czas\r\n");
	uart_puts("2. Ustaw date\r\n");
	uart_puts("Exit: E\r\n");
}

/**
* @brief Funkcja wyswietlajaca menu dotyczace wyswietlenia poprzednich zdarzen przekroczenia poziomu wody.
*
* @return VOID
*/
void show_state2_menu()
{
	//tr_cls(0);
	tr_locate(2,1);
	tr_cursor_hide(1);//???????????
	tr_attr(1,BLUE,BLACK);
	uart_puts("Wyswietl poprzednie zdarzenia\r\n");
	uart_puts("Exit: E\r\n");
}

/**
* @brief Funkcja odczytujaca wybor pozycji menu wpisana przez uzytkownika.
*
* @return Pozycja menu.
*/
int get_menu_position()
{
	char* bufor[3];
	int position;
	uart_get_str(bufor);
	//position=atoi(bufor);
	//uart_clear_RxBuffer();
	position = bufor[0];
	switch(position)
	{
		case '1':  
		tr_cls(0);
		return 1;
		
		case '2':
		tr_cls(0);
		return 2;
		
		case 'E':
		tr_cls(0);
		return 3;
		
		default:
		return 0;
	}
}

/**
* @brief Funkcja odpowiedzialna za odebranie danych dotyczacych godziny do ustawienia czasu w RTC.
*
* @return time Bajt z wartosci¹ oznaczajaca godzine wpisana przez uzytkownika.
*/
uint8_t type_hours()
{
	char bufor[3];
	uint8_t time;
	tr_cls(0);	
	tr_locate(1,1);
	tr_attr(1,BLUE,BLACK);
	uart_puts("Wpisz godziny");
	uart_puts(" hh\r\n");
	tr_cursor_hide(0);//????????????????
	uart_clear_RxBuffer();
	_delay_ms(1000);
	uart_get_str(bufor);
	_delay_ms(1000);
	time=atoi(bufor);
	uart_putint(time,10);
	if(time>24) return 0;
	return time;	
}

/**
* @brief Funkcja odpowiedzialna za odebranie danych dotycz¹cych minut do ustawienia czasu w RTC.
*
* @return time Bajt z wartoscia oznaczajaca wpisana minute przez uzytkownika.
*/
uint8_t type_minutes()
{
	char bufor[3];
	uint8_t time;
	tr_cls(0);
	tr_locate(1,1);
	tr_attr(1,BLUE,BLACK);
	uart_puts("Wpisz minuty");
	uart_puts(" mm\r\n");
	tr_cursor_hide(0);//????????????????
	uart_clear_RxBuffer();
	_delay_ms(1000);
	uart_get_str(bufor);
	_delay_ms(1000);
	time=atoi(bufor);
	uart_putint(time,10);
	if(time>60) return 0;
	return time;
}

/**
* @brief Funkcja odpowiedzialna za odebranie danych dotyczacych sekund do ustawienia czasu w RTC.
*
* @return time Bajt z wartosci¹ oznaczajaca sekundy wpisana przez uzytkownika.
*/
uint8_t type_seconds()
{
	char bufor[3];
	uint8_t time;
	tr_cls(0);
	tr_locate(1,1);
	tr_attr(1,BLUE,BLACK);
	uart_puts("Wpisz sekundy");
	uart_puts(" ss\r\n");
	tr_cursor_hide(0);//????????????????
	uart_clear_RxBuffer();
	_delay_ms(1000);
	uart_get_str(bufor);
	_delay_ms(1000);
	time=atoi(bufor);
	uart_putint(time,10);
	if(time>60) return 0;
	return time;
}

/**
* @brief Funkcja odpowiedzialna za odebranie danych dotyczacych dni do ustawienia daty w RTC.
*
* @return time Bajt z wartosci¹ oznaczajaca dzien miesiaca wpisana przez uzytkownika.
*/
uint8_t type_days()
{
	char bufor[3];
	uint8_t time;
	tr_cls(0);
	tr_locate(1,1);
	tr_attr(1,BLUE,BLACK);
	uart_puts("Wpisz dni");
	uart_puts(" DD\r\n");
	tr_cursor_hide(0);//????????????????
	uart_clear_RxBuffer();
	_delay_ms(2000);
	uart_get_str(bufor);
	_delay_ms(1000);
	time=atoi(bufor);
	uart_putint(time,10);
	if(time>31) return 1;
	return time;
}

/**
* @brief Funkcja odpowiedzialna za odebranie danych dotyczacych miesiecy do ustawienia daty w RTC.
*
* @return time Bajt z wartosci¹ oznaczajaca miesiac wpisana przez uzytkownika.
*/
uint8_t type_months()
{
	char bufor[3];
	uint8_t time;
	tr_cls(0);
	tr_locate(1,1);
	tr_attr(1,BLUE,BLACK);
	uart_puts("Wpisz miesiace");
	uart_puts(" MM\r\n");
	tr_cursor_hide(0);//????????????????
	uart_clear_RxBuffer();
	_delay_ms(2000);
	uart_get_str(bufor);
	_delay_ms(1000);
	time=atoi(bufor);
	uart_putint(time,10);
	if(time>12) return 1;
	return time;
}

/**
* @brief Funkcja odpowiedzialna za odebranie danych dotyczacych roku do ustawienia daty w RTC.
*
* @return time Dwa bajty z wartoscia oznaczajaca rok wpisana przez uzytkownika.
*/
uint16_t type_year()
{
	char bufor[6];
	uint16_t time;
	tr_cls(0);
	tr_locate(1,1);
	tr_attr(1,BLUE,BLACK);
	uart_puts("Wpisz rok");
	uart_puts(" YYYY\r\n");
	tr_cursor_hide(0);//????????????????
	uart_clear_RxBuffer();
	_delay_ms(3000);
	uart_get_str(bufor);
	_delay_ms(1000);
	time=atoi(bufor);
	uart_putint(time,10);
	if(time>2030) return 2000;
	return time;
}