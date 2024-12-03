/**
* @file water_sens.c
* @author Hubert Szewczyk
* @date Pazdziernik 27, 2019
* @brief Ten plik zawiera obsluge czujnikow poziomu cieczy oraz sterowania pompa.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "water_sens.h"
#include "../global.h"

/**
* @brief Ró¿ne stany flag sygnalizuj¹cych stan czujników
*
*/
enum 
{
	high, /**< Wysoki poziom wody dla czujnika */
	low, /**< Niski poziom wody dla czujnika */
	last_high, /**< Odczytany wysoki poziom wody dla czujnika */
	last_low /**< Odczytany niski poziom wody dla czujnika */
};
	
/**
* @brief WskaŸnik do funkcji callback dla zdarzenia SENS_EVENT().
*
*/
static void (*sens1_h_callback)( void);

/**
* @brief Funkcja do rejestracji funkcji zwrotnej w zdarzeniu SENS_EVENT()
* @param[in]  callback Wskaznik na funkcje collback.
* @return VOID
*/
void register_sens1_h_event_callback(void(*callback)(void)) {
	sens1_h_callback = callback;
}

/**
* @brief Wskaznik do funkcji callback dla zdarzenia SENS_EVENT().
*
*/
static void (*sens1_l_callback)( void);

/**
* @brief Funkcja do rejestracji funkcji zwrotnej w zdarzeniu SENS_EVENT()
* @param[in]  callback Wskaznik na funkcje collback.
* @return VOID
*/
void register_sens1_l_event_callback(void(*callback)(void)) {
	sens1_l_callback = callback;
}

/**
* @brief Wskaznik do funkcji callback dla zdarzenia SENS_EVENT().
*
*/
static void (*sens2_h_callback)( void);

/**
* @brief Funkcja do rejestracji funkcji zwrotnej w zdarzeniu SENS_EVENT()
* @param[in]  callback Wskaznik na funkcje collback.
* @return VOID
*/
void register_sens2_h_event_callback(void(*callback)(void)) {
	sens2_h_callback = callback;
}

/**
* @brief Wskaznik do funkcji callback dla zdarzenia SENS_EVENT().
*
*/
static void (*sens2_l_callback)( void);

/**
* @brief Funkcja do rejestracji funkcji zwrotnej w zdarzeniu SENS_EVENT()
* @param[in]  callback Wskaznik na funkcje collback.
* @return VOID
*/
void register_sens2_l_event_callback(void(*callback)(void)) {
	sens2_l_callback = callback;
}

/**
* @brief Funkcja inicjalizujaca LED-y sygnalizujace poziom wody.
* Funkcja ustawia wyjscia mikrokontrolera do ktorych sa podlaczone LED-y.
* @return VOID
*/
void LED_init()
{
	SET_OUTPUT(LED_YELLOW);
	SET_OUTPUT(LED_GREEN);
}

/*void WaterSensors_init()
{
	DDRB &= ~(PCINT_CZ1 | PCINT_CZ2);//wejœcia
	PORTB|= (PCINT_CZ1 | PCINT_CZ2); //podciaganie do VCC
	
	PCICR |= 1<<PCIE0; //zezwolenie na przerwanie PCIE0 (PCINT7-0)
	PCMSK0 |= (1<<PCINT6) | (1<<PCINT4); //przerwanie PCINT na pinie PB6 i PB4
}*/

/**
* @brief Funkcja inicjalizujaca czujniki poziomu wody.
* Funkcja ustawia wyjscia mikrokontrolera do ktorych sa podlaczone czujniki poziomu wody.
* Funkcja ta inicjalizuje timer odliczajacy 10ms.
* @return VOID
*/
void WaterSensors_init()
{
	//DDRB &= ~(PCINT_CZ1 | PCINT_CZ2);//wejœcia
	SET_INPUT(SENSOR1);
	SET_INPUT(SENSOR2);
	
	//PORTB|= (PCINT_CZ1 | PCINT_CZ2); //podciaganie do VCC
	SET(SENSOR1);
	SET(SENSOR2);
	
	TCCR2A |= (1<<WGM21); //tryb CTC
	TCCR2B |= (1<<CS22)|(1<<CS20); //preskaler 1024
	OCR2A = 156;//10ms
	TIMSK2 |= (1<<OCIE2A); //zezw na przerwanie CM
}

/**
* @brief Funkcja inicjalizujaca sterownik pompy cieczy.
* Funkcja ta ustawia piny sterujace L293D jako wyjscia.
* @return VOID
*/
void L293D_init()
{
	 /* ustawiamy piny steruj¹ce L293D jako wyjœcia */
	SET_OUTPUT(WE_A);
	SET_OUTPUT(WE_B);
	DC_STOP;
}

/**
* @brief Funkcja wlaczaj¹ca pompe cieczy.
* Funkcja ta ustawia piny sterujace L293D.
* @return VOID
*/
void waterpump_ON(void)
{
	DC_PRAWO;
}

/**
* @brief Funkcja wylaczajaca pompe cieczy.
* Funkcja ta ustawia piny sterujace L293D.
* @return VOID
*/
void waterpump_OFF(void)
{
	DC_STOP;
}

/**
* @brief Funkcja sprawdzaj¹ca czy nast¹pilo zdarzenie od czujnikow poziomu cieczy. 
* Funkcja ta sprawdza jakie zdarzenie nast¹pi³o i wywoluje funkcje collback.
* @return VOID
*/
void SENSORS_EVENT( void )  
{ 
	if(SENSOR1_flag==high)
	{
		 SENSOR1_flag=last_high;
		 if( sens1_h_callback ) sens1_h_callback();
	}
	if(SENSOR1_flag==low)
	{
		SENSOR1_flag=last_low;
		if( sens1_l_callback ) sens1_l_callback();
	}
	if(SENSOR2_flag==high) 
	{
		SENSOR2_flag=last_high;
		if( sens2_h_callback ) sens2_h_callback();
	}
	if(SENSOR2_flag==low) 
	{
		SENSOR2_flag=last_low;
		if( sens2_l_callback ) sens2_l_callback();
	}
	//TIMER0A_flag=0;
}


	
/**
* @brief Procedura obs³ugii przerwania compare match od TIMERA2.
* Zlicza ile czasu jest wlaczony czujnik poziomu wody i ustawia flagi  SENSOR1_flag, SENSOR2_flag.
*/
ISR(TIMER2_COMPA_vect){ //ISR(PCINT0_vect){
	static uint8_t CNT_SENSOR1_ON;
	static uint8_t CNT_SENSOR1_OFF;
	
	static uint8_t CNT_SENSOR2_ON;
	static uint8_t CNT_SENSOR2_OFF;
	
	if ( IS_SET(SENSOR1) )
	{
		CNT_SENSOR1_ON++;
		if(CNT_SENSOR1_ON>=100)
		{	
			CNT_SENSOR1_ON=0;
			CNT_SENSOR1_OFF=0;
			
			if(SENSOR1_flag==last_low)
			SENSOR1_flag=high;			
		}
	}
	else if(!IS_SET(SENSOR1))
	{
		CNT_SENSOR1_OFF++;
		if(CNT_SENSOR1_OFF>=100)
		{
			CNT_SENSOR1_ON=0;
			CNT_SENSOR1_OFF=0;
			
			if(SENSOR1_flag==last_high)
			SENSOR1_flag=low;
		}
	}
	
	
	
	if ( IS_SET(SENSOR2) )
	{
		CNT_SENSOR2_ON++;
		if(CNT_SENSOR2_ON>=100)
		{
			CNT_SENSOR2_ON=0;
			CNT_SENSOR2_OFF=0;
			
			if(SENSOR2_flag==last_low)
			SENSOR2_flag=high;
		}
	}
	else if(!IS_SET(SENSOR2))
	{
		CNT_SENSOR2_OFF++;
		if(CNT_SENSOR2_OFF>=100)
		{
			CNT_SENSOR2_ON=0;
			CNT_SENSOR2_OFF=0;
			
			if(SENSOR2_flag==last_high)
			SENSOR2_flag=low;
		}
	}

}