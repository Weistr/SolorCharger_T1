#include "bsp_key.h"
#include "bsp_pw.h"
sgKeyTypedef sgKey[3];
void bsp_key_init()
{
    sgKey[0].GPIO_Port_Base = GPIOC_BASE;
    sgKey[0].GPIO_Pin_Base = GPIO_PIN_13;
    sgKey[0].val_pushed = 1;//按键按下置1


}
void power_key_handle_20ms()
{
	static uint8_t step=0;
	static uint8_t cnt = 0;
	static bool key0LonClicFlag_pre=0;
	switch(step)
	{
		case 0:
			if(sgKey[0].LonClicFlag != key0LonClicFlag_pre)//按键长按标志
			{	
				key0LonClicFlag_pre = sgKey[0].LonClicFlag ;//清除标志位
				bsp_VoDisable();
				step++;
			}
			break;
		case 1:
			if(sgKey[0].val == 0)//按键松开
			{
				step++;
			}
			break;
		case 2:
			cnt++;
			if(cnt > 5)//100ms
			{
				mcu_standby();
			}
			break;
	}
}

void bsp_keyScan_20ms()
{
    for(uint8_t i = 0; i< 3; i++)
    {
        sgKey[i].val = HAL_GPIO_ReadPin((GPIO_TypeDef*)sgKey[i].GPIO_Port_Base,(uint16_t)sgKey[i].GPIO_Pin_Base);
        if(sgKey[i].val == 1)
        {
            sgKey[i].push_time_x20ms++;
            if(sgKey[i].push_time_x20ms > 75)//长按1500ms
            {
                sgKey[i].push_time_x20ms = 0;
                sgKey[i].LonClicFlag = !sgKey[i].LonClicFlag;
            }
        }
        else
        {
            sgKey[i].push_time_x20ms=0;
        }
        
        if(sgKey[i].valPrevious < sgKey[i].val)//按键按下
        {
            sgKey[i].clicCnt++;
            sgKey[i].timeOutCnt=0;
            sgKey[i].OneClicFlag =! sgKey[i].OneClicFlag;//单击
            if(sgKey[i].clicCnt % 2 == 0)//双击
                sgKey[i].TwoClicFlag =! sgKey[i].TwoClicFlag;
            if(sgKey[i].clicCnt % 3 == 0)//三击
            {}
            
        }
        else
        {
            sgKey[i].timeOutCnt++;
            if(sgKey[i].timeOutCnt > 25)//双击时间间隔 默认500ms
            {
                sgKey[i].timeOutCnt = 0;
                sgKey[i].clicCnt = 0;
            }
        }
        sgKey[i].valPrevious = sgKey[i].val;
    }
    power_key_handle_20ms();
}

