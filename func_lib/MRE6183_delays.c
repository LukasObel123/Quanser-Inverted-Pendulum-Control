//Documentation Section

#include "..\func_lib\MRE6183_delays.h" //including STM32 header file


void Delay(__IO uint32_t nCount){
//delay function
for(; nCount != 0; nCount--);
}

void TimerDelay_Init(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);

	// set the prescaler to change the clock from the SystemCoreClock (24 MHz) to 1 MHz
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock /1000000) - 1;
	// set the period to maximum (16-bit)
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
		
		TIM_Cmd(TIM7,ENABLE); //enable timer 7
}

void delay_us(uint16_t delay_time_us){
	TIM1->CNT = 0;
	while(TIM7->CNT < delay_time_us);
}

void delay_ms(uint16_t delay_time_ms){
	while(delay_time_ms > 0){
		TIM7->CNT = 0;
		delay_time_ms--;
		while(TIM7->CNT < 1000);
	}
}

void TimerInterrupt_Init(uint16_t timer_freq){
/*This function will setup TIM6 at a fixed frequency with an update interrupt.*/

    //create instances of the time base and NVIC setup structures
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE); //Enabling clock on TIM6

    /* setup TIM6 to count up from 0 to a known value (1999) such
that the overall frequency matches the input*/
    TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / timer_freq / 2000) - 1;
    TIM_TimeBaseStructure.TIM_Period = 2000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    /*Enable the update interrupt on TIM6*/
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); 
    
    /*setup the NVIC with highest priority (0)*/
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*Enable TIM6*/
    TIM_Cmd(TIM6,ENABLE); //enable timer 6
}