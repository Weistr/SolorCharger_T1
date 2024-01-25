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



/*public variables ---------------------------------------------------------*/

int16_t  bv_mv,pv_mv,cref_mv;
int16_t pv_ma,pi_ma,bi_ma;
int16_t vrefmv;
uint8_t cnt1=0,cnt2=0,cnt3=50;

//adc convert time
static int32_t adc1CovStrat_cnt = 0;
static int32_t adc1CovStop_cnt = 0;
int32_t adc1CovTime_us=0;
//adc start convert flag (app)
static uint8_t adcStartConvFlag = 0;
static uint8_t adc1ConvFinish = 0;
static uint8_t adc2ConvFinish = 0;
static uint16_t u16adc_buf[4];
static uint16_t u16adc2_buf[3];
static uint16_t u16adc1Filted_buf[4];
static uint16_t u16adc2Filted_buf[3];
/* END-------------------------------------------------------------------------*/

/* Private define ---------------------------------------------------------*/


#define bv_adc u16adc1Filted_buf[1] //buck输出电压
#define pv_adc u16adc1Filted_buf[2] //buck输入电压
#define vref_adc u16adc1Filted_buf[3]
#define bi_adc u16adc2Filted_buf[2] //buck输出电流
#define pi_adc u16adc2Filted_buf[1] //buck输入电流
#define cref_adc u16adc2Filted_buf[0] //电流运放基准电压
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
	cref_mv = (float)cref_adc * VrefRat;

	bv_mv = (float)bv_adc * VrefRat * 1.99893f;
	pv_mv = (float)pv_adc * VrefRat * 5.664175f;

	bi_ma = (float)(bi_adc-cref_adc) * VrefRat * 2;
	pi_ma = (float)(pi_adc-cref_adc) * VrefRat * 2;

	adcConvStart();
	//adc time counting start
	adc1CovStrat_cnt = SysTick->VAL;
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC2)
	{
		adc2ConvFinish = 1;
	}
	else if(hadc->Instance == ADC1)
	{
		adc1ConvFinish = 1;
	}

	//all adc conversions complete
	if((adc1ConvFinish == 1) && (adc2ConvFinish == 1))
	{
		adc1ConvFinish = 0;
		adc2ConvFinish = 0;
		for (uint16_t i = 0; i < 4; i++)
		{
			u16adc1Filted_buf[i] = u16adc_buf[i];
			if (i < 3)
			{
				u16adc2Filted_buf[i] = u16adc2_buf[i];
			}
		}
		//adc timekeeping 
		adc1CovStop_cnt = SysTick->VAL;
		adc1CovTime_us = adc1CovStrat_cnt - adc1CovStop_cnt;
		if (adc1CovTime_us < 0)
		{
			adc1CovTime_us += SysTick->LOAD;
		}
		adc1CovTime_us /= fac_us;
		/*
		//convert many times
		#define convertN 10
		static uint16_t adcConvNtimes = 0;
		adcConvNtimes ++;
		if(adcConvNtimes >= convertN)
		{
			adcConvNtimes = 0;
			adcStartConvFlag = 0;//stop converting
			//avrenge all data
			for (uint16_t i = 0; i < 4; i++)
			{
				u16adc1Filted_buf[i] = adc1bufSum[i] /convertN;
				adc1bufSum[i]=0;
				if (i < 3)
				{
					u16adc2Filted_buf[i] = adc2bufSum[i] /convertN;
					adc2bufSum[i]=0;
				}
			}


		}

		
		//sum all adc result
		for (uint16_t i = 0; i < 4; i++)
		{
			adc1bufSum[i] += u16adc_buf[i];
			if (i < 3)
			{
				adc2bufSum[i] += u16adc2_buf[i];
			}
		}
		

		//clean adc flags
		adc1ConvFinish = 0;
		adc2ConvFinish = 0;

		//restart conversion
		if (adcStartConvFlag == 1)
		{
			HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&u16adc_buf[0],4);
			HAL_ADC_Start_DMA(&hadc2,(uint32_t*)&u16adc2_buf[0],3);
		}	*/
	}
}

