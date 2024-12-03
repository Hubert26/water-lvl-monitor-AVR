/**
* @file i2c_twi.h
* @author Hubert Szewczyk
* @date Pazdziernik 30, 2019
* @brief Plik konfuguracyjny projektu.
*/
#ifndef I2C_TWI_H_
#define I2C_TWI_H_

#define ACK 1
#define NACK 0



// funkcje

void i2cSetBitrate(uint16_t bitrateKHz); //ustawia szybko�� transmisji

void TWI_start(void);//odblokowuje przerwanie TWI i interfejs TWI, wygenerowuje w trybie Master sekwencje startu transmisji
void TWI_stop(void);//odblokowuje przerwanie TWI i interfejs TWI, aktywuje wygenerowanie sekwencji STOP
void TWI_write(uint8_t bajt); //wysy�a bajt
uint8_t TWI_read(uint8_t ack); //odbiera bajt

void TWI_write_buf( uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf ); //wysy�� bufor o d�ugo�ci len
void TWI_read_buf(uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf); //odbiera bufor o d�ugo�ci len

#endif /* I2C_TWI_H_ */
