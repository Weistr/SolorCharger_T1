#include "bsp_tim.h"
#include "bsp_adc.h"
/* Exported types ------------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern HRTIM_HandleTypeDef hhrtim1;
/* END-------------------------------------------------------------------------*/
/* define ------------------------------------------------------------*/

#define timdutyTotal 15360
#define tdead 960 //120* (32/4) 上升沿和下降沿死区一样

#define timdutyMax 13440//timdutyTotal - tdead*2
#define timdutyMin 1920	

#define pwmHmax 12480//timdutyMax - tdead
#define pwmHmin 960//timdutyMin - tdead
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
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM1)
    {
       
    }
}
