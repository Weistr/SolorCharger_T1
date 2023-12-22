#ifndef		__PID_H
#define		__PID_H
#include "user_main.h"
//PID算法的数据结构 

typedef volatile struct
{
	float desired;     //< set point
	float offset;      //
	float prevError;    //< previous error
	float integ;        //< integral
	float kp;           //< proportional gain
	float ki;           //< integral gain
	float kd;           //< derivative gain
	float integ_ilimt;       //< integral limit
	float integ_start;
	float measured;
	float out;
	float out_limit;
}PidObject;

void pidRest(PidObject **pid,const uint8_t len);
void pid_param_Init(void);
	void pidUpdate(PidObject* pid,const float dt);
void CascadePID(PidObject* pidRate,PidObject* pidAngE,const float dt);  //串级PID



#endif
