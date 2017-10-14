#include "usart.h"
#include "delay.h"
#include "led.h"
#include "cmd.h"

UART_HandleTypeDef UART2_Handler;//UART2���
DMA_HandleTypeDef UART2TxDMA_Handler;//DMA���

UART_HandleTypeDef UART4_Handler;//UART3���
DMA_HandleTypeDef UART4TxDMA_Handler;//UART4_DMA���

uint8_t USART2_TX_BUF[USART_REC_LEN]={0xef,0xfe,0xff,0x01,0,0,0,0,0,0,0,0,0,0,0};
uint8_t USART4_TX_BUF[USART_REC_LEN]={0xef,0xfe,0xff,0x01,0,0,0,0,0,0,0,0,0,0,0};

uint8_t USART2_RX_BUF[USART_REC_LEN];
uint8_t USART4_RX_BUF[USART_REC_LEN];

uint8_t aRx2Buffer[RXBUFFERSIZE];
uint8_t aRx4Buffer[RXBUFFERSIZE];


uint16_t USART2_RX_STA = 0;     //����״̬���
uint16_t USART4_RX_STA = 0;     //����״̬���

uint16_t USART2_TX_STA = 0;


u8 SEND_OVER_FLAG = 0;

#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������
	USART2->DR = (u8) ch;
	return ch;
}
#endif

void usart_init(uint32_t bound)
{
		//ʹ��ʱ��
		__HAL_RCC_DMA1_CLK_ENABLE();
		//UART2 ��ʼ������
		UART2_Handler.Instance = USART2;       				    // USART2
		UART2_Handler.Init.BaudRate = bound;   					//������
		UART2_Handler.Init.WordLength = UART_WORDLENGTH_8B; 	//�ֳ�Ϊ8λ
		UART2_Handler.Init.StopBits = UART_STOPBITS_1;   		 //һ��ֹͣλ
		UART2_Handler.Init.Parity = UART_PARITY_NONE;    		 //����żУ��
		UART2_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;	     //��Ӳ��������
		UART2_Handler.Init.Mode = UART_MODE_TX_RX;  			//�շ�ģʽ
		HAL_UART_Init(&UART2_Handler);    					  //��ʼ�����ڵ�ʱ����Զ�ʹ��UART2�����ҵ��ûص�����
		//����  huart->State= HAL_UART_STATE_READY
		//����DMA����������
		__HAL_LINKDMA(&UART2_Handler, hdmatx, UART2TxDMA_Handler);//��DMA��USART2��ϵ������
		//Tx_DMA����
		UART2TxDMA_Handler.Instance = DMA1_Stream6;							//������
		UART2TxDMA_Handler.Init.Channel = DMA_CHANNEL_4;					//ͨ��ѡ��
		UART2TxDMA_Handler.Init.Direction = DMA_MEMORY_TO_PERIPH;			//�ڴ浽����
		UART2TxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;				//�����ַ����
		UART2TxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;					//�ڴ��ַ����
		UART2TxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;	//�������ݿ��8BIT
		UART2TxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;		//�ڴ����ݿ��8BIT
		UART2TxDMA_Handler.Init.Mode = DMA_NORMAL;							//������ͨģʽ
		UART2TxDMA_Handler.Init.Priority = DMA_PRIORITY_MEDIUM;			   //�е����ȼ�
		UART2TxDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_DISABLE;			//��ʹ��FIFOģʽ
		UART2TxDMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		UART2TxDMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;				//�ڴ�ͻ�����δ���
		UART2TxDMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE;			//����ͻ�����δ���

		HAL_DMA_DeInit(&UART2TxDMA_Handler);//�Ȱ�DMA�Ĵ�������Ϊȱʡֵ��Ĭ��ֵ��
		HAL_DMA_Init(&UART2TxDMA_Handler);//��ʼ��DMA

		//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
		HAL_UART_Transmit_DMA(&UART2_Handler,(u8*)USART2_TX_BUF,USART_REC_LEN); //��������
		HAL_UART_Receive_IT(&UART2_Handler, (uint8_t *)aRx2Buffer, RXBUFFERSIZE);
		//ÿ�ν���RXBUFFERSIZE���ַ��󣬱�ʾ���ܽ������Ӷ�����ص�����HAL_UART_RxCpltCallback������Ӧ�Ĵ���

		//UART4��ʼ������
		UART4_Handler.Instance = UART4;       				    // USART4
		UART4_Handler.Init.BaudRate = 115200;   					//������
		UART4_Handler.Init.WordLength = UART_WORDLENGTH_8B; 	//�ֳ�Ϊ8λ
		UART4_Handler.Init.StopBits = UART_STOPBITS_1;   		 //һ��ֹͣλ
		UART4_Handler.Init.Parity = UART_PARITY_NONE;    		 //����żУ��
		UART4_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;	     //��Ӳ��������
		UART4_Handler.Init.Mode = UART_MODE_TX_RX;  			//�շ�ģʽ
		HAL_UART_Init(&UART4_Handler);    					  //��ʼ�����ڵ�ʱ����Զ�ʹ��UART2�����ҵ��ûص�����

    //����DMA�ʹ�������
    __HAL_LINKDMA(&UART4_Handler,hdmatx,UART4TxDMA_Handler); //��DMA��UART4��ϵ����(����DMA)
    //Tx DMA����
    UART4TxDMA_Handler.Instance=DMA1_Stream4;                           //������ѡ��
    UART4TxDMA_Handler.Init.Channel=DMA_CHANNEL_4;                      //ͨ��ѡ��
    UART4TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;             //�洢��������
    UART4TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //���������ģʽ
    UART4TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ
    UART4TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //�������ݳ���:8λ
    UART4TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //�洢�����ݳ���:8λ
    UART4TxDMA_Handler.Init.Mode=DMA_NORMAL;                            //������ͨģʽ
    UART4TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //�е����ȼ�
    UART4TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;
    UART4TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    UART4TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //�洢��ͻ�����δ���
    UART4TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //����ͻ�����δ���

    HAL_DMA_DeInit(&UART4TxDMA_Handler);//����Ϊȱʡֵ
    HAL_DMA_Init(&UART4TxDMA_Handler);//��ʼ��

    RS485EN_RX();//��ʼ����485Ϊ����ģʽ
	__HAL_UART_ENABLE_IT(&UART4_Handler,USART_IT_RXNE);  //ʹ�ܽ����ж�
	__HAL_UART_DISABLE_IT(&UART4_Handler,USART_IT_TC);   //��ֹ��������ж�

		HAL_UART_Transmit_DMA(&UART4_Handler,(u8*)USART4_TX_BUF,USART_REC_LEN); //��������
		HAL_UART_Receive_IT(&UART4_Handler, (u8 *)aRx4Buffer, RXBUFFERSIZE);	//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
}

//����һ��DMA����
//huart:���ھ��
//pData�����������ָ��
//Size:�����������
void MYDMA_USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    HAL_DMA_Start(huart->hdmatx, (u32)pData, (uint32_t)&huart->Instance->DR, Size);//����DMA����
    huart->Instance->CR3 |= USART_CR3_DMAT;//ʹ�ܴ���DMA����
}

//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStruct;

	if (huart->Instance == USART2)//����Ǵ���2������д���2 MSP ����
	{
		__HAL_RCC_USART2_CLK_ENABLE();  //ʹ��USART2ʱ��
		__HAL_RCC_GPIOD_CLK_ENABLE(); //ʹ��GPIOD

		//�����������
		GPIO_InitStruct.Pin=GPIO_PCTXD_D|GPIO_PCRXD_D;			//PD5 PD6
		GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;		  				//�����������
		GPIO_InitStruct.Pull=GPIO_PULLUP;			    				//����
		GPIO_InitStruct.Speed=GPIO_SPEED_FAST;		  			//����
		GPIO_InitStruct.Alternate=GPIO_AF7_USART2;				//����ΪUSART2
		HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);	   				//��ʼ��PD5,PD6

		//����2�ж�����
		HAL_NVIC_EnableIRQ(USART2_IRQn);//ʹ��USART2�ж�ͨ��
		HAL_NVIC_SetPriority(USART2_IRQn, 2, 7);

	}
	if (huart->Instance == UART4)//����Ǵ���4
	{
		__HAL_RCC_UART4_CLK_ENABLE();//ʹ�ܴ���4ʱ��
		__HAL_RCC_GPIOA_CLK_ENABLE();//ʹ��GPIOAʱ��
		__HAL_RCC_GPIOC_CLK_ENABLE();//ʹ��GPIOCʱ��

    	//�����������
		GPIO_InitStruct.Pin=GPIO_485RX_C|GPIO_485TX_C;		//PA0 PA1
		GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;		  			//�����������
		GPIO_InitStruct.Pull=GPIO_PULLUP;			   			//����
		GPIO_InitStruct.Speed=GPIO_SPEED_FAST;		  			//����
		GPIO_InitStruct.Alternate=GPIO_AF8_UART4;				//����ΪUSART4
		HAL_GPIO_Init(GPIOC,&GPIO_InitStruct)					//��ʼ��
		//ͨ���������
		GPIO_InitStruct.Pin=GPIO_485EN_A; 			//PA15
		GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;    //�������
		GPIO_InitStruct.Pull=GPIO_PULLUP;            //����
		GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;       //����
		HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
		/*
			���ｫ����2�ʹ���4�����ȼ���Ϊһ�¡�
		*/
		//����4�ж�����
		HAL_NVIC_EnableIRQ(UART4_IRQn);//ʹ��USART2�ж�ͨ��
		HAL_NVIC_SetPriority(UART4_IRQn, 2, 7);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
}

//�����жϺ���
void USART2_IRQHandler(void)
{
	u8 Res;
	if(USART2->SR & (1<<6))//��������ж�,�ñ�־λ
	{
		USART2->SR &= ~(1<<6);//�巢������ж�
	}
	if(USART2->SR & (1<<3))//�������
	{
		USART2->SR &= ~(1<<3);
	}
	if(USART2->SR & (1<<5))
	{
		Res = USART2->DR;   //��ȡ���ݼĴ���,��SR�жϱ�־

		USART2->SR &= ~(1<<5);//�巢������ж�
		if((USART2_RX_STA & 0xfff) == 13)  //����֡β
		{
			++USART2_RX_STA;
			USART2_RX_BUF[USART2_RX_STA & 0xfff] = Res; //����У��λ
			////�յ�һ֡������У�飬��ִ�У�Ȼ��ظ�

			//�ӿ����յ���ȷ��һ֡���ݺ�����
			if(pc_to_control_crc())
			{
				if(run_pc_cmd())
				{
					replay_pc();
				}
			}
//			if(run_pc_cmd())
//			{
//				replay_pc();
//			}
			USART2_RX_STA = 0;
		}
		else if(USART2_RX_STA & 0x4000) //����һ֡���ݵĵ�3��----��9������
		{
			++USART2_RX_STA;
			USART2_RX_BUF[USART2_RX_STA & 0xfff] = Res;
		}
		else if(USART2_RX_STA & 0x8000) //����һ֡���ݵĵڶ������� ---0xfe
		{
			if(Res != 0xfe)
			USART2_RX_STA = 0;
			else
			{
				USART2_RX_BUF[(++USART2_RX_STA)&0xfff] = Res;
				USART2_RX_STA |= 0x4000;
			}
		}
		else if(Res == 0xef)  //����һ֡���ݵĵ�һ������ ---0xef
		{
			USART2_RX_BUF[USART2_RX_STA & 0xfff] = Res;
			USART2_RX_STA |= 0x8000;
		}
	}
}

//����4�жϷ������
void UART4_IRQHandler(void)
{
	u8 Res;
	if(UART4->SR & (1<<6))//��������ж�,�ñ�־λ
	{
		UART4->SR &= ~(1<<6);//�巢������ж�
		SEND_OVER_FLAG=1;
	}
	if(UART4->SR & (1<<3))//�������
	{
		UART4->SR &= ~(1<<3);
	}
	if(UART4->SR & (1<<5))
	{
		Res = UART4->DR;   //��ȡ���ݼĴ���,��SR�жϱ�־
		UART4->SR &= ~(1<<5);//���������ж�

		if((USART4_RX_STA & 0xfff) == 13)  //����֡β
		{
			++USART4_RX_STA;
			USART4_RX_BUF[USART4_RX_STA & 0xfff] = Res; //����У��λ
			//�յ�һ֡������CRCУ�飬��ִ�У�Ȼ��ظ�
			if(sub_to_master_crc())//�յ��ӿذ��һ֡���ݣ�CRCУ��
			{
				run_sub_cmd();
			}

			USART4_RX_STA = 0;//��仰���ܶ�
		}
		else if(USART4_RX_STA & 0x4000) //����һ֡���ݵĵ�3��----��9������
		{
			++USART4_RX_STA;
			USART4_RX_BUF[USART4_RX_STA & 0xfff] = Res;
		}
		else if(USART4_RX_STA & 0x8000) //����һ֡���ݵĵڶ������� ---0xfe
		{
			if(Res != 0xfe)
			USART4_RX_STA = 0;
			else
			{
				USART4_RX_BUF[(++USART4_RX_STA)&0xfff] = Res;
				USART4_RX_STA |= 0x4000;
			}
		}
		else if(Res == 0xef)  //����һ֡���ݵĵ�һ������ ---0xef
		{
			USART4_RX_BUF[USART4_RX_STA & 0xfff] = Res;
			USART4_RX_STA |= 0x8000;
//			COM_485_CNT=0;                  //485ͨ��3s�������
		}
	}
}
