/*=============================================================================
 * Ejercicio 1
 * Authors: Martin Rios <jrios@fi.uba.ar> - Lucas Zalazar <lucas.zalazar6@gmail.com>
 * Date: 2021/07/05
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

    /*Configuración del retardo no bloqueante que determina el tiempo de transicion entre un led y el siguiente en la secuencia*/
    delayConfig ( &ledDelay, DELAY150 );

   // ----- Repeat for ever -------------------------
    while( true ) {

    	/* Lectura de pulsadores para seleccionar el sentido de la secuencia */
    	if(readKey(KEY1)) invertSequence = false;
    	else if(readKey(KEY4)) invertSequence = true;

    	/* Lectura de pulsadores para seleccionar el tiempo de transicion en la secuencia de leds */
    	if(readKey(KEY2)) delayWrite(&ledDelay, DELAY150);
    	else if(readKey(KEY3)) delayWrite(&ledDelay, DELAY750);

    	/* Se activa el led correspondiente de la secuencia. En caso de error el programa se bloquea quedando el led rojo parpadeando. */
    	if(delayRead(&ledDelay)){
    		if(!ledSequenceOn(ledSequence, invertSequence)) blinkError ( ERROR_SEQ );
    	}
    }

    return 0;
}

// FUNCIÓN DE APAGADO DE LEDS
bool_t ledsOff ( const gpioMap_t* _ledSequence )
{
	int8_t i = 0;

	for ( i = 0; i < lastLed; i++ ) {
		/* Se apaga cada led disponible en la secuencia */
		if ( !gpioWrite ( _ledSequence[i], OFF ) ) return FALSE;
		/* Se verifica el apagado de leds, leyendo el estado GPIO de cada leds */
		if ( gpioRead ( _ledSequence[i] ) == ON ) return FALSE;
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

// FUNCIÓN PARA ACTIVAR EL LED CORRESPONDIENTE DE LA SECUENCIA DE LEDS
bool_t ledSequenceOn ( const gpioMap_t* _ledSequence, bool_t invert )
{
	static int8_t ledIndex = 0;

	/* Secuencia 1: LEDB -> LED1 -> LED2 -> LED3 -> LEDB -> ... */
	if ( invert ) ledIndex--;
	/* Secuencia 1 invertida: LED3 -> LED2 -> LED1 -> LEDB -> LED3 -> ... */
	else ledIndex++;

	/* ledIndex no debe exceder limite superior lastLed */
	if ( ledIndex >= lastLed ) ledIndex = 0;
	/* ledIndex no debe exceder limite inferior 0 */
	else if ( ledIndex < 0 ) ledIndex = lastLed - 1;

	/* Función de apagado de leds y captura de errores de apagado */
	if ( !ledsOff(_ledSequence) ) blinkError ( ERROR_OFF );

	/* Funcion de encendido de leds y captura de errores de encendido */
	if ( !ledOn ( _ledSequence[ledIndex] ) ) blinkError ( ERROR_ON );

	return TRUE;
}

// FUNCIÓN INDICACIÓN DE ERROR
// SE PASA POR ARGUMENTO EL TIEMPO DE ENCENDIDO Y APAGADO DEL LED SEGUN EL TIPO DE ERROR QUE SE QUIERA MOSTRAR
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

// LECTURA DE TECLAS CON FUNCION ANTIRREBOTE
// Recibe como argumento el indice correspondiente de la tecla dentro del array de teclas
// Devuelve TRUE si la tecla fue presionada o FALSE en caso contrario
bool_t readKey ( uint8_t keyIndex )
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
			/* Si la tecla sigue presionada entonces se concluye que fue efectivamente presionada */
			if(gpioRead(keyArray[keyIndex]) == PRESSED) keyPressed = TRUE;
			/* Cambio de estado de tecla */
			debounceState[keyIndex] = WAITING;
		}
		break;
	}

	return keyPressed;
}
