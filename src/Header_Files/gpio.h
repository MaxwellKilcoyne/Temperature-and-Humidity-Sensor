//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef GPIO_HG
#define GPIO_HG

/* System include statements */


/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_assert.h"
#include "scheduler.h"
#include "app.h"

/* The developer's include statements */
#include "brd_config.h"

//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpio_open(void);
void GPIO_ODD_IRQHandler(void);
void GPIO_EVEN_IRQHandler(void);

#endif
