#include "bsp_tim.h"
#include "bsp_adc.h"
/* Exported types ------------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern HRTIM_HandleTypeDef hhrtim1;
/* END-------------------------------------------------------------------------*/
/* define ------------------------------------------------------------*/
#define timdutyMax 13000
#define timdutyMin 2000
#define timdutyTotal 15360
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
	if(pwmh > timdutyMax)pwmh = timdutyMax;//max 80%
	else if(pwmh < timdutyMin) pwmh=timdutyMin;	
	hhrtim1.Instance->sTimerxRegs[0].CMP1xR = timdutyMax - pwmh + 2000;
}

void bsp_timInit(void)
{
    //HAL_TIM_Base_Start_IT(&htim7);
		HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //通道打开
		HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_A); //开启子定时器A
		setPwmhDuty(5000);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM1)
    {
       
    }
}
