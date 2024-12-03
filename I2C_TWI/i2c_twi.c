/**
* @file i2c_twi.c
* @author Hubert Szewczyk
* @date Pazdziernik 30, 2019
* @brief Ten plik zawiera funkcje obslugujace komunikacje I2C.
*/
#include <avr/io.h>
#include "i2c_twi.h"

/**
* @brief Funkcja ustawia szybkosc transmisji interfejsu I2C.
* @param[in]  bitrateKHz Szybkosc transmisji w kHz.
* @return VOID
*/
void i2cSetBitrate(uint16_t bitrateKHz) {
	uint8_t bitrate_div;

	bitrate_div = ((F_CPU/1000l)/bitrateKHz);
	if(bitrate_div >= 16)
		bitrate_div = (bitrate_div-16)/2;

	TWBR = bitrate_div;
}

/**
* @brief Funkcja odblokowujaca transmisje poprzez interfejs I2C.
* Generuje sekwencje startowa w trybie Master.
* @return VOID
*/
void TWI_start(void) {
	 //odblokowujemy flage TWINT (ust bitu na 1) aby odblokowaæ przerwanie TWI
	 //ustawiaj¹c TWEN odblokowujemy interfejs TWI
	 //ustawiaj¹c TWSTA aktywujemy wygenerowanie w trybie Master sekwencji startu transmisji
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
	while (!(TWCR&(1<<TWINT))); //czekamy az zostanie ustawiona flaga TWINT co oznacza ¿e uk³¹d wykona³ prawid³ow¹ sekwencje startu
}

/**
* @brief Funkcja odblokowujaca transmisje poprzez interfejs I2C.
* Generuje sekwencje stop..
* @return VOID
*/
void TWI_stop(void) {
	 //odblokowujemy flage TWINT (ust bitu na 1) aby odblokowaæ przerwanie TWI
	 //ustawiaj¹c TWEN odblokowujemy interfejs TWI
	 //ustawiaj¹c TWSTA aktywujemy wygenerowanie sekwencji STOP
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while ( (TWCR&(1<<TWSTO)) ); //czekamy na wyzerowanie bitu TWSTO -> prawid³owe zakoñczenie tej operacji
}

/**
* @brief Funkcja wysylajaca jeden bajt poprzez interfejs I2C.
* @param[in]  bajt Wysylany bajt.
* @return VOID
*/
void TWI_write(uint8_t bajt) {
	TWDR = bajt; //zapis bajt do nadania do rejestru nadawczego
	TWCR = (1<<TWINT)|(1<<TWEN); //odblokowujemy przerwanie TWI oraz w³¹czamy modu³
	while ( !(TWCR&(1<<TWINT))); //prawid³owe zakoñczenie tej operacji zaostanie zg³oszone ustawieniem flagi TWINT
}

/**
* @brief Funkcja odbierajaca jeden bajt poprzez interfejs I2C.
* @param[in]  ack Zmienna oznazajaca czy generowac ACK czy NACK.
* @return Wartosc rejestru TWDR
*/
uint8_t TWI_read(uint8_t ack) {
	TWCR = (1<<TWINT)|(ack<<TWEA)|(1<<TWEN); //TWEA -> generowanie ACK
	while ( !(TWCR & (1<<TWINT)));
	return TWDR;
}


/**
* @brief Funkcja wysylajaca zawartosc bufora poprzez interfejs I2C.
* @param[in]  SLA Adres ukladu slave.
* @param[in]  adr Adres pamieci RAM ukladu slave do ktorego chcemy wyslac pierwszy bajt bufora.
* @param[in]  len Dlugosc bufora.
* @param[in]  buf Wskaznik na pierwszy element bufora.
* @return VOID
*/
void TWI_write_buf( uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf ) {

	TWI_start();
	TWI_write(SLA);//wysy³amy bajt kontrolny zawieraj¹cy adres uk³adu SLAVE
	TWI_write(adr);//adres pamiêci do którego chcemy rozpocz¹æ zapis
	while (len--) TWI_write(*buf++); //wysy³anie danych
	TWI_stop();//sygna³ I2C STOP
}


/**
* @brief Funkcja odczytujaca zawartosc pamieci RAM ukladu slave poprzez interfejs I2C.
* @param[in]  SLA Adres ukladu slave.
* @param[in]  adr Adres pamieci RAM ukladu slave z ktorego chcemy odczytac pierwszy bajt bufora.
* @param[in]  len Ilosc bajtow ktore chcemy odczytac.
* @param[in]  buf Wskaznik na pierwszy element bufora do ktorego zapisywane sa dane.
* @return VOID
*/
void TWI_read_buf(uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf) {

	TWI_start();
	TWI_write(SLA);
	TWI_write(adr);
	TWI_start();//ponowny start aby prze³¹czyæ SLAVE na tryb odczytu tak aby to on móg³ wysy³aæ dane do uk³adu MASTER
	TWI_write(SLA + 1);//do odczytu adres trzeba zwiêkszyæ o 1
	while (len--) *buf++ = TWI_read( len ? ACK : NACK );
	TWI_stop();
}
