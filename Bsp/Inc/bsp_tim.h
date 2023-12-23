#ifndef _BSP_TIM_H
#define _BSP_TIM_H

#include "main.h"


void bsp_timInit(void);
void setHrtimDutyNum(uint16_t dutyn);
void setPwmhDuty(uint16_t pwmh);
void  pwmStopAll(void);
void  pwmStartAll(void);
/* define ------------------------------------------------------------*/

#define timdutyTotal 15360
#define tdead 960 //200* (32/4) 上升沿和下降沿死区一样

#define timdutyMax 12160//timdutyTotal - tdead*2
#define timdutyMin 3200	

#define pwmHmax 12480//timdutyMax - tdead
//#define pwmHmin 1920//timdutyMin - tdead
#define pwmHmin 6300//timdutyMin - tdead
/* END-------------------------------------------------------------------------*/

#endif
