/**
* @file water_sens.h
* @author Hubert Szewczyk
* @date PaŸdziernik 27, 2019
* @brief Plik konfuguracyjny projektu.
*/

#ifndef WATER_SENS_H_
#define WATER_SENS_H_


#define LED_YELLOW E,5//3
#define LED_GREEN E,4//2


#define SENSOR1 B,6//12
#define SENSOR2 B,4//10

#define WE_A J,1//14
#define  WE_B J,0//15

//definicje poleceñ steruj¹cych prac¹ silnika
#define DC_LEWO RESET(WE_A); SET(WE_B)
#define DC_PRAWO SET(WE_A); RESET(WE_B)
#define DC_STOP RESET(WE_A); RESET(WE_B)

/**
* @brief Flaga sygnalizujaca stan czujnika pierwszego.
*
* Flaga przybiera rozne stany w zaleznosci od poziomu wody.
*/
volatile uint8_t SENSOR1_flag;

/**
* @brief Flaga sygnalizujaca stan czujnika drugiego.
*
* Flaga przybiera rozne stany w zaleznosci od poziomu wody.
*/
volatile uint8_t SENSOR2_flag;

void register_sens1_h_event_callback(void(*callback)(void));
void register_sens1_l_event_callback(void(*callback)(void));
void register_sens2_h_event_callback(void(*callback)(void));
void register_sens2_l_event_callback(void(*callback)(void));

void LED_init();
void WaterSensors_init();
void L293D_init();

void SENSORS_EVENT( void );
void waterpump_ON(void);
void waterpump_OFF(void);

#endif /* WATER_SENSOR_H_ */