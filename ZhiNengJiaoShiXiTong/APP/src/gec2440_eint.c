#include "2440addr.h"
#include "2440lib.h"
#include "def.h"
//蜂鸣器函数
void Buzzer_Freq_Set( U32 freq );
void Buzzer_Stop( void );

//---------------------------------------------------------------------------------
//外部函数
extern int Test_Adc();   //光照传感器
extern void Test_Lcd_HZ(unsigned char* test);   //LED文字显示屏
//--------------------------------------------------------------------------------- 


//=================================================
//led
/*  定义端口寄存器  */
#define rGPBCON    (*(volatile unsigned *)0x56000010) //Port B control
#define rGPBDAT    (*(volatile unsigned *)0x56000014) //Port B data
#define rGPBUP     (*(volatile unsigned *)0x56000018) //Pull-up control B
//步进电机
#define   GPIOSET(PIN)  rGPBDAT = rGPBDAT | PIN         /* 设置PIN输出1，PIN为MOTOA--MOTOD */
#define   GPIOCLR(PIN)  rGPBDAT = rGPBDAT & (~(PIN))      /* 设置PIN输出0，PIN为MOTOA--MOTOD */

//=================================================



//=================================================================================== 
//变量 
int shifouyijinru = 1;  //值为1时，表示当前课室没人，用于是否使用光照传感器
int qidong = 1;  //用于按键中断中开启与关闭整个系统 
int shifouyourenjinru;  //用于接收光照传感器返回的是否有人进入值 
int x = 0,j = 1,k;

U16 i,a;
//定时器
int dingshiqi = 0;  //定时器记录次数
int shangxiake = 0;  //上下课记录,0下课


//lcd显示屏
unsigned char test[20000] = "欢迎来到实训楼四零三教室！\n";   //显示屏启动时输出的文字

//步进电机
unsigned int moto_control[]={0x020,0x040,0x080,0x100};
unsigned int moto_control_r[]={0x100,0x080,0x040,0x020};
int bujin = 0;

//电阻（检测功率）
int gonglvjiance = 1;
int shangyicigonglvguoda  = 0; //记录上一次关闭系统是否由于功率过高
//=================================================================================== 

//步进电机延迟代码
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
//步进电机旋转代码
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
//定时器
void __irq Timer2_ISR(void)
{
    Uart_Printf("this is interrupt\n");
	dingshiqi++;
	if(shangxiake == 0){   //下课期间
		if(dingshiqi==100)     //上课,当i达到一定次数，即达到指定时间时，执行蜂鸣器和灯光，约0.099秒一次i
		{
			Uart_Printf( "下课！\n");
			//rGPBDAT= rGPBDAT & ~(0xf<<5);//GPB7-8 output 0  led亮
			Buzzer_Freq_Set(2000) ;     //蜂鸣器响
			delay(60000000);    //等待
			Buzzer_Stop() ; //蜂鸣器停止
			//rGPBDAT= rGPBDAT|(0xf<<5);//GPB7-8 output 1  led暗
			dingshiqi=0;  //将定时器次数置零
			shangxiake = 1;  //置为上课
		}
	} 
	else{   //上课期间
		if(dingshiqi==400)     //下课,当i达到一定次数，即达到指定时间时，执行蜂鸣器和灯光，约0.099秒一次i
		{
			Uart_Printf( "上课！\n");
			//rGPBDAT= rGPBDAT & ~(0xf<<5);//GPB7-8 output 0  led亮
			Buzzer_Freq_Set(2000) ;     //蜂鸣器响
			delay(60000000);    //等待
			Buzzer_Stop() ; //蜂鸣器停止
			//rGPBDAT= rGPBDAT|(0xf<<5);//GPB7-8 output 1  led暗
			dingshiqi=0;  //将定时器次数置零
			shangxiake = 0;  //置为下课
		}
	}
	
 
  ClearPending(BIT_TIMER2);
}


/****  watchdog_init  ****/

//*********************************************************************************************









//*********************************************************************************************
//蜂鸣器
void Buzzer_Freq_Set( U32 freq )  //蜂鸣器打开
{   
	rGPBCON = rGPBCON & ~(3<<0)|(1<<1);//set GPB0 as tout0, pwm output
		
	rTCFG0 = rTCFG0 & ~0xff|15; //prescaler = 15
	rTCFG1 = rTCFG1 & ~0xf|2;//divider = 1/8
			
	rTCNTB0 = (PCLK>>7)/freq;//rTCNTB0=PCLK/{(prescaler+1) * divider *freq}
	rTCMPB0 = rTCNTB0>>1;	//占空比50%
	
	//disable deadzone, auto-reload, inv-off, update TCNTB0&TCMPB0, start timer 0
	rTCON = rTCON & ~0x1f|(0<<4)|(1<<3)|(0<<2)|(1<<1)|(1);
	rTCON &= ~(1<<1);			//clear manual update bit
}

void Buzzer_Stop( void )   //蜂鸣器关闭
{
	rGPBCON |= 1;
		
	rGPBCON = rGPBCON & ~3|1;			//set GPB0 as output
	rGPBDAT &= ~1;//output 0
}
//*********************************************************************************************




//************************************************************************************************************************************************************************************************************ 
//按键中断 
static void __irq Eint1_ISR(void)
{
	x = 1;
    ClearPending(BIT_EINT1);
    Uart_Printf("EINT1  is occurred.\n");
    qidong = 1;    //开启
}

static void __irq Eint2_ISR(void)
{
    ClearPending(BIT_EINT2);
    Uart_Printf("EINT2 is occurred.\n");
    qidong = 0;    //关闭
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
    //烟雾报警器 
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
	//led初始化
	rGPBCON= (rGPBCON & ~(0xff<<10)) | (0x55<<10);//GPB5-8  set output
    rGPBUP = (rGPBUP & ~(0xf<<5)) | (0xf<<5);   //disable GPB pull up
 	//===========================================================================
 	
	
	ChangeClockDivider(3,1);
	ChangeMPllValue(127,2,1);		//405MHZ
	//Rtc_Init();				//实时时钟初始化
	Isr_Init();
	Port_Init();   //adc 
	Uart_Init(0, 115200);
	Uart_Select(0);
	
	//======================================================================
	//led初始化
	rGPBCON=(rGPBCON & ~(0xFF<<10)) | (0x55<<10);//GPB5-8  set output
    rGPBUP |= (0xFF<<5);   //disable GPB pull up
	//======================================================================
	
	//======================================================================
	//直流电机初始化
	rGPGCON &= ~(3<<12);//00
 	rGPGCON |= (1<<12);//01
 	
 	rGPGCON &= ~(3<<14);
 	rGPGCON |= (1<<14);
 	
    rGPGUP &=~(3<<6);
	
	//======================================================================

	INIT_interrupt();
	
	rGPBDAT= rGPBDAT |(0xf<<5);  //灯光关闭


//*********************************************************************************************
//定时器
	Uart_Printf("init\n");
      pISR_TIMER2=(U32)Timer2_ISR;//建立Timer1中断 设中断向量
  //开中断
  ClearPending(BIT_TIMER2);//清中断
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
	Test_Lcd_HZ(test);     //考勤显示屏启动

	while(1){
		if(qidong == 1){  //系统处于开启状态 
			Uart_Printf( "启动\n");
				if(shifouyijinru == 1){       //当没人进入时检测是否有人进入
					shifouyourenjinru = Test_Adc1(); //光照传感器，若返回为1表示有人进入 
					shifouyijinru = 0;      //将其置零
				}
				if(shangyicigonglvguoda == 0){   //本次关闭系统并非由于功率过大而关闭
					if(bujin == 0){
						for(i=0; i<10; i++)   //步进电机正转（打开窗帘）
					        MOTO_Mode1();
					    bujin = 1;   //停止步进电机
					}
					rGPBDAT= rGPBDAT & ~(0xf<<5);//开启灯光（led）
				}
				shangyicigonglvguoda = 0;   //将上一次功率过大置为0
			Uart_Printf( "有人进入！\n");
			if(shifouyourenjinru == 1){   //有人进入，启动相应设备
				Uart_Printf( "灯光启动！\n");
				
				Uart_Printf( "风扇启动！\n");
				rGPGDAT= rGPGDAT & ~(0x3<<6)|(0x1<<6);//风扇（直流电机）正转启动
	
				
				gonglvjiance = Test_Adc2(); //电阻检测功率
				if(gonglvjiance >= 1000){   //如果功率过大
					qidong = 0;            //关闭所有电路
					shangyicigonglvguoda = 1;   //将上一次功率过大置为1
				}
				wenshidu();
			}
			
			
		}
		else{    //系统处于关闭状态 s
			if(shangyicigonglvguoda == 0){   //本次关闭系统并非由于功率过大而关闭
				shifouyijinru = 1;   //将其置一，表示当前课室没人，用于是否使用光照传感器
				rGPBDAT= rGPBDAT |(0xf<<5);  //关闭灯光（led）
				if(bujin ==1){
				for(i=0; i<10; i++)    //步进电机反转（关闭窗帘）
	        		MOTO_Mode2();
	        		bujin = 0; //停止步进电机
	    		}
			}

			
			rGPGDAT &= ~(0x3<<6);     //风扇（直流电机）关闭
			
			//数码管清零
			ZLG7290_SendCmd(0x60 +0, 0x1f);			
			ZLG7290_SendCmd(0x60 +1, 0x1f);
			ZLG7290_SendCmd(0x60 +2, 0x1f);
			ZLG7290_SendCmd(0x60 +3, 0x1f);
			ZLG7290_SendCmd(0x60 +4, 0x1f);
			ZLG7290_SendCmd(0x60 +5, 0x1f);
			ZLG7290_SendCmd(0x60 +6, 0x1f);
			ZLG7290_SendCmd(0x60 +7, 0x1f);
			//
			
	    	gonglvjiance = 1;   //将功率置一
			Uart_Printf( "关闭\n");
			//continue;
		}
	};
	

	
      
}

