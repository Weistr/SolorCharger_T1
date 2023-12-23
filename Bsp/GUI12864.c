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



//�����Դ浽OLED	
void GUI_DisplayRefresh(void)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
	  OLED_WR_Byte(0xb0+i,OLED_CMD); //��������ʼ��ַ
	  OLED_WR_Byte(0x00,OLED_CMD);   //���õ�����ʼ��ַ
	  OLED_WR_Byte(0x10,OLED_CMD);   //���ø�����ʼ��ַ
	  //for(n=0;n<128;n++)
		OLED_DC_Set();
		OLED_CS_Clr();
		HAL_SPI_Transmit(&hspi1,&GRAM[i][0],128,100);
//		OLED_WR_Byte(GRAM[n][i],OLED_DATA);
  }
}


//��������
void GUI_Clear(void)
{
	uint8_t i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 GRAM[i][n]=0x00;//�����������
			}
  }
}

//���� 
//x:0~127
//y:0~63
void GUI_DrawPoint(uint8_t x,uint8_t y,uint8_t mode)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(mode)GRAM[pos][x]|=temp;
	else GRAM[pos][x]&=~temp;	    
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void GUI_Fill(uint8_t  x1,uint8_t  y1,uint8_t  x2,uint8_t  y2,uint8_t  dot)  
{  
	uint8_t  x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)GUI_DrawPoint(x,y,dot);
	}													    
}
//����
//x:0~128
void GUI_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode)
{
	uint8_t i,small_x,small_y,abs_x;
	float k=0;
	if((x2>128)||(y2>64)){}
	else
	{
		if(x1==x2)    //������
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
		else if(y1==y2)   //������
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
		else      //��б��
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
		GUI_DrawPoint(x2,y2,mode);//���һ����
		GUI_DrawPoint(x1,y1,mode);//���һ����
		
	}
}
//��restart=1�����»���
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
//x,y:Բ������
//r:Բ�İ뾶
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
        num = (a * a + b * b) - r*r;//���㻭�ĵ���Բ�ĵľ���
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//size:ѡ������ 12/16/24
//ȡģ��ʽ ����ʽ
void GUI_ShowChar(uint8_t x,uint8_t y,char chr,uint8_t size,uint8_t rev)
{
	uint8_t temp,t,t1;
	uint8_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
		else return;								//û�е��ֿ�
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

//��ʾ�ַ���
//x,y:�������  
//size1:�����С 
//*chr:�ַ�����ʼ��ַ 
void GUI_ShowString(uint8_t x,uint8_t y,char *p,uint8_t size,uint8_t rev)
{
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
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
////��ʾ2������
////x,y :�������	 
////len :���ֵ�λ��
////size:�����С
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
				else//��һ����ʾ0��ת�ո�
				  GUI_ShowChar(x+(size1/2)*t,y,' ',size1,rev);
					
      }
			else 
			{
				flag1 = 1;
				if(pos)//��ǰ��Ӹ���
				{
					pos = 0;
					GUI_ShowChar(x+(size1/2)*(t-1),y,'-',size1,rev);
				}
			  GUI_ShowChar(x+(size1/2)*t,y,temp+'0',size1,rev);
			}
  }
}
//��ʾ����
//x,y:�������
//num:���ֶ�Ӧ�����
//ȡģ��ʽ ����ʽ
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
					temp=Hzk0[chr1][i];//����12*12����
				else
				if(size1==14)
					temp=Hzk1[chr1][i];//����14*14����
				else 
				if(size1==16)
						{temp=Hzk2[chr1][i];}//����16*16����
						/*
				else if(size1==24)
						{temp=Hzk3[chr1][i];}//����24*24����
				else if(size1==32)       
						{temp=Hzk4[chr1][i];}//����32*32����
				else if(size1==64)
						{temp=Hzk5[chr1][i];}//����64*64����*/
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

/*****************GUI��������END*****************************************************/
/*******************************************************************************/





/*******************************************************************************/
/*******************�û�����************************************************************/
//========================================================================
// ����:  icon_LoadAnim_RollRec()
// ����: һ�����صĶ�̬ͼ��--��ת����,ÿ����һ�Σ�λ�ø���һ��.
// ����: x,y���Ͻ����꣬recsize�����С��pointsize,��ת���С��ct������ɫ.
// ����: None.
//========================================================================

void icon_LoadAnim_RollRec(uint8_t x,uint8_t y,uint8_t recSize,uint8_t pointSize,uint8_t ct)
{
	static uint8_t xpos=0,ypos=0;
	static bool flag=0;
	
	
	if((recSize <= pointSize)||(pointSize<2))return;//����Χ
	GUI_Fill(x,y,x+recSize,y+recSize,ct);//��ճ���
	GUI_Fill(x+xpos,y+ypos,x+xpos+pointSize,y+ypos+pointSize,!ct);//����ת��
	
	//���ϰ벿�ֵ�·��
	if(flag==0)
	{
		if(xpos+pointSize < recSize)
			xpos++;
		else if(ypos+pointSize < recSize)
			ypos++;
		else flag = 1;		
	}
	//���°벿�ֵ�·��	
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
// ����: DrawBatteryRec 
// ����: ����ط���.
// ����: .
// ����: None.
//========================================================================

void DrawBatteryRec(uint8_t x, uint8_t y,uint8_t charge,uint8_t RecUpdate,uint8_t rvs)
{
	uint8_t i=0,j=0,y0=0,y1= 0;
	
	if(x <= 101)
	{
		if(RecUpdate)//������
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
			if(i < 2)//��صļ�ͷ
			{
				y0 = y+4;
				y1 = y+7;
					
			}//�������
			else
			{
				y0 = y+2;
				y1 = y+9;
			}
				
			if(charge >= 23-i)//�������
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
	GUI_Fill(0,0,127,15,white);//������ʾ
	DrawBatteryRec(98,2,0,UpdateALL,dark);//��صķ���
	
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
	

	DrawBatteryRec(98,2,bttCharge/4.3478f,OnlyNum,dark);//ʵʱ���µ��
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

uint8_t CentX0=22,CentY0=42,CentX1=105,CentY1=42;//��������

void main_page_start()
{
	GUI_Fill(0,15,127,63,0);//�����ǰ��ҳ��
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
