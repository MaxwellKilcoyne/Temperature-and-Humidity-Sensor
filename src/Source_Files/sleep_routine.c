/**
 * @file scheduler.c
 * @author Max Kilcoyne
 * @date September 18th, 2022
 * @brief Contains all the LETIMER driver functions
 *
 */

/**************************************************************************

 * @file sleep.c

 ***************************************************************************

 * @section License

 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>

 ***************************************************************************

 *

 * Permission is granted to anyone to use this software for any purpose,

 * including commercial applications, and to alter it and redistribute it

 * freely, subject to the following restrictions:

 *

 * 1. The origin of this software must not be misrepresented; you must not

 *    claim that you wrote the original software.

 * 2. Altered source versions must be plainly marked as such, and must not be

 *    misrepresented as being the original software.

 * 3. This notice may not be removed or altered from any source distribution.

*

 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no

 * obligation to support this Software. Silicon Labs is providing the

 * Software "AS IS", with no express or implied warranties of any kind,

 * including, but not limited to, any implied warranties of merchantability

 * or fitness for any particular purpose or warranties against infringement

 * of any proprietary rights of a third party.

 *

 * Silicon Labs will not be liable for any consequential, incidental, or

 * special damages, or any other relief, or for any claim by any third party,

 * arising from your use of this Software.

 *

**************************************************************************/


/*
 * sleep_routine.c
 *
 *  Created on: Sep 29, 2022
 *      Author: mjkil
 */

#include "sleep_routine.h"

static int lowest_energy_mode [MAX_ENERGY_MODES];

/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief: sets the lowest_energy_mode array to 0
 *
 * @details: It completes this with a while loop
 *
 * @param[in]: void
 *
 * @param[in]: void
 *
 ******************************************************************************/

void sleep_open(void){

  for(int i = 0; i < 5; i++){
      lowest_energy_mode[i] = 0;
  }

}

/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief: enters correct sleep mode
 *
 *
 * @note: It does this within a CORE IRQ STATE function
 *
 * @param[in]: void
 *
 * @param[in]: void
 *
 ******************************************************************************/

void enter_sleep(void){

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  if(lowest_energy_mode[EM0] > 0){
      CORE_EXIT_CRITICAL();
      return;
  }
  else if(lowest_energy_mode[EM1] > 0){
      CORE_EXIT_CRITICAL();
      return;
  }
  else if(lowest_energy_mode[EM2] > 0){
      EMU_EnterEM1();
      CORE_EXIT_CRITICAL();
      return;
  }
  else if(lowest_energy_mode[EM3] > 0){
      EMU_EnterEM2(true);
      CORE_EXIT_CRITICAL();
      return;
  }
  else{
      EMU_EnterEM3(true);
      CORE_EXIT_CRITICAL();
      return;
  }



}

/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief: blocks the sleep mode that we are not allowed to enter
 *          completes this in the CORE IRQ STATE
 *
 * @param[in]: void
 *
 *
 * @param[in]: void
 *
 ******************************************************************************/

void sleep_block_mode(uint32_t EM){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  lowest_energy_mode[EM] += 1;


  CORE_EXIT_CRITICAL();



}

/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief: unblocks the energy modes that we are allowed to enter based on the energy mode we are currently in
 *          Completes this in a CORE IRQ STATE function and calls an EFM ASSERT statement to ensure it was handles properly
 *
 * @param[in]:  uint32_t EM (our current energy state)

 *
 ******************************************************************************/

void sleep_unblock_mode(uint32_t EM){

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  lowest_energy_mode[EM] --;


  EFM_ASSERT(lowest_energy_mode[EM] >= 0); // If the assert statment is called the application if calling more unblock sleep modes than
                                             // block sleep modes so there is an issue.

  CORE_EXIT_CRITICAL();

}

/***************************************************************************//**
 *@author Max Kilcoyne
 *
 * @brief: returns the cureent energy mode that we are in
 *
 *
 * @param[in]: void
 *
 * @param[out]: uint32_t (the current energy mode)
 *
 ******************************************************************************/


uint32_t current_block_energy_mode(void){
//  int i = 0;
//  while(lowest_energy_mode[i] == 0){
////      if(lowest_energy_mode[i] > 0){
////          return lowest_energy_mode[i];
////      }
//      i++;
//      return lowest_energy_mode[i];
//  }
//
//  return 0;
  if(lowest_energy_mode[EM0] > 0){
      return EM0;
  }
  if(lowest_energy_mode[EM1] > 0){
      return EM1;
  }
  if(lowest_energy_mode[EM2] > 0){
      return EM2;
  }
  if(lowest_energy_mode[EM3] > 0){
      return EM3;
  }
  else{
      return EM4;
  }


}


