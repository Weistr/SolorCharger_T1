#ifndef _GUI12864_H
#define _GUI12864_H
#include "main.h"
#include "stdbool.h"
#define white 1
#define dark 0
#define UpdateALL 1
#define OnlyNum 0
//GUI基础操作
#define GUI_ColorTurn(N)     OLED_ColorTurn(N)
#define GUI_DisplayTurn(N)   OLED_DisplayTurn(N)
#define GUI_DisPlay_On()     OLED_DisPlay_On()
#define GUI_DisPlay_Off()    OLED_DisPlay_Off()
#define GUI_Set_Pos(X,Y)     OLED_Set_Pos(X,Y);
void GUI_DisplayRefresh(void);
void GUI_Clear(void);
void GUI_DrawPoint(uint8_t x,uint8_t y,uint8_t mode);
void GUI_Fill(uint8_t  x1,uint8_t  y1,uint8_t  x2,uint8_t  y2,uint8_t  dot)  ;
void GUI_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode);
void GUI_DrawMultiLine(uint8_t X,uint8_t Y,uint8_t restart,uint8_t mode);
void GUI_Draw_Rect(uint8_t x1,uint8_t y1,uint8_t x2, uint8_t y2,uint8_t mode);
void GUI_DrawCircle(uint8_t x,uint8_t y,uint8_t r);
void GUI_ShowChar(uint8_t x,uint8_t y,char chr,uint8_t size,uint8_t rev);
void GUI_ShowString(uint8_t x,uint8_t y,char *p,uint8_t size,uint8_t rev);
uint32_t GUI_Pow(uint8_t m,uint8_t n);
void GUI_ShowNum(uint8_t x,uint8_t y,int16_t num,uint8_t len,uint8_t size1,uint8_t rev);
void GUI_ShowNum_Simplify(uint8_t x,uint8_t y,int16_t num,uint8_t len,uint8_t size1,uint8_t rev);
void GUI_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1);




//各种图标
void DrawBatteryRec(uint8_t x, uint8_t y,uint8_t charge,uint8_t RecUpdate,uint8_t rvs);
void showSignal(uint8_t x, uint8_t y,uint8_t num,uint8_t RecUpdate,uint8_t rvs);
void showChVal(uint8_t x, uint8_t y,uint8_t RecUpdate);
void showRxVot(uint8_t x, uint8_t y,uint16_t mV,uint8_t RecUpdate, uint8_t rev);
void icon_LoadAnim_RollRec(uint8_t x,uint8_t y,uint8_t recSize,uint8_t pointSize,uint8_t ct);

//页面
void status_bar_start(void);
void status_bar_update(void);

void main_page_start(void);
void main_page_update(void);


#endif
