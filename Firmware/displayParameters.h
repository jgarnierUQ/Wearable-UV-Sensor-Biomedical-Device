/*
 * displayParameters.h
 *
 *  Created on: 16 Aug 2019
 *      Author: jonga
 */

#ifndef DISPLAYPARAMETERS_H_
#define DISPLAYPARAMETERS_H_

#include "DEV_Config.h"

typedef struct {
	float UVIndex;
	uint8_t fitzpatrickType;
	int MED;
	float accumulatedDosage;
	int totalPoints;
	int years;
	uint8_t months;
	uint8_t days;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t sdCardInserted;
	uint8_t refreshCount;
	uint8_t firstFullRefresh;
	float lastSEDMeasurement;
	uint8_t firstMeasurementCycle;
} displayParameters;

UBYTE* createImage(void);
void display_uv_parameters(displayParameters* params, int fullOrPartial, UBYTE *Image);
void display_fitzpatrick_type(displayParameters* params, int fullOrPartial, UBYTE *Image);
void display_med(displayParameters* params, int fullOrPartial, UBYTE *Image);
void display_accumulated_dosage(displayParameters* params, int fullOrPartial, UBYTE *Image);
void display_sd_card_params(displayParameters* params, int fullOrPartial, UBYTE *Image);
void update_full_display(displayParameters* params, UBYTE *Image);
int protection_necessary(int uvIndex);
int calculate_MED(displayParameters* params);
int exceeded_dosage(displayParameters* params);
void setup_display_parameters(displayParameters* params);
void display_busy(UBYTE *Image);


#endif /* DISPLAYPARAMETERS_H_ */
