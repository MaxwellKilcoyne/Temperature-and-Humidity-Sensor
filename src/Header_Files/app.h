//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef APP_HG
#define APP_HG

/* System include statements */


/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_assert.h"

/* The developer's include statements */
#include "cmu.h"
#include "gpio.h"
#include "letimer.h"
#include "brd_config.h"
#include "scheduler.h"
#include "sleep_routine.h"
#include "I2C.h"
#include "Si7021.h"


// Application scheduled events
#define LETIMER0_COMP0_CB 0b000000001 //0b0001
#define LETIMER0_COMP1_CB 0b000000010 //0b0010
#define LETIMER0_UF_CB 0b000000100 //0b0100

#define GPIO_ODD_IRQ_CB 0b000001000
#define GPIO_EVEN_IRQ_CB 0b000010000

#define SI7021_READ_CB 0b000100000
#define SI7021_READ_TEMP_CB 0b010000000
#define SH_CB               0b100000000

#define SI7021_HUMIDITY_LED_THRESHOLD 30




//***********************************************************************************
// defined files
//***********************************************************************************
#define   PWM_PER       3.0   // PWM period in seconds
#define   PWM_ACT_PER     0.002  // PWM active period in seconds




//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void app_peripheral_setup(void);
void scheduled_letimer0_UF_cb(void);
void scheduled_letimer0_COMP0_cb(void);
void scheduled_letimer0_COMP1_cb(void);
void scheduled_gpio_odd_irq_cb(void);
void scheduled_gpio_even_irq_cb(void);
void scheduled_si7021_read_cb(void);
void scheduled_si7021_read_temp_cb(void);


#endif
