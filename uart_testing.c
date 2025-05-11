#include "stm32f10x.h"
#include "../func_lib/MRE6183_usart.h"
#include "../func_lib/MRE6183_delays.h"
#include <stdio.h>

char uart_test_string[100];  // buffer for UART output

int main(void) {
		SystemInit();
    UART_Init();         // Initialize UART
    TimerDelay_Init();   // Enable delay_ms()
    
    // Clear screen and move cursor to top-left
    sprintf(uart_test_string, "\033[2J\033[H");
    UART_StringOut(uart_test_string);
    UART_StringOut("UART Testing Started...\r\n");

    int count = 0;
    float test_float = 3.141;

    while (1) {
        sprintf(uart_test_string, "Count: %d | Test Float: %.3f\r\n", count++, test_float);
        UART_StringOut(uart_test_string);
        delay_ms(1000);  // Wait 1 second
    }
}
