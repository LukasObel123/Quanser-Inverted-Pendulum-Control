//Documentation Session
//MRE6183_pwm.h
#ifndef __MRE6183_PWM_H
#define __MRE6183_PWM_H

//Preprocesser and Header Files
#include "stm32f10x.h" //including STM32 header file


//Macros

//Function Prototypes
void PWM_Configuration(); //prototyping PWM_Configuration function. Will setup Tim3 for use with PWM
void PWM_SetValue (uint16_t PWM_value); //prototyping a PWM_SetValue function that will set the PWM duty cycle
void PWM_Enable(); //Enable PWM
void PWM_Disable(); //Disable PWM



#endif /*__MRE6183_PWM_H */
