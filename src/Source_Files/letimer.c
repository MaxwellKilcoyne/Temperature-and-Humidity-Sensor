/**
 * @file letimer.c
 * @author Max Kilcoyne
 * @date September 18th, 2022
 * @brief Contains all the LETIMER driver functions
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Libraries

//** Silicon Lab include files

//** User/developer include files
#include "letimer.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************

static uint32_t scheduled_comp0_cb;
static uint32_t scheduled_comp1_cb;
static uint32_t scheduled_uf_cb;


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief
 *   Driver to open an set an LETIMER peripheral in PWM mode
 *
 * @details
 * 	 This routine is a low level driver.  The application code calls this function
 * 	 to open one of the LETIMER peripherals for PWM operation to directly drive
 * 	 GPIO output pins of the device and/or create interrupts that can be used as
 * 	 a system "heart beat" or by a scheduler to determine whether any system
 * 	 functions need to be serviced.
 *
 * @note
 *   This function is normally called once to initialize the peripheral and the
 *   function letimer_start() is called to turn-on or turn-off the LETIMER PWM
 *   operation.
 *
 * @param[in] letimer
 *   Pointer to the base peripheral address of the LETIMER peripheral being opened
 *
 * @param[in] app_letimer_struct
 *   Is the STRUCT that the calling routine will use to set the parameters for PWM
 *   operation
 *
 ******************************************************************************/
void letimer_pwm_open(LETIMER_TypeDef *letimer, APP_LETIMER_PWM_TypeDef *letimerPWM){
	LETIMER_Init_TypeDef letimer_pwm_values;
//	sleep_block_mode(LETIMER_EM);



	unsigned int period_cnt;
	unsigned int period_active_cnt;

	/*  Initializing LETIMER for PWM mode */
	if(LETIMER0 == letimer){
	    CMU_ClockEnable(cmuClock_LETIMER0, true);
	}


	letimer_start(letimer,false);
	/*  Enable the routed clock to the LETIMER0 peripheral */


//	 Use EFM_ASSERT statements to verify whether the LETIMER clock tree is properly
//	 * configured and enabled
//	 * You must select a register that utilizes the clock enabled to be tested
//	 *
	letimer->CMD = LETIMER_CMD_START;
	while(letimer->SYNCBUSY);
	EFM_ASSERT(letimer->STATUS & LETIMER_STATUS_RUNNING);
	letimer->CMD = LETIMER_CMD_STOP;
	while(letimer->SYNCBUSY);
//	 * With the LETIMER regiters being in the low frequency clock tree, you must
//	 * use a while SYNCBUSY loop to verify that the write of the register has propagated
//	 * into the low frequency domain before reading it.


	// Must reset the LETIMER counter register since enabling the LETIMER to verify that
	// the clock tree has been correctly configured to the LETIMER may have resulted in
	// the counter counting down from 0 and underflowing which by default will load
	// the value of 0xffff.  To load the desired COMP0 value quickly into this
	// register after complete initialization, it must start at 0 so that the underflow
	// will happen quickly upon enabling the LETIMER loading the desired top count from
	// the COMP0 register.

	// Reset the Counter to a know value such as 0
	letimer->CNT = 0;	// What is the register enumeration to use to specify the LETIMER Counter Register?

	// Initialize letimer for PWM operation
	// XXX are values passed into the driver via app_letimer_struct
	// ZZZ are values that you must specify for this PWM specific driver
	letimer_pwm_values.bufTop = false;		// Comp1 will not be used to load comp0, but used to create an on-time/duty cycle
	letimer_pwm_values.comp0Top = true;		// load comp0 into cnt register when count register underflows enabling continuous looping
	letimer_pwm_values.debugRun = letimerPWM->debugRun;
	letimer_pwm_values.enable = letimerPWM->enable;
	letimer_pwm_values.out0Pol = 0;			// While PWM is not active out, idle is DEASSERTED, 0
	letimer_pwm_values.out1Pol = 0;			// While PWM is not active out, idle is DEASSERTED, 0
	letimer_pwm_values.repMode = letimerRepeatFree;	// Setup letimer for free running for continuous looping
	letimer_pwm_values.ufoa0 = letimerUFOAPwm ;		// Using the HAL documentation, set to PWM mode
	letimer_pwm_values.ufoa1 = letimerUFOAPwm ;		// Using the HAL documentation, set to PWM mode


	LETIMER_Init(letimer, &letimer_pwm_values);		// Initialize letimer


	/* Calculate the value of COMP0 and COMP1 and load these control registers
	 * with the calculated values
	 */
	period_cnt = letimerPWM->period * LETIMER_HZ;
	letimer->COMP0 = period_cnt;
	period_active_cnt = letimerPWM->active_period * LETIMER_HZ;
	letimer->COMP1 = period_active_cnt;

	while(letimer->SYNCBUSY);


	/* Set the REP0 mode bits for PWM operation directly since this driver is PWM specific.
	 * Datasheets are very specific and must be read very carefully to implement correct functionality.
	 * Sometimes, the critical bit of information is a single sentence out of a 30-page datasheet
	 * chapter.  Look careful in the following section of the Pearl Gecko Reference Manual,
	 * 20.3.4 Underflow Output Action, to learn how to correctly set the REP0 and REP1 bits
	 *
	 * Use the values from app_letimer_struct input argument for ROUTELOC0 and ROUTEPEN enable
	 */
letimer->REP0 = 12;
letimer->REP1 = 15;
letimer->ROUTELOC0 = letimerPWM->out_pin_route0;
letimer->ROUTELOC0 |= letimerPWM->out_pin_route1;
letimer->ROUTEPEN = letimerPWM->out_pin_0_en * LETIMER_ROUTEPEN_OUT0PEN;
letimer->ROUTEPEN |= letimerPWM->out_pin_1_en * LETIMER_ROUTEPEN_OUT1PEN;



scheduled_comp0_cb = letimerPWM->comp0_cb;
scheduled_comp1_cb = letimerPWM->comp1_cb;
scheduled_uf_cb = letimerPWM->uf_cb;



	/* We are not enabling any interrupts at this tie.  If you were, you would enable them now */

//letimerPWM interrupt enable values are defined on app.c

letimer->IFC = letimer->IF;

letimer->IEN |= letimerPWM->comp0_irq_enable * LETIMER_IEN_COMP0;
letimer->IEN |= letimerPWM->comp1_irq_enable * LETIMER_IEN_COMP1;
letimer->IEN |= letimerPWM->uf_irq_enable * LETIMER_IEN_UF;
//letimer->IFS = LETIMER_IF_COMP0;
//letimer->IFS = LETIMER_IF_COMP1;
//letimer->IFS = LETIMER_IF_UF;



NVIC_EnableIRQ(LETIMER0_IRQn);


	/* We will not enable or turn-on the LETIMER0 at this time */

if(letimer->STATUS == 0){
    sleep_block_mode(LETIMER_EM);
}

}

/***************************************************************************//**
 * @brief
 *enables the letimer.
 *
 * @details
 * parameters into the function are an address to LETIMER_TypeDef struct and a boolean.
 *
 * Calls the function LETIMER_Enable and passes in the struct and boolean to enable the letimer.

 *
 * @note
 *
 *
 ******************************************************************************/

void letimer_start(LETIMER_TypeDef *letimer, bool enable){


if(enable == true && letimer->STATUS == notRunning){
    sleep_block_mode(LETIMER_EM);
    LETIMER_Enable(letimer, enable);
    while(letimer->SYNCBUSY);
}
else if(enable == false && letimer->STATUS == running){
    sleep_unblock_mode(LETIMER_EM);
    LETIMER_Enable(letimer, enable);
}
else{

}



}

/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief This is the Interrupt service routine function for the Letimer
 *
 *
 * @details
 * This function checks if the interrupt is flagged and enabled, if it is flagged and enabled then
 * it is added to the schedule.
 *
 *
 * @note
 * There are also EFM_ASSERT statements to check before each event is scheduled
 *
 *
 * @param[in] void
 *
 * @param[in] void
 *
 ******************************************************************************/

void LETIMER0_IRQHandler(void){
  uint32_t int_flag;
  int_flag = (LETIMER0->IF) & (LETIMER0->IEN);
  LETIMER0->IFC = int_flag;

  if(int_flag & LETIMER_IF_COMP0){
      EFM_ASSERT(LETIMER_IF_COMP0 & LETIMER0->IF);
      add_scheduled_events(scheduled_comp0_cb);
  }
  if(int_flag & LETIMER_IF_COMP1){
      EFM_ASSERT(!(LETIMER_IF_COMP1 & LETIMER0->IF));
      add_scheduled_events(scheduled_comp1_cb);
  }
  if(int_flag & LETIMER_IF_UF){
      EFM_ASSERT(!(LETIMER_IF_UF & LETIMER0->IF));
      add_scheduled_events(scheduled_uf_cb);
  }










  // private variables scheduled_cb are being anded into event_scheduled in scheduler.c




//  EFM_ASSERT(!(LETIMER_IF_COMP0 & LETIMER0->IF));


//  int_flag = (LETIMER_IF_COMP0) & (LETIMER_IEN_COMP0);
//  LETIMER0->IFC = int_flag;
//  EFM_ASSERT(!(LETIMER_IF_COMP0 & LETIMER0->IF));



//  int_flag = (LETIMER_IF_COMP1) & (LETIMER_IEN_COMP1);
//  LETIMER0->IFC= int_flag;
//  EFM_ASSERT(!(LETIMER_IF_COMP1 & LETIMER0->IF));



//  int_flag = (LETIMER_IF_UF) & (LETIMER_IEN_UF);
//  LETIMER0->IFC = int_flag;
//  EFM_ASSERT(!(LETIMER_IF_UF & LETIMER0->IF));



}

