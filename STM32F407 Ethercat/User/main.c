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


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define EtherCAT_TASK_PRIO		2
//任务堆栈大小	
#define EtherCAT_STK_SIZE 		128  
//任务句柄
TaskHandle_t EtherCAT_Task_Handler;
//任务函数
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
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
		MYDMA_Config(DMA2_Stream0,DMA_Channel_0,(u32)&ADC->CDR,(u32)adc_value,2);	//DMA初始化
		/* initialize the Hardware and the EtherCAT Slave Controller */
		HW_Init();
		MainInit();
		delay_init(168);     	//初始化延时函数
		uart_init(115200);		//串口初始化波特率为115200
		OLED_Init();					//初始化OLED
		BEEP_Init();         	//初始化蜂鸣器端口
		OLED_ShowString(0,0,"EtherCAT SERVER ",16);  
		OLED_ShowString(0,16,"LAN9252+F407",12); 	
		OLED_ShowString(0,28,"TIME 2021/02/23",12);  
		OLED_ShowString(0,40,"ADC1:",12);  
		OLED_ShowString(0,52,"ADC2:",12);
		OLED_Refresh_Gram();//更新显示到OLED
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}
 


//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建EtherCAT任务
    xTaskCreate((TaskFunction_t )EtherCAT_task,     	
                (const char*    )"EtherCAT_task",   	
                (uint16_t       )EtherCAT_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )EtherCAT_TASK_PRIO,	
                (TaskHandle_t*  )&EtherCAT_Task_Handler);   
    //创建OLED任务
    xTaskCreate((TaskFunction_t )OLED_task,     
                (const char*    )"OLED_task",   
                (uint16_t       )OLED_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )OLED_TASK_PRIO,
                (TaskHandle_t*  )&OLED_Task_Handler);        
    //创建USART任务
    xTaskCreate((TaskFunction_t )USART_task,     
                (const char*    )"USART_task",   
                (uint16_t       )USART_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )USART_TASK_PRIO,
                (TaskHandle_t*  )&USART_Task_Handler);  
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//EtherCAT任务函数 
void EtherCAT_task(void *pvParameters)
{
    while(1)
    {
			// 取出ADC数据寄存器的低16位，这个是ADC1的转换数据
			adc1_1 = (adc_value[0]&0XFFFF);
			adc1_2 = (adc_value[1]&0XFFFF);
						
			// 取出ADC数据寄存器的高16位，这个是ADC2的转换数据
			adc2_1 = (adc_value[0]&0XFFFF0000) >> 16;
			adc2_2 = (adc_value[1]&0XFFFF0000) >> 16;
		
			Adc_CH1 = adc2_1-adc1_1;
			Adc_CH2 = adc2_2-adc1_2;
			
      MainLoop();
    }
}   

//OLED任务函数
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
			OLED_Refresh_Gram();        //更新显示到OLED
			
      vTaskDelay(500);
    }
}

//USART任务函数
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

