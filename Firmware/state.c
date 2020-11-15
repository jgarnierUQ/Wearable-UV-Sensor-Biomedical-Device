/*
 * state.c
 *
 *  Created on: Sep 18, 2019
 *      Author: jonga
 */

#include "state.h"
#include "uvi.h"
#include "stm32l4xx_hal.h"
#include "SD_Card.h"
#include <stdio.h>

#define TRUE 1
#define FALSE 0

programState state;

uint8_t pauseMode = 0;
uint8_t forceMode = 0;
uint8_t measurementMode;
GPIO_InitTypeDef GPIO_InitStruct = {0};

/**
  * @brief  Handles program behaviour when in measurement mode
  * @param  Struct for display parameters
  * @retval Void
  */
void measurement_mode(displayParameters* params, RTC_HandleTypeDef hrtc, UBYTE *image, FATFS fs, SD_HandleTypeDef *hsd) {

	//Take measurement
	float newIndex = uvi_update();

	//Update display where necessary
	if (newIndex != params->UVIndex) {
		if (params->firstMeasurementCycle == FALSE) {
			display_uv_parameters(params, 0, image);
		}
		params->UVIndex = newIndex;
	}

	//Perform dosage calculation
	//TODO Change 60 to be a calculated time since last measurement in case its returned from pause mode
	params->lastSEDMeasurement = 60 * params->UVIndex * 0.25;

	//Update accumulated SED
	params->accumulatedDosage += params->lastSEDMeasurement;
	if (params->firstMeasurementCycle == FALSE) {
		display_accumulated_dosage(params, 0, image); //This checks whether 40% of MED has been exceeded
	}

	//Store calcs on SD card
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET); 	//Turn on Blue LED
	sd_card_write(hrtc, params, fs, hsd);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);	//Turn off Blue LED
	if (params->firstMeasurementCycle == FALSE) {
		display_sd_card_params(params, 0, image);
	}

	//Full update if its the first measurement cycle
	if (params->firstMeasurementCycle == TRUE) {
		update_full_display(params, image);
		params->firstMeasurementCycle = FALSE;
	}

	EPD_Display(image);
	DEV_Delay_ms(100);
	params->refreshCount++;

	printf("measurement complete\n\r");
}

/**
  * @brief  Handles program behaviour when connected to GUI
  * @param  None
  * @retval Void
  */
void software_mode(void) {
	float newIndex = uvi_update();
	printf("Index: %.1f\r\n", newIndex);
	HAL_Delay(800);
}

/**
  * @brief  Handles program behaviour for going into and exiting sleep mode
  * @param  None
  * @retval Void
  */
void enter_sleep_mode(void) {

	// Flash LEDs
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);

	// Turn off Switch to Peripherals
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);

	// Disable Peripheral Clocks
	__HAL_RCC_I2C1_CLK_DISABLE();
	__HAL_RCC_SPI1_CLK_DISABLE();
	__HAL_RCC_SDMMC1_CLK_DISABLE();

	// Change GPIO Pins to Analog: C0, C1, C2, C3, C4
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	// Change GPIO Pins to Analog: A4, A5, A6
	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Turn off Systick
	HAL_SuspendTick();

	// Sleep
	HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON , PWR_SLEEPENTRY_WFI);
}

void exit_sleep_mode (void) {

	// Turn on Systick
	HAL_ResumeTick();

	// Enable Peripheral Clocks
	__HAL_RCC_I2C1_CLK_ENABLE();
	__HAL_RCC_SPI1_CLK_ENABLE();
	__HAL_RCC_SDMMC1_CLK_ENABLE();

	// Change GPIO Pins back to their original status
	GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	// Turn on a LED and Peripheral Clock Switch
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

	// Reinitialise the UV settings
	uvi_init();
}

/**
  * @brief  Constantly takes measurements and sends them over serial to PC (printf)
  * @param  None
  * @retval Void
  */
void force_mode(RTC_HandleTypeDef hrtc, displayParameters* params, FATFS fs, SD_HandleTypeDef *hsd) {
	float newIndex = uvi_update();
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET); 	//Turn on Blue LED
	sd_card_write(hrtc, params, fs, hsd);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);	//Turn off Blue LED
	printf("%f\n\r", newIndex);
}

/**
  * @brief  Pause Mode
  * @param  None
  * @retval Void
  */
void pause_mode(void) {

}
