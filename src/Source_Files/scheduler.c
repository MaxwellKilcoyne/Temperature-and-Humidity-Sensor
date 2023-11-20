/*
 * scheduler.c
 *
 *  Created on: Sep 22, 2022
 *      Author: mjkil
 */



#include "scheduler.h"
#include "em_assert.h"
#include "em_core.h"
#include "em_emu.h"





//static Variables;

static uint32_t event_scheduled;

/*
 * This function lets the clearing of an event take priorit over everything else so that it can complete it's function before any
 * other interrupts are called
 * To prevent a static or shared resource data coherency or timing issue,
access to private (static) variables should always be atomic if they can be
altered by an Interrupt Service Routine or an external asynchronous
function or event
 */
//void schedule_function(void){
//  CORE_DECLARE_IRQ_STATE;
//  CORE_ENTER_CRITICAL();
//  event_scheduled = event_scheduled & ~remove_scheduled_event(event_scheduled);
//  CORE_EXIT_CRITICAL();
//
//}



void scheduler_open(void){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  event_scheduled = 0;
  CORE_EXIT_CRITICAL();

}
void add_scheduled_events(uint32_t event){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  event_scheduled |= event;
  CORE_EXIT_CRITICAL();

}
void remove_scheduled_events(uint32_t event){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  event_scheduled &= ~event;
  CORE_EXIT_CRITICAL();

}

uint32_t get_scheduled_events(void){

  return event_scheduled;
}

