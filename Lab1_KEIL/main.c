#include <stdint.h>
#include <stdbool.h>
//#include "driverlib/sysctl.h"
//#include "Task1.h"
//#include "driverlib/systick.h"
//#include "driverlib/interrupt.h"
//#include "inc/hw_memmap.h"
//#include "inc/hw_gpio.h"
//#include "inc/hw_types.h"
#include <stdio.h>
#include <stdlib.h>
//#include "driverlib/timer.h"
//#include "driverlib/gpio.h"
#include "tm4c123gh6pm.h"

#define LED_RED   (1U << 1)
#define LED_BLUE  (1U << 2)
#define LED_GREEN (1U << 3)

static uint32_t volatile l_tickCtr;
uint32_t start;
uint32_t ticks_red=5; //LED_RED TICKS
uint32_t ticks_blue=10; //LED_BLUE TICKS

void main_blinky1(){
  while(1){
  GPIO_PORTF_DATA_R = LED_RED;
  __asm("CPSID I");
    start=l_tickCtr;
    __asm("CPSIE I");
    while((l_tickCtr-start)<ticks_red){}
    GPIO_PORTF_DATA_R &= ~LED_RED;
    __asm("CPSID I");
    start=l_tickCtr;
    __asm("CPSIE I");
    while((l_tickCtr-start)<ticks_red){}
  }
}
void main_blinky2(){
  while(1){
  GPIO_PORTF_DATA_R = LED_BLUE;
  __asm("CPSID I");
    start=l_tickCtr;
    __asm("CPSIE I");
    while((l_tickCtr-start)<ticks_blue){}
    GPIO_PORTF_DATA_R &= ~LED_BLUE;
    __asm("CPSID I");
    start=l_tickCtr;
    __asm("CPSIE I");
    while((l_tickCtr-start)<ticks_blue){}
  }
}
int main()
{
    __asm("CPSID I");
    SYSCTL_RCGCGPIO_R=0x20;
    GPIO_PORTF_DIR_R=0x0E;
    GPIO_PORTF_DEN_R=0x0E;
    NVIC_ST_RELOAD_R=0xFFFFFF;
    NVIC_ST_CTRL_R=7;
    __asm("CPSIE I");

   
      main_blinky1();
      main_blinky2();
     

}
void SysTick_Handler(void) {
    ++l_tickCtr;
}
