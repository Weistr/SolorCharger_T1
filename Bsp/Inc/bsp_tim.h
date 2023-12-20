#ifndef _BSP_TIM_H
#define _BSP_TIM_H

#include "main.h"


void bsp_timInit(void);
void setHrtimDutyNum(uint16_t dutyn);
void setPwmhDuty(uint16_t pwmh);

#endif
