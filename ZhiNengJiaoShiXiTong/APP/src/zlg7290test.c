#include "2440addr.h"
//#include "2410lib.h"
#include "2440lib.h"
#include "ZLG7290.h"
#include "I2C.h"
#include "myDef.h"


char    disp_buf[40];           // ������ʾ������

#define Glitter_COM     0x70    // ZLG7290����LED�������˸����

//static unsigned char BITMAMK[8]={0x01,0x02,0x04,0x08,0x20,0x40,0x80};
//static unsigned char KEYCODE[16]={0x00,0x04,0x08,0x0C,0x01,0x05,0x0f,0x0D,0x02,0x06,0x0A,0x0E,0x03,0x07,0x0B,0x09};

void  DelayNS2(uint32  dly)
{
	uint32  i;

   	for(; dly>0; dly--)
       for(i=0; i<50000; i++);
}


/*
void zlg7290test(void)
{
    int  i, j;
    uint16  key;
	unsigned char BITMAMK[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	unsigned char KEYCODE[16]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x0F,0x0A,0x0B,0x0C,0x0D,0x0E,0x09};
    Uart_Printf("\nBegin to test, press ESC key to exit!\n");

	rGPGCON = rGPGCON & (~(0x3<<0));
	rGPGUP = rGPGUP | 1;
    // ��ʼ��I2C�ӿ�
    InitI2C();
/*
    // ȫ��ʾ0~9����
    Uart_Printf("All digits to display 0~9!\n");
    for(i=0; i<=9; i++)
    {
        for(j=0; j<8; j++) disp_buf[j] = i;
        ZLG7290_SendBuf((uint8 *)disp_buf, 8);
        DelayNS2(200);
    }

    // ��ʾ"87654321"ȷ�������λ��
    Uart_Printf("Display 1~8, make sure the digit position!\n");
    for(j=0; j<8; j++) disp_buf[j] = j+1;
    ZLG7290_SendBuf((uint8 *)disp_buf, 8);
    DelayNS2(200);

    // ��λ��˸
    Uart_Printf("Flash one by one!\n");
    for(i=0; i<8; i++)
    {
        ZLG7290_SendCmd(Glitter_COM, BITMAMK[i]);
        DelayNS2(1000);
    }
    ZLG7290_SendCmd(Glitter_COM, 0x00);
*/
    // ��ʾ"HELLO   "
/*    
    Uart_Printf("Display hello, waiting for keypad operation!\n");
    disp_buf[3] = 14;   // H
    disp_buf[2] = 20;   // E
    disp_buf[1] = 20;   // L
    disp_buf[0] = 0;    // L
    disp_buf[7] = 31;   // O
    disp_buf[6] = 31;   // -
    disp_buf[5] = 31;   // -
    disp_buf[4] = 17;   // -
    ZLG7290_SendBuf((uint8 *)disp_buf, 8);
    DelayNS2(200);


			ZLG7290_SendCmd(0x60 +0, 0x1f);			
		    ZLG7290_SendCmd(0x60 +1, 0x1f);
		    ZLG7290_SendCmd(0x60 +2, 0x1f);
		    ZLG7290_SendCmd(0x60 +3, 0x1f);
		    ZLG7290_SendCmd(0x60 +4, 0x1f);
		    ZLG7290_SendCmd(0x60 +5, 0x1f);
		    ZLG7290_SendCmd(0x60 +6, 0x1f);
		    ZLG7290_SendCmd(0x60 +7, 0x1f);

    // ��ȡ���������ü�ֵ��Ӧ����ʾλ��˸
    while(1){
    	while((rGPGDAT&0x1)==0)
   		{
     		 DelayNS2(100);
       		 key = ZLG7290_GetKey();
       		 if(key != 0x00)
       		 {
        		   key =key -1;
           		   key = key&0x000F;



					ZLG7290_SendCmd(0x60 , KEYCODE[key]);	

				
        	 }
        }
    }
 }
*/ 
/* end of file */



//��ʪ��
#define DEVICE_NAME "humidity"

//XEINT24/KP_ROW0/GPH3_0
unsigned long receive_value;
unsigned long receive_jy;

/*  �Զ�����ӳٳ���  */ 
static void mdelay(unsigned int t)
{

	 rTCFG1 &= ~0x0F;
	 rTCFG1 |= 2;
	 rTCNTB0 =67500000/8/1000; //ʱ��Ƶ��ΪPCLK/8 6 //����1000��ʾ����m�뼶��ʱ
	 rTCON &= ~0x0F;
	 rTCON |= (1<<3)|(1<<1)|(1<<0);
	 rTCON &= ~(1<<1);
	
	
	 while(t--)
	 {
	     while(rTCNTO0!=0);
	 }
	
	 rTCON &= ~(1<<0); //ֹͣ��ʱ��0

}


static void udelay(unsigned int t)
{

//	 rTCFG1 &= ~0x0F;
//	 rTCFG1 |= 2;
//	 rTCNTB0 =67500000/8/1000000; //ʱ��Ƶ��ΪPCLK/8 6 //����1000000��ʾ����΢�뼶��ʱ
//	 rTCON &= ~0x0F;
//	 rTCON |= (1<<3)|(1<<1)|(1<<0);
//	 rTCON &= ~(1<<1);		
	
	 while(t--)
	 {
	    // while(rTCNTO0!=0);
     __asm{ nop
		nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop }
			
	 }
	
	 //rTCON &= ~(1<<0); //ֹͣ��ʱ��0		 

	 
}


int data_in(void)
{
	//s3c_gpio_cfgpin(S5PV210_GPH3(0), S3C_GPIO_INPUT);
	//return gpio_get_value(S5PV210_GPH3(0));
	rGPGCON= rGPGCON & ~(0x03<<22);
	rGPGUP = (rGPGUP & ~(0x1<<11));
	if(rGPGDAT & (1 << 11))
	{ 
	//	 Uart_Printf("000000\n");
	   return 1;
	}
	else 
	{
	//	 Uart_Printf("3333333\n");
	   return 0;
	}
}


void data_out(int data)
{
	//s3c_gpio_cfgpin(S5PV210_GPH3(0), S3C_GPIO_OUTPUT);
	//gpio_set_value(S5PV210_GPH3(0), data); 
//	rGPGUP = (rGPGUP & ~(0x1<<11)) | (0x1<<11); 
	rGPGCON= (rGPGCON & ~(0x03 << 22)) | (0x01 << 22);
	if(data)rGPGDAT |= 0x01 << 11;
      else   rGPGDAT &= ~(0x01 << 11);
}

void read_data(void)
{
	unsigned int flag = 0;
	unsigned int u32i = 0;
	
	data_out(0);
	mdelay(20);
	data_out(1);
	udelay(40);
	if (data_in() == 0)
	{
		while( (data_in() == 0) && ((flag++)<50000) );
		flag = 0;
		receive_value = 0;
		receive_jy = 0;
		while( data_in() != 0 );
		for (u32i=0x80000000; u32i>0; u32i>>=1)
		{
			flag = 0;
			while( (data_in() == 0) && ((flag++)<50000) );
			flag = 0;
			while( (data_in() != 0) && ((flag)<500))
			{
				udelay(10);
				flag++;
			}
			
			if(flag > 5)
			{
	//			printk("flag 1= %d\n",flag);
				receive_value |= u32i;
			}
		}
	//			printk("flag 0= %d\n",flag);
		for (u32i=0x80; u32i>0; u32i>>=1)
		{
			flag = 0;
			while( (data_in() == 0) && ((flag++)<50000) );
			flag = 0;
			while( (data_in() != 0) && ((flag)<500))
			{
				udelay(10);
				flag++;
			}
			
			if(flag > 5)
			{
				receive_jy |= u32i;
			}
		}
	}
}


void humidity_read_data(void)
{
	unsigned int flag = 0;
	unsigned int u32i = 0;
        receive_value = 0;
        receive_jy = 0;
        data_out(0);
	mdelay(20);
	data_out(1);
	//udelay(40);
	udelay(200);	 //10us
	udelay(200);
	udelay(200);
	udelay(200);
	if (data_in() == 0)
	{
                flag = 0;
                while(data_in() == 0) //��Ӧ80us�ĵ͵�ƽ
                {
                    //udelay(10);
										udelay(200);	 //10us
                    flag++;
                    if(flag > 10)
                    return;//����100us����������û����Ӧ����Ϊ��������⣬�˳�
                }
               // Uart_Printf("80us low flag=%d\n",flag);
                flag  = 0;
                while(data_in() == 1) //��Ӧ80us�ĸߵ�ƽ 
                {
                   // udelay(10);
									 udelay(200);	 //10us
                    flag++;
                    if(flag > 10)
                    return;//����100us����������û����Ӧ����Ϊ��������⣬�˳�
                }
               // Uart_Printf("80us high flag=%d\n",flag);                    
                flag  = 0;
        
		for (u32i=0x80000000; u32i>0; u32i>>=1)
		{
		        flag = 0;
                        while(data_in() == 0) //��Ӧ50us�ĵ͵�ƽ ,��ʼ��������
                        {
                           // udelay(10);
													 udelay(200);	 //10us
                            flag++;
                            if(flag > 10)
                            {
                                 	//  Uart_Printf("ccccc\n");
																    break;
														}
                        }

			flag = 0;
			while( data_in() == 1)  //��Ӧ�ߵ�ƽ�� �����ߵ�ƽʱ����಻����70us
			{
				//udelay(10);
				udelay(200);	 //10us
				flag++;
                                if(flag > 10)
																{
                                 	//  Uart_Printf("aaaa\n");
																    break;
																}
			}
			
			if(flag > 5) //�͵�ƽ����ʱ��Ϊ26us-28us Ϊ����0 �ߵ�ƽ����ʱ��Ϊ70usΪ����1
			{
				receive_value |= u32i;
			}
		}

        #if 1
	//			printk("flag 0= %d\n",flag);
		for (u32i=0x80; u32i>0; u32i>>=1)
		{
		        flag = 0;
                        while(data_in() == 0) //��Ӧ50us�ĵ͵�ƽ ,��ʼ��������
                        {
                           // udelay(10);
													 udelay(200);	 //10us
                            flag++;
                            if(flag > 10)
                            {
                                 	 // Uart_Printf("bbbbb\n");
																    break;
														}
                        }
			flag = 0;
			while( data_in() == 1)  //��Ӧ�ߵ�ƽ�� �����ߵ�ƽʱ����಻����70us
			{
				//udelay(10);
				udelay(200);	 //10us
				flag++;
                                if(flag > 10)
                                {
                                 	//  Uart_Printf("dddddd\n");
																    break;
																}
			}
			
			if(flag > 5) //�͵�ƽ����ʱ��Ϊ26us-28us Ϊ����0 �ߵ�ƽ����ʱ��Ϊ70usΪ����1
			{
				receive_jy |= u32i;
			}
		}
        #endif
	}
}


static void gec210_humidiy_read()
{
	unsigned char tempz = 0;
	unsigned char tempx = 0;
	unsigned char humidityz = 0;
	unsigned char humidityx = 0;
  unsigned char  ecc,jy;


	//����� 
	int  i, j;
    uint16  key;
	unsigned char BITMAMK[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	unsigned char KEYCODE[16]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xA,0x0B,0x0C,0x0D,0x0E,0x09};

	rGPGCON = rGPGCON & (~(0x3<<0));
	rGPGUP = rGPGUP | 1;
    // ��ʼ��I2C�ӿ�
    InitI2C();
    
    
    
    

  humidity_read_data();
	//read_data();   receive_value  receive_value= 8bitʪ����������+8bitʪ��С������+8bi�¶���������+8bit�¶�С������
	
	
    humidityz = (receive_value & 0xff000000)>>24;//1000>>2 = 10    10<<2 1000
          
	humidityx = (receive_value & 0x00ff0000)>>16;
	tempz = (receive_value & 0x0000ff00)>>8;
	tempx = (receive_value & 0x000000ff);
	
	//�ն���ʾ
	Uart_Printf("humidity is %d.%d\n",humidityz,humidityx);   
	Uart_Printf("temp is %d.%d\n",tempz,tempx);  
	
	//�������ʾ 
	ZLG7290_SendCmd(0x60 +3, KEYCODE[humidityz/10]);// 
	ZLG7290_SendCmd(0x60 +2, KEYCODE[humidityz%10]|0x80);
	ZLG7290_SendCmd(0x60 +1, KEYCODE[humidityx]);
	ZLG7290_SendCmd(0x60 +6, KEYCODE[tempz/10]);
	ZLG7290_SendCmd(0x60 +5, KEYCODE[tempz%10]|0x80);
	ZLG7290_SendCmd(0x60 +4, KEYCODE[tempx]);
}


void wenshidu(void)
{


	ChangeClockDivider(3,1);
	ChangeMPllValue(127,2,1);		//405MHZ

	Uart_Select(0);			//ʵʱʱ�ӳ�ʼ��
	Uart_Init(0, 115200);	

	 /* �������ϵ��Ҫ�ȴ�1S��Խ�����ȶ�״̬ */
	 mdelay(1000);
	gec210_humidiy_read();
	mdelay(1000);         													 
}
 
