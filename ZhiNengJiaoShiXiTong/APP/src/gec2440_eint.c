#include "2440addr.h"
#include "2440lib.h"
#include "def.h"
//����������
void Buzzer_Freq_Set( U32 freq );
void Buzzer_Stop( void );

//---------------------------------------------------------------------------------
//�ⲿ����
extern int Test_Adc();   //���մ�����
extern void Test_Lcd_HZ(unsigned char* test);   //LED������ʾ��
//--------------------------------------------------------------------------------- 


//=================================================
//led
/*  ����˿ڼĴ���  */
#define rGPBCON    (*(volatile unsigned *)0x56000010) //Port B control
#define rGPBDAT    (*(volatile unsigned *)0x56000014) //Port B data
#define rGPBUP     (*(volatile unsigned *)0x56000018) //Pull-up control B
//�������
#define   GPIOSET(PIN)  rGPBDAT = rGPBDAT | PIN         /* ����PIN���1��PINΪMOTOA--MOTOD */
#define   GPIOCLR(PIN)  rGPBDAT = rGPBDAT & (~(PIN))      /* ����PIN���0��PINΪMOTOA--MOTOD */

//=================================================



//=================================================================================== 
//���� 
int shifouyijinru = 1;  //ֵΪ1ʱ����ʾ��ǰ����û�ˣ������Ƿ�ʹ�ù��մ�����
int qidong = 1;  //���ڰ����ж��п�����ر�����ϵͳ 
int shifouyourenjinru;  //���ڽ��չ��մ��������ص��Ƿ����˽���ֵ 
int x = 0,j = 1,k;

U16 i,a;
//��ʱ��
int dingshiqi = 0;  //��ʱ����¼����
int shangxiake = 0;  //���¿μ�¼,0�¿�


//lcd��ʾ��
unsigned char test[20000] = "��ӭ����ʵѵ¥���������ң�\n";   //��ʾ������ʱ���������

//�������
unsigned int moto_control[]={0x020,0x040,0x080,0x100};
unsigned int moto_control_r[]={0x100,0x080,0x040,0x020};
int bujin = 0;

//���裨��⹦�ʣ�
int gonglvjiance = 1;
int shangyicigonglvguoda  = 0; //��¼��һ�ιر�ϵͳ�Ƿ����ڹ��ʹ���
//=================================================================================== 

//��������ӳٴ���
void  DelayNS(U32 dly)
{
    U32  i;

    for(; dly>0; dly--)
       for(i=0; i<50000; i++);
}



int delay(int times)
{
    int o;
    for(o=0;o<times;o++);
    return 0;
}

void INIT_interrupt();

//*********************************************************************************************
//���������ת����
void MOTO_Mode1(void)
{
	U32 i,k,j=10;

	
			for( i = 0;i<4;i++)
			{
				rGPBDAT &=  ~(0xf<<5);//GPBDAT8 GPBDAT7 GPBDAT6 GPBDAT5 //0000<5
				
				GPIOSET(moto_control[i]);// i=0 1 2 3  	
				
				DelayNS(50);
			}
}

void MOTO_Mode2(void)
{
	U32 i,k,j=10;

	
			for( i=0;i<4;i++)
			{
				rGPBDAT &=  ~(0xf<<5);
				GPIOSET(moto_control_r[i]);  	
				DelayNS(50);
			}
}
//*********************************************************************************************






//*********************************************************************************************
//��ʱ��
void __irq Timer2_ISR(void)
{
    Uart_Printf("this is interrupt\n");
	dingshiqi++;
	if(shangxiake == 0){   //�¿��ڼ�
		if(dingshiqi==100)     //�Ͽ�,��i�ﵽһ�����������ﵽָ��ʱ��ʱ��ִ�з������͵ƹ⣬Լ0.099��һ��i
		{
			Uart_Printf( "�¿Σ�\n");
			//rGPBDAT= rGPBDAT & ~(0xf<<5);//GPB7-8 output 0  led��
			Buzzer_Freq_Set(2000) ;     //��������
			delay(60000000);    //�ȴ�
			Buzzer_Stop() ; //������ֹͣ
			//rGPBDAT= rGPBDAT|(0xf<<5);//GPB7-8 output 1  led��
			dingshiqi=0;  //����ʱ����������
			shangxiake = 1;  //��Ϊ�Ͽ�
		}
	} 
	else{   //�Ͽ��ڼ�
		if(dingshiqi==400)     //�¿�,��i�ﵽһ�����������ﵽָ��ʱ��ʱ��ִ�з������͵ƹ⣬Լ0.099��һ��i
		{
			Uart_Printf( "�ϿΣ�\n");
			//rGPBDAT= rGPBDAT & ~(0xf<<5);//GPB7-8 output 0  led��
			Buzzer_Freq_Set(2000) ;     //��������
			delay(60000000);    //�ȴ�
			Buzzer_Stop() ; //������ֹͣ
			//rGPBDAT= rGPBDAT|(0xf<<5);//GPB7-8 output 1  led��
			dingshiqi=0;  //����ʱ����������
			shangxiake = 0;  //��Ϊ�¿�
		}
	}
	
 
  ClearPending(BIT_TIMER2);
}


/****  watchdog_init  ****/

//*********************************************************************************************









//*********************************************************************************************
//������
void Buzzer_Freq_Set( U32 freq )  //��������
{   
	rGPBCON = rGPBCON & ~(3<<0)|(1<<1);//set GPB0 as tout0, pwm output
		
	rTCFG0 = rTCFG0 & ~0xff|15; //prescaler = 15
	rTCFG1 = rTCFG1 & ~0xf|2;//divider = 1/8
			
	rTCNTB0 = (PCLK>>7)/freq;//rTCNTB0=PCLK/{(prescaler+1) * divider *freq}
	rTCMPB0 = rTCNTB0>>1;	//ռ�ձ�50%
	
	//disable deadzone, auto-reload, inv-off, update TCNTB0&TCMPB0, start timer 0
	rTCON = rTCON & ~0x1f|(0<<4)|(1<<3)|(0<<2)|(1<<1)|(1);
	rTCON &= ~(1<<1);			//clear manual update bit
}

void Buzzer_Stop( void )   //�������ر�
{
	rGPBCON |= 1;
		
	rGPBCON = rGPBCON & ~3|1;			//set GPB0 as output
	rGPBDAT &= ~1;//output 0
}
//*********************************************************************************************




//************************************************************************************************************************************************************************************************************ 
//�����ж� 
static void __irq Eint1_ISR(void)
{
	x = 1;
    ClearPending(BIT_EINT1);
    Uart_Printf("EINT1  is occurred.\n");
    qidong = 1;    //����
}

static void __irq Eint2_ISR(void)
{
    ClearPending(BIT_EINT2);
    Uart_Printf("EINT2 is occurred.\n");
    qidong = 0;    //�ر�
}  

 static void __irq Eint3_ISR(void)
{
    ClearPending(BIT_EINT3);
    Uart_Printf("EINT3 is occurred.\n");
}

static void __irq Eint4_7_ISR(void)
{
    if(rEINTPEND & (1<<4))
    {  
	Uart_Printf("EINT4 is occurred.\n");
	rEINTPEND=(1<<4);
   	ClearPending(BIT_EINT4_7);
    }
    else if(rEINTPEND & (1<<5))
    {
	Uart_Printf("EINT5 is occurred.\n");
	rEINTPEND=(1<<5);
    ClearPending(BIT_EINT4_7);
    }
   	else if(rEINTPEND & (1<<7))
    {
    //�������� 
	Uart_Printf("EINT7 is occurred.\n");
	rEINTPEND=(1<<7);
    ClearPending(BIT_EINT4_7);
    }
    else
    {
	Uart_Printf("others ENTs are occurred\n");
	rEINTPEND=0xffffff;
    ClearPending(BIT_EINT4_7);
    }
}
//************************************************************************************************************************************************************************************************************ 




void INIT_interrupt()
{
	rGPFCON = (rGPFCON & ~((3<<2)|(3<<4)|(3<<6)|(3<<8)|(3<<10)|(3<<12)))|(1<<3)|(1<<5)|(1<<7)|(1<<9)|(1<<11)|(1<<13);		//GPF0/2 = EINT0/2
  
	
	rEXTINT0 = (rEXTINT0 & ~(7<<4))|(0x2<<4); //EINT1=Low level  triggered
	rEXTINT0 = (rEXTINT0 & ~(7<<8))|(0x2<<8); //EINT2=falling edge triggered
	rEXTINT0 = (rEXTINT0 & ~(7<<12))|(0x2<<12); //EINT3=falling edge triggered
	rEXTINT0 = (rEXTINT0 & ~(7<<16))|(0x2<<16); //EINT4=falling edge triggered
	rEXTINT0 = (rEXTINT0 & ~(7<<20))|(0x2<<20); //EINT5=falling edge triggered
	rEXTINT0 = (rEXTINT0 & ~(7<<24))|(0x2<<24); //EINT7=falling edge triggered


    pISR_EINT1=(U32)Eint1_ISR;
    pISR_EINT2=(U32)Eint2_ISR;
	pISR_EINT3=(U32)Eint3_ISR;
    pISR_EINT4_7=(U32)Eint4_7_ISR;
	
	
	rEINTPEND = 0xffffff;
    rSRCPND |= BIT_EINT1|BIT_EINT2|BIT_EINT3|BIT_EINT4_7; //to clear the previous pending states
    rINTPND |= BIT_EINT1|BIT_EINT2|BIT_EINT3|BIT_EINT4_7;
    
    rEINTMASK=~( (1<<4)|(1<<5)|(1<<6) );
    rINTMSK=~(BIT_EINT1|BIT_EINT2|BIT_EINT3|BIT_EINT4_7);
    
}



int f_ucSencondNo=0;
void xmain(void)
{
	//==========================================================================
	//led��ʼ��
	rGPBCON= (rGPBCON & ~(0xff<<10)) | (0x55<<10);//GPB5-8  set output
    rGPBUP = (rGPBUP & ~(0xf<<5)) | (0xf<<5);   //disable GPB pull up
 	//===========================================================================
 	
	
	ChangeClockDivider(3,1);
	ChangeMPllValue(127,2,1);		//405MHZ
	//Rtc_Init();				//ʵʱʱ�ӳ�ʼ��
	Isr_Init();
	Port_Init();   //adc 
	Uart_Init(0, 115200);
	Uart_Select(0);
	
	//======================================================================
	//led��ʼ��
	rGPBCON=(rGPBCON & ~(0xFF<<10)) | (0x55<<10);//GPB5-8  set output
    rGPBUP |= (0xFF<<5);   //disable GPB pull up
	//======================================================================
	
	//======================================================================
	//ֱ�������ʼ��
	rGPGCON &= ~(3<<12);//00
 	rGPGCON |= (1<<12);//01
 	
 	rGPGCON &= ~(3<<14);
 	rGPGCON |= (1<<14);
 	
    rGPGUP &=~(3<<6);
	
	//======================================================================

	INIT_interrupt();
	
	rGPBDAT= rGPBDAT |(0xf<<5);  //�ƹ�ر�


//*********************************************************************************************
//��ʱ��
	Uart_Printf("init\n");
      pISR_TIMER2=(U32)Timer2_ISR;//����Timer1�ж� ���ж�����
  //���ж�
  ClearPending(BIT_TIMER2);//���ж�
  EnableIrq(BIT_TIMER2);
  rTCFG0=rTCFG0 &~0xff<<8|15<<8;  //15-->  0-255
  rTCFG1=rTCFG1 &~0xf<<8|2<<8;  //2--->  0-3
  rTCNTB2=52500;  //0--->2^16
  rTCMPB2 = rTCNTB2>>1;
  rTCON=rTCON & ~0xf<<12|(1<<15)|(0<<14)|(1<<13)|(1<<12);
   // rTCON |=(1<<13);
   rTCON &=~(1<<13);

  Uart_Printf("init  end\n");
//*********************************************************************************************
	Test_Lcd_HZ(test);     //������ʾ������

	while(1){
		if(qidong == 1){  //ϵͳ���ڿ���״̬ 
			Uart_Printf( "����\n");
				if(shifouyijinru == 1){       //��û�˽���ʱ����Ƿ����˽���
					shifouyourenjinru = Test_Adc1(); //���մ�������������Ϊ1��ʾ���˽��� 
					shifouyijinru = 0;      //��������
				}
				if(shangyicigonglvguoda == 0){   //���ιر�ϵͳ�������ڹ��ʹ�����ر�
					if(bujin == 0){
						for(i=0; i<10; i++)   //���������ת���򿪴�����
					        MOTO_Mode1();
					    bujin = 1;   //ֹͣ�������
					}
					rGPBDAT= rGPBDAT & ~(0xf<<5);//�����ƹ⣨led��
				}
				shangyicigonglvguoda = 0;   //����һ�ι��ʹ�����Ϊ0
			Uart_Printf( "���˽��룡\n");
			if(shifouyourenjinru == 1){   //���˽��룬������Ӧ�豸
				Uart_Printf( "�ƹ�������\n");
				
				Uart_Printf( "����������\n");
				rGPGDAT= rGPGDAT & ~(0x3<<6)|(0x1<<6);//���ȣ�ֱ���������ת����
	
				
				gonglvjiance = Test_Adc2(); //�����⹦��
				if(gonglvjiance >= 1000){   //������ʹ���
					qidong = 0;            //�ر����е�·
					shangyicigonglvguoda = 1;   //����һ�ι��ʹ�����Ϊ1
				}
				wenshidu();
			}
			
			
		}
		else{    //ϵͳ���ڹر�״̬ s
			if(shangyicigonglvguoda == 0){   //���ιر�ϵͳ�������ڹ��ʹ�����ر�
				shifouyijinru = 1;   //������һ����ʾ��ǰ����û�ˣ������Ƿ�ʹ�ù��մ�����
				rGPBDAT= rGPBDAT |(0xf<<5);  //�رյƹ⣨led��
				if(bujin ==1){
				for(i=0; i<10; i++)    //���������ת���رմ�����
	        		MOTO_Mode2();
	        		bujin = 0; //ֹͣ�������
	    		}
			}

			
			rGPGDAT &= ~(0x3<<6);     //���ȣ�ֱ��������ر�
			
			//���������
			ZLG7290_SendCmd(0x60 +0, 0x1f);			
			ZLG7290_SendCmd(0x60 +1, 0x1f);
			ZLG7290_SendCmd(0x60 +2, 0x1f);
			ZLG7290_SendCmd(0x60 +3, 0x1f);
			ZLG7290_SendCmd(0x60 +4, 0x1f);
			ZLG7290_SendCmd(0x60 +5, 0x1f);
			ZLG7290_SendCmd(0x60 +6, 0x1f);
			ZLG7290_SendCmd(0x60 +7, 0x1f);
			//
			
	    	gonglvjiance = 1;   //��������һ
			Uart_Printf( "�ر�\n");
			//continue;
		}
	};
	

	
      
}

