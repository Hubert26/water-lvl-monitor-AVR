/**
* @file common.h
* @author Hubert Szewczyk
* @date Wrzesieñ 19, 2019
* @brief Ten plik jest plikiem konfiguracyjnym projektu.
*/


#ifndef COMMON_H_
#define COMMON_H_

#define LED_DEBUG B,7

//int8_t debug_text[256];

uint8_t dec2bcd(uint8_t dec);
uint8_t bcd2dec(uint8_t bcd);
int ASCII_number(int ascii);
int STRING_number(char* ascii, int size);


#endif /* COMMON_H_ */