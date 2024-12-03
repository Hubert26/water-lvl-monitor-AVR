/**
* @file menu.h
* @author Hubert Szewczyk
* @date Grudzien 27, 2019
* @brief To jest plik konfiguracyjny projektu.
*/ 


#ifndef MENU_H_
#define MENU_H_

int main_menu_state;
int menu1_state;

void MENU(void);
void show_main_menu();
void show_state1_menu();
void show_state2_menu();
int get_menu_position();
uint8_t type_hours();
uint8_t type_minutes();
uint8_t type_seconds();
uint8_t type_days();
uint8_t type_months();
uint16_t type_year();

void register_main_menu2_callback(void(*callback)(void));




#endif /* MENU_H_ */