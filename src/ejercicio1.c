/*=============================================================================
 * Ejercicio 1
 * Author: Martin Rios <jrios@fi.uba.ar>
 * Date: 2021/06/29
 * Version: 1.0
 *===========================================================================*/

#include "sapi.h"
#include "ejercicio1.h"

const gpioMap_t ledSequence[] = {LEDB, LED1, LED2, LED3};
const uint8_t lastLed = sizeof(ledSequence)/sizeof(gpioMap_t);

const gpioMap_t keyArray[] = {TEC1, TEC2, TEC3, TEC4};

int main( void )
{
	delay_t ledDelay;
	bool_t invertSequence = false;

    // ----- Setup -----------------------------------
    boardInit();

    if((TICK_RATE < TICK_RATE_MIN) || (TICK_RATE > TICK_RATE_MAX))  blinkError();
    if(!tickConfig(TICK_RATE))  blinkError();

    delayConfig(&ledDelay, DELAY1);

   // ----- Repeat for ever -------------------------
    while( true ) {
    	if(readKey(KEY1)) invertSequence = false;
    	else if(readKey(KEY4)) invertSequence = true;
    	if(readKey(KEY2)) delayWrite(&ledDelay, DELAY1);
    	else if(readKey(KEY3)) delayWrite(&ledDelay, DELAY2);

    	if(delayRead(&ledDelay)){
    		if(!ledSequenceOn(ledSequence, invertSequence)) blinkError();
    	}
    }

    return 0;
}

bool_t ledsOff()
{
	int8_t i = 0;

	for(i = 0; i < lastLed; i++){
		if(!gpioWrite(ledSequence[i], OFF)) return FALSE;
	}

	return TRUE;
}

bool_t ledOn(gpioMap_t led)
{
	if(!gpioWrite(led, ON)) return FALSE;
	else return TRUE;
}

bool_t ledSequenceOn(const gpioMap_t* _ledSequence, bool_t invert)
{
	static int8_t ledIndex = 0;

	if(invert) ledIndex--;
	else ledIndex++;
	if(ledIndex >= lastLed) ledIndex = 0;
	else if(ledIndex < 0) ledIndex = lastLed - 1;

	if(!ledsOff()) return FALSE;
	if(!ledOn(_ledSequence[ledIndex])) return FALSE;

	return TRUE;
}

void blinkError()
{
	while(true){
		gpioToggle(LEDR);
		delay(500);
	}
}

bool_t readKey(uint8_t keyIndex)
{
	static dbSt_t debounceState[LAST_KEY] = {WAITING, WAITING, WAITING, WAITING};
	static delay_t debounceDelay[LAST_KEY];
	bool_t ret = false;

	switch(debounceState[keyIndex]){

	case WAITING:
		if(gpioRead(keyArray[keyIndex]) == PRESSED){
			debounceState[keyIndex] = DEBOUNCING;
			delayInit(&debounceDelay[keyIndex], DEBOUNCE_TIME);
		}
		break;

	case DEBOUNCING:
		if(delayRead(&debounceDelay[keyIndex])){
			if(gpioRead(keyArray[keyIndex]) == PRESSED){
				debounceState[keyIndex] = WAITING;
				ret = true;
			}
			else debounceState[keyIndex] = WAITING;
		}
		break;
	}

	return ret;
}
