#ifndef  __ZLG7290_H
#define  __ZLG7290_H

#ifndef  IN_ZLG7290

    #ifdef __cplusplus
    extern "C" {
    #endif


/*********************************************************************************************************
** Function name: ZLG7290_SendData
** Descriptions : 发送数据
** Input : SubAdd, 子地址
**         DATA, 数据值
** Output: 0, Fail
**         1, OK
** Created Date : 2007-8-2 10:22上午
********************************************************************************************************/
extern unsigned char ZLG7290_SendData(unsigned char SubAdd,unsigned char Data);


/*********************************************************************************************************
** Function name: ZLG7290_SendCmd
** Descriptions : 发送命令（对子地址7、8）
** Input : DATA1, 命令1, 对应ZLG7290的CmdBuf0寄存器
**         DATA2, 命令2, 对应ZLG7290的CmdBuf1寄存器
** Output: 0, Fail
**         1, OK
** Created Date : 2007-8-2 11:27上午
********************************************************************************************************/
extern unsigned char ZLG7290_SendCmd(unsigned char Data1,unsigned char Data2);


/*********************************************************************************************************
** Function name: ZLG7290_SendBuf
** Descriptions : 向显示缓冲区发送数据
** Input : *disp_buf, 要发送数据的起始地址
**          num, 发送个数
** Output: -
** Created Date : 2007-8-2 11:28上午
********************************************************************************************************/
extern void  ZLG7290_SendBuf(unsigned char *disp_buf,unsigned char num);


/*********************************************************************************************************
** Function name: ZLG7290_GetKey
** Descriptions : 读按键
** Input : -
** Output: >0 键值  (低8位为键码，高8位为按键次数)
**         =0 无键按下
** Created Date : 2007-8-2 11:30上午
********************************************************************************************************/
extern unsigned short	ZLG7290_GetKey(void);


/********************************************************************************************************/
    #ifdef __cplusplus
    }
    #endif 

#endif      // IN_ZLG7290    

#endif      // __ZLG7290_H

/* End Of File */

