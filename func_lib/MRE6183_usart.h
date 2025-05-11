#ifndef __MRE6183_UART_H 
#define __MRE6183_UART_H 

// includes go here
#include "stm32f10x.h"

//macros
#define UART_BUFFER_SIZE 50



// function prototypes go here
void UART_Init(void);
void UART_CharOut(char serial_char_out);
void UART_StringOut(char serial_string_out[UART_BUFFER_SIZE]);


#endif /*__MRE6183_UART_H */


