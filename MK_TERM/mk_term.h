/**
* @file mk_term.h
* @author Miroslaw Kardas
* @date Kwiecien 30, 2013
* @brief Plik konfuguracyjny projektu.
*/

#ifndef MK_TERM_H_
#define MK_TERM_H_

// atrybuty znaku
//#define RESET		0
#define BOLD 		1
#define DIM			2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

// kolory czcionki lub t³a
#define BLACK 		0
#define RED			1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7


void tr_cls(uint8_t cur_onoff);						// kasuj ekran , w³/wy³ kursor
void tr_cursor_hide( uint8_t hide );				// cursor show=0 / hide=1
void tr_locate( uint8_t y, uint8_t x );				// ustaw wiersz i kolumnê
void tr_pen_color( uint8_t cl );					// ustaw kolor czcionki
void tr_brush_color( uint8_t cl );					// ustaw kolor t³a
void tr_attr( uint8_t atr, uint8_t fg, uint8_t bg );// ustaw atrybut: znaku oraz kolor czcionki i t³a

void fill_line( char ascii, uint8_t cnt );


#endif /* MK_TERM_H_ */
