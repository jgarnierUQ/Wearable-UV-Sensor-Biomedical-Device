/* uvi.h */

#ifndef UVI_H_
#define UVI_H_

#include "i2c.h"

int uvi_init(void);
float uvi_update(void);

#define SI1133_HI2C             &hi2c1
#define SI1133_ADDRESS          0x55

typedef enum SI1133_Command
{
    SI1133_COMMAND_NOP               =   0x00,
    SI1133_COMMAND_RESET             =   0x01,
    SI1133_COMMAND_NEW_ADDR          =   0x02,
    SI1133_COMMAND_FORCE_CH          =   0x11,
    SI1133_COMMAND_PAUSE_CH          =   0x12,
    SI1133_COMMAND_AUTO_CH           =   0x13,
    SI1133_COMMAND_PARAM_QUERY       =   0x40,
    SI1133_COMMAND_PARAM_SET         =   0x80
} SI1133_Command;

typedef enum SI1133_Reg
{
    SI1133_REG_PART_ID      =   0x00,
    SI1133_REG_HW_ID        =   0x01,
    SI1133_REG_REV_ID       =   0x02,
    SI1133_REG_HOSTIN0      =   0x0A,
    SI1133_REG_COMMAND      =   0x0B,
    SI1133_REG_IRQ_ENABLE   =   0x0F,
    SI1133_REG_RESPONSE1    =   0x10,
    SI1133_REG_RESPONSE0    =   0x11,
    SI1133_REG_IRQ_STATUS   =   0x12,
    SI1133_REG_HOSTOUT0     =   0x13,
    SI1133_REG_HOSTOUT1     =   0x14

} SI1133_Reg;

typedef enum SI1133_Param
{
    SI1133_PARAM_NONE            =      -1,
    SI1133_PARAM_I2C_ADDR        =    0x00,
    SI1133_PARAM_CH_LIST         =    0x01,
    SI1133_PARAM_ADCCONFIG0      =    0x02,
    SI1133_PARAM_ADCSENS0        =    0x03,
    SI1133_PARAM_ADCPOST0        =    0x04,
    SI1133_PARAM_MEASCONFIG0     =    0x05,
    SI1133_PARAM_ADCCONFIG1      =    0x06,
    SI1133_PARAM_ADCSENS1        =    0x07,
    SI1133_PARAM_ADCPOST1        =    0x08,
    SI1133_PARAM_MEASCONFIG1     =    0x09

} SI1133_Param;

HAL_StatusTypeDef uv_read_reg(SI1133_Reg addr, uint8_t* si1133Data);
HAL_StatusTypeDef uv_write_reg(SI1133_Reg addr, uint8_t si1133DataIn);
HAL_StatusTypeDef uv_command(SI1133_Command command, SI1133_Param param);
HAL_StatusTypeDef uv_write_param(SI1133_Param param, uint8_t si1133DataIn);
HAL_StatusTypeDef uv_read_param(SI1133_Param param, uint8_t* si1133Data);

#endif /* UVI_H_ */
