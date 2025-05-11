// pwm_test.c

#include "stm32f10x.h"    // Or your specific MCU header
#include "../func_lib/MRE6183_pwm.h"  // Your PWM setup header
#include "../func_lib/MRE6183_delays.h" // Your delay function

int main(void) {
    static uint16_t pwm_value = 0;
	
	TimerDelay_Init();
			/*PWM CONFIG*/
		PWM_Configuration(); //calling the PWM_Configuration function
		PWM_Enable();
	
    while (1) { // Infinite loop
        PWM_SetValue(pwm_value); // Your function to set PWM duty cycle
        delay_ms(5000);     // Wait 5 seconds
				
		
        pwm_value += 50;    // Increase PWM by 50

        if (pwm_value > 1000) {
            pwm_value = 0;  // Reset back to 0 after reaching 1000
        }
    }
}
