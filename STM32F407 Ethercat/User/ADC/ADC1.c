//////////////////////////////////////////////////////////////////////////////////	 
#include "stm32f4xx.h" 
#include "ADC1.h"	
#include "delay.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ADC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


//��ʼ��ADC															   
void Adc_Init(void)
{    
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOA��Bʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_ADC2, ENABLE); //ʹ��ADC1��2ʱ��

  //��ʼ��ADC1ͨ��1��4 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_4;//PA1��4 ͨ��1��4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA  
	
	//��ʼ��ADC2ͨ��8��9 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;//PB0��1 ͨ��8��9
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB 
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,ENABLE);	  //ADC2��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,DISABLE);	//��λ����	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_RegSimult;//����ģʽ������ͬ��ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��,���ADCͬʱ�������ͬһ��ͨ���Ż��õ���
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_2; //DMAģʽ2,����Ƕ��ADCͬʱʹ�õ�ʱ��Ż��õ���
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//��������ת��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 2;//2��ת���ڹ��������� Ҳ����ֻת����������2
  ADC_Init(ADC1, &ADC_InitStructure);//ADC1��ʼ��
	ADC_Init(ADC2, &ADC_InitStructure);//ADC1��ʼ��
	
	// ���� ADC ͨ��ת��˳��Ͳ���ʱ������
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,2,ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC2,ADC_Channel_8,1,ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC2,ADC_Channel_9,2,ADC_SampleTime_480Cycles);
	
	//ADC_DMARequestAfterLastTransferCmd(ADC3,ENABLE); // ʹ��DMA����(Single-ADC mode)
	ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
	
	//ADC_DMACmd(ADC3, ENABLE);// ʹ��ADC DMA
	ADC_Cmd(ADC1, ENABLE);//����ADת����1	
	ADC_Cmd(ADC2, ENABLE);//����ADת����2	
	ADC_SoftwareStartConv(ADC1);//��ʼADC1ת�����������
	ADC_SoftwareStartConv(ADC2);//��ʼADC1ת�����������

}				  
//���ADCֵ
//ch: @ref ADC_channels 
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_16
//����ֵ:ת�����
u16 Get_Adc(u8 ch)   
{
	  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC3, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    
  
	ADC_SoftwareStartConv(ADC3);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC3);	//�������һ��ADC1�������ת�����
}
//��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//ch:ͨ�����
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(3);
	}
	return temp_val/times;
} 

