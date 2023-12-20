#include "common_filter.h"

/*********************************************************************************************************
https://blog.csdn.net/xiahouzuoxin/article/details/39582483
 * @brief   
 *   1 Dimension Kalman filter
 * @inputs  
 *   state - Klaman filter structure
 *   z_measure - Measure value
 * @outputs 
 * @retval  
 *   Estimated result
 */
 
//float kalman1_filter(kalman1_state *state, float z_measure)
//{
    /* Predict */
//    state->x = state->A * state->x;
//    state->p = state->A * state->A * state->p + state->q;  /* p(n|n-1)=A^2*p(n-1|n-1)+q */
//
//    /* Measurement */
//    state->gain = state->p * state->H / (state->p * state->H * state->H + state->r);
//    state->x = state->x + state->gain * (z_measure - state->H * state->x);
//   state->p = (1 - state->gain * state->H) * state->p;

//    return state->x;
//}
void kalman_1(struct _1_ekf_filter *ekf,float input)  //һά������
{
	ekf->Now_P = ekf->LastP + ekf->Q;
	ekf->Kg = ekf->Now_P / (ekf->Now_P + ekf->R);
	ekf->out = ekf->out + ekf->Kg * (input - ekf->out);
	ekf->LastP = (1-ekf->Kg) * ekf->Now_P ;
}

//
float S1_IIR_filter(float factor, float measure)
{
	static float last;
	float out;
	out = measure * factor + last * (1-factor);
	last = measure;
	return out;
}
//IIR�İ�
float XIIR_filter(float factor, float measure, uint8_t ch)
{
	static float out[8];
	out[ch] = measure * factor + out[ch] * (1-factor);
	return out[ch];
}
//���������˲�


//ƽ��ֵ�˲�
/*
uint16_t avrange_val(uint16_t num,uint8_t times,uint8_t ch)//0~65536/2��
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
