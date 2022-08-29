#define  IN_ZLG7290
#include "2440addr.h"
//#include "2410lib.h"
#include "2440lib.h"
#include "I2C.h"
#include "myDef.h"


#define  ZLG7290 	0x70    /* ZLG7290��IIC��ַ */

// ZLG7290�Ĵ�����ַ(�ӵ�ַ)
#define  SubKey  	0x01
#define  SubCmdBuf 	0x07
#define  SubDpRam 	0x10



/*********************************************************************************************************
** Function name: DelayMS
** Descriptions : �����ʱ  **��ʱʱ����ϵͳʱ���й�**
** Input : dly
** Output: -
** Created Date : 2007-8-2 09:53����
********************************************************************************************************/
void DelayMS(uint32 dly)
{
    uint8  i;

    for(; dly>0; dly--)
      for(i=0; i<100; i++);
}



/*********************************************************************************************************
** Function name: ZLG7290_SendData
** Descriptions : ��������
** Input : SubAdd, �ӵ�ַ
**         DATA, ����ֵ
** Output: 0, Fail
**         1, OK
** Created Date : 2007-8-2 10:22����
********************************************************************************************************/
unsigned char ZLG7290_SendData(unsigned char SubAdd,unsigned char Data)
{
    unsigned char suba[2];              // �ӵ�ַ����, �ӵ�ֵַ

	if(SubAdd>0x17) return 0;           // ����ַ0x17

	suba[0] = 1;                        // �ӵ�ַ����
	suba[1] = SubAdd;                   // �ӵ�ַ
	ISendStr(ZLG7290, suba, &Data, 1);
	DelayMS(10);
	return 1;
}



/*********************************************************************************************************
** Function name: ZLG7290_SendCmd
** Descriptions : ����������ӵ�ַ7��8��
** Input : DATA1, ����1, ��ӦZLG7290��CmdBuf0�Ĵ���
**         DATA2, ����2, ��ӦZLG7290��CmdBuf1�Ĵ���
** Output: 0, Fail
**         1, OK
** Created Date : 2007-8-2 11:27����
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
** Descriptions : ����ʾ��������������
** Input : *disp_buf, Ҫ�������ݵ���ʼ��ַ
**          num, ���͸���
** Output: -
** Created Date : 2007-8-2 11:28����
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
** Descriptions : ������
** Input : -
** Output: >0 ��ֵ  (��8λΪ���룬��8λΪ��������)
**         =0 �޼�����
** Created Date : 2007-8-2 11:30����
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
