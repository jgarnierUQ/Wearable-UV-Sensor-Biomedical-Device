/* uvi.c */

#include "uvi.h"
#include <stdio.h>
#include "stm32l4xx_hal_i2c.h"
#include "i2c.h"


#define HW_GAIN 3 // Calibrated gain

/* Read from Sensor address */
HAL_StatusTypeDef uv_read_reg(SI1133_Reg addr, uint8_t* si1133Data) {
	HAL_StatusTypeDef r = HAL_I2C_Mem_Read(SI1133_HI2C, SI1133_ADDRESS << 1, addr | 0x40, 1, si1133Data, 1, 100);

	return r;
}

/* Write to Sensor Address */
HAL_StatusTypeDef uv_write_reg(SI1133_Reg addr, uint8_t si1133DataIn) {
	HAL_StatusTypeDef r = HAL_I2C_Mem_Write(SI1133_HI2C, SI1133_ADDRESS << 1, addr | 0x40, 1, &si1133DataIn, 1, 100);

	HAL_Delay(100);

	return r;
}

/*	Command Send */
HAL_StatusTypeDef uv_command(SI1133_Command c, SI1133_Param p) {
	HAL_StatusTypeDef r;

	if (p == SI1133_PARAM_NONE) {
		r = uv_write_reg(SI1133_REG_COMMAND, c);
	}
	else {
		r = uv_write_reg(SI1133_REG_COMMAND, c | p);
	}

	return r;
}

/* UV Parameter Send */
HAL_StatusTypeDef uv_write_param(SI1133_Param param, uint8_t si1133DataIn) {
	uint8_t res0 = 0;
	uv_read_reg(SI1133_REG_RESPONSE0, &res0);
	uint8_t cmd = res0 | 00001111;

	uv_write_reg(SI1133_REG_HOSTIN0, si1133DataIn);

	uv_command(SI1133_COMMAND_PARAM_SET, param);

	uv_read_reg(SI1133_REG_RESPONSE0, &res0);

	if (cmd >= (res0 | 00001111)) {
		printf("CMD_CTR FAILED TO INCREMENT IN WRITE PARAM.\r\n");
		uv_command(SI1133_COMMAND_RESET, SI1133_PARAM_NONE);
		return uv_write_param(param, si1133DataIn);
	}

	return HAL_OK;
}

/* Read */
HAL_StatusTypeDef uv_read_param(SI1133_Param param, uint8_t* si1133Data) {
	uint8_t res0 = 0;
	uv_read_reg(SI1133_REG_RESPONSE0, &res0);
	uint8_t cmd = res0 | 00001111;

	uv_command(SI1133_COMMAND_PARAM_QUERY, param);

	uv_read_reg(SI1133_REG_RESPONSE0, &res0);

	if (cmd >= (res0 | 00001111)) {
		printf("CMD_CTR FAILED TO INCREMENT IN READ PARAM.\r\n");
		uv_command(SI1133_COMMAND_RESET, SI1133_PARAM_NONE);
		return uv_read_param(param, si1133Data);
	}

	uv_read_reg(SI1133_REG_RESPONSE1, si1133Data);

	return HAL_OK;
}

/* UV init */
int uvi_init(void) {

	HAL_Delay(30);

	uv_command(SI1133_COMMAND_RESET, SI1133_PARAM_NONE);

	uv_write_param(SI1133_PARAM_CH_LIST, 	0b00000001);
	uv_write_reg(SI1133_REG_IRQ_ENABLE, 	0xFF);
	uv_write_reg(SI1133_REG_IRQ_STATUS, 	0xFF);

	/* Configure the sensor  */
	uv_write_param(SI1133_PARAM_ADCCONFIG0, 0x78);
	uv_write_param(SI1133_PARAM_ADCSENS0, 	0xf1 | HW_GAIN);
	uv_write_param(SI1133_PARAM_ADCPOST0, 	0x00);
	uv_write_param(SI1133_PARAM_MEASCONFIG0, 0x00);

	return 0;
}

/* UV index Conversion */
float raw_to_uvi(int raw) {
	return 0.0187 * ((0.00391 * raw * raw) + raw);
}

/* Function to call */
float uvi_update(void) {


	uv_command(SI1133_COMMAND_FORCE_CH, SI1133_PARAM_NONE);

	uint8_t state = 0;
	while (state != 0b00000001) {
		uv_read_reg(SI1133_REG_IRQ_STATUS, &state);
		HAL_Delay(100);
	}

	uint8_t uvData[2];
	uv_read_reg(SI1133_REG_HOSTOUT0, &uvData[0]);
	uv_read_reg(SI1133_REG_HOSTOUT1, &uvData[1]);

	int uvRaw = (int)(uint16_t)((uvData[0] << 8) | uvData[1]);


	/* Multiply by 10, cast to an int, then cast back to a float, then divide by ten. */
	float uvIndex = raw_to_uvi(uvRaw);

	//printf("UV Raw: %d. Uv Index: %.1f\n\r", uvRaw, uvIndex);

	return uvIndex;
}
