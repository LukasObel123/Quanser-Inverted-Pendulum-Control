// MRE6183_Cart_Encoder.c

#include "../func_lib/MRE6183_Cart_Encoder.h"

#define CART_ENCODER_RES 4096
#define CART_WHEEL_RADIUS 0.01482975f
#define PI 3.1415926f

static int32_t cart_position_accum = 0;
static uint16_t prev_cart_count = 0;

void Cart_Encoder_Configuration(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    // Configure PB6 and PB7 for encoder input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Encoder mode setup
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12,
                               TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_SetAutoreload(TIM4, 0xFFFF);
    TIM_SetCounter(TIM4, 0);

    // Start timer
    TIM_Cmd(TIM4, ENABLE);

    // Set initial count
    prev_cart_count = TIM4->CNT;
}

void Cart_UpdatePosition(void) {
    uint16_t curr_count = TIM4->CNT;
    int16_t delta = (int16_t)(curr_count - prev_cart_count);
    cart_position_accum += delta;
    prev_cart_count = curr_count;
}


void Cart_ResetValue(void) {
    TIM4->CNT = 0;
    prev_cart_count = 0;
    cart_position_accum = 0;
}


uint32_t Cart_ReadRaw(void) {
    return TIM4->CNT;
}

float Cart_ReadValueRad(void) {
		Cart_UpdatePosition();
    return CART_WHEEL_RADIUS*cart_position_accum * (2.0f * PI / CART_ENCODER_RES);
}


void Cart_Enable(void) {
    TIM_Cmd(TIM4, ENABLE);
}

void Cart_Disable(void) {
    TIM_Cmd(TIM4, DISABLE);
}

