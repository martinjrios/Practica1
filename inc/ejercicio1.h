/*=============================================================================
 * Authors: Martin Rios <jrios@fi.uba.ar> - Lucas Zalazar <lucas.zalazar6@gmail.com>
 * Date: 2021/07/05
 * Version: 1.0
 *===========================================================================*/

#ifndef __EJERCICIO1_H__
#define __EJERCICIO1_H__

//#define OPCIONAL_3	// Descomentar para correr el punto opcional 3
#define OPCIONAL_4

#define TICK_RATE		1 // Default 1ms

#define TICK_RATE_MIN	1  // ms
#define TICK_RATE_MAX	50 // ms

// TIEMPOS DE SECUENCIA (ms)
#define DELAY150		150
#define DELAY750		750

// TIEMPOS DE REBOTE (ms)
#define DEBOUNCE_TIME	100

// TIEMPOS DE ERROR (ms)
#define ERROR_OFF		500
#define ERROR_ON		1000
#define ERROR_SEQ		2000
#define ERROR_TIME		4000

// TECLAS
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

#ifdef OPCIONAL_4
/* Estructura para controlar la secuencia de LEDs */
typedef struct{
	const gpioMap_t *ledSequence;	// Puntero a la secuencia de LEDs que se quiere activar
	uint8_t lastLed;				// Cantidad de elementos en la secuencia de LEDs
	uint8_t ledIndex;				// Posicion en la secuencia del LED que esta siendo activado
	bool_t inverted;				// Sentido de la secuencia
} controlSequence_t;
#endif

bool_t ledsOff ( const gpioMap_t* _ledSequence, const uint8_t _sequenceLength );
bool_t ledOn ( gpioMap_t led );
void blinkError ( tick_t delayError );
bool_t readKey ( const gpioMap_t *_keyArray, uint8_t keyIndex );
#ifdef OPCIONAL_3
bool_t ledSequenceOn ( const gpioMap_t* _ledSequence, const uint8_t _sequenceLength, bool_t invert );
#endif
#ifdef OPCIONAL_4
bool_t ledSequenceOn(controlSequence_t *controlSequence);
#endif

#endif /* __EJERCICIO1_H__ */
