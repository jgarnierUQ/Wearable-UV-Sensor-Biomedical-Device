/*
 * state.h
 *
 *  Created on: Sep 18, 2019
 *      Author: jonga
 */

#ifndef STATE_H_
#define STATE_H_

#include <stdint.h>
#include "displayParameters.h"
#include "stm32l4xx_hal.h"
#include "fatfs.h"
#include <time.h>

typedef enum {
	MEASUREMENT,
	SLEEP,
	FORCE,
	PAUSE,
	PC_SOFTWARE
} programState;

extern programState state;
extern uint8_t sleepMode;
extern uint8_t forceMode;
extern uint8_t pauseMode;
extern uint8_t measurementMode;
extern uint8_t softwareMode;
extern uint8_t wakeFromSleep;

void measurement_mode(displayParameters* params, RTC_HandleTypeDef hrtc, UBYTE *image, FATFS fs, SD_HandleTypeDef *hsd);
void enter_sleep_mode(void);
void exit_sleep_mode(void);
void software_mode(void);
void force_mode(RTC_HandleTypeDef hrtc, displayParameters* params, FATFS fs, SD_HandleTypeDef *hsd);
void pause_mode(void);

#endif /* STATE_H_ */
