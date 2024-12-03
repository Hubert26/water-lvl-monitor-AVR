/**
* @file global.h
* @author Hubert Szewczyk
* @date Listopad 19, 2019
* @brief Plik konfuguracyjny projektu.
*/


#ifndef GLOBAL_H_
#define GLOBAL_H_

// ----------------------------------------------------------------------------

#define	true	1
#define	false	0

#define	True	1
#define	False	0
//makra upraszczaj¹ce dostêp do portów
//****PORT
#define PORT(x) SPORT(x)
#define SPORT(x) (PORT##x)
//****PIN
#define PIN(x) SPIN(x)
#define SPIN(x) (PIN##x)
//****DDR
#define DDR(x) SDDR(x)
#define SDDR(x) (DDR##x)

#define	RESET(x)		_XRS(x)
#define	SET(x)			_XS(x)
#define	TOGGLE(x)		_XT(x)

#define	SET_OUTPUT(x)	_XSO(x)
#define	SET_INPUT(x)	_XSI(x)
#define	IS_SET(x)		_XR(x)

#define	_XRS(x,y)	PORT(x) &= ~(1<<y)
#define	_XS(x,y)	PORT(x) |= (1<<y)
#define	_XT(x,y)	PORT(x) ^= (1<<y)

#define	_XSO(x,y)	DDR(x) |= (1<<y)
#define	_XSI(x,y)	DDR(x) &= ~(1<<y)
#define	_XR(x,y)	((PIN(x) & (1<<y)) != 0)



#endif /* GLOBAL_H_ */