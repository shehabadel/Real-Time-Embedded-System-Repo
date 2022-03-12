#include "tm4c123gh6pm.h"
#include <stdint.h> 
#include "bsp.h"

void main_blinky1(void);
void main_blinky2(void);
#define LED_RED   (1U << 1)
#define LED_BLUE  (1U << 2)
#define LED_GREEN (1U << 3)

static uint32_t volatile l_tickCtr;
uint32_t start;
uint32_t ticks_red=5; //LED_RED TICKS
uint32_t ticks_blue=10; //LED_BLUE TICKS

uint32_t stack_blinky1[40];
uint32_t *sp_blinky1 = &stack_blinky1[40];
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

uint32_t stack_blinky2[40];
uint32_t *sp_blinky2 = &stack_blinky2[40];
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

    
    
      /*main_blinky1();
      main_blinky2();*/
	
	/* fabricate Cortex-M ISR stack frame for blinky1 */
    *(--sp_blinky1) = (1U << 24);  /* xPSR */
    *(--sp_blinky1) = (uint32_t)&main_blinky1; /* PC */
    *(--sp_blinky1) = 0x0000000EU; /* LR  */
    *(--sp_blinky1) = 0x0000000CU; /* R12 */
    *(--sp_blinky1) = 0x00000003U; /* R3  */
    *(--sp_blinky1) = 0x00000002U; /* R2  */
    *(--sp_blinky1) = 0x00000001U; /* R1  */
    *(--sp_blinky1) = 0x00000000U; /* R0  */
    /* fabricate Cortex-M ISR stack frame for blinky2 */
    *(--sp_blinky2) = (1U << 24);  /* xPSR */
    *(--sp_blinky2) = (uint32_t)&main_blinky2; /* PC */
    *(--sp_blinky2) = 0x0000000EU; /* LR  */
    *(--sp_blinky2) = 0x0000000CU; /* R12 */
    *(--sp_blinky2) = 0x00000003U; /* R3  */
    *(--sp_blinky2) = 0x00000002U; /* R2  */
    *(--sp_blinky2) = 0x00000001U; /* R1  */
    *(--sp_blinky2) = 0x00000000U; /* R0  */

    while (1) {
    }
    

}
void SysTick_Handler(void) {
    ++l_tickCtr;
}