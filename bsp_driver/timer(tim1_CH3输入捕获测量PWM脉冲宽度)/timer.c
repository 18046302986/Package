 #include "timer.h"


//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
u8  TIM1CH3_CAPTURE_STA=0;	//���벶��״̬		    				
u32	TIM1CH3_CAPTURE_VAL;	//���벶��ֵ(TIM2/TIM5��32λ)



/***************************************************************************
****************************************************************************
  ���������벶�����Դ��ʵ����غ���Դ��
****************************************************************************
****************************************************************************/


TIM_HandleTypeDef TIM1_Handler;         //��ʱ��1���


//��ʱ��1ͨ��3���벶������
//arr���Զ���װֵ(TIM2,TIM5��32λ��!!)
//psc��ʱ��Ԥ��Ƶ��
void TIM1_CH3_Cap_Init(u32 arr,u16 psc)
{  
    TIM_IC_InitTypeDef TIM1_CH3Config;  
    
    TIM1_Handler.Instance=TIM1;                          //�߼���ʱ��1
    TIM1_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM1_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM1_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM1_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_IC_Init(&TIM1_Handler);//��ʼ�����벶��ʱ������
    
    TIM1_CH3Config.ICPolarity=TIM_ICPOLARITY_RISING;    //�����ز���
    TIM1_CH3Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI1��
    TIM1_CH3Config.ICPrescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM1_CH3Config.ICFilter=0;                          //���������˲��������˲�
    HAL_TIM_IC_ConfigChannel(&TIM1_Handler,&TIM1_CH3Config,TIM_CHANNEL_3);//����TIM1ͨ��3
	
    HAL_TIM_IC_Start_IT(&TIM1_Handler,TIM_CHANNEL_3);   //����TIM1�Ĳ���ͨ��3�����ҿ��������ж�
    __HAL_TIM_ENABLE_IT(&TIM1_Handler,TIM_IT_UPDATE);   //ʹ�ܸ����ж�
}


//��ʱ��5�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_IC_Init()����
//htim:��ʱ��5���
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM1_CLK_ENABLE();            //ʹ��TIM1ʱ��
    __HAL_RCC_GPIOE_CLK_ENABLE();					//����GPIOEʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_13;            		//P13
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //�����������
    GPIO_Initure.Pull=GPIO_PULLUP;       			 //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    GPIO_Initure.Alternate=GPIO_AF1_TIM1; 	  //PA0����ΪTIM5ͨ��1
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM1_CC_IRQn,2,1);    //�����ж����ȼ�����ռ���ȼ�2�������ȼ�1
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);          //����ITM5�ж�ͨ��  
}


//��ʱ��1�жϷ�����
void TIM1_CC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM1_Handler);//��ʱ�����ô�����
}
 

//��ʱ�������жϣ�����������жϴ���ص������� �ú�����HAL_TIM_IRQHandler�лᱻ����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//�����жϣ����������ʱִ��
{
	if (htim->Instance == TIM1)//��ʱ��1���
	{
		if((TIM1CH3_CAPTURE_STA&0X80)==0)//��δ�ɹ�����
		{
				if(TIM1CH3_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
				{
					if((TIM1CH3_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
					{
						TIM1CH3_CAPTURE_STA|=0X80;		//��ǳɹ�������һ��
						TIM1CH3_CAPTURE_VAL=0XFFFFFFFF;
					}else TIM1CH3_CAPTURE_STA++;
				}	 
		}	
	}		
}

//��ʱ�����벶���жϴ���ص��������ú�����HAL_TIM_IRQHandler�лᱻ����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//�����жϷ���ʱִ��
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)//����ͨ��3
	{
		if((TIM1CH3_CAPTURE_STA&0X80)==0)//��δ�ɹ�����
		{
			if(TIM1CH3_CAPTURE_STA&0X40)		//����һ���½��� 		
				{	  			
					TIM1CH3_CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
					TIM1CH3_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM1_Handler,TIM_CHANNEL_3);//��ȡ��ǰ�Ĳ���ֵ.
					TIM_RESET_CAPTUREPOLARITY(&TIM1_Handler,TIM_CHANNEL_3);   //һ��Ҫ�����ԭ�������ã�������״̬
					TIM_SET_CAPTUREPOLARITY(&TIM1_Handler,TIM_CHANNEL_3,TIM_ICPOLARITY_RISING);//����TIM1ͨ��3�����ز���
				}else  								//��δ��ʼ,��һ�β���������
				{
					TIM1CH3_CAPTURE_STA=0;			//���
					TIM1CH3_CAPTURE_VAL=0;
					TIM1CH3_CAPTURE_STA|=0X40;		//��ǲ�����������
					__HAL_TIM_DISABLE(&TIM1_Handler);    //�رն�ʱ��1
					__HAL_TIM_SET_COUNTER(&TIM1_Handler,0);//���õ�ǰ����ֵCNT
					TIM_RESET_CAPTUREPOLARITY(&TIM1_Handler,TIM_CHANNEL_3);   //һ��Ҫ�����ԭ�������ã���
					TIM_SET_CAPTUREPOLARITY(&TIM1_Handler,TIM_CHANNEL_3,TIM_ICPOLARITY_FALLING);//��ʱ��5ͨ��1����Ϊ�½��ز���
					__HAL_TIM_ENABLE(&TIM5_Handler);//ʹ�ܶ�ʱ��5
				}		    
		}
	}		
	
}
