/**
 * @file Si7021.c
 * @author Max kilcoyne
 * @date 09/15/22
 * @brief The driver file for the si7021 sensor
 *
 */

#include "Si7021.h"


uint32_t read_result = 0;
uint32_t writeValue = writeData;
uint32_t temp_result = 0;


/***************************************************************************/
/**
 * @brief
 * si7021 open function
 *
 * @details
 *  This function is used assign necessary values to the I2C_OPEN_STRUCT_TypeDef in order to communicate with the sensor.
 *
 * @note
 * It then calls the i2c_open function at the end and passes in the I2C_OPEN_STRUCT_TypeDef that we defined within the function.
 *
 * @param[in] void
 *
 *
 ******************************************************************************/

void si7021_i2c_open(){


  timer_delay(SI7021_POWERON_DELAY);
  I2C_OPEN_STRUCT_TypeDef I2C_si;

  I2C_si.freq = I2C_freq;
  I2C_si.clhr = i2cClockHLRAsymetric;;
  I2C_si.ROUTEsda = I2C_ROUTE_SDA;
  I2C_si.ROUTEscl = I2C_ROUTE_SCL;
  I2C_si.scl_pin_en = true;
  I2C_si.sda_pin_en = true;
  I2C_si.enable =  true;
  I2C_si.master = true;
  I2C_si.refFreq = 0;


  i2c_open(SI7021_I2C, &I2C_si);


}

/***************************************************************************/
/**
 * @brief
 * si7021 read_helper function
 *
 * @details
 *  This function is used to assign values to the state machine that are unique for the current operation you want to complete
 *
 *
 * @note
 *This function allows the application driver to access the private variables with the Si7021.c file.
 *
 * @param[in] uint8_t command, uint8_t bytes, uint32_t callback
 *
 *
 ******************************************************************************/


void SI7021_Read_Helper(uint8_t command, uint8_t bytes, uint32_t callback){
  STATE_MACHINE_START_STRUCT startStruct;
  startStruct.newDeviceAddress = SI7021_Address;
  read_result = 0;
  if(command == 0xF5){
      startStruct.newBufferAddress = &read_result;
  }
  else{
      startStruct.newBufferAddress = &temp_result;
  }
  startStruct.newRead = true;
  startStruct.newCommand = command;
  startStruct.newBytesleft = bytes;
  startStruct.newCallBack = callback;
  startStruct.newNumCmdBytes = 1;


  i2c_start(SI7021_I2C, &startStruct);


}

void SI7021_Write_Helper(uint8_t bytes, uint8_t command){
  STATE_MACHINE_START_STRUCT startStruct;
  startStruct.newDeviceAddress = SI7021_Address;
  read_result = 0;
  startStruct.newBufferAddress = &writeValue;
  startStruct.newRead = false;
  startStruct.newCommand = command;
  startStruct.newBytesleft = bytes;
  startStruct.newNumCmdBytes = 1;

  i2c_start(SI7021_I2C, &startStruct);
}



uint32_t decode_temp(uint8_t temp) // for temp & ment to use my array instead, not for this lab disregard
{
  float actual_temp;
  uint32_t rounded_temp;

  // Formula to decode read Temperature from the Si7021 Datasheet
  actual_temp = (((temp) * 175.72) / 65536) - 46.85;

  // Round the temperature to an integer value
  rounded_temp = (uint32_t)(actual_temp + 0.5 - (actual_temp < 0));

  return rounded_temp;
}

uint32_t get_Si7021_temp(void){
  return decode_temp(temp_result);
}


/***************************************************************************//**
 * Function to decode Relative Humidity from the read value
 *
 * This function decodes RH data using the formula provided in the Si7021
 * datasheet. Returns the value in Percent.
 ******************************************************************************/

float decode_rh(float humidity){

  humidity = (((humidity) * 125) / 65536) - 6;

  return humidity;
}


/***************************************************************************/
/**
 * @brief
 *  get_si7021_rh
 * @details
 *  This function returns the relative humidity that the sensor recorded.
 * @note
 * The function calls the decode rh function and passes in the private read_result variable that has the recorded data from the sensor. The
 * get_si7021_rh function then returns the decoded value.
 *
 * @param[in] void
 *
 *
 ******************************************************************************/

float get_si7021_rh(void) {
  return decode_rh(read_result); // need to share the private variable
}
