/**
 * @file cmu.c
 * @author Max Kilcoyne
 * @date September 18th
 * @brief Contains all the letimer PWM open function
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief
 * initializes the clocks necessary to make the led blink.
 *
 *
 * @param[in] address of LETIMER_TypeDef struct, and address of APP_LETIMER_PWM_TypeDef struct
 *
 *
 ******************************************************************************/


void cmu_open(void){

    CMU_ClockEnable(cmuClock_HFPER, true);

    // By default, Low Frequency Resistor Capacitor Oscillator, LFRCO, is enabled,
    // Disable the LFRCO oscillator
    CMU_OscillatorEnable(cmuOsc_LFRCO  , false, false);  // What is the enumeration required for LFRCO?

    // Disable the Low Frequency Crystal Oscillator, LFXO
    CMU_OscillatorEnable(cmuOsc_LFXO , false, false);  // What is the enumeration required for LFXO?

    // No requirement to enable the ULFRCO oscillator.  It is always enabled in EM0-4H

    // Route LF clock to LETIMER0 clock tree
    CMU_ClockSelectSet(cmuClock_LFA , cmuSelect_ULFRCO); // What clock tree does the LETIMER0 reside on?

    // Now, you must ensure that the global Low Frequency is enabled
    CMU_ClockEnable(cmuClock_CORELE , true); //This enumeration is found in the Lab 2 assignment

}

