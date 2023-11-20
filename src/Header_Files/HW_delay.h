/*
 * HW_delay.h
 *
 *  Created on: Oct 11, 2022
 *      Author: mjkil
 */

#ifndef SRC_HEADER_FILES_HW_DELAY_H_
#define SRC_HEADER_FILES_HW_DELAY_H_

#include "em_timer.h"
#include "em_cmu.h"

void timer_delay(uint32_t ms_delay);

#endif /* SRC_HEADER_FILES_HW_DELAY_H_ */
