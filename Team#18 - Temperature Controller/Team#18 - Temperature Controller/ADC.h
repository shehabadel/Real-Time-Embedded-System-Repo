#include <FreeRTOS.h>
#include "bsp.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "inc/hw_memmap.h" 
#include "inc/hw_types.h" 
#include "inc/hw_gpio.h"
#include "TM4C123.h"



void adc_init(void); //pe3

unsigned int adc_read (void);