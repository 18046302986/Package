/*
********************************************************************************
  * @author  xdh
  * @version V1.0
  * @date    2017-xx-xx
  * @apply   3000WControl
********************************************************************************
*/
#ifndef __SHT10_H__
#define __SHT10_H__
#include "sys.h"

enum {TEMP, HUMI};

extern u32 M_T;
extern u32 M_H;

/*
    SHT10_DATA --> PF0
    STH10_CLK--> PF1
*/
/* GPIO��غ궨�� --> ��ֲ�޸��ļ�*/
#define SHT10_RCC_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOF_CLK_ENABLE()
#define GPIO_SHT10_DATA_F       GPIO_PIN_0//Pin
#define GPIO_SHT10_CLK_F        GPIO_PIN_1
#define SHT10_DATA_PORT        GPIOF//port �˿�
#define SHT10_SCK_PORT         GPIOF

#define SHT10_DATA_H()				PFout(0)=1			 //����DATA������
#define SHT10_DATA_L()				PFout(0)=0			 //����DATA������
#define SHT10_DATA_R()				PFin(0)			    //��DATA������
#define SHT10_SCK_H()	  			PFout(1)=1			 //����SCKʱ����
#define SHT10_SCK_L()				PFout(1)=0		   //����SCKʱ����


/* ��������غ궨�� */
#define        noACK        0
#define      ACK             1
                                                                //addr  command         r/w
#define STATUS_REG_W        0x06        //000         0011          0          д״̬�Ĵ���
#define STATUS_REG_R        0x07        //000         0011          1          ��״̬�Ĵ���
#define MEASURE_TEMP         0x03        //000         0001          1          �����¶�
#define MEASURE_HUMI         0x05        //000         0010          1          ����ʪ��
#define SOFTRESET       0x1E        //000         1111          0          ��λ


void SHT10_Config(void);
void SHT10_ConReset(void);
u8 SHT10_SoftReset(void);
u8 SHT10_Measure(u16 *p_value, u8 *p_checksum, u8 mode);
void SHT10_Calculate(u16 t, u16 rh,float *p_temperature, float *p_humidity);
float SHT10_CalcuDewPoint(float t, float h);


#endif
