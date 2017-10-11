#include "iic.h"
/*
********************************************************************************
  * @author  xdh
  * @version V1.0
  * @date    2017-xx-xx
  * @apply   pcf8563_IIC
********************************************************************************
*/
/******************************************************************************

******************************************************************************/
void IIC_SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin=GPIO_PCF_SDA_B;                			 //PB7
    GPIO_InitStruct.Mode=GPIO_MODE_INPUT;      					 //����
    GPIO_InitStruct.Pull=GPIO_PULLUP;        					 	 //����
    GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;     					 //����
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
}
/******************************************************************************

******************************************************************************/
void IIC_SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin=GPIO_PCF_SDA_B;                //PB7
    GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;      //�������
    GPIO_InitStruct.Pull=GPIO_PULLUP;            	 //����
    GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;       	 //����
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
}

/*************************************
IIC��ʼ��
PCF_SCL--> PB8
PCF_SDA--> PB9
**************************************/
void PCF8563_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure
    __HAL_RCC_GPIOB_CLK_ENABLE();//��ʱ��

    GPIO_InitStruct.Pin=GPIO_PCF_SCL_B|GPIO_PCF_SDA_B;      //PB8,PB9
    GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;      //�������
    GPIO_InitStruct.Pull=GPIO_PULLUP;            	 //����
    GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;       	 //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);

    IIC_SDA() = 1;
    IIC_SCL() = 1;
}
/******************************************************************************
* Function Name --> IIC����
* Description   --> SCL�ߵ�ƽ�ڼ䣬SDA�ɸߵ�ƽͻ�䵽�͵�ƽʱ��������
*                   SCL: __________
*                                  \__________
*                   SDA: _____
*                             \_______________
* Input         --> none
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void IIC_Start(void)
{
		IIC_SDA_OUT();					//���ó����
		IIC_SDA() = 1;					//ΪSDA�½�������׼��
		IIC_SCL() = 1;					//��SCL�ߵ�ƽʱ��SDAΪ�½���ʱ����������
#if _USER_DELAY_CLK==1  	/* ��������ʹ����ʱ�������ı�ͨѶƵ�� */
		IIC_Delay();
		IIC_SDA() = 0;					//ͻ�䣬��������
		IIC_Delay();
		IIC_SCL() = 0;
		IIC_Delay();
#else  /* ����ʹ����ʱ�����ı�ͨѶƵ�� */
		IIC_SDA() = 0;					//ͻ�䣬��������
		IIC_SCL() = 0;
#endif
}
/******************************************************************************
* Function Name --> IICֹͣ
* Description   --> SCL�ߵ�ƽ�ڼ䣬SDA�ɵ͵�ƽͻ�䵽�ߵ�ƽʱֹͣ����
*                   SCL: ____________________
*                                  __________
*                   SDA: _________/
* Input         --> none
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void IIC_Stop(void)
{
		IIC_SDA_OUT();				//���ó����
		IIC_SDA() = 0;				//ΪSDA������׼��
#if _USER_DELAY_CLK==1 		/* ��������ʹ����ʱ�������ı�ͨѶƵ�� */
		IIC_Delay();
		IIC_SCL() = 1;				//��SCL�ߵ�ƽʱ��SDAΪ������ʱ������ֹͣ
		IIC_Delay();
		IIC_SDA() = 1;				//ͻ�䣬����ֹͣ
		IIC_Delay();
#else  /* ����ʹ����ʱ�����ı�ͨѶƵ�� */
		IIC_SCL() = 1;				//��SCL�ߵ�ƽʱ��SDAΪ������ʱ������ֹͣ
		IIC_SDA() = 1;				//ͻ�䣬����ֹͣ
#endif
}
/******************************************************************************
* Function Name --> ������ӻ�����Ӧ���ź�
* Description   --> none
* Input         --> a��Ӧ���ź�
*                      0��Ӧ���ź�
*                      1����Ӧ���ź�
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void IIC_Ack(u8 a)
{
		IIC_SDA_OUT();				//���ó����
		if(a)	IIC_SDA() = 1;	//����Ӧ���źŵ�ƽ
		else	IIC_SDA() = 0;
#if _USER_DELAY_CLK==1  	/* ��������ʹ����ʱ�������ı�ͨѶƵ�� */
		IIC_Delay();
		IIC_SCL() = 1;				//ΪSCL�½���׼��
		IIC_Delay();
		IIC_SCL() = 0;				//ͻ�䣬��Ӧ���źŷ��͹�ȥ
		IIC_Delay();
#else  /* ����ʹ����ʱ�����ı�ͨѶƵ�� */
		IIC_SCL() = 1;				//ΪSCL�½���׼��
		IIC_SCL() = 0;				//ͻ�䣬��Ӧ���źŷ��͹�ȥ
#endif  /* end __USER_DELAY_CLK */
}
/******************************************************************************
* Function Name --> ��IIC���߷���һ���ֽ�����
* Description   --> none
* Input         --> dat��Ҫ���͵�����
* Output        --> none
* Reaturn       --> ack������Ӧ���ź�
******************************************************************************/
u8 IIC_Write_Byte(u8 dat)
{
		u8 i;
		u8 iic_ack=0;											//iicӦ���־
		IIC_SDA_OUT();										//���ó����
		for(i = 0;i < 8;i++)
		{
			if(dat & 0x80)	IIC_SDA() = 1;	//�жϷ���λ���ȷ��͸�λ
			else	IIC_SDA() = 0;
#if _USER_DELAY_CLK==1  							/* ��������ʹ����ʱ�������ı�ͨѶƵ�� */
			IIC_Delay();
			IIC_SCL() = 1;									//ΪSCL�½���׼��
			IIC_Delay();
			IIC_SCL() = 0;									//ͻ�䣬������λ���͹�ȥ
			dat<<=1;												//��������һλ
		}																	//�ֽڷ�����ɣ���ʼ����Ӧ���ź�
		IIC_SDA() = 1;										//�ͷ�������
		IIC_SDA_IN();											//���ó�����
		IIC_Delay();
		IIC_SCL() = 1;										//ΪSCL�½���׼��
		IIC_Delay();
#else  /* ����ʹ����ʱ�����ı�ͨѶƵ�� */
			IIC_SCL() = 1;									//ΪSCL�½���׼��
			IIC_SCL() = 0;									//ͻ�䣬������λ���͹�ȥ
			dat<<=1;												//��������һλ
		}																	//�ֽڷ�����ɣ���ʼ����Ӧ���ź�
		IIC_SDA() = 1;										//�ͷ�������
		IIC_SDA_IN();											//���ó�����
		IIC_SCL() = 1;										//ΪSCL�½���׼��
#endif
		iic_ack |= IN_SDA();								//����Ӧ��λ
		IIC_SCL() = 0;
		return iic_ack;										//����Ӧ���ź�
}
/******************************************************************************
* Function Name --> ��IIC�����϶�ȡһ���ֽ�����
* Description   --> none
* Input         --> none
* Output        --> none
* Reaturn       --> x����ȡ��������
******************************************************************************/
u8 IIC_Read_Byte(void)
{
		u8 i;
		u8 x=0;
		IIC_SDA() = 1;						//������������Ϊ�ߵ�ƽ
		IIC_SDA_IN();							//���ó�����
		for(i = 0;i < 8;i++)
		{
			x <<= 1;								//�������ݣ���λ��ǰ
#if _USER_DELAY_CLK==1  			/* ��������ʹ����ʱ�������ı�ͨѶƵ�� */
			IIC_Delay();
			IIC_SCL() = 1;					//ͻ��
			IIC_Delay();
			if(IN_SDA())	x |= 0x01;	//�յ��ߵ�ƽ
			IIC_SCL() = 0;
			IIC_Delay();
		}													//���ݽ������
#else  /* ����ʹ����ʱ�����ı�ͨѶƵ�� */
			IIC_SCL() = 1;					//ͻ��
			if(IN_SDA)	x |= 0x01;	//�յ��ߵ�ƽ
			IIC_SCL() = 0;
		}													//���ݽ������
#endif
		IIC_SCL() = 0;
		return x;									//���ض�ȡ��������
}
