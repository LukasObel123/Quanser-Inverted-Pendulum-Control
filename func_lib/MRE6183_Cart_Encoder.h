// MRE6183_Cart_Encoder.h

#ifndef __MRE6183_Cart_Encoder_H
#define __MRE6183_Cart_Encoder_H

#include "stm32f10x.h"

extern int32_t cart_full_rotations;

void Cart_Encoder_Configuration(void);
void Cart_ResetValue(void);
uint32_t Cart_ReadRaw(void);
float Cart_ReadValueRad(void);
void Cart_Enable(void);
void Cart_Disable(void);
void Cart_UpdatePosition(void);

#endif /* __MRE6183_Cart_Encoder_H */
