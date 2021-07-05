/*=============================================================================
 * Ejercicio 1
 * Authors: Martin Rios <jrios@fi.uba.ar> - Lucas Zalazar <lucas.zalazar6@gmail.com>
 * Date: 2021/07/05
 * Version: 1.0
 *===========================================================================*/

#include "sapi.h"
#include "ejercicio1.h"

/* FUNCION PRINCIPAL */
int main( void )
{

	// INICIALIZAR Y CONFIGURAR PLATAFORMA
    boardInit();

    // DEFINICION DE VARIABLES
#ifdef OPCIONAL_3
    /*Arreglo del tipo gpioMap_t para la secuencia de leds */
    const gpioMap_t ledSequence[] = { LEDB, LED1, LED2, LED3 };
    /* Cantidad de elementos en la secuencia de LEDs*/
    const uint8_t lastLed = sizeof ( ledSequence ) / sizeof ( gpioMap_t );

    /* Varibles del tipo booleano */
    bool_t invertSequence = false;
#endif

#ifdef OPCIONAL_4
    // Se declara la estructura para el control de la secuencia con el LED RGB
    controlSequence_t controlSequenceRGB = {
    	NULL,
		0,
		0,
		false,
    };
    // Se inicializan los parametros de la estructura
    const gpioMap_t sequence1[] = {LEDR, LEDG, LEDB};
    controlSequenceRGB.ledSequence = sequence1;
    controlSequenceRGB.lastLed = sizeof ( sequence1 ) / sizeof ( gpioMap_t );

    // Se declara la estructura para el control de la secuencia de los LEDs 1 a 3
    controlSequence_t controlSequenceLeds = {
    	NULL,
		0,
		0,
		false,
    };
    // Se inicializan los parametros de la estructura
    const gpioMap_t sequence2[] = {LED1, LED2, LED3};
    controlSequenceLeds.ledSequence = sequence2;
    controlSequenceLeds.lastLed = sizeof ( sequence2 ) / sizeof ( gpioMap_t );
#endif

    /*Arreglo del tipo gpioMap_t para teclas */
    const gpioMap_t keyArray[] = { TEC1, TEC2, TEC3, TEC4 };

    /* Variables del tipo tick para retardos */
    delay_t ledDelay;

    /* Verificación TICK_RATE rango permitido de tiempo: 1 a 50 ms */
    if ( ( TICK_RATE < TICK_RATE_MIN ) || ( TICK_RATE > TICK_RATE_MAX ) )  blinkError( ERROR_TIME );
    if ( !tickConfig ( TICK_RATE ) )  blinkError( ERROR_TIME );

	/* Apagado de leds y captura de errores de apagado */

#ifdef OPCIONAL_3
	if ( !ledsOff( ledSequence, lastLed) ) blinkError ( ERROR_OFF );
#endif

#ifdef OPCIONAL_4
	if ( !ledsOff( controlSequenceRGB.ledSequence, controlSequenceRGB.lastLed) ) blinkError ( ERROR_OFF );
	if ( !ledsOff( controlSequenceLeds.ledSequence, controlSequenceLeds.lastLed) ) blinkError ( ERROR_OFF );
#endif

    /*Configuración del retardo no bloqueante que determina el tiempo de transicion entre un led y el siguiente en la secuencia*/
    delayConfig ( &ledDelay, DELAY150 );

   // ----- Repeat for ever -------------------------
    while( true ) {

#ifdef OPCIONAL_3

    	/* Lectura de pulsadores para seleccionar el sentido de la secuencia */
    	if(readKey(keyArray, KEY1)) invertSequence = false;
    	else if(readKey(keyArray, KEY4)) invertSequence = true;

    	/* Lectura de pulsadores para seleccionar el tiempo de transicion en la secuencia de leds */
    	if(readKey(keyArray, KEY2)) delayWrite(&ledDelay, DELAY150);
    	else if(readKey(keyArray, KEY3)) delayWrite(&ledDelay, DELAY750);

    	/* Se activa el led correspondiente de la secuencia. En caso de error el programa se bloquea quedando el led rojo parpadeando. */
    	if(delayRead(&ledDelay)){
    		if(!ledSequenceOn(ledSequence, lastLed, invertSequence)) blinkError ( ERROR_SEQ );
    	}
#endif

#ifdef OPCIONAL_4
    	/* Lectura de pulsadores para seleccionar el sentido de la secuencia */
    	if(readKey(keyArray, KEY1)){
    		controlSequenceRGB.inverted = false;
    		controlSequenceLeds.inverted = false;
    	}
    	else if(readKey(keyArray, KEY4)){
    		controlSequenceRGB.inverted = true;
    		controlSequenceLeds.inverted = true;
    	}

    	/* Lectura de pulsadores para seleccionar el tiempo de transicion en la secuencia de leds */
    	if(readKey(keyArray, KEY2)) delayWrite(&ledDelay, DELAY150);
    	else if(readKey(keyArray, KEY3)) delayWrite(&ledDelay, DELAY750);

    	/* Se activa el led correspondiente de la secuencia. En caso de error el programa se bloquea quedando el led rojo parpadeando. */
    	if(delayRead(&ledDelay)){
    		if(!ledSequenceOn(&controlSequenceRGB)) blinkError ( ERROR_SEQ );
    		if(!ledSequenceOn(&controlSequenceLeds)) blinkError ( ERROR_SEQ );
    	}
#endif

    }

    return 0;
}

//***** FUNCIÓN DE APAGADO DE LEDS *************************************************************
// Recibe: un puntero a la secuencia de LEDs y la cantidad de elementos en la secuencia.
// Devuelve: TRUE si pudo apagar correctamente los LEDs o FALSE en caso contrario.
// *********************************************************************************************
bool_t ledsOff ( const gpioMap_t* _ledSequence, const uint8_t _sequenceLength )
{
	int8_t i = 0;

	for ( i = 0; i < _sequenceLength; i++ ) {
		/* Se apaga cada led disponible en la secuencia */
		if ( !gpioWrite ( _ledSequence[i], OFF ) ) return FALSE;
		/* Se verifica el apagado de leds, leyendo el estado GPIO de cada leds */
		if ( gpioRead ( _ledSequence[i] ) == ON ) return FALSE;
	}

	return TRUE;
}

//***** FUNCIÓN DE ENCENDIDO DE LEDS ***********************************************************
// Recibe: el numero de pin correspondiente a la GPIO del LED que se quiere encender
// Devuelve: TRUE en caso de encender correctamente el LED o FALSE en caso contrario.
// *********************************************************************************************
bool_t ledOn ( gpioMap_t led )
{
	/* Se enciende led pasado por argumento */
	if ( !gpioWrite ( led, ON ) ) return FALSE;
	/* Se verifica el apagado del led, leyendo el estado GPIO */
	if ( gpioRead ( led ) == OFF ) return FALSE;
	else return TRUE;
}

#ifdef OPCIONAL_3
//***** FUNCIÓN PARA ACTIVAR EL LED CORRESPONDIENTE DE LA SECUENCIA DE LEDS ********************
// Recibe: Argumento 1: un puntero a la secuencia de LEDs que se quiere activar.
//         Argumento 2: TRUE para sentido normal de la secuencia, FALSE para sentido inverso.
//		   Argumento 3: Cantidad de elementos en la sequencia.
// Devuelve: TRUE en caso de encender correctamente el LED o FALSE en caso contrario.
// *********************************************************************************************
bool_t ledSequenceOn ( const gpioMap_t* _ledSequence, const uint8_t _sequenceLength, bool_t invert )
{
	static int8_t ledIndex = 0;
/*	 Cantidad de elementos en la secuencia de LEDs
	const uint8_t _lastLed = sizeof ( _ledSequence ) / sizeof ( gpioMap_t );*/

	/* Secuencia 1 invertida: LED3 -> LED2 -> LED1 -> LEDB -> LED3 -> ... */
	if ( invert ) ledIndex--;
	/* Secuencia 1: LEDB -> LED1 -> LED2 -> LED3 -> LEDB -> ... */
	else ledIndex++;

	/* ledIndex no debe exceder limite superior lastLed */
	if ( ledIndex >= _sequenceLength ) ledIndex = 0;
	/* ledIndex no debe exceder limite inferior 0 */
	else if ( ledIndex < 0 ) ledIndex = _sequenceLength - 1;

	/* Se apagan todos los LEDs de la secuencia*/
	if ( !ledsOff(_ledSequence, _sequenceLength) ) return FALSE;

	/* Se enciende el LED correspondiente de la secuencia */
	if ( !ledOn ( _ledSequence[ledIndex] ) ) return FALSE;

	return TRUE;
}
#endif

#ifdef OPCIONAL_4
//***** FUNCIÓN PARA ACTIVAR EL LED CORRESPONDIENTE DE LA SECUENCIA DE LEDS ********************
// Recibe: un puntero a la estructura de la secuencia de LEDs que se quiere activar.
// Devuelve: TRUE en caso de encender correctamente el LED o FALSE en caso contrario.
// *********************************************************************************************
bool_t ledSequenceOn(controlSequence_t *controlSequence)
{
	if(controlSequence->ledSequence == NULL) return FALSE;  // Se comprueba que se haya asignado algo al puntero de la secuencia

	int8_t ledIndex = controlSequence->ledIndex;

	if( ledIndex >= controlSequence->lastLed ) return FALSE;  // Se comprueba que el indice del LED este dentro de los valores posibles de la secuencia

	if ( controlSequence->inverted ) ledIndex--;  /* Secuencia invertida */
	else ledIndex++;  /* Secuencia normal */

	/* ledIndex no debe exceder limite superior lastLed */
	if ( ledIndex >= controlSequence->lastLed ) ledIndex = 0;
	/* ledIndex no debe exceder limite inferior 0 */
	else if ( ledIndex < 0 ) ledIndex = controlSequence->lastLed - 1;

	/* Se apagan todos los LEDs de la secuencia*/
	if ( !ledsOff(controlSequence->ledSequence, controlSequence->lastLed) ) return FALSE;

	/* Se enciende el LED correspondiente de la secuencia */
	if ( !ledOn ( controlSequence->ledSequence[ledIndex] ) ) return FALSE;

	controlSequence->ledIndex = ledIndex;  // Se actualiza el indice del led que fue activado

	return TRUE;
}
#endif

//***** FUNCIÓN INDICACIÓN DE ERROR ************************************************************
// Recibe: El tiempo de encendido y apagado del led segun el tipo de error que se quiera mostrar
// Devuelve: Nada
// *********************************************************************************************
void blinkError ( tick_t delayError )
{
	/* El programa se queda aca PARA SIEMPRE */
	while ( true ) {
		/* Parpadeo de led Rojo */
		gpioToggle ( LEDR );
		/* Retardo bloqueante segun tipo de error */
		delay ( delayError );
	}
}

//***** LECTURA DE TECLAS CON FUNCION ANTIRREBOTE **********************************************
// Recibe: el arreglo de teclas y el indice correspondiente de la tecla dentro del arreglo
// Devuelve: TRUE si la tecla fue presionada o FALSE en caso contrario
// *********************************************************************************************
bool_t readKey ( const gpioMap_t *_keyArray, uint8_t keyIndex )
{
	//DEFINICION DE VARIABLES LOCALES
	/* Array de estados para asignarle un estado a cada tecla */
	static dbSt_t debounceState[LAST_KEY] = {WAITING, WAITING, WAITING, WAITING};
	/* Array de retardos para poder asignarle un tiempo antirrebote a cada tecla presionada*/
	static delay_t debounceDelay[LAST_KEY];
	/* Variable que indica si la tecla fue efectivamente presionada */
	bool_t keyPressed = false;

	/* Cambio del estado de las teclas */
	switch(debounceState[keyIndex]){

	/* WAITING: */
	/* Si fue presionada alguna tecla, pasa al estado de DEBOUNCING */
	case WAITING:
		/* Se consulta por la tecla presionada */
		if(gpioRead(_keyArray[keyIndex]) == PRESSED){
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
			/* Si la tecla sigue presionada entonces se concluye que fue efectivamente presionada */
			if(gpioRead(_keyArray[keyIndex]) == PRESSED) keyPressed = TRUE;
			/* Cambio de estado de tecla */
			debounceState[keyIndex] = WAITING;
		}
		break;
	}

	return keyPressed;
}
