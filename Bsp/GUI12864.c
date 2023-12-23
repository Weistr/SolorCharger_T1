#include "GUI12864.h"
#include "oledfont.h"
#include "ssd1306.h"

#include "bsp_adc.h"
#include "ssd1306.h"
#include "bsp_spi.h"

#include "stdbool.h"
#include "string.h"

extern uint16_t battVot_mv ,bv_mv,pv_mv,cref_mv;
extern int16_t pv_ma,pi_ma,bi_ma;
extern SPI_HandleTypeDef hspi1;


uint8_t GRAM[8][128];
uint8_t RF_Singnal=0;
uint8_t bttCharge = 69;
uint16_t bttTemp = 256;



//更新显存到OLED	
void GUI_DisplayRefresh(void)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
	  OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址
	  OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
	  OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
	  //for(n=0;n<128;n++)
		OLED_DC_Set();
		OLED_CS_Clr();
		HAL_SPI_Transmit(&hspi1,&GRAM[i][0],128,100);
//		OLED_WR_Byte(GRAM[n][i],OLED_DATA);
  }
}


//清屏函数
void GUI_Clear(void)
{
	uint8_t i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 GRAM[i][n]=0x00;//清除所有数据
			}
  }
}

//画点 
//x:0~127
//y:0~63
void GUI_DrawPoint(uint8_t x,uint8_t y,uint8_t mode)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(mode)GRAM[pos][x]|=temp;
	else GRAM[pos][x]&=~temp;	    
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void GUI_Fill(uint8_t  x1,uint8_t  y1,uint8_t  x2,uint8_t  y2,uint8_t  dot)  
{  
	uint8_t  x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)GUI_DrawPoint(x,y,dot);
	}													    
}
//画线
//x:0~128
void GUI_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode)
{
	uint8_t i,small_x,small_y,abs_x;
	float k=0;
	if((x2>128)||(y2>64)){}
	else
	{
		if(x1==x2)    //画竖线
		{
			if(y2>y1)
			{
				for(i=0;i<(y2-y1);i++)
				{
					GUI_DrawPoint(x1,y1+i,mode);
				}
			}
			else
			{
				for(i=0;i<(y1-y2);i++)
				{
					GUI_DrawPoint(x1,y2+i,mode);
				}
				
			}
		}
		else if(y1==y2)   //画横线
		{
			if(x2>x1)
			{
				for(i=0;i<(x2-x1);i++)
				{
					GUI_DrawPoint(x1+i,y1,mode);
				}
			}
			else
			{
				for(i=0;i<(x1-x2);i++)
				{
					GUI_DrawPoint(x2+i,y1,mode);
				}
			}
		}
		else      //画斜线
		{
			k = (float)(y2-y1)/(x2-x1);
			if(x1>x2)
			{
				abs_x = x1 - x2;
				small_x = x2;
				small_y = y2;
			}
			else
			{
				abs_x = x2 - x1;
				small_x = x1;
				small_y = y1;
				
			}
			for(i=0;i < abs_x;i++)
			{
				GUI_DrawPoint(small_x+i,small_y+i*k,mode);	
			}
		}
		GUI_DrawPoint(x2,y2,mode);//最后一个点
		GUI_DrawPoint(x1,y1,mode);//最后一个点
		
	}
}
//当restart=1，重新画线
void GUI_DrawMultiLine(uint8_t X,uint8_t Y,uint8_t restart,uint8_t mode)
{
	static uint8_t pre_pointX,pre_pointY;
	uint8_t x1,x2,y1,y2;
	if(!restart)
	{
		if(pre_pointX > X)
		{
			x1 = X;
			x2 = pre_pointX;
		}
		else
		{
			x2 = X;
			x1 = pre_pointX;		
		}
		if(pre_pointY > Y)
		{
			y1 = Y;
			y2 = pre_pointY;
		}
		else
		{
			y2 = Y;
			y1 = pre_pointY;
		}		
		GUI_DrawLine(x1,y1,x2,y2,mode);
	}
	pre_pointX = X;
	pre_pointY = Y;
	
}
void GUI_Draw_Rect(uint8_t x1,uint8_t y1,uint8_t x2, uint8_t y2,uint8_t mode)
{
	GUI_DrawLine(x1,y1,x2,y1,mode);
	GUI_DrawLine(x1,y2,x2,y2,mode);
	GUI_DrawLine(x1,y1,x1,y2,mode);
	GUI_DrawLine(x2,y1,x2,y2,mode);
}
void GUI_Draw_CenterRect(uint8_t x1,uint8_t y1, uint8_t halfL,uint8_t halfW,uint8_t mode)
{
	
	GUI_DrawLine(x1-halfL,y1-halfW,x1+halfL,y1-halfW,mode);
	GUI_DrawLine(x1-halfL,y1+halfW,x1+halfL,y1+halfW,mode);
	GUI_DrawLine(x1-halfL,y1-halfW,x1-halfL,y1+halfW,mode);
	GUI_DrawLine(x1+halfL,y1-halfW,x1+halfL,y1+halfW,mode);
}
//x,y:圆心坐标
//r:圆的半径
void GUI_DrawCircle(uint8_t x,uint8_t y,uint8_t r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        GUI_DrawPoint(x + a, y - b,1);
        GUI_DrawPoint(x - a, y - b,1);
        GUI_DrawPoint(x - a, y + b,1);
        GUI_DrawPoint(x + a, y + b,1);
 
        GUI_DrawPoint(x + b, y + a,1);
        GUI_DrawPoint(x + b, y - a,1);
        GUI_DrawPoint(x - b, y - a,1);
        GUI_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size:选择字体 12/16/24
//取模方式 逐列式
void GUI_ShowChar(uint8_t x,uint8_t y,char chr,uint8_t size,uint8_t rev)
{
	uint8_t temp,t,t1;
	uint8_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
		else return;								//没有的字库
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)GUI_DrawPoint(x,y,rev);
			else GUI_DrawPoint(x,y,!rev);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }  
}

//显示字符串
//x,y:起点坐标  
//size1:字体大小 
//*chr:字符串起始地址 
void GUI_ShowString(uint8_t x,uint8_t y,char *p,uint8_t size,uint8_t rev)
{
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;GUI_Clear();}
        GUI_ShowChar(x,y,*p,size, rev);	 
        x+=size/2;
        p++;
    }  
}
//m^n
uint32_t GUI_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}
////显示2个数字
////x,y :起点坐标	 
////len :数字的位数
////size:字体大小
void GUI_ShowNum(uint8_t x,uint8_t y,int16_t num,uint8_t len,uint8_t size1,uint8_t rev)
{
	uint8_t t,temp;
	if(num<0)
	{
		t = 1;
		
		num = -num;
		GUI_ShowChar(x,y,'-',size1,rev);
	}
	else t = 0;
	for(;t<len;t++)
	{
		temp=(num/GUI_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				GUI_ShowChar(x+(size1/2)*t,y,'0',size1,rev);
      }
			else 
			{
			  GUI_ShowChar(x+(size1/2)*t,y,temp+'0',size1,rev);
			}
  }
}
void GUI_ShowNum_Simplify(uint8_t x,uint8_t y,int16_t num,uint8_t len,uint8_t size1,uint8_t rev)
{
	uint8_t t,temp,pos=0,flag1=0;
	if(num<0)
	{
		pos = 1;
		num = -num;
	}
	for(t = 0;t<len;t++)
	{
		temp=(num/GUI_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				if(flag1 || (t == len-1))//
				  GUI_ShowChar(x+(size1/2)*t,y,'0',size1,rev);
				else//第一次显示0，转空格
				  GUI_ShowChar(x+(size1/2)*t,y,' ',size1,rev);
					
      }
			else 
			{
				flag1 = 1;
				if(pos)//在前面加负号
				{
					pos = 0;
					GUI_ShowChar(x+(size1/2)*(t-1),y,'-',size1,rev);
				}
			  GUI_ShowChar(x+(size1/2)*t,y,temp+'0',size1,rev);
			}
  }
}
//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
//取模方式 列行式
void GUI_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1)
{
	uint8_t i,m,n=0,temp,chr1;
	uint8_t y0=y,x0=x;
	uint8_t size3=size1/8;
	uint8_t size2;
	while(size3--)
	{		
		if(size1<16)
		{
			chr1=num+n;
			size2=size1*2;
		}
		else
		{
			chr1=num*size1/8+n;
			size2=size1;
		}		
		n++;
			
			for(i=0;i<size2;i++)
			{
				if(size1==12)
					temp=Hzk0[chr1][i];//调用12*12字体
				else
				if(size1==14)
					temp=Hzk1[chr1][i];//调用14*14字体
				else 
				if(size1==16)
						{temp=Hzk2[chr1][i];}//调用16*16字体
						/*
				else if(size1==24)
						{temp=Hzk3[chr1][i];}//调用24*24字体
				else if(size1==32)       
						{temp=Hzk4[chr1][i];}//调用32*32字体
				else if(size1==64)
						{temp=Hzk5[chr1][i];}//调用64*64字体*/
				else return;
							
						for(m=0;m<8;m++)
							{
								if(temp&0x01)GUI_DrawPoint(x,y,1);
								else GUI_DrawPoint(x,y,0);
								temp>>=1;
								if(y<(size1+y))
								y++;
							}
							x++;
							if((x-x0)==size1)
							{x=x0;y0=y0+8;}
							y=y0;
				 }
	}
}

/*****************GUI基础函数END*****************************************************/
/*******************************************************************************/





/*******************************************************************************/
/*******************用户函数************************************************************/
//========================================================================
// 函数:  icon_LoadAnim_RollRec()
// 描述: 一个加载的动态图标--旋转方框,每调用一次，位置更新一次.
// 参数: x,y左上角坐标，recsize方框大小，pointsize,旋转点大小，ct背景颜色.
// 返回: None.
//========================================================================

void icon_LoadAnim_RollRec(uint8_t x,uint8_t y,uint8_t recSize,uint8_t pointSize,uint8_t ct)
{
	static uint8_t xpos=0,ypos=0;
	static bool flag=0;
	
	
	if((recSize <= pointSize)||(pointSize<2))return;//超范围
	GUI_Fill(x,y,x+recSize,y+recSize,ct);//清空场地
	GUI_Fill(x+xpos,y+ypos,x+xpos+pointSize,y+ypos+pointSize,!ct);//画旋转点
	
	//右上半部分的路径
	if(flag==0)
	{
		if(xpos+pointSize < recSize)
			xpos++;
		else if(ypos+pointSize < recSize)
			ypos++;
		else flag = 1;		
	}
	//左下半部分的路径	
	else
	{
		if(xpos>0)
			xpos--;
		else if(ypos>0)
			ypos--;
		else flag=0;
	}
}

//========================================================================
// 函数: DrawBatteryRec 
// 描述: 画电池方框.
// 参数: .
// 返回: None.
//========================================================================

void DrawBatteryRec(uint8_t x, uint8_t y,uint8_t charge,uint8_t RecUpdate,uint8_t rvs)
{
	uint8_t i=0,j=0,y0=0,y1= 0;
	
	if(x <= 101)
	{
		if(RecUpdate)//画方框
		{
			//0-23
			GUI_DrawMultiLine(x,y+2,1,rvs);
			GUI_DrawMultiLine(x+2,y+2,0,rvs);
			GUI_DrawMultiLine(x+2,y,0,rvs);
			GUI_DrawMultiLine(x+26,y,0,rvs);
			GUI_DrawMultiLine(x+26,y+11,0,rvs);
			GUI_DrawMultiLine(x+2,y+11,0,rvs);
			GUI_DrawMultiLine(x+2,y+9,0,rvs);
			GUI_DrawMultiLine(x,y+9,0,rvs);
			GUI_DrawMultiLine(x,y+2,0,rvs);		
		}
		if(charge > 23)return;
		
		for(i=0;i<23;i++)
		{
			if(i < 2)//电池的尖头
			{
				y0 = y+4;
				y1 = y+7;
					
			}//电池身子
			else
			{
				y0 = y+2;
				y1 = y+9;
			}
				
			if(charge >= 23-i)//电量填充
				j=rvs;
			else
				j=!rvs;
			GUI_DrawLine(x+2+i, y0, x+2+i, y1,j);
				
		}		
	}
}
void showRxVot(uint8_t x, uint8_t y,uint16_t mV,uint8_t RecUpdate, uint8_t rev)
{
	if(RecUpdate)
	{
		GUI_ShowChar(x+16,y,'.',16,rev);
		GUI_ShowChar(x+32,y,'V',16,rev);
	}
	GUI_ShowNum(x,y,mV/100,2,16,rev);
	GUI_ShowNum(x+24,y,mV%100/10,1,16,rev);
}

void status_bar_start()
{
	GUI_Fill(0,0,127,15,white);//反相显示
	DrawBatteryRec(98,2,0,UpdateALL,dark);//电池的方框
	
	if(bttTemp > 999)bttTemp=999;
	GUI_ShowNum(2,2,bttTemp/10,2,12,dark);
	GUI_ShowChar(14,2,'.',12,dark);
	GUI_ShowNum(18,2,bttTemp%10,1,12,dark);
	GUI_ShowChar(26,2,'C',12,dark);
	GUI_DrawPoint(32,3,dark);
	GUI_DrawPoint(33,2,dark);
	GUI_DrawPoint(34,3,dark);
	GUI_DrawPoint(33,4,dark);
}



void status_bar_update()
{


	if(bttCharge > 100)bttCharge = 100;
	

	DrawBatteryRec(98,2,bttCharge/4.3478f,OnlyNum,dark);//实时更新电池
	if(bttCharge==100)
	{
		GUI_ShowNum(78,2,100,3,12,dark);
	}
	else
	{
		GUI_ShowNum(84,2,bttCharge,2,12,dark);
	}
	if(bttTemp > 999)bttTemp=999;
	GUI_ShowNum(2,2,bttTemp/10,2,12,dark);
	GUI_ShowNum(18,2,bttTemp%10,1,12,dark);
}

uint8_t CentX0=22,CentY0=42,CentX1=105,CentY1=42;//中心坐标

void main_page_start()
{
	GUI_Fill(0,15,127,63,0);//清除以前的页面
	GUI_ShowString(0,16,"PV:",16, white);
	GUI_ShowString(64,16,"mV",16, white);
	
	GUI_ShowString(0,32,"PI:", 16, white);
	GUI_ShowString(64,32,"mA",16, white);
	
	GUI_ShowChar(100,24,'.',16,white);
	GUI_ShowString(116,24,"W",16, white);
	
	GUI_DrawLine(0,50,127,50,white);
	
	GUI_ShowString(0,52,"BV:",12, white);
	GUI_ShowString(42,52,"mV",12, white);	
	
	GUI_ShowString(66,52,"BI:",12, white);
	GUI_ShowString(108,52,"mA",12, white);	

}
int32_t showPw=0;
int32_t sumres[4];
void main_page_update()
{
	static uint8_t cnt=0;
	if(cnt > 25)
	{
		sumres[0]/=cnt;
		sumres[1]/=cnt;
		sumres[2]/=cnt;
		sumres[3]/=cnt;
		GUI_ShowNum(32,16,sumres[0]/1000,2,16,white);	
		GUI_ShowChar(48,16,'.',16,white);
		GUI_ShowNum(56,16,sumres[0]%1000/10,2,16,white);	

		GUI_ShowNum(32,32,sumres[1],4,16,white);
		GUI_ShowNum(18,52,sumres[2],4,12,white);
		GUI_ShowNum(84,52,sumres[3],4,12,white);
		
		showPw = sumres[1]*sumres[0]/1000;//mW
		if(showPw < 0)showPw=0;
		if(showPw > 99999)showPw = 99999;//99W		
		GUI_ShowNum(84,24,showPw/1000,2,16,white);
		GUI_ShowNum(108,24,showPw%1000/100,1,16,white);
		cnt=0;
		for (uint8_t i = 0; i < 4; i++)
		{
			sumres[i]=0;
		}
		
	}
	else
	{
		sumres[0] += pv_mv;
		sumres[1] += pi_ma;
		sumres[2] += bv_mv;
		sumres[3] += bi_ma;
		cnt++;
	}
}

void GUI_KeyTask()
{
//	if()
}
