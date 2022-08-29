//====================================================================
// File Name : Adc.c
// Function  : S3C2410 ADC Test 
// Program   : Kang, Weon Tark 
// Date      : May 22, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (March 29,2002) -> KWT
//         ADC Test                          -> May 15, 2002 SOP
//====================================================================

#include "def.h"
#include "2440addr.h"
#include "2440lib.h"
#include "adc.h"
int jinrucishu = 0;  //���ڼ�¼����Ĵ���������Ϊ���� 
//==================================================================================		
int ReadAdc(int ch)
{
    int i;
    static int prevCh=-1;

    //(1<<14)--> A/D converter prescaler enable.
    //(preScaler<<6)--> A/D converter prescaler value.
    //(ch<<3)--> Analog input channel select.
    rADCCON = (1<<14)|(49<<6)|(ch<<3);	//setup channel

    if(prevCh!=ch)
    {
	    for(i=0;i<1000;i++);	//delay to set up the next channel
	    prevCh=ch;
    }
    rADCCON|=0x1;   //start ADC

    while(!(rADCCON & 0x8000));	//check if EC(End of Conversion) flag is high
    //Normal ADC conversion data value: 0 ~ 3FF
    return ( (int)rADCDAT0 & 0x3ff );
}


//==================================================================================
//����Ƿ����˽���
int Test_Adc1() 
{
    int adcdata=0; //Initialize variables
    int adc_ch=0;
   	Uart_Printf( "\n����Ƿ�����Ա����!\n" ) ;
    adc_ch= 49;
    while( adc_ch != ESC_KEY )
    {
        adcdata=ReadAdc(adc_ch&0x3);
	    Uart_Printf( "AIN%d: %2d\n", adc_ch&0x3,adcdata );
		Delay( 20000000 ) ;
		if(adcdata >= 500){
			jinrucishu += 1;
			if(jinrucishu == 2){
				jinrucishu = 0;   //�������� 
				return 1;	      //����1ʱ��ʾ���˽���	
			} 
		} 
		else{    //���ڵ�ǰһ�μ�⵽�ˣ�����500������ʱ��ⲻ���ˣ���������
			jinrucishu = 0;
		}
		
    }
 }


//�������ܹ���
int Test_Adc2() 
{
    int adcdata=0; //Initialize variables
    int adc_ch=0;
   	Uart_Printf( "\n��ǰ���ʣ�" ) ;
    adc_ch= 50;
    adcdata=ReadAdc(adc_ch&0x3);
	Uart_Printf( "AIN%d: %2d\n", adc_ch&0x3,adcdata );
	return adcdata;  //���ص�ǰ����
		
 }