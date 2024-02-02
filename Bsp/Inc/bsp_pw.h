#ifndef _BSP_PW_H
#define _BSP_PW_H

#include "user_main.h"

void bsp_RccInit(void);
void bsp_RccChange(void);
void mcu_standby(void);
void bsp_VoDisable(void);
#endif // !_BSP_PW_H