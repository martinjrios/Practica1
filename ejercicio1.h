/*=============================================================================
 * Author: Martin Rios <jrios@fi.uba.ar>
 * Date: 2021/06/29
 * Version: 1.0
 *===========================================================================*/

#ifndef __EJERCICIO1_H__
#define __EJERCICIO1_H__

#define TICK_RATE		1  // ms

#define TICK_RATE_MIN	1  // ms
#define TICK_RATE_MAX	50 // ms

#define DELAY1			150  // ms
#define DELAY2			750  // ms

#define DEBOUNCE_TIME	100  // ms

#define KEY1			0
#define KEY2			1
#define KEY3			2
#define KEY4			3

#define LAST_KEY		4

typedef enum
{
	PRESSED,
	RELEASED,
}keyStatus_t;

typedef enum
{
	WAITING,
	DEBOUNCING,
}dbSt_t;


bool_t ledsOff();
bool_t ledOn(gpioMap_t led);
void blinkError();
bool_t readKey(uint8_t keyIndex);
bool_t ledSequenceOn(const gpioMap_t* _ledSequence, bool_t invert);

#endif /* __EJERCICIO1_H__ */
