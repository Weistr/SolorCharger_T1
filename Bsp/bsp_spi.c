#include "bsp_spi.h"
#include "ssd1306.h"

/* Exported types ------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
extern uint8_t GRAM[8][128];
extern uint8_t GRAM_Y;
/* END-------------------------------------------------------------------------*/



uint8_t SPI1_ReadWriteByte(uint8_t txdata)
{
	uint8_t rxdata=00;
	HAL_SPI_TransmitReceive(&hspi1,&txdata,&rxdata,1,55);
	return rxdata;
}



void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi ->Instance == SPI1)
	{
		/*
		if(GRAM_Y<7)
		{
			GRAM_Y++;
			OLED_WR_Byte(0xb0+GRAM_Y,OLED_CMD); //设置行起始地址
			OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
			OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
			HAL_SPI_Transmit_DMA(&hspi1,&GRAM[GRAM_Y][0],128);		
		}
		*/
	}
}
