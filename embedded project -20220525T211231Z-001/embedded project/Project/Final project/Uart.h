#include <stdint.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include <stdlib.h>


//   Function Prototypes

void UART0_Init(void);
void printString(char *string);
void printChar(char c);
char readChar(void);