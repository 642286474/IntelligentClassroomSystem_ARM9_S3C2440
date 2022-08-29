#define  IN_ZLG7290
#include "2440addr.h"
//#include "2410lib.h"
#include "2440lib.h"
#include "I2C.h"
#include "myDef.h"


#define  ZLG7290 	0x70    /* ZLG7290的IIC地址 */

// ZLG7290寄存器地址(子地址)
#define  SubKey  	0x01
#define  SubCmdBuf 	0x07
#define  SubDpRam 	0x10



/*********************************************************************************************************
** Function name: DelayMS
** Descriptions : 软件延时  **延时时间与系统时钟有关**
** Input : dly
** Output: -
** Created Date : 2007-8-2 09:53上午
********************************************************************************************************/
void DelayMS(uint32 dly)
{
    uint8  i;

    for(; dly>0; dly--)
      for(i=0; i<100; i++);
}



/*********************************************************************************************************
** Function name: ZLG7290_SendData
** Descriptions : 发送数据
** Input : SubAdd, 子地址
**         DATA, 数据值
** Output: 0, Fail
**         1, OK
** Created Date : 2007-8-2 10:22上午
********************************************************************************************************/
unsigned char ZLG7290_SendData(unsigned char SubAdd,unsigned char Data)
{
    unsigned char suba[2];              // 子地址个数, 子地址值

	if(SubAdd>0x17) return 0;           // 最大地址0x17

	suba[0] = 1;                        // 子地址个数
	suba[1] = SubAdd;                   // 子地址
	ISendStr(ZLG7290, suba, &Data, 1);
	DelayMS(10);
	return 1;
}



/*********************************************************************************************************
** Function name: ZLG7290_SendCmd
** Descriptions : 发送命令（对子地址7、8）
** Input : DATA1, 命令1, 对应ZLG7290的CmdBuf0寄存器
**         DATA2, 命令2, 对应ZLG7290的CmdBuf1寄存器
** Output: 0, Fail
**         1, OK
** Created Date : 2007-8-2 11:27上午
********************************************************************************************************/
unsigned char ZLG7290_SendCmd(unsigned char Data1,unsigned char Data2)
{
    unsigned char Data[2];
    unsigned char suba[2];

	Data[0] = Data1;
	Data[1] = Data2;
	suba[0] = 1;
	suba[1] = SubCmdBuf;
	ISendStr(ZLG7290, suba, Data, 2);
	DelayMS(100);

	return(1);
}


/*********************************************************************************************************
** Function name: ZLG7290_SendBuf
** Descriptions : 向显示缓冲区发送数据
** Input : *disp_buf, 要发送数据的起始地址
**          num, 发送个数
** Output: -
** Created Date : 2007-8-2 11:28上午
********************************************************************************************************/
void  ZLG7290_SendBuf(unsigned char *disp_buf,unsigned char num)
{
	unsigned char  i;

	for(i=0; i<num; i++)
	{
		ZLG7290_SendCmd(0x60 + i, *disp_buf);
		disp_buf++;
	}
}


/*********************************************************************************************************
** Function name: ZLG7290_GetKey
** Descriptions : 读按键
** Input : -
** Output: >0 键值  (低8位为键码，高8位为按键次数)
**         =0 无键按下
** Created Date : 2007-8-2 11:30上午
********************************************************************************************************/
unsigned short	ZLG7290_GetKey(void)
{
    unsigned char rece[2];
    unsigned char suba[2];

    rece[0] = rece[1] =  0;
	suba[0] = 1;
	suba[1] = SubKey;
	IRcvStr(ZLG7290, suba, rece, 2);
	DelayMS(10);

	return (rece[0] | (rece[1]<<8));
}


/* end of file */
