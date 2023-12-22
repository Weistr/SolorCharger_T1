#include "user_main.h"

/* Exported types ------------------------------------------------------------*/
extern int16_t  bv_mv,pv_mv,cref_mv,bi_ma;
/* END-------------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int16_t pvSet_mv=0;
int16_t targetPv_mv = 7000;
int16_t pwm_H_cnt_Base = 0;

#define diodeOn_mv = 5000;

//const int16_t deadTcnt = 

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
/* END-------------------------------------------------------------------------*/



void vioutControlTask()
{
    /*输出端限压控制*/
    if (bv_mv > battVotLimit_mv)
    {
        bvPid_out = (bv_mv - battVotLimit_mv)*kp_bv;
    }
    /*输出端限流控制*/
    if(bi_ma > battCurLimit_ma)
    {
       
    }   
    
    /*总输出*/
    pidFinalOut = pidOutTest - pidFinalOut;
    
    
}