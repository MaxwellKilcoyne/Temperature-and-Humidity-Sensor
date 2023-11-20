/**
 * @file app.c
 * @author Max kilcoyne
 * @date 09/15/22
 * @brief The application file for app.c
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************

static void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *app_peripheral_setup will call cmu_open, gpio_open, app_letimer_pwm_open, and letimer_start to setup the peripheral.
 *
 * @details
 *  it is a void function with no parameters
 *
 * @note
 *
 *
 ******************************************************************************/

void app_peripheral_setup(void){
  cmu_open();
  sleep_open();
  gpio_open();
  scheduler_open();
  gpio_open();
  si7021_i2c_open();
  SH_I2C_open();
  app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1);
  letimer_start(LETIMER0, true);   // letimer_start will inform the LETIMER0 peripheral to begin counting.
}

/***************************************************************************//**
 * @brief
 *
 *
 * This function defines a struct of type APP_LETIMER_PWM_Typedef and then we set all of it's elements to the values
 * that are passed into the function. The function letimer_pwm_open is called with the struct we created.
 *
 * @param[in] period, act_period, out0_route, out1_route
 * period: is the period that the led will blink
 * act_period: is the active period defined
 * out0_route: is the route to the led0.
 * out1_route is the route to led1.
 *
 ******************************************************************************/
void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route){
  // Initializing LETIMER0 for PWM operation by creating the
  // letimer_pwm_struct and initializing all of its elements
  APP_LETIMER_PWM_TypeDef letimerPWM;

  letimerPWM.active_period = act_period;
  letimerPWM.debugRun = false;
  letimerPWM.enable = false;
  letimerPWM.out_pin_0_en = true;
  letimerPWM.period = period;
  letimerPWM.out_pin_1_en = true;
  letimerPWM.out_pin_route0 = out0_route;
  letimerPWM.out_pin_route1 = out1_route;
  letimerPWM.comp0_irq_enable = false;
  letimerPWM.comp1_irq_enable = true;
  letimerPWM.uf_irq_enable = true;
  letimerPWM.comp0_cb = LETIMER0_COMP0_CB;
  letimerPWM.comp1_cb = LETIMER0_COMP1_CB;
  letimerPWM.uf_cb = LETIMER0_UF_CB;


  letimer_pwm_open(LETIMER0, &letimerPWM);


}
/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief: removes the scheduled the letimer0 UF event via the callback if the EFM_ASSERT(!(get_scheduled_events() & LETIMER0_UF_CB)) is passed
 *
 *
 *
 * @param[in] void
 *
 * @param[in] void
 *
 ******************************************************************************/

void scheduled_letimer0_UF_cb(void){
  EFM_ASSERT(!(get_scheduled_events() & LETIMER0_UF_CB));
  SI7021_Read_Helper(SI7021_CMD_MEASURE_RH_NO_HOLD, 2, SI7021_READ_CB);
  SI7021_Read_Helper(SI7021_CMD_MEASURE_TEMP, 2, SI7021_READ_TEMP_CB);
  shtc3_read_data_and_crc(SH_CB);




}


/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief:  makes sure the COMP0 interrupt is not scheduled

 * @param[in] void
 *
 * @param[in] void
 *
 ******************************************************************************/
void scheduled_letimer0_COMP0_cb(void){

  EFM_ASSERT(false);
  //  EFM_ASSERT(!(get_scheduled_events() & LETIMER0_COMP0_CB));
//  remove_scheduled_event(LETIMER0_COMP0_CB);
}

/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief: removes COMP1 scheduled event if the scheduled event doesn't match the
 *

 * @param[in] void
 *
 * @param[in] void
 *
 ******************************************************************************/

void scheduled_letimer0_COMP1_cb(void){
  EFM_ASSERT(!(get_scheduled_events() & LETIMER0_COMP1_CB));
  remove_scheduled_events(LETIMER0_COMP1_CB);
}

/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief: creates the scheduled event for the gpio odd event
 *
 * @note: increments the energy mode
 *
 *
 * @param[in] void
 *
 * @param[in] void
 *
 ******************************************************************************/

void scheduled_gpio_odd_irq_cb(void){
  uint32_t current_block_em;
  current_block_em = current_block_energy_mode();
  sleep_unblock_mode(current_block_em);
  if(current_block_em < 4){
      sleep_block_mode(current_block_em + 1);
  }
  else{
      sleep_block_mode(EM0);
  }

}
/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief: creates the scheduled event for the gpio even event
 *
 * @note: decrements the energy mode
 *
 *
 * @param[in] void
 *
 * @param[in] void
 *
 ******************************************************************************/

void scheduled_gpio_even_irq_cb(void){
  uint32_t current_block_em;
  current_block_em = current_block_energy_mode();
  sleep_unblock_mode(current_block_em);
  if(current_block_em > 0){
      sleep_block_mode(EM4 - 1);
  }
  else{
      sleep_block_mode(EM4);
  }


}

/***************************************************************************/
/**
 * @brief
 *  scheduled_si7021_read_cb function
 * @details
 * This function gets the relative humidity reading and then either turns on the led or keeps the led off depending the what the relative humidity reading is.
 *
 * @param[in] void
 *
 *
 ******************************************************************************/


void scheduled_si7021_read_cb(void) {
  float humidity = get_si7021_rh();
  if (humidity >= SI7021_HUMIDITY_LED_THRESHOLD){
      GPIO_PinOutSet(LED1_PORT, LED1_PIN);
  } else {
      GPIO_PinOutClear(LED1_PORT, LED1_PIN);
  }
}

void scheduled_si7021_read_temp_cb(void){
  uint32_t temp = get_Si7021_temp();
//  EFM_ASSERT(temp > 0); tried to get rid of the warning but it didnt work so I just commented it out
}

void scheduled_SHTC3_read_cb(void){
  float H;
  float T;
  shtc3_app_get_temp_and_hum(&T, &H);
}



