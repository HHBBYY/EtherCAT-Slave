//////////////////////////////////////////////////////////////////////////////////	 
#include "stm32f4xx.h" 
#include "ADC1.h"	
#include "delay.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//ADC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


//初始化ADC															   
void Adc_Init(void)
{    
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOA、B时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_ADC2, ENABLE); //使能ADC1、2时钟

  //初始化ADC1通道1、4 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_4;//PA1、4 通道1、4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA  
	
	//初始化ADC2通道8、9 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;//PB0、1 通道8、9
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB 
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,ENABLE);	  //ADC2复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,DISABLE);	//复位结束	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_RegSimult;//工作模式：规则同步模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟,多个ADC同时交替采用同一个通道才会用到的
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_2; //DMA模式2,这个是多个ADC同时使用的时候才会用到的
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//开启连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 2;//2个转换在规则序列中 也就是只转换规则序列2
  ADC_Init(ADC1, &ADC_InitStructure);//ADC1初始化
	ADC_Init(ADC2, &ADC_InitStructure);//ADC1初始化
	
	// 配置 ADC 通道转换顺序和采样时间周期
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,2,ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC2,ADC_Channel_8,1,ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC2,ADC_Channel_9,2,ADC_SampleTime_480Cycles);
	
	//ADC_DMARequestAfterLastTransferCmd(ADC3,ENABLE); // 使能DMA请求(Single-ADC mode)
	ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
	
	//ADC_DMACmd(ADC3, ENABLE);// 使能ADC DMA
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器1	
	ADC_Cmd(ADC2, ENABLE);//开启AD转换器2	
	ADC_SoftwareStartConv(ADC1);//开始ADC1转换，软件触发
	ADC_SoftwareStartConv(ADC2);//开始ADC1转换，软件触发

}				  
//获得ADC值
//ch: @ref ADC_channels 
//通道值 0~16取值范围为：ADC_Channel_0~ADC_Channel_16
//返回值:转换结果
u16 Get_Adc(u8 ch)   
{
	  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC3, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
  
	ADC_SoftwareStartConv(ADC3);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC3);	//返回最近一次ADC1规则组的转换结果
}
//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
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

