/*
 * Si7021.h
 *
 *  Created on: Oct 11, 2022
 *      Author: mjkil
 */

#ifndef SRC_HEADER_FILES_SI7021_H_
#define SRC_HEADER_FILES_SI7021_H_


#include "em_letimer.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_assert.h"
#include "scheduler.h"
#include "sleep_routine.h"
#include "I2C.h"
#include "em_i2c.h"
#include "brd_config.h"
#include "HW_delay.h"

#define I2C_freq I2C_FREQ_FAST_MAX;
#define I2C_clk_ratio i2cClockHLRAsymetric;
#define SI7021_POWERON_DELAY   80
#define SI7021_Address    0x40
#define SI7021_CMD_MEASURE_RH_NO_HOLD    0xF5         /**< Measure Relative Humidity, No Hold Master Mode */
#define SI7021_CMD_MEASURE_TEMP           0xE0
#define writeData         0x01



void si7021_i2c_open();
void SI7021_Read_Helper(uint8_t command, uint8_t bytes, uint32_t callback);
void SI7021_Write_Helper(uint8_t bytes, uint8_t command);

uint32_t get_Si7021_temp(void);
float get_si7021_rh(void);


#endif /* SRC_HEADER_FILES_SI7021_H_ */
