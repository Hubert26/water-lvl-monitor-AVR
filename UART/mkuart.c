/**
 * @file mkuart.c
 * @author Miroslaw Kardas
 * @date Wrzesien 4, 2010
 * @brief Plik ten odpowiedzialny jest za komunikacje poprzez interfejs UART. Dane wysylane sa w postaci znakow ASCII. 
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>

#include "mkuart.h"

/**
* @brief Licznik napisow w buforze odbiorczym.
*
*/
volatile uint8_t ascii_line;



/**
* @brief Bufor odbiorczy na dane z przesylane poprzez interfejs UART.
*
*/
volatile char UART_RxBuf[UART_RX_BUF_SIZE];

/**
* @brief Indeks bufora odbiorczego oznaczajacy poczatek danych.
*
* Indeks oznaczajacy „g³owe weza”.
*/
volatile uint8_t UART_RxHead;

/**
* @brief Indeks bufora odbiorczego oznaczajacy koniec danych.
*
* Indeks oznaczajacy „ogon weza”.
*/
volatile uint8_t UART_RxTail;



/**
* @brief Bufor nadawczy na dane z przesylane poprzez interfejs UART.
*
*/
volatile char UART_TxBuf[UART_TX_BUF_SIZE];

/**
* @brief Indeks bufora nadawczego oznaczajacy poczatek danych.
*
* Indeks oznaczaj¹cy „glowe weza”.
*/
volatile uint8_t UART_TxHead;

/**
* @brief Indeks bufora nadawczego oznaczajacy koniec danych.
*
* Indeks oznaczajacy „ogon weza”.
*/
volatile uint8_t UART_TxTail;

/**
* @brief Wsakznik do funkcji collback dla zdarzenia UART_RX_STR_EVENT().
*
*/
static void (*uart_rx_str_event_callback)(char * pBuf); //dziêki któremu bedziemy mogli dokonywaæ rejestracji w³asnej funkcji w programie, jak i wywo³ywaæ w³asn¹ funkcjê wewn¹trz zdarzenia

/**
* @brief Funkcja do rejestracji funkcji zwrotnej w zdarzeniu UART_RX_STR_EVENT()
* @param[in]  callback Wskaznik na funkcje collback ktorej parametrem jest wskaznik na zmienna typu char.
* @return VOID
*/
void register_uart_str_rx_event_callback(void (*callback)(char * pBuf)) {
	uart_rx_str_event_callback = callback;
}

/**
* @brief Zdarzenie do odbioru danych lancucha tekstowego z bufora cyklicznego.
* @param[in]  rbuf Wskaznik na pierwszy znak bufora odbiorczego.
* @return VOID
*/
void UART_RX_STR_EVENT(char * rbuf) {

	if( ascii_line ) { //dotar³a conajmniej jedna gotowa ramka/linia danych
		if( uart_rx_str_event_callback ) { //czy zarejestrowana jest w³asna funkcja u¿ytkownika do do analizy nadlatuj¹cych danych (badanie wskaŸnika do funkcji callback)
			uart_get_str( rbuf ); //zmniejszy o 1 ascii_line i jeœli zostanie wyzerowana tzn nie ma w buforze kolejnych danych i w kolejnym obiegu pêtli g³ównej zdarzenie nic nam nie zasygnalizuje
			(*uart_rx_str_event_callback)( rbuf );//uruchamiamy w³asn¹ funkcjê u¿ytkownika przekazuj¹c do niej odebrany ci¹g znaków do analizy
		} else {
			UART_RxHead = UART_RxTail;
		}
	}
}


/**
* @brief Funkcja inicjalizujaca interfejs UART.
* @param[in]  rbuf Predkoœæ transmisji.
* @return VOID
*/
void USART_Init( uint16_t baud ) {
	/* Ustawienie prêdkoœci */
	UBRR0H = (uint8_t)(baud>>8);
	UBRR0L = (uint8_t)baud;
	/* Za³¹czenie nadajnika I odbiornika */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Ustawienie format ramki: 8bitów danych, bez parzystoœci, 1 bit stopu */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
}

/**
* @brief Funkcja dodajaca jeden bajt do bufora nadawczego.
* @param[in]  data Bajt danych do wyslania.
* @return VOID
*/
void uart_putc( char data ) {
	uint8_t tmp_head;
	ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
		tmp_head  = (UART_TxHead + 1) & UART_TX_BUF_MASK;
	}
          // pêtla oczekuje je¿eli brak miejsca w buforze cyklicznym na kolejne znaki
    while ( tmp_head == UART_TxTail ){}

    UART_TxBuf[tmp_head] = data;
    UART_TxHead = tmp_head;

    // inicjalizujemy przerwanie wystêpuj¹ce, gdy bufor jest pusty, dziêki
    // czemu w dalszej czêœci wysy³aniem danych zajmie siê ju¿ procedura
    // obs³ugi przerwania
    UCSR0B |= (1<<UDRIE0);
}

/**
* @brief Funkcja dodajaca jeden lancuch znakow z pamieci RAM do bufora nadawczego.
* @param[in]  s Wskaznik na pierwszy znak napisu do wyslania.
* @return VOID
*/
void uart_puts(char *s)		// wysy³a ³añcuch z pamiêci RAM na UART
{
  register char c;
  while ((c = *s++)) uart_putc(c);			// dopóki nie napotkasz 0 wysy³aj znak
}

/**
* @brief Funkcja dodajaca jeden lancuch znakow z pamieci FLASH do bufora nadawczego.
* @param[in]  s Wskaznik na pierwszy znak napisu do wyslania.
* @return VOID
*/
void uart_puts_P(char *s)		// wysy³a ³añcuch z pamiêci FLASH na UART
{
	register char c;
	while (( c = pgm_read_byte(s++) )) uart_putc(c);			// dopóki nie napotkasz 0 wysy³aj znak
}

/**
* @brief Funkcja dodajaca liczbe jako znak typu ASCII do bufora nadawczego.
* @param[in]  value Liczba do wyslania.
* @param[in]  radix Postac w jakiej jest liczba.
* @return VOID
*/
void uart_putint(int value, int radix)	// wysy³a na port szeregowy tekst
{
	char string[17];			// bufor na wynik funkcji itoa
	itoa(value, string, radix);		// konwersja value na ASCII
	uart_puts(string);			// wyœlij string na port szeregowy
}

/**
* @brief Procedura obslugii przerwania nadawczego.
* Pobiera dane z bufora nadawczego i wysyla.
*/
ISR( USART0_UDRE_vect)  {
    // sprawdzamy czy indeksy s¹ ró¿ne
    if ( UART_TxHead != UART_TxTail ) {
    	// obliczamy i zapamiêtujemy nowy indeks ogona wê¿a (mo¿e siê zrównaæ z g³ow¹)
    	UART_TxTail = (UART_TxTail + 1) & UART_TX_BUF_MASK;
    	// zwracamy bajt pobrany z bufora  jako rezultat funkcji
#ifdef UART_DE_PORT
    	UART_DE_NADAWANIE;
#endif
    	UDR0 = UART_TxBuf[UART_TxTail];
    } else {
	// zerujemy flagê przerwania wystêpuj¹cego gdy bufor pusty
	UCSR0B &= ~(1<<UDRIE0);
    }
}

/**
* @brief Funkcja pobierajaca jeden bajt danych z bufora odbiorczego.
* @return data Bajt pobranych danych.
*/
int uart_getc(void) {
	int data = -1;
    // sprawdzamy czy indeksy s¹ równe
    if ( UART_RxHead == UART_RxTail ) return data;
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
        // obliczamy i zapamiêtujemy nowy indeks „ogona wê¿a” (mo¿e siê zrównaæ z g³ow¹)
        UART_RxTail = (UART_RxTail + 1) & UART_RX_BUF_MASK;
        // zwracamy bajt pobrany z bufora  jako rezultat funkcji
        data = UART_RxBuf[UART_RxTail];
    }
    return data;
}

/**
* @brief Funkcja pobierajaca jeden lancuch znakow z bufora odbiorczego.
* @param[in]  s Wskaznik na pierwszy znak bufora na odebrany napis.
* @return wsk Wskaznik na pierwszy znak odebranego napisu.
*/
char * uart_get_str(char * buf) {
	int c;
	char * wsk = buf;
	if( ascii_line ) {
		while( (c = uart_getc()) ) {
			if( 13 == c || c < 0) break; //uart_getc mo¿e zwróciæ -1 gdy dojdzie do zrównania g³owy z ogonem w razie gdyby nadpisano CR
			*buf++ = c;
		}
		*buf=0;//zamiana CR na 0 aby zakoñczy³o string
		ascii_line--;
	}
	return wsk;
}

/**
* @brief Procedura obslugii przerwania odbiorczego.
* Pobiera dane z rejestru i zapisuje do bufora odbiorczego.
*/
ISR( USART0_RX_vect ) {

    register uint8_t tmp_head;
    register char data;

    data = UDR0; //pobieramy natychmiast bajt danych z bufora sprzêtowego

    // obliczamy nowy indeks „g³owy wê¿a”
    tmp_head = ( UART_RxHead + 1) & UART_RX_BUF_MASK;

    // sprawdzamy, czy w¹¿ nie zacznie zjadaæ w³asnego ogona
    if ( tmp_head == UART_RxTail ) {
    	UART_RxHead = UART_RxTail; //Nadpisuje dane gdy nast¹pi przepe³nienie bufora
    } else {
    	switch( data ) {
    		case 0:					// ignorujemy bajt = 0
    		case 10: break;			// ignorujemy znak LF
    		case 13: ascii_line++;	// sygnalizujemy obecnoœæ kolejnej linii w buforze, znak CR potem trafia do bufora jak inne znaki
    		default : UART_RxHead = tmp_head; UART_RxBuf[tmp_head] = data;
    	}

    }
}

/**
* @brief Funkcja czyszczaca bufor odbiorczy.
* Funkcja zrownuje indeks konczacy i indeks rozpoczynajacy bufor odbiorczy.
*/
void uart_clear_RxBuffer(void)
{
	UART_RxHead = UART_RxTail;
}