#include "bsp_pw.h"
void bsp_powerOFF()
{
    HAL_GPIO_WritePin(VOCT2_GPIO_Port,VOCT2_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(VOCT_GPIO_Port,VOCT_Pin,GPIO_PIN_RESET);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_PWR_EnterSTANDBYMode();
}
void bsp_powerOn()
{

}
void bsp_powerInit()
{
    __HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);
}