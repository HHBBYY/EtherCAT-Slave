/**
\addtogroup EL9800Appl EL9800 application
@{
*/

/**
\file el9800appl.c
\author EthercatSSC@beckhoff.com
\brief Implementation

\version 5.11

<br>Changes to version V5.10:<br>
V5.11 ECAT11: create application interface function pointer, add eeprom emulation interface functions<br>
V5.11 EL9800 1: reset outputs on fallback from OP state<br>
<br>Changes to version V5.01:<br>
V5.10 ECAT6: Add "USE_DEFAULT_MAIN" to enable or disable the main function<br>
<br>Changes to version V5.0:<br>
V5.01 EL9800 2: Add TxPdo Parameter object 0x1802<br>
<br>Changes to version V4.30:<br>
V4.50 ECAT2: Create generic application interface functions. Documentation in Application Note ET9300.<br>
V4.50 COE2: Handle invalid PDO assign values.<br>
V4.30 : create file
*/


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"

#if EL9800_APPLICATION

/* ECATCHANGE_START(V5.11) ECAT11*/
#include "applInterface.h"
/* ECATCHANGE_END(V5.11) ECAT11*/

//#include "el9800hw.h"

#define _EVALBOARD_
#include "el9800appl.h"
#undef _EVALBOARD_

#if MCI_HW
#include "mcihw.h"
#endif
#if EL9800_HW
#include "el9800hw.h"
#endif

#include "oled.h"
#include "beep.h"
#include "delay.h"
#include "ADC1.h"
#include "dma.h"
#include "usart.h"

u16 adc1_1=0,		adc1_2=0,		adc2_1=0,		adc2_2=0;
s16 Adc_CH1=0,	Adc_CH2=0;
/*--------------------------------------------------------------------------------------
------
------    local types and defines
------
--------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    application specific functions
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    generic functions
------
-----------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    The function is called when an error state was acknowledged by the master

*////////////////////////////////////////////////////////////////////////////////////////

void    APPL_AckErrorInd(UINT16 stateTrans)
{ 
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from INIT to PREOP when
           all general settings were checked to start the mailbox handler. This function
           informs the application about the state transition, the application can refuse
           the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartMailboxHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from PREEOP to INIT
             to stop the mailbox handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopMailboxHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    pIntMask    pointer to the AL Event Mask which will be written to the AL event Mask
                        register (0x204) when this function is succeeded. The event mask can be adapted
                        in this function
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from PREOP to SAFEOP when
             all general settings were checked to start the input handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
            The return code NOERROR_INWORK can be used, if the application cannot confirm
            the state transition immediately, in that case the application need to be complete 
            the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartInputHandler(UINT16 *pIntMask)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from SAFEOP to PREEOP
             to stop the input handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopInputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from SAFEOP to OP when
             all general settings were checked to start the output handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartOutputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from OP to SAFEOP
             to stop the output handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopOutputHandler(void)
{
/*ECATCHANGE_START(V5.11) EL9800 1*/
    sDOOutputs.bLED1 = 0;
    sDOOutputs.bLED2 = 0;
    sDOOutputs.bLED3 = 0;
    sDOOutputs.bLED4 = 0;
#if _STM32_IO8
    sDOOutputs.bLED5 = 0;
    sDOOutputs.bLED7 = 0;
    sDOOutputs.bLED6 = 0;
    sDOOutputs.bLED8 = 0;
#endif
    
    LED_1                        = sDOOutputs.bLED1;
    LED_2                        = sDOOutputs.bLED2;
    LED_3                        = sDOOutputs.bLED3;
    LED_4                        = sDOOutputs.bLED4;
#if _STM32_IO8
    LED_5                        = sDOOutputs.bLED5;
    LED_7                        = sDOOutputs.bLED7;
    LED_6                        = sDOOutputs.bLED6;
    LED_8                        = sDOOutputs.bLED8;
#endif
/*ECATCHANGE_END(V5.11) EL9800 1*/
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0(ALSTATUSCODE_NOERROR), NOERROR_INWORK
\param      pInputSize  pointer to save the input process data length
\param      pOutputSize  pointer to save the output process data length

\brief    This function calculates the process data sizes from the actual SM-PDO-Assign
            and PDO mapping
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GenerateMapping(UINT16* pInputSize,UINT16* pOutputSize)
{
#if COE_SUPPORTED
    UINT16 result = ALSTATUSCODE_NOERROR;
    UINT16 PDOAssignEntryCnt = 0;
    OBJCONST TOBJECT OBJMEM * pPDO = NULL;
    UINT16 PDOSubindex0 = 0;
    UINT32 *pPDOEntry = NULL;
    UINT16 PDOEntryCnt = 0;
    UINT16 InputSize = 0;
    UINT16 OutputSize = 0;

    /*Scan object 0x1C12 RXPDO assign*/
    for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sRxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
    {
        pPDO = OBJ_GetObjectHandle(sRxPDOassign.aEntries[PDOAssignEntryCnt]);
        if(pPDO != NULL)
        {
            PDOSubindex0 = *((UINT16 *)pPDO->pVarPtr);
            for(PDOEntryCnt = 0; PDOEntryCnt < PDOSubindex0; PDOEntryCnt++)
            {
			//				result = *(UINT8 *)pPDO->pVarPtr;
			//				result=(OBJ_GetEntryOffset((PDOEntryCnt+1),pPDO)>>3);
                pPDOEntry = (UINT32 *)((UINT8 *)pPDO->pVarPtr + (OBJ_GetEntryOffset((PDOEntryCnt+1),pPDO)>>3));    //goto PDO entry
                // we increment the expected output size depending on the mapped Entry
                OutputSize += (UINT16) ((*pPDOEntry) & 0xFF);
            }
        }
        else
        {
            /*assigned PDO was not found in object dictionary. return invalid mapping*/
            OutputSize = 0;
            result = ALSTATUSCODE_INVALIDOUTPUTMAPPING;
            break;
        }
    }

    OutputSize = (OutputSize + 7) >> 3;

    if(result == 0)
    {
        /*Scan Object 0x1C13 TXPDO assign*/
        for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sTxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
        {
            pPDO = OBJ_GetObjectHandle(sTxPDOassign.aEntries[PDOAssignEntryCnt]);
            if(pPDO != NULL)
            {
                PDOSubindex0 = *((UINT16 *)pPDO->pVarPtr);
                for(PDOEntryCnt = 0; PDOEntryCnt < PDOSubindex0; PDOEntryCnt++)
                {
                    pPDOEntry = (UINT32 *)((UINT8 *)pPDO->pVarPtr + (OBJ_GetEntryOffset((PDOEntryCnt+1),pPDO)>>3));    //goto PDO entry
                    // we increment the expected output size depending on the mapped Entry
                    InputSize += (UINT16) ((*pPDOEntry) & 0xFF);
                }
            }
            else
            {
                /*assigned PDO was not found in object dictionary. return invalid mapping*/
                InputSize = 0;
                result = ALSTATUSCODE_INVALIDINPUTMAPPING;
                break;
            }
        }
    }
    InputSize = (InputSize + 7) >> 3;

    *pInputSize = InputSize;
    *pOutputSize = OutputSize;
    return result;
#else
    *pInputSize = 6;
    *pOutputSize = 2;
    return ALSTATUSCODE_NOERROR;
#endif

}


/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to input process data
\brief      This function will copies the inputs from the local memory to the ESC memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
#if _STM32_IO4 && AL_EVENT_ENABLED
/* the pragma interrupt_level is used to tell the compiler that these functions will not
   be called at the same time from the main function and the interrupt routine */
#pragma interrupt_level 1
#endif
void APPL_InputMapping(UINT16* pData)
{
    UINT16 j = 0;
    UINT16 *pTmpData = (UINT16 *)pData;

    /* we go through all entries of the TxPDO Assign object to get the assigned TxPDOs */
   for (j = 0; j < sTxPDOassign.u16SubIndex0; j++)
   {
      switch (sTxPDOassign.aEntries[j])
      {
      /* TxPDO 1 */
      case 0x1A00:
         *pTmpData++ = SWAPWORD(((UINT16 *) &sDIInputs)[1]);
         break;
      /* TxPDO 3 */
      case 0x1A02:
         //*pTmpData++ = SWAPWORD(((UINT16 *) &sAIInputs)[1]);
         *pTmpData++ = SWAPWORD(((UINT16 *) &sAIInputs)[2]);
				 *pTmpData++ = SWAPWORD(((UINT16 *) &sAIInputs)[3]);
         break;
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to output process data

\brief    This function will copies the outputs from the ESC memory to the local memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
#if _STM32_IO4  && AL_EVENT_ENABLED
/* the pragma interrupt_level is used to tell the compiler that these functions will not
   be called at the same time from the main function and the interrupt routine */
#pragma interrupt_level 1
#endif
void APPL_OutputMapping(UINT16* pData)
{
    UINT16 j = 0;
    UINT16 *pTmpData = (UINT16 *)pData;

    /* we go through all entries of the RxPDO Assign object to get the assigned RxPDOs */
    for (j = 0; j < sRxPDOassign.u16SubIndex0; j++)
    {
        switch (sRxPDOassign.aEntries[j])
        {
        /* RxPDO 2 */
        case 0x1601:
            ((UINT16 *) &sDOOutputs)[1] = SWAPWORD(*pTmpData++);
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief    This function will called from the synchronisation ISR 
            or from the mainloop if no synchronisation is supported
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_Application(void)
{
#if _STM32_IO4
    UINT16 analogValue;
#endif
    LED_1                        = sDOOutputs.bLED1;
    LED_2                        = sDOOutputs.bLED2;
    LED_3                        = sDOOutputs.bLED3;
    LED_4                        = sDOOutputs.bLED4;
#if _STM32_IO8
    LED_5                        = sDOOutputs.bLED5;
    LED_7                        = sDOOutputs.bLED7;
    LED_6                        = sDOOutputs.bLED6;
    LED_8                        = sDOOutputs.bLED8;
#endif


    sDIInputs.bSwitch1    = SWITCH_1;
    sDIInputs.bSwitch2    = SWITCH_2;
    sDIInputs.bSwitch3    = SWITCH_3;
    sDIInputs.bSwitch4    = SWITCH_4;
#if _STM32_IO8
    sDIInputs.bSwitch5    = SWITCH_5;
    sDIInputs.bSwitch6    = SWITCH_6;
    sDIInputs.bSwitch7    = SWITCH_7;
    sDIInputs.bSwitch8    = SWITCH_8;
#endif

				/* start the conversion of the A/D converter */
//		while(!(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==SET));
					
		sAIInputs.i16AnaloginputCH1  = Adc_CH1;
		sAIInputs.i16AnaloginputCH2 = Adc_CH2;
		
    /* we toggle the TxPDO Toggle after updating the data of the corresponding TxPDO */
    sAIInputs.bTxPDOToggle ^= 1;

    /* we simulate a problem of the analog input, if the Switch4 is on in this example,
       in this case the TxPDO State has to set to indicate the problem to the master */
    if ( sDIInputs.bSwitch4 )
        sAIInputs.bTxPDOState = 1;
    else
        sAIInputs.bTxPDOState = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     index               index of the requested object.
 \param     subindex            subindex of the requested object.
 \param     objSize             size of the requested object data, calculated with OBJ_GetObjectLength
 \param     pData               Pointer to the buffer where the data can be copied to
 \param     bCompleteAccess     Indicates if a complete read of all subindices of the
                                object shall be done or not

 \return    ABORTIDX_XXX

 \brief     Handles SDO read requests to TxPDO Parameter
*////////////////////////////////////////////////////////////////////////////////////////
UINT8 ReadObject0x1802( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess )
{

    if(bCompleteAccess)
        return ABORTIDX_UNSUPPORTED_ACCESS;

    if(subindex == 0)
    {
        *pData = TxPDO1802Subindex0;
    }
    else if(subindex == 6)
    {
        /*clear destination buffer (no excluded TxPDO set)*/
        if(dataSize > 0)
            MBXMEMSET(pData,0x00,dataSize);
    }
    else if(subindex == 7)
    {
        /*min size is one Byte*/
        UINT8 *pu8Data = (UINT8*)pData;
        
        //Reset Buffer
        *pu8Data = 0; 

        *pu8Data = sAIInputs.bTxPDOState;
    }
    else if(subindex == 9)
    {
        /*min size is one Byte*/
        UINT8 *pu8Data = (UINT8*)pData;
        
        //Reset Buffer
        *pu8Data = 0; 

        *pu8Data = sAIInputs.bTxPDOToggle;
    }
    else
        return ABORTIDX_SUBINDEX_NOT_EXISTING;

    return 0;
}

#if EXPLICIT_DEVICE_ID
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    The Explicit Device ID of the EtherCAT slave

 \brief     Calculate the Explicit Device ID
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GetDeviceID()
{
    UINT16 Value = (UINT16)(SWITCH_8<<7)|(SWITCH_7<<6)|(SWITCH_6<<5)|(SWITCH_5<<4)|(SWITCH_4<<3)|(SWITCH_3<<2)|(SWITCH_2<<1)|(SWITCH_1);
    return Value;
}
#endif

#if USE_DEFAULT_MAIN
/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This is the main function

*////////////////////////////////////////////////////////////////////////////////////////
uint32_t times = 0x1000000;
u32 adc_value[2];
#if _STM32_IO8
int main(void)
#else
void main(void)
#endif
{	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
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
		OLED_ShowString(0,28,"TIME 2021/02/13",12);  
		OLED_ShowString(0,40,"ADC1:",12);  
		OLED_ShowString(0,52,"ADC2:",12);
		OLED_Refresh_Gram();//������ʾ��OLED	 
    bRunApplication = TRUE;
    do
    {
        MainLoop();
				//adcx = Get_Adc_Average(ADC_Channel_7, 20);//��ȡͨ��5��ת��ֵ��20��ȡƽ��
				//OLED_ShowNum(30,40,adcx,4,12);
			
				// ȡ��ADC���ݼĴ����ĵ�16λ�������ADC1��ת������
				adc1_1 = (adc_value[0]&0XFFFF);
				adc1_2 = (adc_value[1]&0XFFFF);
							
				// ȡ��ADC���ݼĴ����ĸ�16λ�������ADC2��ת������
				adc2_1 = (adc_value[0]&0XFFFF0000) >> 16;
				adc2_2 = (adc_value[1]&0XFFFF0000) >> 16;
			
			  Adc_CH1 = adc2_1-adc1_1;
				Adc_CH2 = adc2_2-adc1_2;
			
				OLED_ShowNum(30,40,adc1_1,4,12);
				OLED_ShowNum(60,40,adc1_2,4,12);
				OLED_ShowNum(100,40,Adc_CH1,4,12);
			
				OLED_ShowNum(30,52,adc2_1,4,12);
				OLED_ShowNum(60,52,adc2_2,4,12);
				OLED_ShowNum(100,52,Adc_CH2,4,12);
				OLED_Refresh_Gram();        //������ʾ��OLED
				
				printf("Adc_CH1:%d\r\n",Adc_CH1);
				printf("Adc_CH2:%d\r\n",Adc_CH2);
				printf("\r\n");
				
				delay_ms(100);
				

    } while (bRunApplication == TRUE);

    HW_Release();
#if _STM32_IO8
    return 0;
#endif
}
#endif //#if USE_DEFAULT_MAIN
#endif //#if EL9800_APPLICATION

/** @} */
