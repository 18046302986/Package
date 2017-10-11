/*
********************************************************************************
  * @author  xdh
  * @version V1.0
  * @date    2017-xx-xx
  * @apply   3000WControl
********************************************************************************
*/
#include "sht10.h"
#include <math.h>
#include "delay.h"
//������ȡ��ʪ�ȵı���
u32 M_T=0;
u32 M_H=0;

/*************************************************************
  Function   ��SHT10_Config
  Description����ʼ�� SHT10����
  Input      : none
  return     : none
*************************************************************/
void SHT10_Config(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        //��ʼ��SHT10����ʱ��
        SHT10_RCC_GPIO_CLK_ENABLE();
        //PF0   DATA 
        GPIO_InitStruct.Pin=GPIO_SHT10_DATA_F;
        GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;    //�������
        GPIO_InitStruct.Pull=GPIO_PULLUP;            //����
        GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;       //����
        HAL_GPIO_Init(SHT10_DATA_PORT,&GPIO_InitStruct);
        //PF1 SCK �������
        GPIO_InitStruct.Pin=GPIO_SHT10_CLK_F;
        GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;    //�������
        GPIO_InitStruct.Pull=GPIO_PULLUP;            //����
        GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;       //����
        HAL_GPIO_Init(SHT10_SCK_PORT,&GPIO_InitStruct);
        delay_ms(50);
        SHT10_ConReset();        //��λͨѶ
        delay_ms(20);
}


/*************************************************************
  Function   ��SHT10_DATAOut
  Description������DATA����Ϊ���
  Input      : none
  return     : none
*************************************************************/
void SHT10_DATAOut(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
		//PF0 DATA �������
	GPIO_InitStructure.Pin=GPIO_SHT10_DATA_F;
	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_PP;	  //ͨ���������
	GPIO_InitStructure.Pull=GPIO_NOPULL;					  //��������
	GPIO_InitStructure.Speed=GPIO_SPEED_FAST;			  //����
	HAL_GPIO_Init(SHT10_DATA_PORT,&GPIO_InitStructure);
}


/*************************************************************
  Function   ��SHT10_DATAIn
  Description������DATA����Ϊ����
  Input      : none
  return     : none
*************************************************************/
void SHT10_DATAIn(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	//PF0 DATA ��������
	GPIO_InitStructure.Pin=GPIO_SHT10_DATA_F;			  //PD3
	GPIO_InitStructure.Mode=GPIO_MODE_INPUT;				//����
	GPIO_InitStructure.Pull=GPIO_NOPULL;				  //��������
	GPIO_InitStructure.Speed=GPIO_SPEED_FAST;			  //����
	HAL_GPIO_Init(SHT10_DATA_PORT,&GPIO_InitStructure);
}


/*************************************************************
  Function   ��SHT10_WriteByte
  Description��д1�ֽ�
  Input      : value:Ҫд����ֽ�
  return     : err: 0-��ȷ  1-����
*************************************************************/
u8 SHT10_WriteByte(u8 value)
{
		u32 i, err = 0;

		SHT10_DATAOut();			        //����DATA������Ϊ���

		for(i = 0x80; i > 0; i /= 2)  //д1���ֽ�
		{
			if(i & value)
				SHT10_DATA_H();
			else
				SHT10_DATA_L();
			delay_us(2);
			SHT10_SCK_H();
			delay_us(2);
			SHT10_SCK_L();
			delay_us(2);
		}
		SHT10_DATA_H();
		SHT10_DATAIn();				    //����DATA������Ϊ����,�ͷ�DATA��
		SHT10_SCK_H();
		delay_us(2);
		err = SHT10_DATA_R();		  //��ȡSHT10��Ӧ��λ
		SHT10_SCK_L();

		return err;
}

/*************************************************************
  Function   ��SHT10_ReadByte
  Description����1�ֽ�����
  Input      : Ack: 0-��Ӧ��  1-Ӧ��
  return     : err: 0-��ȷ 1-����
*************************************************************/
u8 SHT10_ReadByte(u8 Ack)
{
		u8 i, val = 0;

		SHT10_DATAIn();				  //����DATA������Ϊ����
		for(i = 0x80; i > 0; i /= 2)  //��ȡ1�ֽڵ�����
		{
			delay_us(2);
			SHT10_SCK_H();
			delay_us(2);
			if(SHT10_DATA_R())
				val = (val | i);
			SHT10_SCK_L();
		}
		SHT10_DATAOut();			  //����DATA������Ϊ���
		if(Ack)
			SHT10_DATA_L();			  //Ӧ��������ȥ������ȥ������(У������)
		else
			SHT10_DATA_H();			  //��Ӧ���������˽���
		delay_us(2);
		SHT10_SCK_H();
		delay_us(2);
		SHT10_SCK_L();
		delay_us(2);

		return val;					  //���ض�����ֵ
}


/*************************************************************
  Function   ��SHT10_TransStart
  Description����ʼ�����źţ�ʱ�����£�
                     _____         ________
               DATA:      |_______|
                         ___     ___
               SCK : ___|   |___|   |______
  Input      : none
  return     : none
*************************************************************/
void SHT10_TransStart(void)
{
        SHT10_DATAOut();                          //����DATA������Ϊ���

        SHT10_DATA_H();
        SHT10_SCK_L();
        delay_us(2);
        SHT10_SCK_H();
        delay_us(1);
        SHT10_DATA_L();
        delay_us(1);
        SHT10_SCK_L();
        delay_us(2);
        SHT10_SCK_H();
        delay_us(1);
        SHT10_DATA_H();
        delay_us(1);
        SHT10_SCK_L();
}

/*************************************************************
  Function   ��SHT10_ConReset
  Description��ͨѶ��λ��ʱ�����£�
                     _____________________________________________________         ________
               DATA:                                                      |_______|
                        _    _    _    _    _    _    _    _    _        ___     ___
               SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
  Input      : none
  return     : none
*************************************************************/
void SHT10_ConReset(void)
{
        u8 i;

        SHT10_DATAOut();

        SHT10_DATA_H();
        SHT10_SCK_L();

        for(i = 0; i < 9; i++)                  //����SCKʱ��9c��
        {
                SHT10_SCK_H();
                delay_us(2);
                SHT10_SCK_L();
                delay_us(2);
        }
        SHT10_TransStart();                          //��������
}



/*************************************************************
  Function   ��SHT10_SoftReset
  Description����λ
  Input      : none
  return     : err: 0-��ȷ  1-����
*************************************************************/
u8 SHT10_SoftReset(void)
{
        u8 err = 0;

        SHT10_ConReset();                              //ͨѶ��λ
        err += SHT10_WriteByte(SOFTRESET);//дRESET��λ����

        return err;
}


/*************************************************************
  Function   ��SHT10_ReadStatusReg
  Description����״̬�Ĵ���
  Input      : p_value-���������ݣ�p_checksun-������У������
  return     : err: 0-��ȷ  0-����
*************************************************************/
u8 SHT10_ReadStatusReg(u8 *p_value, u8 *p_checksum)
{
        u8 err = 0;

        SHT10_TransStart();                                        //��ʼ����
        err = SHT10_WriteByte(STATUS_REG_R);//дSTATUS_REG_R��ȡ״̬�Ĵ�������
        *p_value = SHT10_ReadByte(ACK);                //��ȡ״̬����
        *p_checksum = SHT10_ReadByte(noACK);//��ȡ���������

        return err;
}



/*************************************************************
  Function   ��SHT10_WriteStatusReg
  Description��д״̬�Ĵ���
  Input      : p_value-Ҫд�������ֵ
  return     : err: 0-��ȷ  1-����
*************************************************************/
u8 SHT10_WriteStatusReg(u8 *p_value)
{
        u8 err = 0;

        SHT10_TransStart();                                         //��ʼ����
        err += SHT10_WriteByte(STATUS_REG_W);//дSTATUS_REG_Wд״̬�Ĵ�������
        err += SHT10_WriteByte(*p_value);         //д������ֵ

        return err;
}



/*************************************************************
  Function   ��SHT10_Measure
  Description������ʪ�ȴ�������ȡ��ʪ��
  Input      : p_value-������ֵ��p_checksum-������У����
  return     : err: 0-��ȷ 1������
*************************************************************/
u8 SHT10_Measure(u16 *p_value, u8 *p_checksum, u8 mode)
{
        u8 err = 0;
        // u32 i;
        u8 value_H = 0;
        u8 value_L = 0;

        SHT10_TransStart();                                                 //��ʼ����
        switch(mode)
        {
        case TEMP:                                                                 //�����¶�
                err += SHT10_WriteByte(MEASURE_TEMP);//дMEASURE_TEMP�����¶�����
                break;
        case HUMI:
                err += SHT10_WriteByte(MEASURE_HUMI);//дMEASURE_HUMI����ʪ������
                break;
        default:
                break;
        }
        SHT10_DATAIn();
        // for(i = 0; i < 72000000; i++)                             //�ȴ�DATA�źű�����
        // {
        //         if(SHT10_DATA_R() == 0) break;             //��⵽DATA�������ˣ�����ѭ��
        // }
        delay_ms(320);
        if(SHT10_DATA_R() == 1)                                //����ȴ���ʱ��
                err += 1;
        value_H = SHT10_ReadByte(ACK);
        value_L = SHT10_ReadByte(ACK);
        *p_checksum = SHT10_ReadByte(noACK);           //��ȡУ������
        *p_value = (value_H << 8) | value_L;
        return err;
}

/*************************************************************
  Function   ��SHT10_Calculate
  Description��������ʪ�ȵ�ֵ
  Input      : Temp-�Ӵ������������¶�ֵ��Humi-�Ӵ�����������ʪ��ֵ
               p_humidity-�������ʵ�ʵ�ʪ��ֵ��p_temperature-�������ʵ���¶�ֵ
  return     : none
	mode       : 14bit�¶Ȳ���     12bitʪ�Ȳ���
14bit�¶Ȳ���: d2=0.01   d1=-40.1(5V) d1=-39.8(4V) d1=-39.7(3.5V) d1=-39.6(3V) d1=-39.4(2.5V)
12bitʪ�Ȳ���: t1=0.01 t2=0.00008 c1=-2.0468  c2=0.0367 c3=-1.5955E-6
	temperature: temperature=d1+d2*t        rh_linear = c1 + c2*rh + c3 * rh*rh
		 humidity: humidity = ( temperature - 25 ) * ( t1 + t2*rh )+ rh_linear
*************************************************************/
void SHT10_Calculate(u16 t, u16 rh, u32 *p_temperature, u32 *p_humidity)
{
		unsigned long int RHlinear,RHture;
		t=t&0x3fff;
		rh=rh&0xfff;
		if(t>3965)t-=3965;   	//14bit   3.3V
		else
			t=0;

		RHlinear = rh*3.67-204.68-(rh*rh*1.6/10000);
		RHture = RHlinear+(t-2500)*(0.01+0.00008*rh/100);
		if(RHture>9999)
			RHture=9999;
		*p_humidity = RHture;
		*p_temperature = t;
}
/***************************************************************
��ȡ�¶ȣ������ȶ�������Ҫ320ms
***************************************************************/
u32 SHT10_Get_Temp(void)
{
	u8 err=0,checksum=0;
	u16 humi_val=0,temp_val;
	err += SHT10_Measure(&temp_val, &checksum, TEMP);
	if(err != 0)
	{
		err = 0;
		SHT10_ConReset();
	}
	else
	{
		SHT10_Calculate(temp_val, humi_val, &M_T, &M_H);
        return M_T;
		// GD_BUF[INDEX_CONTROL_TEMP]=M_T
	}
}
/***************************************************************
��ȡʪ�ȣ������ȶ�������Ҫ320ms
***************************************************************/
u32 SHT10_Get_Humi(void)
{
	u8 err=0,checksum=0;
	u16 humi_val,temp_val=0;
	err += SHT10_Measure(&humi_val, &checksum, HUMI);
	if(err != 0)
	{
		err = 0;
		SHT10_ConReset();
	}
	else
	{
		SHT10_Calculate(temp_val, humi_val, &M_T, &M_H);
		// GD_BUF[INDEX_CONTROL_HUMI]=M_H;
        return M_H;
	}
}
/************************************************************
  Function   ��SHT10_CalcuDewPoint
  Description������¶��
  Input      : h-ʵ�ʵ�ʪ�ȣ�t-ʵ�ʵ��¶�
  return     : dew_point-¶��
**************************************************************/
float SHT10_CalcuDewPoint(float t, float h)
{
	float logEx, dew_point;

  logEx = (log10(h)-2)/0.4343f + (17.62f*t)/(243.12f+t);
  dew_point = 243.12f*logEx/(17.62f-logEx);

	return dew_point;
}
