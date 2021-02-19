#ifndef ADC1_H_
#define ADC1_H_
	void Adc_Init(void); 				//ADC通道初始化
	unsigned short int  Get_Adc(unsigned char ch); 				//获得某个通道值 
	unsigned short int  Get_Adc_Average(unsigned char ch,unsigned char times);//得到某个通道给定次数采样的平均值  

#endif
