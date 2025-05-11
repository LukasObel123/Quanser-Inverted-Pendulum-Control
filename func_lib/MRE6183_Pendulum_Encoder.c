// Pendulum_Encoder.c

#include "../func_lib/MRE6183_Pendulum_Encoder.h" 
// Define the encoder resolution (4096 counts per revolution)
#define PENDULUM_ENCODER_RES 4096

// Global variable to keep track of full rotations (initialized to -1 as a hack)
int32_t pendulum_full_rotations = -1;

void Pendulum_Encoder_Configuration(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // Configure NVIC for TIM1 update interrupt.
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM16_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Initialize the GPIO and TIM structures to default values.
    GPIO_StructInit(&GPIO_InitStructure);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    // Enable clock on TIM1 and GPIOA.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Configure PA8 and PA9 for floating input mode (for encoder signals).
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure TIM1 base for encoder interface.
    TIM_TimeBaseStructure.TIM_Period = PENDULUM_ENCODER_RES - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // Set up the encoder interface (both channels trigger on rising edges).
    TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    // Enable the update interrupt.
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
}

void Pendulum_ResetValue(void) {
    //TIM1->CNT = 0;
	  TIM1->CNT                  = PENDULUM_ENCODER_RES / 2;  // half-turn  
    //pendulum_full_rotations    = -1;  // makes encoder start at -pi
		pendulum_full_rotations 	 = 0; 	// makes encoder start at pi
}

uint32_t Pendulum_ReadRaw(void) {
    return TIM1->CNT;
}

float Pendulum_ReadValueRad(void) {
    // Convert the reading to radians:
    // full rotations * 2pi plus the current counter fraction.
    return pendulum_full_rotations * (2 * 3.14f) + TIM1->CNT * (2 * 3.14f / PENDULUM_ENCODER_RES);
}

void Pendulum_Enable(void) {
    TIM_Cmd(TIM1, ENABLE);
}

void Pendulum_Disable(void) {
    TIM_Cmd(TIM1, DISABLE);
}

// TIM1 update interrupt handler for full rotation counting.
void TIM1_UP_TIM16_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update)) {
        // If the current count is very low, assume a counter wrap-around from a decrement.
        if (TIM1->CNT <= 100) {
            pendulum_full_rotations += 1;
        }
        // If the count is near the period (within 100 counts), assume a wrap-around decrement.
        else if (TIM1->CNT >= (PENDULUM_ENCODER_RES - 100)) {
            pendulum_full_rotations -= 1;
        }
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}
