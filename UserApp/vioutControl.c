#include "user_main.h"
#include "bsp_tim.h"
#include "bsp_delay.h"
#include "vioutControl.h"
/* Exported types ------------------------------------------------------------*/
extern int16_t  bv_mv,pv_mv,cref_mv,bi_ma;
extern HRTIM_HandleTypeDef hhrtim1;
extern uint8_t fac_us;
/* END-------------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//PID CONTROL
int32_t pidFinalOut = 0;
int16_t pidOutTest = 7000;
//BATTERY votage pid obj
int16_t battVotLimit_mv = 4200;
int16_t kp_bv = 100;
int16_t bvPid_out = 0;
//BATTERY current pid obj
int16_t battCurLimit_ma = 500;
int16_t kp_bi = 100;
int16_t biPid_out = 0;
//mppt pid obj
int16_t pvSet_mv=0;
int32_t pwm_H_cnt_Base = 0;
float pvPidOut;
float dt = 0.02;
float pvPid_kp = 100;
float pvPid_ki = 10;
float pvPid_kd = 1;
float pvPid_err = 0;
float pvPid_preErr = 0;
float pvPid_integ = 0;
float pvPid_deriv = 0;
float pvPid_integLimit = 2000;
//时间测量
int32_t vioutLoopTime_cntStart = 0;
int32_t vioutLoopTime_cntStop = 0;
int32_t vioutLoopTime_us = 0;
/* END-------------------------------------------------------------------------*/



void vioutControlTask()
{
    vioutLoopTime_cntStart = SysTick->VAL;//开始计时

    /*输出端限压控制*/
    if (bv_mv > battVotLimit_mv)
    {
        bvPid_out = (bv_mv - battVotLimit_mv)*kp_bv;
    }
    /*输出端限流控制*/
    if(bi_ma > battCurLimit_ma)
    {
       biPid_out = (bi_ma - battCurLimit_ma)*kp_bi;
    }   
    /*输入端恒压*/
    pwm_H_cnt_Base = (int32_t)bv_mv * timdutyTotal / pvSet_mv; //Ton = Vo/Vi * T

    pvPid_err = pv_mv - pvSet_mv;//误差

    pvPid_integ += pvPid_err * pvPid_ki * dt;//积分

    if(pvPid_integ > pvPid_integLimit)pvPid_integ = pvPid_integLimit;//进行积分限幅
    else if(pvPid_integ <  -pvPid_integLimit)pvPid_integ = -pvPid_integLimit;

    pvPid_deriv = (pvPid_err - pvPid_preErr)/dt*pvPid_kd;//微分项

    pvPidOut = pvPid_err * pvPid_kp + pvPid_integ + pvPid_deriv;//pid输出
    pvPid_preErr = pvPid_err;
    /*总输出*/
    pidFinalOut = pwm_H_cnt_Base + pvPidOut - biPid_out - bvPid_out;
    
    if(pidFinalOut > pwmHmax) pidFinalOut = pwmHmax;
    else if(pidFinalOut < pwmHmin) pidFinalOut = pwmHmin;
    hhrtim1.Instance->sTimerxRegs[0].CMP1xR = timdutyTotal - pidFinalOut + tdead ;

    //计时结束
    vioutLoopTime_cntStop = SysTick->VAL;
    vioutLoopTime_us = vioutLoopTime_cntStart - vioutLoopTime_cntStop;
    if (vioutLoopTime_us < 0)
    {
        vioutLoopTime_us += SysTick->LOAD;
    }
    vioutLoopTime_us /= fac_us;
}