#define  IN_I2C
#include "2440addr.h"
//#include "2410lib.h"
#include "2440lib.h"
#include "myDef.h"


// I2C总线设置
//*********************************************************************
// register name: rIICCON
// -------------------------------------------------------------------
// description :
//
// multi-master IIC-BUS control register.
//
// 0x54000000,R/W,0x0x
// 7      6       5      4     3...0
// ENACK  CLKSRC  ENINT  PEND  TXCLK
// 0      0       0      0     xxxx
//
// ENACK: IIC-bus acknowledge enable bit. In Tx mode, the IICSDA is free
//        in the ack time. In Rx mode, the IICSDA is L in the ack time.
//        0 = Disable
//        1 = Enable
//
// CLKSRC: Source clock of IIC-bus transmit clock prescaler selection bit.
//         0 = IICCLK = fPCLK /16
//         1 = IICCLK = fPCLK /512
//
// ENINT: IIC-Bus Tx/Rx interrupt enable/disable bit.
//        0 = Disable
//        1 = Enable
//
// PEND: IIC-bus Tx/Rx interrupt pending flag. This bit cannot be written
//       to 1. When this bit is read as 1, the IICSCL is tied to L and the
//       IIC is stopped. To resume the operation, clear this bit as 0.
//       - when read:
//         0 = No interrupt pending
//         1 = Interrupt is pending
//       - when write:
//         0 = Clear pending condition and Resume the operation
//         1 = N/A (cannot be written to 1)
//
// TXCLK: IIC-Bus transmit clock prescaler. IIC-Bus transmit clock fre. is
//        determined by this 4-bit prescaler value, .i.e,
//        Tx clock = IICCLK/(IICCON[3:0]+1).
// -------------------------------------------------------------------
// setting:
// - enable ACK
// - IICCLK=PCLK/512=97,656.25Hz @ FCLK=200MHz,HCLK=100MHz,PCLK=50MHz
// - Tx clock = IICCLK/(3+1)= 24.414KHz @ IICCLK=PCLK/512=97,656.25Hz
// - enable interrupt
//*********************************************************************
#define   IICCON_DACK       ((1<<7) | (1<<6) | (1<<5) | (3<<0))/* 使能ACK位，IICCLK=PCLK/512=97656.25 */
#define   IICCON_DNACK      ((0<<7) | (1<<6) | (1<<5) | (3<<0)) /* 中断使能(这样才能正确操作I2C) */


void  InitI2C(void)
{
    // 设置管脚连接
    rGPECON = (rGPECON & 0x0FFFFFFF) | 0xA0000000;  // 设置I2C口使能
    rGPEUP = rGPEUP | 0xC000;                       // 禁止内部上拉电阻

    // 设置I2C控制寄存器 (使能ACK位时才能接收从机的应答位)
    rIICCON = IICCON_DACK;

    // 设置I2C为主机模式
    rIICSTAT = (3<<6)|(1<<4);

    // 从机地址 (作主机时没有用)
    rIICADD = 0x10;
}


int  StartI2C(uint8 sla)
{
    uint32  i;

    rIICDS = sla;                           // 设置从机地址

    if(sla&0x01)                            // 写
    {
        rIICSTAT = (2<<6)|(1<<5)|(1<<4);    // 主接收模式，发送使能，启动总线
        rIICCON = IICCON_DACK;              // 若是重启总线，则需要有此操作
    }
    else                                    // 读
    {
        rIICSTAT = (3<<6)|(1<<5)|(1<<4);    // 主发送模式，发送使能，启动总线
    }

    while((rIICCON&0x10) == 0);             // 等待操作完成


    if((rIICSTAT&0x09) == 0)                // 判断操作是否成功 (总线仲裁和应答)
    {
        return(TRUE);
    }
    else                                    // 如果失败,则发送结束信号
    {
        if(sla&0x01) rIICSTAT = (2<<6)|(0<<5)|(1<<4);
          else  rIICSTAT = (3<<6)|(0<<5)|(1<<4);
        rIICCON = IICCON_DACK;

        for(i=0; i<5000; i++);              // 等待结束信号产生完毕
        return(FALSE);
    }
}



int  I2C_SendByte(uint8 dat)
{
    uint32  i;

    rIICDS = dat;                           // 将数据写入I2C数据寄存器
    rIICCON = IICCON_DACK;                  // 清除中断标志，允许发送数据操作

    while((rIICCON&0x10) == 0);             // 等待操作完成

    
    if((rIICSTAT&0x09) == 0)                // 判断总线仲裁和从机应答
    {
        return(TRUE);
    }
    else                                    // 如果失败,则发送结束信号
    {
        rIICSTAT = (3<<6)|(0<<5)|(1<<4);
        rIICCON = IICCON_DACK;

        for(i=0; i<5000; i++);              // 等待结束信号产生完毕
        return(FALSE);
    }
}


int  I2C_RcvByteNA(uint8 *dat)
{
    uint32  i;

    // 允许接收数据
    rIICCON = IICCON_DNACK;

    // 等待接收数据操作完成
    while((rIICCON&0x10) == 0);

    // 判断操作是否成功 (总线仲裁)
    if((rIICSTAT&0x08) != 0)
    {
        rIICSTAT = (2<<6)|(0<<5)|(1<<4);    // 发送结束信号
        rIICCON = IICCON_DACK;

        for(i=0; i<5000; i++);              // 等待结束信号产生完毕
        return(FALSE);
    }

    *dat = rIICDS;                          // 读取数据
    return(TRUE);
}




int  I2C_RcvByteA(uint8 *dat)
{
    uint32  i;

    // 允许接收数据
    rIICCON = IICCON_DACK;

    // 等待接收数据操作完成
    while((rIICCON&0x10) == 0);

    // 判断操作是否成功 (总线仲裁)
    if((rIICSTAT&0x08) != 0)
    {
        // 发送结束信号
        rIICSTAT = (2<<6)|(0<<5)|(1<<4);
        rIICCON = IICCON_DACK;

        for(i=0; i<5000; i++);      // 等待结束信号产生完毕
        return(FALSE);
    }

    *dat = rIICDS;      // 读取数据
    return(TRUE);
}




void  StopI2C(uint8 send)
{
    uint32  i;

    if(send)
    {
        rIICSTAT = (3<<6)|(0<<5)|(1<<4);        // 发送结束信号
    }
    else
    {
        rIICSTAT = (2<<6)|(0<<5)|(1<<4);        // 发送结束信号
    }
    rIICCON = IICCON_DACK;
    for(i=0; i<5000; i++);                      // 等待结束信号产生完毕
}




int  ISendStr(uint8 sla, uint8 *suba, uint8 *s, uint8 no)
{
    int  bak;

    sla = sla & 0xFE;
    if(!StartI2C(sla))
    {
        if(!StartI2C(sla)) return(FALSE);       // 启动总线，发送从机地址
    }

    // 发送器件子地址
    bak = *suba++;
    for(; bak>0; bak--)
    {
        if(!I2C_SendByte(*suba++)) return(FALSE);
    }

    // 发送数据
    for(; no>0; no--)
    {
        if(!I2C_SendByte(*s)) return(FALSE);
        s++;
    }

    StopI2C(1);     // 结束总线

    return(TRUE);
}




int  IRcvStr(uint8 sla,uint8 *suba,uint8 *s,uint8 no)
{
    int  bak;

    bak = *suba++;  // 子地址个数
    if(bak > 0)
    {
        sla = sla & 0xFE;
        if(!StartI2C(sla))
        {
            if(!StartI2C(sla)) return(FALSE);   // 启动总线，发送从机地址(写)
        }

        // 发送器件子地址
        for(; bak>0; bak--)
        {
            if(!I2C_SendByte(*suba++)) return(FALSE);
        }
    }

    // 重启总线
    sla = sla | 0x01;
    if(!StartI2C(sla)) return(FALSE);       // 启动总线，发送从机地址(读)

    // 读取数据
    for(; no>1; no--)
    {
        if(!I2C_RcvByteA(s)) return(FALSE); // 发送数据
        s++;
    }
    if(!I2C_RcvByteNA(s)) return(FALSE);

    StopI2C(0);     // 结束总线

    return(TRUE);
}



