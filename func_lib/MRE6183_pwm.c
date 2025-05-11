//Documentation Session
//MRE6183_pwm.c

//Preprocesser and Header Files
#include "..\func_lib\MRE6183_pwm.h" //including pwm header file


//Other Functions

void PWM_Configuration(){
//Lets create instances of the GPIO, TIM, and TIM_OC setup structures
GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

/* Set the desired PWM frequency in Hz */
uint32_t PWMFreq = 1000;
/* Set the desired PWM max value (100%) */
uint32_t PWMMaxValue = 1000;

//Enable the clock for TIM3 on Advanced Peripheral Bus 1 (APB1)
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

// Enable the clock for GPIOC and AFIO on Advanced Peripheral Bus 2 (APB2)
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); 
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); 

//Lets configure PC8 in AF_PP mode
GPIO_StructInit(&GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
GPIO_Init(GPIOC, &GPIO_InitStructure);

//Use pin remap command to tie TIM3 Chanel 3 to PC8
GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

// Use TIM setup structure to configure TIM3
//MIGHT NEED TO INITIALIZE STRUCTURE LIKE WITH GPIO
TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / PWMFreq / PWMMaxValue) - 1;
TIM_TimeBaseStructure.TIM_Period = PWMMaxValue - 1;
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

//Using TIM_OC setup structure to configure PWM mode
//MIGHT NEED TO INITIALIZE STRUCTURE LIKE WITH GPIO
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_Pulse = 0;
TIM_OC3Init(TIM3, &TIM_OCInitStructure);

}

void PWM_SetValue (uint16_t PWM_value){
//Set the register for PWM value
TIM3->CCR3 = PWM_value;

}

void PWM_Enable(){
// TIM3 counter enable
TIM_Cmd(TIM3, ENABLE);
}

void PWM_Disable(){
// TIM3 counter disable
TIM_Cmd(TIM3, DISABLE);
}


