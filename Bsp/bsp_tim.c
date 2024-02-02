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
void pwmStopAll()
{
	HAL_HRTIM_SimplePWMStop(&hhrtim1,HRTIM_TIMERINDEX_TIMER_A,HRTIM_OUTPUT_TA1);
	HAL_HRTIM_SimplePWMStop(&hhrtim1,HRTIM_TIMERINDEX_TIMER_A,HRTIM_OUTPUT_TA2);
}
void pwmStartAll()
{
	HAL_HRTIM_SimplePWMStart(&hhrtim1,HRTIM_TIMERINDEX_TIMER_A,HRTIM_OUTPUT_TA1);
	HAL_HRTIM_SimplePWMStart(&hhrtim1,HRTIM_TIMERINDEX_TIMER_A,HRTIM_OUTPUT_TA2);
}
void bsp_timInit(void)
{
    //HAL_TIM_Base_Start_IT(&htim7);
		HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //通道打开
		HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_A); //开启子定时器A
		pwmStopAll();
		setPwmhDuty(5000);
		HAL_TIM_Base_Start_IT(&htim3);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
		static uint16_t pwmdelay = 0;
		if(pwmdelay < 1000)pwmdelay++;//延时
		if(pwmdelay > 100)//10ms后启动
		{
			adc_scan();
		}
		if(pwmdelay > 900)//90ms后启动
		{
			vioutControlTask();
		}		
    }
}
