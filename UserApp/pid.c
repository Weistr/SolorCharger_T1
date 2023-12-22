#include "pid.h"

/**************************************************************
 *������λPID����
 * @param[in] 
 * @param[out] 
 * @return     
 ***************************************************************/	
void pidRest(PidObject **pid,const uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
	  	pid[i]->integ = 0;
	    pid[i]->prevError = 0;
	    pid[i]->out = 0;
		pid[i]->offset = 0;
	}
}

/**************************************************************
 * Update the PID parameters.
 *
 * @param[in] pid         A pointer to the pid object.
 * @param[in] measured    The measured value
 * @param[in] updateError Set to TRUE if error should be calculated.
 *                        Set to False if pidSetError() has been used.
 * @return PID algorithm output
 ***************************************************************/	
void pidUpdate(PidObject* pid,const float dt)
{
	 float error;
	 float deriv;
	
    error = pid->desired - pid->measured; //��ǰ�Ƕ���ʵ�ʽǶȵ����
	if((pid->integ_start < error)||(-pid->integ_start > error))//������Χ
    pid->integ += error * dt;	 //�������ۼ�ֵ
	else
		pid->integ=0;
	
	if(pid->integ > pid->integ_ilimt)//���л����޷�
		pid->integ = pid->integ_ilimt;
	else if(pid->integ < -pid->integ_ilimt)
		pid->integ = -pid->integ_ilimt;
	
	deriv = (error - pid->prevError)/dt;  //ǰ�����������΢��
		
	pid->out = pid->kp * error + pid->ki * pid->integ + pid->kd * deriv;//PID���
	if(pid->out > pid->out_limit)
		pid->out = pid->out_limit;
	else if(pid->out < -pid->out_limit)
		pid->out = -pid->out_limit;
		
	
	pid->prevError = error;  //
		
}


void pid_param_Init(void)
{


}

