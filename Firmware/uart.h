/*
 * uart.h
 *
 *  Created on: 24 Sep 2019
 *      Author: Jonathan
 */

#ifndef UART_H_
#define UART_H_

#include "displayParameters.h"
#include "stm32l4xx_hal.h"

uint8_t process_serial_data(displayParameters* dispParams, uint8_t* rxBuffer, RTC_HandleTypeDef hrtc, UBYTE *image);

#endif /* UART_H_ */
