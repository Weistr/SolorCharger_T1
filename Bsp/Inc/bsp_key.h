#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#include "stdbool.h"
#include "user_main.h"

typedef struct 
{
    bool val;//按键按下置1，松开置0
    bool val_pushed; //
    bool OneClicFlag;//按键单击标志位  触发取反
    bool TwoClicFlag;//按键双击标志位  触发取反
    bool LonClicFlag;//按键长按标志位  触发置1

    bool valPrevious;//
    uint8_t timeOutCnt;
    uint8_t push_time_x20ms;
    uint8_t clicCnt;
    uint16_t GPIO_Pin_Base;
    uint32_t GPIO_Port_Base;
}sgKeyTypedef;

void bsp_keyScan_20ms(void);
void bsp_key_init(void);

#endif
