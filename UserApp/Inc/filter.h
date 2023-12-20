#ifndef		__COMMON_FILTER_H
#define		__COMMON_FILTER_H
#include "main.h"

#ifndef		ch0
#define		ch0 0
#endif
#ifndef		ch1
#define		ch1 1
#endif
#ifndef		ch2
#define		ch2 2
#endif
#ifndef		ch3
#define		ch3 3
#endif

typedef struct{
	float x;
	float p;
	float A;
	float H;
	float q;
	float r;
	float gain;
}kalman1_state;

struct _1_ekf_filter
{
	float LastP;
	float	Now_P;
	float out;
	float Kg;
	float Q;
	float R;	
};

//void ekf_1(struct EKF *ekf,void *input);  //Ò»Î¬¿¨¶ûÂü
extern void kalman_1(struct _1_ekf_filter *ekf,float input);  //Ò»Î¬¿¨¶ûÂü

float kalman1_filter(kalman1_state *state, float z_measure);
float S1_IIR_filter(float factor, float measure);
uint16_t avrange_val(uint16_t num,uint8_t times,uint8_t ch);//0~65536/2£¬
float XIIR_filter(float factor, float measure, uint8_t ch);


#endif
