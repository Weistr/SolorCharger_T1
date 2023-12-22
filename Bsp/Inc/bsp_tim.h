#ifndef _BSP_TIM_H
#define _BSP_TIM_H

#include "main.h"


void bsp_timInit(void);
void setHrtimDutyNum(uint16_t dutyn);
void setPwmhDuty(uint16_t pwmh);
/* define ------------------------------------------------------------*/

#define timdutyTotal 15360
#define tdead 960 //120* (32/4) 上升沿和下降沿死区一样

#define timdutyMax 13440//timdutyTotal - tdead*2
#define timdutyMin 1920	

#define pwmHmax 12480//timdutyMax - tdead
#define pwmHmin 960//timdutyMin - tdead
/* END-------------------------------------------------------------------------*/

#endif
