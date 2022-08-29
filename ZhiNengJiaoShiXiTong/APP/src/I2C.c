#define  IN_I2C
#include "2440addr.h"
//#include "2410lib.h"
#include "2440lib.h"
#include "myDef.h"


// I2C��������
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
#define   IICCON_DACK       ((1<<7) | (1<<6) | (1<<5) | (3<<0))/* ʹ��ACKλ��IICCLK=PCLK/512=97656.25 */
#define   IICCON_DNACK      ((0<<7) | (1<<6) | (1<<5) | (3<<0)) /* �ж�ʹ��(����������ȷ����I2C) */


void  InitI2C(void)
{
    // ���ùܽ�����
    rGPECON = (rGPECON & 0x0FFFFFFF) | 0xA0000000;  // ����I2C��ʹ��
    rGPEUP = rGPEUP | 0xC000;                       // ��ֹ�ڲ���������

    // ����I2C���ƼĴ��� (ʹ��ACKλʱ���ܽ��մӻ���Ӧ��λ)
    rIICCON = IICCON_DACK;

    // ����I2CΪ����ģʽ
    rIICSTAT = (3<<6)|(1<<4);

    // �ӻ���ַ (������ʱû����)
    rIICADD = 0x10;
}


int  StartI2C(uint8 sla)
{
    uint32  i;

    rIICDS = sla;                           // ���ôӻ���ַ

    if(sla&0x01)                            // д
    {
        rIICSTAT = (2<<6)|(1<<5)|(1<<4);    // ������ģʽ������ʹ�ܣ���������
        rIICCON = IICCON_DACK;              // �����������ߣ�����Ҫ�д˲���
    }
    else                                    // ��
    {
        rIICSTAT = (3<<6)|(1<<5)|(1<<4);    // ������ģʽ������ʹ�ܣ���������
    }

    while((rIICCON&0x10) == 0);             // �ȴ��������


    if((rIICSTAT&0x09) == 0)                // �жϲ����Ƿ�ɹ� (�����ٲú�Ӧ��)
    {
        return(TRUE);
    }
    else                                    // ���ʧ��,���ͽ����ź�
    {
        if(sla&0x01) rIICSTAT = (2<<6)|(0<<5)|(1<<4);
          else  rIICSTAT = (3<<6)|(0<<5)|(1<<4);
        rIICCON = IICCON_DACK;

        for(i=0; i<5000; i++);              // �ȴ������źŲ������
        return(FALSE);
    }
}



int  I2C_SendByte(uint8 dat)
{
    uint32  i;

    rIICDS = dat;                           // ������д��I2C���ݼĴ���
    rIICCON = IICCON_DACK;                  // ����жϱ�־�����������ݲ���

    while((rIICCON&0x10) == 0);             // �ȴ��������

    
    if((rIICSTAT&0x09) == 0)                // �ж������ٲúʹӻ�Ӧ��
    {
        return(TRUE);
    }
    else                                    // ���ʧ��,���ͽ����ź�
    {
        rIICSTAT = (3<<6)|(0<<5)|(1<<4);
        rIICCON = IICCON_DACK;

        for(i=0; i<5000; i++);              // �ȴ������źŲ������
        return(FALSE);
    }
}


int  I2C_RcvByteNA(uint8 *dat)
{
    uint32  i;

    // �����������
    rIICCON = IICCON_DNACK;

    // �ȴ��������ݲ������
    while((rIICCON&0x10) == 0);

    // �жϲ����Ƿ�ɹ� (�����ٲ�)
    if((rIICSTAT&0x08) != 0)
    {
        rIICSTAT = (2<<6)|(0<<5)|(1<<4);    // ���ͽ����ź�
        rIICCON = IICCON_DACK;

        for(i=0; i<5000; i++);              // �ȴ������źŲ������
        return(FALSE);
    }

    *dat = rIICDS;                          // ��ȡ����
    return(TRUE);
}




int  I2C_RcvByteA(uint8 *dat)
{
    uint32  i;

    // �����������
    rIICCON = IICCON_DACK;

    // �ȴ��������ݲ������
    while((rIICCON&0x10) == 0);

    // �жϲ����Ƿ�ɹ� (�����ٲ�)
    if((rIICSTAT&0x08) != 0)
    {
        // ���ͽ����ź�
        rIICSTAT = (2<<6)|(0<<5)|(1<<4);
        rIICCON = IICCON_DACK;

        for(i=0; i<5000; i++);      // �ȴ������źŲ������
        return(FALSE);
    }

    *dat = rIICDS;      // ��ȡ����
    return(TRUE);
}




void  StopI2C(uint8 send)
{
    uint32  i;

    if(send)
    {
        rIICSTAT = (3<<6)|(0<<5)|(1<<4);        // ���ͽ����ź�
    }
    else
    {
        rIICSTAT = (2<<6)|(0<<5)|(1<<4);        // ���ͽ����ź�
    }
    rIICCON = IICCON_DACK;
    for(i=0; i<5000; i++);                      // �ȴ������źŲ������
}




int  ISendStr(uint8 sla, uint8 *suba, uint8 *s, uint8 no)
{
    int  bak;

    sla = sla & 0xFE;
    if(!StartI2C(sla))
    {
        if(!StartI2C(sla)) return(FALSE);       // �������ߣ����ʹӻ���ַ
    }

    // ���������ӵ�ַ
    bak = *suba++;
    for(; bak>0; bak--)
    {
        if(!I2C_SendByte(*suba++)) return(FALSE);
    }

    // ��������
    for(; no>0; no--)
    {
        if(!I2C_SendByte(*s)) return(FALSE);
        s++;
    }

    StopI2C(1);     // ��������

    return(TRUE);
}




int  IRcvStr(uint8 sla,uint8 *suba,uint8 *s,uint8 no)
{
    int  bak;

    bak = *suba++;  // �ӵ�ַ����
    if(bak > 0)
    {
        sla = sla & 0xFE;
        if(!StartI2C(sla))
        {
            if(!StartI2C(sla)) return(FALSE);   // �������ߣ����ʹӻ���ַ(д)
        }

        // ���������ӵ�ַ
        for(; bak>0; bak--)
        {
            if(!I2C_SendByte(*suba++)) return(FALSE);
        }
    }

    // ��������
    sla = sla | 0x01;
    if(!StartI2C(sla)) return(FALSE);       // �������ߣ����ʹӻ���ַ(��)

    // ��ȡ����
    for(; no>1; no--)
    {
        if(!I2C_RcvByteA(s)) return(FALSE); // ��������
        s++;
    }
    if(!I2C_RcvByteNA(s)) return(FALSE);

    StopI2C(0);     // ��������

    return(TRUE);
}



