/*
 * I2C.h
 *
 *  Created on: Oct 6, 2022
 *      Author: mjkil
 */

#ifndef SRC_HEADER_FILES_I2C_H_
#define SRC_HEADER_FILES_I2C_H_

#include "em_i2c.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_assert.h"
#include "scheduler.h"
#include "sleep_routine.h"
#include "Si7021.h"



#define I2C_EM_BLOCK EM2
#define WdeviceAdd        0x40
#define WslaveAdd         0x014
#define READ    1
#define WRITE   0
#define Measure_cmd       0xF5
#define bufferAddressReset    0
#define writeCommand      0xE6
#define twoBytesLeft            2
#define SH_address    0x70




//#define I2C0_freq = I2C_FREQ_FAST_MAX;
//#define I2C0_clk_ratio = i2cClockHLRAsymetric;

typedef struct{


  bool enable;
  bool master;
  uint32_t refFreq;
  uint32_t freq;
  I2C_ClockHLR_TypeDef clhr;
  uint32_t ROUTEscl;
  uint32_t ROUTEsda;
  bool scl_pin_en;
  bool sda_pin_en;



}I2C_OPEN_STRUCT_TypeDef;


typedef enum{
  Init,
  SetReg,
  Hold,
  Write,
  RXState,
  Close

}DEFINED_STATES;


typedef struct{
  uint32_t newDeviceAddress;
  uint32_t newRegisterAddress;
  uint32_t newData;
  bool newRead;
  uint32_t *newBufferAddress;
  uint32_t newBytesleft;
  uint32_t newCallBack;
  uint32_t newCommand;
  uint32_t newRepeatedStart;
  uint32_t newCombinedBytes;
  uint32_t newNumCmdBytes;


}STATE_MACHINE_START_STRUCT;




void i2c_start(I2C_TypeDef *i2c, STATE_MACHINE_START_STRUCT *openStruct);
void i2c_open(I2C_TypeDef *i2c_v, I2C_OPEN_STRUCT_TypeDef *I2C_T);

#endif /* SRC_HEADER_FILES_I2C_H_ */
