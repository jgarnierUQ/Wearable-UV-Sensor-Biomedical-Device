/*
 * uart.c
 *
 *  Created on: 24 Sep 2019
 *      Author: Jonathan
 */

#include "uart.h"
#include "state.h"

uint8_t softwareMode = 0;

/**
  * @brief  Handles program behaviour for processing serial data
  * @param  dispParams - parameters for displaying on ePaper
  * 		rxBuffer - Buffer containing serial data
  * 		hrtc - Real Time Clock Settings
  * @retval Void
  */
uint8_t process_serial_data(displayParameters* dispParams, uint8_t* rxBuffer, RTC_HandleTypeDef hrtc, UBYTE *image) {
	if (rxBuffer[0] == 0x03) {
		dispParams->fitzpatrickType = (int) rxBuffer[1];
		dispParams->MED = (int) (rxBuffer[2] << 8 | rxBuffer[3]);
		display_fitzpatrick_type(dispParams, 0, image);
		display_med(dispParams, 0, image);
		return 1;
	}
	if (rxBuffer[0] == 0x01) {
		RTC_TimeTypeDef sTime;
		RTC_DateTypeDef sDate;
		sDate.WeekDay = (int) rxBuffer[1];
		sDate.Date = (int) rxBuffer[2];
		sDate.Month = (int) rxBuffer[3];
		sDate.Year = (int) rxBuffer[4];
		sTime.Hours = (int) rxBuffer[5];
		sTime.Minutes = (int) rxBuffer[6];
		sTime.Seconds = (int) rxBuffer[7];
		sTime.TimeFormat = RTC_HOURFORMAT_24;
		HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		softwareMode = 1;
		return 1;
	}
	if (rxBuffer[0] == 0x02) {
		softwareMode = 0;
		return 1;
	}
	if (rxBuffer[0] == 0x04 || rxBuffer[0] == 0xFE) {
		rxBuffer[0] = 0x04;
		return 1;
	}
	return 0;
}
