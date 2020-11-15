/*
 * displayParameters.c
 *
 *  Created on: 16 Aug 2019
 *      Author: jonga
 */

#include "displayParameters.h"
#include "EPD_2in9.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "imagedata.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define FULL 1
#define PARTIAL 0

/**
  * @brief  Updates all uv index parameters on waveshare display including uv index, bar graph, and alert.
  * @param  Struct for display parameters
  * @retval Void
  */
void display_uv_parameters(displayParameters* params, int fullOrPartial, UBYTE *Image) {

	if ((fullOrPartial == PARTIAL) && (params->refreshCount < 5)) {
		//Set up to display in partial mode
		EPD_Init(lut_partial_update);
	} else {
		//Full Display Mode
		EPD_Init(lut_full_update);
		if (params->firstFullRefresh == 1) {
			EPD_Clear();
			DEV_Delay_ms(500);

			//Reset refresh count
			params->refreshCount = 0;
		}
	}

	Paint_ClearWindows(0, EPD_WIDTH - 31, EPD_HEIGHT, EPD_WIDTH, WHITE);
	Paint_DrawRectangle(0, EPD_WIDTH - 30, (params->UVIndex * EPD_HEIGHT / 11), EPD_WIDTH, BLACK, DRAW_FILL_FULL, DOT_PIXEL_1X1);

	//Draw bars for different segments of bar graph
	//Top and bottom horizontal lines
	Paint_DrawLine(0, EPD_WIDTH - 30, EPD_HEIGHT, EPD_WIDTH - 30, BLACK, LINE_STYLE_SOLID, DOT_PIXEL_1X1);
	Paint_DrawLine(0, EPD_WIDTH, EPD_HEIGHT, EPD_WIDTH, BLACK, LINE_STYLE_SOLID, DOT_PIXEL_1X1);

	//Vertical black lines
	for (int i = 1; i < 12; i++) {
		Paint_DrawLine((i * EPD_HEIGHT / 11), EPD_WIDTH - 30, (i * EPD_HEIGHT / 11), EPD_WIDTH, BLACK, LINE_STYLE_SOLID, DOT_PIXEL_1X1);
	}

	//Vertical white lines
	for (int i = 1; i < params->UVIndex; i++) {
		Paint_DrawLine((i * EPD_HEIGHT / 11), EPD_WIDTH - 29, (i * EPD_HEIGHT / 11), EPD_WIDTH - 1, WHITE, LINE_STYLE_SOLID, DOT_PIXEL_1X1);
	}

	//UV Index
	char indexConverted[10];
	sprintf(indexConverted, "%.1f", params->UVIndex);
	char indexString[13] = "UV Index: ";
	strcat(indexString, indexConverted);

	if (params->UVIndex == 11) {
		strcat(indexString, "+");
	}

	Paint_ClearWindows(0, 0, Font12.Width * 14, Font12.Height, WHITE);
	Paint_DrawString_EN(0, 0, indexString, &Font12, WHITE, BLACK);

	//Sun protection necessary alert
	if (protection_necessary(params->UVIndex)) {
		Paint_ClearWindows(EPD_HEIGHT - 140, Font16.Height + 2, EPD_HEIGHT - 140 + Font12.Width * 20, Font16.Height + Font12.Height, WHITE);
		Paint_DrawString_EN(EPD_HEIGHT - 140, Font16.Height + 2, "PROTECTION NECESSARY", &Font12, WHITE, BLACK);
	} else {
		Paint_ClearWindows(EPD_HEIGHT - 140, Font16.Height + 2, EPD_HEIGHT - 140 + Font12.Width * 20, Font16.Height + Font12.Height, WHITE);
		Paint_DrawString_EN(EPD_HEIGHT - 80, Font16.Height + 2, "n/a", &Font12, WHITE, BLACK);
	}

	//Display
//	if (fullOrPartial == PARTIAL) {
//		EPD_Display(Image);
//		DEV_Delay_ms(100);
//		params->refreshCount++;
//	}
}

/**
  * @brief  Updates fitxpatrick skin type value on waveshare display
  * @param  Struct for display parameters, FULL or PARTIAL update mode, Image to change
  * @retval Void
  */
void display_fitzpatrick_type(displayParameters* params, int fullOrPartial, UBYTE *Image) {

	if ((fullOrPartial == PARTIAL) && (params->refreshCount < 5)) {
		//Set up to display in partial mode
		if(EPD_Init(lut_partial_update) != 0) {
			//printf("e-Paper init failed\r\n");
		}
	} else {
		//Full Display Mode
		if(EPD_Init(lut_full_update) != 0) {
				//printf("e-Paper init failed\r\n");
		}
		if (params->firstFullRefresh == 1) {
			EPD_Clear();
			DEV_Delay_ms(500);

			//Reset refresh count
			params->refreshCount = 0;
		}
	}

	char fitzConverted[2];
	if (params->fitzpatrickType == 7) {
		sprintf(fitzConverted, "?");
	} else {
		sprintf(fitzConverted, "%d", params->fitzpatrickType);
	}
	char fitzString[20] = "Fitzpatrick Type: ";
	strcat(fitzString, fitzConverted);

	Paint_ClearWindows(0, Font12.Height + 2, Font12.Width * 20, 2*Font12.Height + 2, WHITE);
	Paint_DrawString_EN(0, Font12.Height + 2, fitzString, &Font12, WHITE, BLACK);

	//Display
	if (fullOrPartial == PARTIAL) {
		EPD_Display(Image);
		DEV_Delay_ms(100);
		params->refreshCount++;
	}

}

/**
  * @brief  Updates MED value on waveshare display
  * @param  Struct for display parameters, FULL or PARTIAL update mode, Image to change
  * @retval Void
  */
void display_med(displayParameters* params, int fullOrPartial, UBYTE *Image) {

	if ((fullOrPartial == PARTIAL) && (params->refreshCount < 5)) {
		//Set up to display in partial mode
		EPD_Init(lut_partial_update);
	} else {
		//Full Display Mode
		EPD_Init(lut_full_update);
		if (params->firstFullRefresh == 1) {
			EPD_Clear();
			DEV_Delay_ms(500);

			//Reset refresh count
			params->refreshCount = 0;
		}
	}

	char medConverted[3];
	sprintf(medConverted, "%d", params->MED);
	char medString[9] = "MED: ";
	strcat(medString, medConverted);

	Paint_ClearWindows(0, 2 * Font12.Height + 2, Font12.Width * 8, 3 * Font12.Height + 2, WHITE);
	Paint_DrawString_EN(0, 2 * Font12.Height + 2, medString, &Font12, WHITE, BLACK);

	//Display exceeded 40% alert
	if (exceeded_dosage(params)) {
		Paint_ClearWindows(EPD_HEIGHT - 140, Font16.Height + Font12.Height + 2, EPD_HEIGHT - 140 + Font12.Width * 20, Font16.Height + 2 * Font12.Height, WHITE);
		Paint_DrawString_EN(EPD_HEIGHT - 125, Font16.Height + Font12.Height + 2, "EXCEEDED 40% MED", &Font12, WHITE, BLACK);
	} else {
		Paint_ClearWindows(EPD_HEIGHT - 140, Font16.Height + Font12.Height + 2, EPD_HEIGHT - 140 + Font12.Width * 20, Font16.Height + 2 * Font12.Height, WHITE);
		Paint_DrawString_EN(EPD_HEIGHT - 80, Font16.Height + Font12.Height + 2, "n/a", &Font12, WHITE, BLACK);
	}

	//Display
	if (fullOrPartial == PARTIAL) {
		EPD_Display(Image);
		DEV_Delay_ms(1000);
		params->refreshCount++;
	}

}

/**
  * @brief  Updates accumulated dosage value on waveshare display
  * @param  Struct for display parameters, FULL or PARTIAL update mode, Image to change
  * @retval Void
  */
void display_accumulated_dosage(displayParameters* params, int fullOrPartial, UBYTE *Image) {

	if ((fullOrPartial == PARTIAL) && (params->refreshCount < 5)) {
		//Set up to display in partial mode
		EPD_Init(lut_partial_update);
	} else {
		//Full Display Mode
		EPD_Init(lut_full_update);
		if (params->firstFullRefresh == 1) {
			EPD_Clear();
			DEV_Delay_ms(500);

			//Reset refresh count
			params->refreshCount = 0;
		}
	}

	char accumConverted[10];
	sprintf(accumConverted, "%0.3f", params->accumulatedDosage);
	char accumString[30] = "Accum Dosage: ";
	strcat(accumString, accumConverted);
	strcat(accumString, "SED");

	Paint_ClearWindows(0, 3 * Font12.Height + 2, Font12.Width * 24, 4 * Font12.Height + 2, WHITE);
	Paint_DrawString_EN(0, 3 * Font12.Height + 2, accumString, &Font12, WHITE, BLACK);
	Paint_DrawLine(EPD_HEIGHT - 140, 0, EPD_HEIGHT - 140, EPD_WIDTH/2 - 10, BLACK, LINE_STYLE_SOLID, DOT_PIXEL_1X1);

	//Display exceeded 40% alert
	if (exceeded_dosage(params)) {
		Paint_ClearWindows(EPD_HEIGHT - 140, Font16.Height + Font12.Height + 2, EPD_HEIGHT - 140 + Font12.Width * 20, Font16.Height + 2 * Font12.Height, WHITE);
		Paint_DrawString_EN(EPD_HEIGHT - 125, Font16.Height + Font12.Height + 2, "EXCEEDED 40% MED", &Font12, WHITE, BLACK);
	} else {
		Paint_ClearWindows(EPD_HEIGHT - 140, Font16.Height + Font12.Height + 2, EPD_HEIGHT - 140 + Font12.Width * 20, Font16.Height + 2 * Font12.Height, WHITE);
		Paint_DrawString_EN(EPD_HEIGHT - 80, Font16.Height + Font12.Height + 2, "n/a", &Font12, WHITE, BLACK);
	}

	//Display
//	if (fullOrPartial == PARTIAL) {
//		EPD_Display(Image);
//		DEV_Delay_ms(1000);
//		params->refreshCount++;
//	}

}

/**
  * @brief  Updates sd card parameters on waveshare display
  * @param  Struct for display parameters, FULL or PARTIAL update mode, Image to change
  * @retval Void
  */
void display_sd_card_params(displayParameters* params, int fullOrPartial, UBYTE *Image) {

	if ((fullOrPartial == PARTIAL) && (params->refreshCount < 5)) {
		//Set up to display in partial mode
		EPD_Init(lut_partial_update);
	} else {
		//Full Display Mode
		EPD_Init(lut_full_update);
		if (params->firstFullRefresh == 1) {
			EPD_Clear();
			DEV_Delay_ms(500);

			//Reset refresh count
			params->refreshCount = 0;
		}
	}

	//SD Card inserted and total number of points
	if (params->sdCardInserted == TRUE) {
		//Display total points
		char totalPointsConverted[10];
		sprintf(totalPointsConverted, "%d", params->totalPoints);
		char totalPointsString[30] = "Total Points: ";
		strcat(totalPointsString, totalPointsConverted);

		Paint_ClearWindows(0, 5 * Font12.Height + 2, Font12.Width * 21, 6 * Font12.Height, WHITE);
		Paint_DrawString_EN(0, 5 * Font12.Height + 2, totalPointsString, &Font12, WHITE, BLACK);
	} else {
		//Display SD card not inserted
		Paint_ClearWindows(0, 5 * Font12.Height + 2, Font12.Width * 21, 6 * Font12.Height, WHITE);
		Paint_DrawString_EN(0, 5 * Font12.Height + 2, "SD Card Not Inserted", &Font12, WHITE, BLACK);
	}

	//Time since last measurement
	char timeString[50];
	sprintf(timeString, "%d/%d/%d %d:%d", params->years, params->months, params->days, params->hours, params->minutes);

	Paint_ClearWindows(0, 6 * Font12.Height + 2, Font12.Width * 40, 7 * Font12.Height, WHITE);
	Paint_DrawString_EN(0, 6 * Font12.Height + 2, "Last Measurement: ", &Font12, WHITE, BLACK);
	Paint_DrawString_EN(19 * Font12.Width, 6 * Font12.Height + 2, timeString, &Font12, WHITE, BLACK);


	//Display
//	if (fullOrPartial == PARTIAL) {
//		EPD_Display(Image);
//		DEV_Delay_ms(1000);
//		params->refreshCount++;
//	}
}





/**
  * @brief  Updates entire waveshare display
  * @param  Struct for display parameters, FULL or PARTIAL update mode, Image to change
  * @retval Void
  */
void update_full_display(displayParameters* params, UBYTE *Image) {

	//Set initial struct values
	setup_display_parameters(params);

	//Set up fill display mode
	EPD_Init(lut_full_update);
	EPD_Clear();
	DEV_Delay_ms(500);

	Paint_NewImage(Image, EPD_WIDTH, EPD_HEIGHT, 270, WHITE);
	Paint_SelectImage(Image);
	Paint_Clear(WHITE);

	//Draw Alerts Box
	Paint_DrawString_EN(EPD_HEIGHT - 100, 0, "ALERTS", &Font16, WHITE, BLACK);
	Paint_DrawLine(EPD_HEIGHT - 100, Font16.Width + 2, EPD_HEIGHT - 100 + (4 * Font16.Height), Font16.Width + 2, BLACK, LINE_STYLE_SOLID, DOT_PIXEL_1X1);
	Paint_DrawLine(EPD_HEIGHT - 140, 0, EPD_HEIGHT - 140, EPD_WIDTH/2 - 10, BLACK, LINE_STYLE_SOLID, DOT_PIXEL_1X1);
	Paint_DrawLine(0, EPD_WIDTH/2 - 10, EPD_HEIGHT, EPD_WIDTH/2 - 10, BLACK, LINE_STYLE_SOLID, DOT_PIXEL_1X1);

	//Draw each item
	display_uv_parameters(params, FULL, Image);
	display_fitzpatrick_type(params, FULL, Image);
	display_med(params, FULL, Image);
	display_accumulated_dosage(params, FULL, Image);
	display_sd_card_params(params, FULL, Image);

	//Display
	EPD_Display(Image);
	DEV_Delay_ms(2000);

	params->firstFullRefresh = 1;
}

/**
  * @brief  Determines whether or not to display alert icon
  * @param  UV Index
  * @retval True if alert should be displayed, False if not
  */
int protection_necessary(int uvIndex) {
	if (uvIndex >= 1) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
  * @brief  Calculates MED based off Fitzpatrick Type
  * @param  params struct
  * @retval MED value of user
  */
int calculate_MED(displayParameters* params) {
	int medValue;
	switch (params->fitzpatrickType) {
		case 1:
			medValue = 150;
			break;
		case 2:
			medValue = 220;
			break;
		case 3:
			medValue = 290;
			break;
		case 4:
			medValue = 370;
			break;
		case 5:
			medValue = 440;
			break;
		case 6:
			medValue = 440;
			break;
	}
	return medValue;
}

/**
  * @brief  Determines whether or not the user has exceeded 40% of their
  * 		MED in accumulated dosage
  * @param  MED, Accumulated Dosage
  * @retval True if dosage has been exceeded, False if not
  */
int exceeded_dosage(displayParameters* params) {
	//Get MED value
	int userMED = calculate_MED(params);

	//Determine if over 40%
	if (params->accumulatedDosage >= (int)(0.4 * userMED)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void setup_display_parameters(displayParameters* params) {
	params->refreshCount = 0;
	params->firstFullRefresh = 0;
}

void display_busy(UBYTE *Image) {
	//Set up fill display mode
	EPD_Init(lut_full_update);
	EPD_Clear();
	DEV_Delay_ms(500);

	Paint_NewImage(Image, EPD_WIDTH, EPD_HEIGHT, 270, WHITE);
	Paint_SelectImage(Image);
	Paint_Clear(WHITE);

	Paint_DrawString_EN(100, 60, "BUSY!", &Font24, WHITE, BLACK);
	EPD_Display(Image);
	DEV_Delay_ms(2000);
}
