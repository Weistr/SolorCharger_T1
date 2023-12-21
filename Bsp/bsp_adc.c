
#include "bsp_adc.h"
#include "bsp_adc.h"
#include "filter.h"
#include "stdbool.h"
#include "stm32f3xx_ll_adc.h"
#include "bsp_delay.h"

/* Exported types ------------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern uint8_t fac_us;
/* END-------------------------------------------------------------------------*/



/* Private variables ---------------------------------------------------------*/
uint16_t u16adc_buf[4];
uint16_t u16adc2_buf[3];
uint16_t adcBufFilted[4];

uint16_t battVot_mv ,bv_mv,pv_mv,cref_mv;
int16_t pv_ma,pi_ma,bi_ma;

uint16_t vrefmv;
uint8_t cnt1=0,cnt2=0,cnt3=50;

//adc convert time
int32_t adc1CovStrat_cnt = 0;
int32_t adc1CovStop_cnt = 0;
int32_t adc1CovTime_us=0;
//adc start convert flag (app)
uint8_t adcStartConvFlag = 0;
uint8_t adc1ConvFinish = 0;
uint8_t adc2ConvFinish = 0;

/* END-------------------------------------------------------------------------*/

/* Private define ---------------------------------------------------------*/


#define bv_adc u16adc_buf[1] //buck输出电压
#define pv_adc u16adc_buf[2] //buck输入电压
#define vref_adc u16adc_buf[3]
#define bi_adc u16adc2_buf[2] //buck输出电流
#define pi_adc u16adc2_buf[1] //buck输入电流
#define cref_adc u16adc2_buf[0] //电流运放基准电压
#define vrefmv 1242
/* END-------------------------------------------------------------------------*/


void adc_init()
{
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&u16adc_buf[0],4);
	HAL_ADC_Start_DMA(&hadc2,(uint32_t*)&u16adc2_buf[0],3);

}

void adcConvStart()
{
	adcStartConvFlag = 1;
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&u16adc_buf[0],4);
	HAL_ADC_Start_DMA(&hadc2,(uint32_t*)&u16adc2_buf[0],3);
}

void adc_scan()
{
//buck out votage VO
	
	float VrefRat = (float)vrefmv/vref_adc;
	cref_mv = (float)u16adc2_buf[0] * VrefRat;

	bv_mv = (float)u16adc_buf[1] * VrefRat * 1.99893f;
	pv_mv = (float)u16adc_buf[2] * VrefRat * 5.664175f;

	bi_ma = (float)(bi_adc-cref_adc) * VrefRat * 2;
	pi_ma = (float)(pi_adc-cref_adc) * VrefRat * 2;

	adcConvStart();
	//adc time counting start
	adc1CovStrat_cnt = SysTick->VAL;
}

void adcConvAllFinshCallback(ADC_HandleTypeDef* hadc)
{
	static uint16_t adcConvNtimes = 0;
	adcConvNtimes ++;
	if(adcConvNtimes > 10)
	{
		adcConvNtimes = 0;
		//adc time counting end
		adc1CovStop_cnt = SysTick->VAL;
		adc1CovTime_us = adc1CovStrat_cnt - adc1CovStop_cnt;
		if (adc1CovTime_us < 0)
		{
			adc1CovTime_us += SysTick->LOAD;
		}
		adc1CovTime_us /= fac_us;
	}
	if (adcStartConvFlag == 1)
	{
		HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&u16adc_buf[0],4);
		HAL_ADC_Start_DMA(&hadc2,(uint32_t*)&u16adc2_buf[0],3);
	}
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC2)
	{
		adc2ConvFinish = 1;
	}
	if(hadc->Instance == ADC1)
	{
		adc1ConvFinish = 1;
	}
	if((adc1ConvFinish == 1) && (adc2ConvFinish == 1))
	{
		adc1ConvFinish = 0;
		adc2ConvFinish = 0;
	}
}

