/**
 * @file SHTC3.c
 * @author Max kilcoyne
 * @date 09/15/22
 * @brief The driver file for the SHTC3 sensor
 *
 */




#include "SHTC3.h"


uint64_t SH_data = 0;
/***************************************************************************/
/**
 * @brief
 *  Helper fuction to complete a read of the SHTC3
 *
 *
 * @details
 *  This fucntion intializes all the values needed to run the state machine for a read of the SHTC3
 *
 * @note
 *
 * @param[in] callback, bytes, numCmdBytes, command
 *   callbakc is used to shceduled an event at the end of the read
 *   bytes is used to tell the efm32 how many bytes of data we are looking to recieve
 *   numCmdBytes is used to tell the efm32 how many bytes the command we are sending is
 *   command is the command we will send to the sensor to tell it what we want it to do
 *
 *
 ******************************************************************************/

void SH_read_helper(uint32_t callback, uint32_t bytes, uint32_t numCmdBytes, uint32_t command){

  STATE_MACHINE_START_STRUCT SH_start;

  SH_start.newBufferAddress = &SH_data;
  SH_start.newBytesleft = bytes;
  SH_start.newNumCmdBytes = numCmdBytes;
  SH_start.newCallBack = callback;
//  SH_start.newCombinedBytes = SH_start.newNumCmdBytes + SH_start.newBytesleft;
  SH_start.newCommand = command;
  SH_start.newDeviceAddress = SH_address;
  SH_start.newRead = true;


  i2c_start(SH_I2C, &SH_start);


}


/***************************************************************************/
/**
 * @brief
 *  Helper fuction to complete a write of the SHTC3
 *
 *
 * @details
 *  This function intializes all the values needed to run the state machine for a write to the SHTC3
 *
 * @note
 *
 * @param[in] callback, numCmdBytes, command
 *   callbakc is used to shceduled an event at the end of the read
 *   numCmdBytes is used to tell the efm32 how many bytes the command we are sending is
 *   command is the command we will send to the sensor to tell it what we want it to do
 *
 *
 ******************************************************************************/
void SH_write_helper(uint32_t command, uint32_t numCmdBytes, uint32_t callback){


  STATE_MACHINE_START_STRUCT SH_start;

  SH_start.newBufferAddress = &SH_data;
  SH_start.newNumCmdBytes = numCmdBytes;
  SH_start.newCallBack = callback;
//  SH_start.newCombinedBytes = SH_start.newNumCmdBytes + SH_start.newBytesleft;
  SH_start.newCommand = command;
  SH_start.newDeviceAddress = SH_address;
  SH_start.newRead = false;
  SH_start.newData = 0;

  i2c_start(SH_I2C, &SH_start);


}

/***************************************************************************/
/**
 * @brief
 *  This function is used to intialize the routing location and clock the we will need to use the SHTC3
 *
 *
 *
 * @note  At the end of the function it calls i2c_open with the values we intialized in our i2c_open_struct
 *
 *
 *
 ******************************************************************************/

void SH_I2C_open(){
  I2C_OPEN_STRUCT_TypeDef SH_open;

  SH_open.ROUTEscl = SH_SCL_Route;
  SH_open.ROUTEsda = SH_SDA_Route;
  SH_open.clhr = i2cClockHLRStandard;
  SH_open.enable = true;
  SH_open.freq = I2C_FREQ_STANDARD_MAX;
  SH_open.master = true;
  SH_open.refFreq = 0;
  SH_open.scl_pin_en = true;
  SH_open.sda_pin_en = true;

  timer_delay(SHTC3_Delay);

  i2c_open(SH_I2C, &SH_open);
}

/***************************************************************************/
/**
 * @brief
 *  shtc3_read_data_crc is used to intialize a write, read, and then another write to the SHTC3 sensor
 *
 * @details
 *  It completes this task by calling the read and write helper functions with various different inputs
 * @note
 *
 * @param[in] callback
 * The input parameter is the call back event
 *
 *
 ******************************************************************************/

void shtc3_read_data_and_crc(uint32_t callback_event){
  SH_write_helper(SHTC3_wakeup_cmd, twoByte,0);
  SH_read_helper(0, twoByte, sixByte, tempFirstReadCmd);
  SH_write_helper(SHTC3_sleep_cmd, twoByte, callback_event);

}

/***************************************************************************/
/**
 * @brief
 *  Helper function to calculate the temp from he raw data we got from the sensor
 *

 * @note This function returns the temperature in farenheight.
 *

 *
 *
 ******************************************************************************/
float shtc3_calc_temp(uint64_t tempData){
  uint32_t raw_t;
  float realTemp;
  raw_t = (tempData >> 32) & tempHelper;

  realTemp = ((((raw_t*175) >> 16) - 45) * 1.8) + 32; // this is for F

  return realTemp;


}

/***************************************************************************/
/**
 * @brief
 *  Helper function to calculate the humidity from he raw data we got from the sensor
 *

 * @note This function returns the humidity as a percentage.
 *

 *
 *
 ******************************************************************************/
float shtc3_calc_hum(uint64_t humData){
  uint32_t raw_h;
  float realHum;
  raw_h = (humData >> 8) & tempHelper;
  realHum = ((raw_h * 100) >> 16);


  return realHum;
}

/***************************************************************************/
/**
 * @brief
 *  Helper function that returns the humidity that can be used by the application layer
 *

 * @note This function returns the humidity as a percentage
 *

 *
 *
 ******************************************************************************/

float get_SH_rh(void) {
  return shtc3_calc_hum(SH_data);
}

/***************************************************************************/
/**
 * @brief
 *  Helper function to return the temp, so the application layer can access it

 * @note This function returns the temperature in farenheight.
 *

 *
 *
 ******************************************************************************/

float get_SH_temp(void) {
  return shtc3_calc_temp(SH_data);
}

/***************************************************************************/
/**
 * @brief
 *  Helper function that returns the temperature and humidity so that the application layer can see them.
 *

 * @note This function returns the temperature in farenheight and humidity as a percentage.
 *

 *
 *
 ******************************************************************************/

void shtc3_app_get_temp_and_hum(float T, float H){
  H = shtc3_calc_hum(SH_data);
  T = shtc3_calc_temp(SH_data);
}
