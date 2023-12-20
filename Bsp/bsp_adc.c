
#include "bsp_adc.h"
#include "bsp_adc.h"
#include "filter.h"
#include "stdbool.h"



/* Exported types ------------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
/* END-------------------------------------------------------------------------*/



/* Private variables ---------------------------------------------------------*/
uint16_t u16adc_buf[4];
uint16_t u16adc2_buf[3];
uint16_t adcBufFilted[4];

uint16_t battVot_mv ,bv_mv,pv_mv,cref_mv;
int16_t pv_ma,pi_ma,bi_ma;
const uint16_t vref_mV = 1200;

uint8_t cnt1=0,cnt2=0,cnt3=50;
/* END-------------------------------------------------------------------------*/

/* Private define ---------------------------------------------------------*/

#define vref_adc u16adc_buf[3] //stm32内部基准电压
#define bv_adc u16adc_buf[1] //buck输出电压
#define pv_adc u16adc_buf[2] //buck输入电压
#define bi_adc u16adc2_buf[2] //buck输出电流
#define pi_adc u16adc2_buf[1] //buck输入电流
#define cref_adc u16adc2_buf[0] //电流运放基准电压
/* END-------------------------------------------------------------------------*/




void adc_init()
{
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
}


void adc_scan()
{

	
	/*
	for(i=0;i<4;i++)
	{
		adcBufFilted[i] =XIIR_filter(0.8,u16adc_buf[i],i);
	}
	for(i=0;i<3;i++)
	{
		adcBufFilted[i] =XIIR_filter(0.8,u16adc2_buf[i],i);
	}	
	*/
//vref

//buck out votage VO
	
	cref_mv = (int32_t)u16adc2_buf[0] * vref_mV / vref_adc;

	bv_mv = (int32_t)u16adc_buf[1] * vref_mV / vref_adc * 2.0f;
	pv_mv = (int32_t)u16adc_buf[2] * vref_mV / vref_adc * 5.65116f;

	bi_ma = (int32_t)(bi_adc-cref_adc) * vref_mV / vref_adc * 2;
	pi_ma = (int32_t)(pi_adc-cref_adc) * vref_mV / vref_adc * 2;
	
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&u16adc_buf[0],4);
	HAL_ADC_Start_DMA(&hadc2,(uint32_t*)&u16adc2_buf[0],3);
}




