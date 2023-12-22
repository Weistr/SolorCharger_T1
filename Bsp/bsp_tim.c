#include "bsp_tim.h"
#include "bsp_adc.h"
#include "vioutControl.h"
/* Exported types ------------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern HRTIM_HandleTypeDef hhrtim1;
extern TIM_HandleTypeDef htim3;
/* END-------------------------------------------------------------------------*/
/* variables ---------------------------------------------------------*/

/* END-------------------------------------------------------------------------*/






void setHrtimDutyNum(uint16_t dutyn)
{
	if(dutyn > timdutyMax)dutyn = timdutyMax;//max 80%
	else if(dutyn < timdutyMin) dutyn=timdutyMin;
	hhrtim1.Instance->sTimerxRegs[0].CMP1xR = dutyn;
}
void setPwmhDuty(uint16_t pwmh)
{
	if(pwmh > pwmHmax)pwmh = pwmHmax;//max 80%
	else if(pwmh < pwmHmin) pwmh=pwmHmin;	
	hhrtim1.Instance->sTimerxRegs[0].CMP1xR = timdutyTotal - pwmh + tdead ;
}

void bsp_timInit(void)
{
    //HAL_TIM_Base_Start_IT(&htim7);
		HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //通道打开
		HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_A); //开启子定时器A
		setPwmhDuty(5000);
		HAL_TIM_Base_Start_IT(&htim3);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
		static uint16_t pwmdelay = 1000;
		if(pwmdelay > 0)pwmdelay--;//延时
		else
		{
			vioutControlTask();
		}
		adc_scan();
    }
}
