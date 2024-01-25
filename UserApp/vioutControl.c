#include "user_main.h"
#include "bsp_tim.h"
#include "bsp_delay.h"
#include "vioutControl.h"
#include "hrtim.h"
/* Exported types ------------------------------------------------------------*/
extern int16_t  bv_mv,pv_mv,cref_mv,bi_ma;
extern HRTIM_HandleTypeDef hhrtim1;
extern uint8_t fac_us;
extern uint8_t test_ok;
/* END-------------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//PID CONTROL
int32_t pidFinalOut = 0;
int16_t pidOutTest = 7000;
//BATTERY votage 
int16_t battVotLimitMin_mv = 4150;
int16_t battVotLimitMax_mv = 4250;
//mppt pid obj
int16_t pvSet_mv=12000;
int16_t pvTarget_mv = 0;
int32_t pwm_H_cnt_Base = 0;
float pvPidOut;
float dt = 0.00005;
float pvPid_kp = 1;
float pvPid_ki = 1;
float pvPid_kd = 0;
float pvPid_err = 0;
float pvPid_preErr = 0;
float pvPid_integ = 0;
float pvPid_deriv = 0;
float pvPid_integLimit = 7000;
float pvPid_outLimit = 10000;
int16_t pwmMin_fluback ;
//时间测量
int32_t vioutLoopTime_cntStart = 0;
int32_t vioutLoopTime_cntStop = 0;
int32_t vioutLoopTime_us = 0;
/* END-------------------------------------------------------------------------*/



void vioutControlTask()
{
    vioutLoopTime_cntStart = SysTick->VAL;//开始计时

    
    //输入欠压保护
    static uint8_t lvp_ok = 1;//输入欠压保护解除
    static uint16_t lvpcnt=0;
    if(pv_mv < 6500)
    {
        
        if (lvpcnt > 1000)
        {
            lvpcnt = 0;
            pwmStopAll();
            lvp_ok = 0;
            pvPid_integ = 0;
        }
        else
        {
           lvpcnt++; 
        }
        
    }
    else 
    {
        lvp_ok = 1;//输入欠压保护解除
        lvpcnt = 0;
    }
    



    //输出过压保护
    static uint8_t bvp_ok = 1;//电池过压保护解除 
    static uint16_t lvcnt = 0;
    if (bv_mv > battVotLimitMax_mv)
    {
        
        if (lvcnt > 5000)
        {
            lvcnt = 0;
            pwmStopAll();
            bvp_ok = 0;
        }
        else
        {
            lvcnt ++;
        }
        
    }
    else if(bv_mv < battVotLimitMin_mv)
    {
        lvcnt = 0;
        bvp_ok = 1;//电池过压保护解除
    }

    if(
        (bvp_ok==1) &&
        (lvp_ok == 1) && 
        (test_ok == 1)
    )
    {
        pwmStartAll();
    }

    //输出恒压


    //输入端恒压
    pwm_H_cnt_Base = (int32_t)bv_mv * timdutyTotal / pvSet_mv; //Ton = Vo/Vi * T

    pvPid_err = pv_mv - pvSet_mv;//误差

    pvPid_integ += pvPid_err * pvPid_ki * dt;//积分

    if(pvPid_integ > pvPid_integLimit)pvPid_integ = pvPid_integLimit;//进行积分限幅
    else if(pvPid_integ <  -pvPid_integLimit)pvPid_integ = -pvPid_integLimit;

    pvPid_deriv = (pvPid_err - pvPid_preErr)/dt*pvPid_kd;//微分项

    pvPidOut = pvPid_err * pvPid_kp + pvPid_integ + pvPid_deriv;//pid输出
    pvPid_preErr = pvPid_err;
    if(pvPidOut > pvPid_outLimit) pvPidOut = pvPid_outLimit;//pid输出限幅
    else if(pvPidOut < -pvPid_outLimit) pvPidOut = -pvPid_outLimit;
    hhrtim1.Instance->sTimerxRegs[0].CMP1xR = timdutyTotal - pvPidOut + tdead ;
    //总输出
    pidFinalOut = pwm_H_cnt_Base + pvPidOut;
  

    //防止电流倒流
    /*
    pwmMin_fluback = (int32_t)bv_mv * timdutyTotal / pv_mv;
    if(pidFinalOut < pwmMin_fluback)pidFinalOut = pwmMin_fluback;
*/

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

void sysPowerOff()
{
    HAL_GPIO_WritePin(VOCT_GPIO_Port,VOCT_Pin,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(VOCT2_GPIO_Port,VOCT2_Pin,GPIO_PIN_SET);
    
}