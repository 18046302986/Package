#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
/*
********************************************************************************
  * @author  xdh
  * @version V1.0
  * @date    2017-xx-xx
  * @apply   3000WControl
********************************************************************************
*/
#define GPIO_SD_CS_B			     GPIO_PIN_12		//ͨ���������
#define GPIO_SD_CLK_B				 GPIO_PIN_13		//�����������
#define GPIO_SD_MISO_B				GPIO_PIN_14		//��������
#define GPIO_SD_MOSI_B				GPIO_PIN_15	  //�����������

extern SPI_HandleTypeDef SPI2_Handler;  //SPI���

void SPI2_Init(void);
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI2_ReadWriteByte(u8 TxData);
#endif
