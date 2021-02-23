#if EL9800_HW
#include "el9800hw.h"
#endif

#include "applInterface.h"

#include "ADC1.h"
#include "dma.h"
#include "usart.h"
#include "oled.h"
#include "beep.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"

u16 adc1_1=0,		adc1_2=0,		adc2_1=0,		adc2_2=0;
s16 Adc_CH1=0,	Adc_CH2=0;
u32 adc_value[2];


//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define EtherCAT_TASK_PRIO		2
//�����ջ��С	
#define EtherCAT_STK_SIZE 		128  
//������
TaskHandle_t EtherCAT_Task_Handler;
//������
void EtherCAT_task(void *pvParameters);


#define OLED_TASK_PRIO		3
#define OLED_STK_SIZE 		50  
TaskHandle_t OLED_Task_Handler;
void OLED_task(void *pvParameters);

#define USART_TASK_PRIO		4
#define USART_STK_SIZE 		50
TaskHandle_t USART_Task_Handler;
void USART_task(void *pvParameters);


int main(void)
{ 
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
		MYDMA_Config(DMA2_Stream0,DMA_Channel_0,(u32)&ADC->CDR,(u32)adc_value,2);	//DMA��ʼ��
		/* initialize the Hardware and the EtherCAT Slave Controller */
		HW_Init();
		MainInit();
		delay_init(168);     	//��ʼ����ʱ����
		uart_init(115200);		//���ڳ�ʼ��������Ϊ115200
		OLED_Init();					//��ʼ��OLED
		BEEP_Init();         	//��ʼ���������˿�
		OLED_ShowString(0,0,"EtherCAT SERVER ",16);  
		OLED_ShowString(0,16,"LAN9252+F407",12); 	
		OLED_ShowString(0,28,"TIME 2021/02/23",12);  
		OLED_ShowString(0,40,"ADC1:",12);  
		OLED_ShowString(0,52,"ADC2:",12);
		OLED_Refresh_Gram();//������ʾ��OLED
	
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}
 


//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����EtherCAT����
    xTaskCreate((TaskFunction_t )EtherCAT_task,     	
                (const char*    )"EtherCAT_task",   	
                (uint16_t       )EtherCAT_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )EtherCAT_TASK_PRIO,	
                (TaskHandle_t*  )&EtherCAT_Task_Handler);   
    //����OLED����
    xTaskCreate((TaskFunction_t )OLED_task,     
                (const char*    )"OLED_task",   
                (uint16_t       )OLED_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )OLED_TASK_PRIO,
                (TaskHandle_t*  )&OLED_Task_Handler);        
    //����USART����
    xTaskCreate((TaskFunction_t )USART_task,     
                (const char*    )"USART_task",   
                (uint16_t       )USART_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )USART_TASK_PRIO,
                (TaskHandle_t*  )&USART_Task_Handler);  
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//EtherCAT������ 
void EtherCAT_task(void *pvParameters)
{
    while(1)
    {
			// ȡ��ADC���ݼĴ����ĵ�16λ�������ADC1��ת������
			adc1_1 = (adc_value[0]&0XFFFF);
			adc1_2 = (adc_value[1]&0XFFFF);
						
			// ȡ��ADC���ݼĴ����ĸ�16λ�������ADC2��ת������
			adc2_1 = (adc_value[0]&0XFFFF0000) >> 16;
			adc2_2 = (adc_value[1]&0XFFFF0000) >> 16;
		
			Adc_CH1 = adc2_1-adc1_1;
			Adc_CH2 = adc2_2-adc1_2;
			
      MainLoop();
    }
}   

//OLED������
void OLED_task(void *pvParameters)
{
    while(1)
    {      	
			OLED_ShowNum(30,40,adc1_1,4,12);
			OLED_ShowNum(60,40,adc1_2,4,12);
			OLED_ShowNum(100,40,Adc_CH1,4,12);
		
			OLED_ShowNum(30,52,adc2_1,4,12);
			OLED_ShowNum(60,52,adc2_2,4,12);
			OLED_ShowNum(100,52,Adc_CH2,4,12);
			OLED_Refresh_Gram();        //������ʾ��OLED
			
      vTaskDelay(500);
    }
}

//USART������
void USART_task(void *pvParameters)
{
	while(1)
	{
		printf("Adc_CH1:%d\r\n",Adc_CH1);
		printf("Adc_CH2:%d\r\n",Adc_CH2);
		printf("\r\n");
		vTaskDelay(500);
	}
}


//int main(void)
//{
//	bRunApplication = TRUE;
//	do
//	{
//	} while (bRunApplication == TRUE);
//	HW_Release();
//}

