#ifndef MRE6183_PENDULUM_ENCODER_H
#define MRE6183_PENDULUM_ENCODER_H



#include "stm32f10x.h"  // Adjust include to your MCU support package if necessary

// Configures TIM1 and related GPIO pins for the pendulum encoder.
void Pendulum_Encoder_Configuration(void);

// Resets the TIM1 counter value for the pendulum encoder.
void Pendulum_ResetValue(void);

// Returns the raw counter value from TIM1 (pendulum encoder).
uint32_t Pendulum_ReadRaw(void);

// Returns the pendulum encoder value converted to radians.
float Pendulum_ReadValueRad(void);

// Enables TIM1 to start encoder counting.
void Pendulum_Enable(void);

// Disables TIM1 encoder counting.
void Pendulum_Disable(void);

#endif /* MRE6183_PENDULUM_ENCODER_H */
