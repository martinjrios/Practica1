/*=============================================================================
 * Ejercicio 1
 * Author: Martin Rios <jrios@fi.uba.ar>
 * Date: 2021/06/29
 * Version: 1.0
 *===========================================================================*/

#include "sapi.h"
#include "ejercicio1.h"

typedef struct{
	const gpioMap_t * ptrLed;
	const gpioMap_t const* ptrPrimerLed;
	const uint8_t const* ptrUltimoLed;
} controlSecuencia_t;

/*Arreglo del tipo gpioMap_t para la secuencia de leds */
const gpioMap_t ledSequence[] = { LEDB, LED1, LED2, LED3 };

/* Cantidad de elementos del arreglo ledSequence */
const uint8_t lastLed = sizeof ( ledSequence ) / sizeof ( gpioMap_t );

/*Arreglo del tipo gpioMap_t para teclas */
const gpioMap_t keyArray[] = { TEC1, TEC2, TEC3, TEC4 };

/* FUNCION PRINCIPAL */
int main( void )
{

	// INICIALIZAR Y CONFIGURAR PLATAFORMA
    boardInit();

    // DEFINICION DE VARIABLES
    /* Variables del tipo tick para retardos */
    delay_t ledDelay;
    /* Varibles del tipo booleano */
    bool_t invertSequence = false;

    /* Verificación TICK_RATE rango permitido de tiempo: 1 a 50 ms */
    if ( ( TICK_RATE < TICK_RATE_MIN ) || ( TICK_RATE > TICK_RATE_MAX ) )  blinkError( ERROR_TIME );
    if ( !tickConfig ( TICK_RATE ) )  blinkError( ERROR_TIME );

    /*Configuración retardo no bloqueante 150 ms */
    delayConfig ( &ledDelay, DELAY150 );

   // ----- Repeat for ever -------------------------
    while( true ) {

    	/* Lectura de pulsadores de secuencia */
    	if(readKey(KEY1)) invertSequence = false;
    	else if(readKey(KEY4)) invertSequence = true;

    	/* Lectura de pulsadores de retardo no bloqueante */
    	if(readKey(KEY2)) delayWrite(&ledDelay, DELAY150);
    	else if(readKey(KEY3)) delayWrite(&ledDelay, DELAY750);

    	/* Captura de errores de secuencia */
    	if(delayRead(&ledDelay)){
    		if(!ledSequenceOn(ledSequence, invertSequence)) blinkError ( ERROR_SEQ );
    	}
    }

    return 0;
}

// FUNCIÓN DE APAGADO DE LEDS
bool_t ledsOff ( )
{
	int8_t i = 0;

	for ( i = 0; i < lastLed; i++ ) {
		/* Se apaga cada led disponible en la placa */
		if ( !gpioWrite ( ledSequence[i], OFF ) ) return FALSE;
		/* Se verifica el apagado de leds, leyendo el estado GPIO de cada leds */
		if ( gpioRead ( ledSequence[i] ) == ON ) return FALSE;
	}

	return TRUE;
}

// FUNCIÓN DE ENCENDIDO DE LEDS
bool_t ledOn ( gpioMap_t led )
{
	/* Se enciende led pasado por argumento */
	if ( !gpioWrite ( led, ON ) ) return FALSE;
	/* Se verifica el apagado del led, leyendo el estado GPIO */
	if ( gpioRead ( led ) == OFF ) return FALSE;
	else return TRUE;
}

// FUNCIÓN DE SECUENCIA DE LEDS
bool_t ledSequenceOn ( const gpioMap_t* _ledSequence, bool_t invert )
{
	static int8_t ledIndex = 0;

	/* Secuencia 2: LED3 -> LED2 -> LED1 -> LEDA -> LED3 -> ... */
	if ( invert ) ledIndex--;
	/* Secuencia 1: LEDA -> LED1 -> LED2 -> LED3 -> LEDA -> ... */
	else ledIndex++;

	/* ledIndex no debe exceder limite superior lastLed */
	if ( ledIndex >= lastLed ) ledIndex = 0;
	/* ledIndex no debe exceder limite inferior 0 */
	else if ( ledIndex < 0 ) ledIndex = lastLed - 1;

	/* Función de apagado de leds y captura de errores de apagado */
	if ( !ledsOff() ) blinkError ( ERROR_OFF );

	/* Funcion de encendido de leds y captura de errores de encendido */
	if ( !ledOn ( _ledSequence[ledIndex] ) ) blinkError ( ERROR_ON );

	return TRUE;
}

// FUNCIÓN INDICACIÓN DE ERROR
// SE PASA POR ARGUMENTO DIFERENTES TIEMPOS SEGUN TIPO DE ERROR
void blinkError ( tick_t delayError )
{
	/* El programa se queda aca PARA SIEMPRE */
	while ( true ) {
		/* Se apagan todos los leds */
		ledsOff ( );
		/* Parpadeo de led Rojo */
		gpioToggle ( LEDR );
		/* Retardo bloqueante segun tipo de error */
		delay ( delayError );
	}
}

//FUNCION ANTIREBOTE PARA LECTURA DE TECLAS
bool_t readKey ( uint8_t keyIndex )
{
	//DEFINICION DE VARIABLES LOCALES
	/* */
	static dbSt_t debounceState[LAST_KEY] = {WAITING, WAITING, WAITING, WAITING};
	/* */
	static delay_t debounceDelay[LAST_KEY];
	/* */
	bool_t keyPressed = false;

	/* Cambio del estado de las teclas */
	switch(debounceState[keyIndex]){

	/* WAITING: */
	/* Si fue presionada alguna tecla, pasa al estado de DEBOUNCING */
	case WAITING:
		/* Se consulta por la tecla presionada */
		if(gpioRead(keyArray[keyIndex]) == PRESSED){
			/* Cambio de estado de tecla */
			debounceState[keyIndex] = DEBOUNCING;
			/* Se configura tiempo de antirrebote */
			delayInit(&debounceDelay[keyIndex], DEBOUNCE_TIME);
		}
		break;

	/* DEBOUNCING: */
	/* Comfirma si se presiono una tecla, vuelve al estado de WAITING */
	case DEBOUNCING:
		/* Se consulta si se cumplio el tiempo de antirrebote */
		if(delayRead(&debounceDelay[keyIndex])){
			/* Se pregunta por la tecla presionada */
			if(gpioRead(keyArray[keyIndex]) == PRESSED) keyPressed = TRUE;
			/* Cambio de estado de tecla */
			debounceState[keyIndex] = WAITING;
		}
		break;
	}

	return keyPressed;
}
