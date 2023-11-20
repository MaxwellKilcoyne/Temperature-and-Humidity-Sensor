/**
 * @file gpio.c
 * @author Max Kilcoyne
 * @date January 18th 2022
 * @brief Contains the gpio open function
 *
 */



//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
static uint32_t gpio_even_irq_cb = GPIO_EVEN_IRQ_CB;
static uint32_t gpio_odd_irq_cb = GPIO_ODD_IRQ_CB;


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @author Max Kilcoyne
 *
 * @brief
 * enables the clock, calls GPIO_DriveStrengthSet and GPIO_PinModeSet for LED0 and LED1.
 *
 *
 * @details  It is a void function that returns nothing and takes in no parameters.
 *
 * @note
 *
 *
 ******************************************************************************/

void gpio_open(void){

  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure LED pins
  GPIO_DriveStrengthSet(LED0_PORT, LED0_DRIVE_STRENGTH);
  GPIO_PinModeSet(LED0_PORT, LED0_PIN, LED0_GPIOMODE, LED0_DEFAULT);

  GPIO_DriveStrengthSet(LED1_PORT, LED1_DRIVE_STRENGTH);
  GPIO_PinModeSet(LED1_PORT, LED1_PIN, LED1_GPIOMODE, LED1_DEFAULT);

  GPIO_PinModeSet(BUTTON_0_PORT, BUTTON_0_PIN, BUTTON_0_CONFIG, BUTTON_DEFAULT);

  GPIO_PinModeSet(BUTTON_1_PORT, BUTTON_1_PIN, BUTTON_1_CONFIG, BUTTON_DEFAULT);

  GPIO_ExtIntConfig(BUTTON_0_PORT, BUTTON_0_PIN, BUTTON_0_INT_NUM, BUTTON_0_INT_RISING, BUTTON_0_INT_FALLING, BUTTON_0_INT_ENABLE);

  NVIC_EnableIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  GPIO_DriveStrengthSet(SI7021_SENSOR_EN_PORT, gpioDriveStrengthWeakAlternateWeak);
  GPIO_PinModeSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_PIN, gpioModePushPull, 1);

  GPIO_PinModeSet(SI7021_SCL_PORT, SI7021_SCL_PIN, gpioModeWiredAnd, 1);
  GPIO_PinModeSet(SI7021_SDA_PORT, SI7021_SDA_PIN, gpioModeWiredAnd, 1);
 //7
  //9
  GPIO_PinModeSet(SH_SCL_Port, SH_SCL_Pin, gpioModeWiredAnd, 1);
  GPIO_PinModeSet(SH_SDA_Port, SH_SDA_Pin, gpioModeWiredAnd, 1);



}

/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief: GPIO ODD interrupt service handler
 *  adds the gpio even event to the schedule
 *
 *
 * @param[in] void
 *
 * @param[in] void
 *
 ******************************************************************************/

void GPIO_ODD_IRQHandler(void){

  uint32_t int_flag;
  int_flag = (GPIO->IF) & (GPIO->IEN);
  GPIO->IFC = int_flag;
  add_scheduled_events(gpio_even_irq_cb);

}

/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief: GPIO EVEN interrupt service handler function
 *  adds the gpio even event to the schedule if it is flagged and enabled
 *
 *
 * @param[in] void
 *
 * @param[in] void
 *
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void){
  uint32_t int_flag;
  int_flag = (GPIO->IF) & (GPIO->IEN);
  GPIO->IFC = int_flag;
  add_scheduled_events(gpio_odd_irq_cb);

}
