#include "stdbool.h"
#include "user_main.h"
#include "bsp_tim.h"
#include "bsp_adc.h"
#include "ssd1306.h"
#include "GUI12864.h"
#include "bsp_delay.h"
#include "vioutControl.h"
/***************************************************/
/*********************TASK************************/
/***************************************************/
//========================================================================
// 函数: basic  task
// 描述: .
// 参数: None.
// 返回: None.
//========================================================================
	
void basicTask_entry()
{

		
	
}
uint16_t deb1=7500,deb2=0,deb3=0;
int16_t deb4=0;
void OLED_task()
{
	main_page_update();
	status_bar_update();
	GUI_DisplayRefresh();
}
bool rx2_flag=0;
extern HRTIM_HandleTypeDef hhrtim1;
uint8_t test_ok = 1;
void task2()
{
	if (test_ok==0)
	{
		pwmStopAll();
	}
	
}
/***************************************************/
/*********************初始化************************/
/***************************************************/

void user_main()
{
	delay_init();
	bsp_timInit();
	adc_init();

//显示屏	


	OLED_Init();
	GUI_Clear();
	
	status_bar_start();
  	main_page_start();
	
	GUI_DisplayRefresh();

}

//www.stcmcu.com
//========================================================================
//                         本地变量声明 & 任务分配
//========================================================================
static TASK_COMPONENTS Task_Comps[]=
{
//状态  计数  周期  函数
	{0, 20, 20, basicTask_entry},				/* task 1 Period： 2ms*/
	{0, 20, 20, OLED_task},				/* task 2 Period： 100ms */
	{0, 20, 20, task2},					/* task 4 Period： 500ms */
//	{0, 500, 500, task_B},					/* task 5 Period： 500ms */
//	{0, 500, 500, task_C},					/* task 6 Period： 500ms */
//	{0, 500, 500, task_D},					/* task 7 Period： 500ms */
//	{0, 500, 500, task_E},					/* task 8 Period： 500ms */

	/* Add new task here */
};

uint8_t Tasks_Max = sizeof(Task_Comps)/sizeof(Task_Comps[0]);

//========================================================================
// 函数: Task_Handler_Callback
// 描述: 任务标记回调函数.
// 参数: None.
// 返回: None.
//========================================================================
void Task_Marks_Handler_Callback(void)
{
	uint8_t i;
	for(i=0; i<Tasks_Max; i++)
	{
		if(Task_Comps[i].TIMCount)    /* If the time is not 0 */
		{
			Task_Comps[i].TIMCount--;  /* Time counter decrement */
			if(Task_Comps[i].TIMCount == 0)  /* If time arrives */
			{
				/*Resume the timer value and try again */
				Task_Comps[i].TIMCount = Task_Comps[i].TRITime;  
				Task_Comps[i].Run = 1;    /* The task can be run */
			}
		}
	}
}

//========================================================================
// 函数: Task_Pro_Handler_Callback
// 描述: 任务处理回调函数.
// 参数: None.
// 返回: None.
//========================================================================
void Task_Pro_Handler_Callback(void)
{
	uint8_t i;
	for(i=0; i<Tasks_Max; i++)
	{
		if(Task_Comps[i].Run) /* If task can be run */
		{
			Task_Comps[i].Run = 0;    /* Flag clear 0 */
			Task_Comps[i].TaskHook();  /* Run task */
		}
	}
}
//asdasd





