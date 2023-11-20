/*
 * SHTC3.h
 *
 *  Created on: Nov 28, 2022
 *      Author: mjkil
 */

#ifndef SRC_HEADER_FILES_SHTC3_H_
#define SRC_HEADER_FILES_SHTC3_H_

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


// command defines
#define SHTC3_Delay         240
#define SHTC3_sleep_cmd     0xB098
#define SHTC3_wakeup_cmd    0x3517
#define twoByte 2
#define oneByte 1
#define sixByte 6
#define eightByte 8




//temp first command ( Low Energy and not Low Energy)
#define tempFirstReadCmdLP    0x609C
#define tempFirstReadCmd      0x7866

//humidity first command (low energy and not low energy)
#define humFirstReadCmdLP     0x401A
#define humFirstReadCmd       0x58E0

#define SH_address    0x70
//functions

void SH_I2C_OPEN();
void shtc3_read_data_and_crc(uint32_t callback_event);
float get_SH_rh(void);
float get_SH_temp(void);

void return_temp_hum(float *t, float *h);

#define tempHelper 0xFFFF;















#endif /* SRC_HEADER_FILES_SHTC3_H_ */
