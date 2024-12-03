/**
* @file common.c
* @author Hubert Szewczyk
* @date Wrzesieñ 19, 2019
* @brief Ten plik zawiera funkcje uniwersalne wykonujace rozne operacje.
*/

#include <avr/io.h>
#include "common.h"

/**
* @brief Konwersja liczby dziesietnej na BCD.
*
* Funkcja zamienia liczbe dziesietna mniejsza od 100 na liczbe w kodzie BCD.
*/
uint8_t dec2bcd(uint8_t dec) {
	return ((dec / 10)<<4) | (dec % 10);
}
/**
* @brief Konwersja liczby BCD na dziesietna.
*
* Funkcja zamienia liczbe 8 bitowa w kodzie BCD na liczbe dziesietna.
*/
uint8_t bcd2dec(uint8_t bcd) {
	return ((((bcd) >> 4) & 0x0F) * 10) + ((bcd) & 0x0F);
}

/**
* @brief Konwersja znaku ASCII na liczbe.
*
* @return liczba
*/
int ASCII_number(int ascii)
{
	if ((47<ascii)&(ascii<58))
	return 1;
	return 0;
}

/**
* @brief Konwersja tablicy znakow ASCII na liczbe.
*
* @return liczba
*/
int STRING_number(char* ascii, int size)
{
	int cnt = 0;
	int i;
	for (i=0; i<=size; i++)
	{
		if (!ASCII_number(ascii[i]))
		{
			cnt++;
		}
	}
	
	if(cnt) return 0;
}