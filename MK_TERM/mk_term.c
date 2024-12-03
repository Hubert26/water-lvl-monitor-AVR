/**
* @file mk_term.c
* @author Miroslaw Kardas
* @date Kwiecien 30, 2013
* @brief Plik zawierajacy funkcje konfigurujace terminal.
*/
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "../UART/mkuart.h"
#include "mk_term.h"

// aby mieæ polskie ogonki w PUTTY ----> ustawiæ WINDOW / Translation / Win1250

/**
* @brief Polecenie wyczyszczenia ekranu.
*
*/
const char UCLS[] PROGMEM = { "\x1b""[2J" };

/**
* @brief Polecenie ustawienia kursora w pozycji poczatkowej.
*
*/
const char UHOME[] PROGMEM = { "\x1b""[;H" };

/**
* @brief Polecenie ukrycia kursora.
*
*/
const char UCUR_HIDE[] PROGMEM = { "\x1b""[?25l" };
	
/**
* @brief Polecenie pokazania kursora.
*
*/
const char UCUR_SHOW[] PROGMEM = { "\x1b""[?25h" };
	
/**
* @brief Polecenie wylaczenia atrubutow.
*
*/
const char U_ATTR_OFF[] PROGMEM = { "\x1b""[m" };


/**
* @brief Funkcja pokazujaca lub ukrywajaca kursor.
* @param[in]  hide Paramert informujacy czy kursor ma byc ukryty.
* @return VOID
*/
void tr_cursor_hide( uint8_t hide ) {
	if(hide) uart_puts_P( (char*)UCUR_HIDE );
	else uart_puts_P( (char*)UCUR_SHOW );
}

/**
* @brief Funkcja kasujaca ekran oraz pokazujaca lub ukrywajaca kursor.
* @param[in]  cur_onoff Paramert informujacy czy kursor ma byc ukryty.
* @return VOID
*/
void tr_cls(uint8_t cur_onoff) {

	uart_puts_P( (char*)U_ATTR_OFF );
	tr_cursor_hide(cur_onoff);
	uart_puts_P( (char*)UCLS );

	uart_puts_P( (char*)UHOME );

}

/**
* @brief Funkcja wielokrotnie wysylajaca jeden znak poprzez interfejs UART.
* @param[in]  ascii Znak ASCII do wyslania.
* @param[in]  cnt Liczba znakow do wyslania.
* @return VOID
*/
void fill_line( char ascii, uint8_t cnt ) {
	for(uint8_t i=0; i<cnt; i++) uart_putc( ascii );
}

/**
* @brief Funkcja ustawiajaca atrybut.
* @param[in]  atr Atrybut znaku.
* @param[in]  fg Kolor czcionki.
 *@param[in]  bg Kolor tla.
* @return VOID
*/
void tr_attr( uint8_t atr, uint8_t fg, uint8_t bg ) {
	uart_putc( 0x1b );		// <ESC>[0;32;44m
	uart_putc( '[' );
	uart_putc( atr+'0' );
	uart_putc( ';' );
	uart_putc( '3' );
	uart_putc( fg+'0' );
	uart_putc( ';' );
	uart_putc( '4' );
	uart_putc( bg+'0' );
	uart_putc( 'm' );
}

/**
* @brief Funkcja ustawiajaca kolor czcionki.
* @param[in]  cl Kolor czcionki.
* @return VOID
*/
void tr_pen_color( uint8_t cl ) {
	uart_putc( 0x1b );		// <ESC>[34m
	uart_putc( '[' );
	uart_putc( '3' );
	uart_putc( cl+'0' );
	uart_putc( 'm' );
}

/**
* @brief Funkcja ustawiajaca kolor tla.
* @param[in]  cl Kolor tla.
* @return VOID
*/
void tr_brush_color( uint8_t cl ) {
	uart_putc( 0x1b );		// <ESC>[44m
	uart_putc( '[' );
	uart_putc( '4' );
	uart_putc( cl+'0' );
	uart_putc( 'm' );
}

/**
* @brief Funkcja ustawiajaca wiersz i kolumne.
* @param[in]  y Wiersz.
* @param[in]  x Kolumna.
* @return VOID
*/
void tr_locate( uint8_t y, uint8_t x ) {

	uart_putc( 0x1b );	// <ESC>[y;xH
	uart_putc( '[' );
	uart_putint( y, 10 );
	uart_putc( ';' );
	uart_putint( x, 10 );
	uart_putc( 'H' );
}


