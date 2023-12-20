#include "filter.h"


//
float S1_IIR_filter(float factor, float measure)
{
	static float last;
	float out;
	out = measure * factor + last * (1-factor);
	last = measure;
	return out;
}
//IIR改版
float XIIR_filter(float factor, float measure, uint8_t ch)
{
	static float out[8];
	out[ch] = measure * factor + out[ch] * (1-factor);
	return out[ch];
}
//滑动窗口滤波


//平均值滤波
/*
uint16_t avrange_val(uint16_t num,uint8_t times,uint8_t ch)//0~65536/2，
{
	static uint16_t avra[8][10];
	uint8_t av_tims;
	avra[ch][0]=(avra[ch][0]+num)/2;
	for(av_tims=1;av_tims<times;av_tims++)
	{
		avra[ch][av_tims]=(avra[ch][av_tims]+avra[ch][av_tims-1])/2;
	}
	return avra[ch][av_tims-1];
}
*/
