/**
* @file main.c
* @author Hubert Szewczyk
* @date Styczen 26, 2020
* @brief Glowny plik projektu.
*
* \mainpage PMIK
* Projekt urzadzenia kontrolujacego minimalny i maksymalny poziom wody w zbiorniku na wode lub malych zbiornikach wodnych np. oczko wodne
* \section realizowane_funkcje Realizowane funkcje.
* Sygnalizacja przekroczenia poziomu wody (poziom minimalny i maksymalny jest ustanowiony przez uzytkownika odpowiednio mocujacego 2 czujniki poziomu wody).
* Przesylanie ostrzezenia do uzytkownika za pomoca USB (wykorzystana przejsciowka UART-USB) wraz z data i godzina oraz mozliwosc odczytania daty poprzednich zdarzen.
* Zapis na karte SD daty i godziny zdarzenia przekroczenia poziomu wody za pomoca SPI.
* Automatyczne dopompowanie wody gdy zostanie przekroczony minimalny poziom wody z zbiorniku.
* \section liblary_sec Biblioteki uzyte w projekcie.
* \subsection liblary1 FatFS
* Biblioteka ta zawiera uniwersalne funkcje do obslugi plikow FAT.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "common/common.h"
#include "DS1307/ds1307.h"
#include "I2C_TWI/i2c_twi.h"
#include "UART/mkuart.h"
#include "MK_TERM/mk_term.h"
#include "SENSORS/water_sens.h"
#include "FatFS/diskio.h"
#include "FatFS/ff.h"
#include "MENU/menu.h"
#include "global.h"


/**
* @brief Bufor na tekst odczytany z pliku PMIK.txt
*/
char* buffor_from_file[200];

/**
* @brief Zmienna przechowujaca przechowujaca czas zgaszenia diody zielonej.
*/
TDATETIME event_datetime;


static FIL Installation_file;
static FATFS fatfs_volume;
/**
* @brief Zmienna przechowujaca ilosc zapisywanych danych do pliku PMIK.txt.
*/
uint8_t bytes_written;
/**
* @brief Zmienna przechowujaca wartosc bledu zwiazanego z plikiem zapisanym na karcie SD.
*/
uint8_t error;

/**
* @brief Funkcja pobierajaca date i czas na potrzeby bibloteki FasFS
* 
* @return DWORD
*/
DWORD get_fattime (void)
{
	get_rtc_datetime(&datetime);
	
	return	  ((DWORD)(datetime.YY - 1980) << 25)
	| ((DWORD)datetime.MM << 21)
	| ((DWORD)datetime.DD << 16)
	| ((DWORD)datetime.hh << 11)
	| ((DWORD)datetime.mm << 5)
	| ((DWORD)datetime.ss >> 1);
	
	/*return	  ((DWORD)(2020 - 1980) << 25)
	| ((DWORD)6 << 21)
	| ((DWORD)6 << 16)
	| ((DWORD)6 << 11)
	| ((DWORD)6 << 5)
	| ((DWORD)6 >> 1);*/
}

void zapal_yellow_LED(void);
void zgas_yellow_LED(void);
void zapal_green_LED(void);
void zgas_green_LED(void);

void wyswietl_dane(void);


void pokazuj_date_czas( TDATETIME *dt);

/**
* @brief Funkcja glowna projektu.
*
* @return int
*/
int main(void)
{
	//*****DIODA LED
	SET_OUTPUT(LED_DEBUG);
	LED_init();
 
 	USART_Init(__UBRR);
	
	WaterSensors_init();
	L293D_init();

	register_sens1_h_event_callback(zapal_yellow_LED);
	register_sens1_l_event_callback(zgas_yellow_LED);
	register_sens2_h_event_callback(zapal_green_LED);
	register_sens2_l_event_callback(zgas_green_LED);
	
	register_main_menu2_callback(wyswietl_dane);
	
	init_rtc();
	register_rtc_event_callback( pokazuj_date_czas );
	
	//set_rtc_time(&datetime,23,59,55);
	set_rtc_datetime( &datetime, 2019, 12, 31, 23, 59, 55 );

	
	error=disk_initialize (0);
	
	
	error=f_mount (&fatfs_volume,"/",1);
	
	
	
	if((f_open(&Installation_file,"/PMIK.TXT",FA_OPEN_APPEND | FA_WRITE))==FR_OK)
	{
	f_puts("TEST\r\n", &Installation_file);
	//_delay_us(1000);
	f_close(&Installation_file);
	}
	
	
	//tr_cls(0);
	main_menu_state=0;
	menu1_state=0;
	RESET(LED_DEBUG);
	
	sei();
    while (1) 
    {
	
		
		RTC_EVENT();
		SENSORS_EVENT();
		MENU();
				
    }

}
	

/**
* @brief Funkcja wyswietlajaca aktualna date i aktualny czas.
* @param[in]  dt Wskaznik na struktore TDATETIME
* @return VOID
*/
void pokazuj_date_czas( TDATETIME *dt) {
	#if DATETIME_AS_STRING == 1
	//tr_cls(0);
	tr_cursor_hide(1);
	tr_locate(1,1);
	tr_attr(1,YELLOW,BLACK);
	uart_puts( dt->time );
	tr_locate(1,10);
	uart_puts( dt->date );
	//tr_locate(5,8);	
	//uart_putint(dt->weekday,10);
	//tr_locate(7,3);
	//uart_puts_P((char*)&dni[dt->weekday * 4]);//co 4 bajty w tablicy
	#endif

}

/**
* @brief Funkcja zapalajaca diode zolta i zapisujaca zdarzenie na karcie SD
* @return VOID
*/
void zapal_yellow_LED(void)
{
	SET(LED_YELLOW);
	//TOGGLE(LED_DEBUG);
	get_rtc_datetime(&event_datetime);
	
	if((f_open(&Installation_file,"/PMIK.TXT",FA_OPEN_APPEND | FA_WRITE))==FR_OK)
	{
		f_write(&Installation_file,event_datetime.date, sizeof(event_datetime.date) ,&bytes_written);
		f_close(&Installation_file);
	}
	
	if((f_open(&Installation_file,"/PMIK.TXT",FA_OPEN_APPEND | FA_WRITE))==FR_OK)
	{
		f_write(&Installation_file,event_datetime.time, sizeof(event_datetime.time) ,&bytes_written);
		f_close(&Installation_file);
	}
	
	if((f_open(&Installation_file,"/PMIK.TXT",FA_OPEN_APPEND | FA_WRITE))==FR_OK)
	{
		f_write(&Installation_file,"\n", sizeof("\n") ,&bytes_written);
		f_close(&Installation_file);
	}
};

/**
* @brief Funkcja gaszaca diode zolta.
* @return VOID
*/
void zgas_yellow_LED(void)
{
	RESET(LED_YELLOW);
};

/**
* @brief Funkcja zapalajaca diode zielona i wylaczajaca pompe.
* @return VOID
*/
void zapal_green_LED(void)
{
	SET(LED_GREEN);
		
	/*if((f_open(&Installation_file,"/PMIK.TXT",FA_OPEN_APPEND | FA_WRITE))==FR_OK)
	{
		f_write(&Installation_file,"TEST3\r\n", sizeof("TEST3\r\n") ,&bytes_written);
		f_close(&Installation_file);
	}*/
	waterpump_OFF();
};

/**
* @brief Funkcja gaszaca diode zielona i wlaczajaca pompe.
* @return VOID
*/
void zgas_green_LED(void)
{
	RESET(LED_GREEN);
	get_rtc_datetime(&event_datetime);
	//pokazuj_date_czas(&event_datetime);
	waterpump_ON();
};

/**
* @brief Funkcja wyswietlajaca tekst zapisany w pliku PMIK.txt na karcie SD.
* @return VOID
*/
void wyswietl_dane(void)
{
	if((f_open(&Installation_file,"/PMIK.TXT",FA_OPEN_EXISTING | FA_READ))==FR_OK)
	{
		
		while(!f_eof(&Installation_file))
		if(f_gets(&buffor_from_file,sizeof(buffor_from_file),&Installation_file)>0)
		{
			tr_cursor_hide(1);
			uart_puts(buffor_from_file);
			
		}
		
		f_close(&Installation_file);
	}
}