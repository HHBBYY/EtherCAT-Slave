#ifndef ADC1_H_
#define ADC1_H_
	void Adc_Init(void); 				//ADCͨ����ʼ��
	unsigned short int  Get_Adc(unsigned char ch); 				//���ĳ��ͨ��ֵ 
	unsigned short int  Get_Adc_Average(unsigned char ch,unsigned char times);//�õ�ĳ��ͨ����������������ƽ��ֵ  

#endif
