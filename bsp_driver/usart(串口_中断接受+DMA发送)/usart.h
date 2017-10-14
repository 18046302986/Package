#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"
/*
********************************************************************************
  * @author  xdh
  * @version V1.0
  * @date    2017-xx-xx
  * @apply   3000WControl
********************************************************************************
*/
/*
    PCͨ��  232  MCU
        USART2_RX  ---> PD6
        USART2_TX  ---> PD5
    ���ذ�   485   �ӿ��ư�ͨ��
        UART4_RX   --> PC11
        UART4_TX   --> PC10
        EN         --> PA15
*/
#define USART_REC_LEN         15 //�������Ľ����ֽ���200

extern UART_HandleTypeDef UART2_Handler;  //UART���
extern DMA_HandleTypeDef UART2TxDMA_Handler;//DMA���

extern UART_HandleTypeDef UART4_Handler;  //UART���
extern DMA_HandleTypeDef UART4TxDMA_Handler;//DMA���

#define RXBUFFERSIZE 1      //�����С

extern uint8_t aRx2Buffer[RXBUFFERSIZE];//HAL��USART2����Buff
extern uint8_t aRx4Buffer[RXBUFFERSIZE];//HAL��USART4����Buff

extern uint8_t  USART2_TX_BUF[USART_REC_LEN];//���巢�ͻ�������
extern uint8_t  USART4_TX_BUF[USART_REC_LEN];//���巢�ͻ�������

extern uint8_t  USART2_RX_BUF[USART_REC_LEN];//������ܻ�������
extern uint8_t  USART4_RX_BUF[USART_REC_LEN];//������ܻ�������


extern uint16_t USART2_RX_STA;            //����״̬��־
extern uint16_t USART4_RX_STA;            //����״̬��־

extern uint16_t USART2_TX_STA;		//����״̬��־

extern u8 SEND_OVER_FLAG;

#define GPIO_PCTXD_D					GPIO_PIN_6		//�����������
#define GPIO_PCRXD_D					GPIO_PIN_5		//�����������


#define GPIO_485TX_C					GPIO_PIN_11		//�����������
#define GPIO_485RX_C					GPIO_PIN_10		//�����������
#define GPIO_485EN_A         	        GPIO_PIN_15      //ͨ���������

#define RS485EN_RX()       PAout(15)=0
#define RS485EN_TX()       PAout(15)=1


void usart_init(uint32_t bound);
void MYDMA_USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);


#endif
