/**************************************************************
The initial and control for 240 16Bpp TFT LCD----3.5寸竖屏
**************************************************************/

#include <string.h>
#include "def.h"
#include "2440addr.h"
#include "2440lib.h"
#define RED 0xf800

#define MVAL		(13)
#define MVAL_USED 	(0)		//0=each frame   1=rate by MVAL
#define INVVDEN		(1)		//0=normal       1=inverted
#define BSWP		(0)		//Byte swap control
#define HWSWP		(1)		//Half word swap control

#define M5D(n) ((n) & 0x1fffff)	// To get lower 21bits


#define LCD_XSIZE_TFT_320240 	(800)	
#define LCD_YSIZE_TFT_320240  	(480)


#define SCR_XSIZE_TFT_320240 	(800)
#define SCR_YSIZE_TFT_320240  	(480)


#define HOZVAL_TFT_320240 		(LCD_XSIZE_TFT_320240 -1)
#define LINEVAL_TFT_320240 	(LCD_YSIZE_TFT_320240 -1)

//Timing parameter for LCD
#define VBPD_320240 		(15)		  //垂直同步信号的后肩
#define VFPD_320240 		(12)		  //垂直同步信号的前肩
#define VSPW_320240 		(3)		  //垂直同步信号的脉宽

#define HBPD_320240 		(38)	  //水平同步信号的后肩
#define HFPD_320240 		(20)	  //水平同步信号的前肩
#define HSPW_320240 		(30)	  //水平同步信号的脉宽
#define CLKVAL_TFT_320240 	(5)   	

static void Lcd_Init(void);
static void Lcd_EnvidOnOff(int onoff);
void PutPixel(U16 x0,U16 y0,U16 c);
static void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color);
static void Glib_Line(int x1,int y1,int x2,int y2,int color);
static void Lcd_ClearScr(U16 c);
static void Paint_Bmp(int x0,int y0,int h,int l,unsigned char bmp[]);

extern unsigned char GEC_320240 [];	

volatile static unsigned short LCD_BUFER[SCR_YSIZE_TFT_320240][SCR_XSIZE_TFT_320240 ];

/**************************************************************/
//640*240 16Bpp TFT LCD数据和控制端口初始化
/**************************************************************/
static void Lcd_Port_Init(void)
{
    rGPCUP = 0x0; // enable Pull-up register
    rGPCCON = 0xaaaa56a9; //Initialize VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND 
    //rGPCCON = 0xaaaaaaaa;
    rGPDUP = 0x0 ; // enable Pull-up register
    rGPDCON=0xaaaaaaaa; //Initialize VD[15:8]
}

/**************************************************************/
//640*240 16Bpp TFT LCD功能模块初始化
/**************************************************************/
static void Lcd_Init(void)
{
	//CLKVAL=1;MMODE=0;PNRMODE=11:11 = TFT LCD panel
	//BPPMODE=1100=16 bpp for TFT;ENVID0=Disable the video output and the LCD control signal.
	rLCDCON1=(CLKVAL_TFT_320240 <<8)|(MVAL_USED<<7)|(3<<5)|(12<<1)|0;
    //VBPD=9;LINEVAL=799;VFPD=6;VSPW=2
	rLCDCON2=(VBPD_320240<<24)|(LINEVAL_TFT_320240<<14)|(VFPD_320240 <<6)|(VSPW_320240 );
	//HBPD=10;HOZVAL=479;HFPD=16
	rLCDCON3=(HBPD_320240<<19)|(HOZVAL_TFT_320240<<8)|(HFPD_320240 );
	//MVAL=13;HSPW=20
	rLCDCON4=(MVAL<<8)|(HSPW_320240);
	//FRM5:6:5,HSYNC and VSYNC are inverted
	rLCDCON5=(1<<11)|(0<<10) |(1<<9)|(1<<8)|(0<<7) |(0<<6)|(BSWP<<1)|(HWSWP);	
	
	rLCDSADDR1=(((U32)LCD_BUFER>>22)<<21)|M5D((U32)LCD_BUFER>>1);
	rLCDSADDR2=M5D( ((U32)LCD_BUFER+(SCR_XSIZE_TFT_320240*LCD_YSIZE_TFT_320240*2))>>1 );
	rLCDSADDR3=(((SCR_XSIZE_TFT_320240-LCD_XSIZE_TFT_320240)/1)<<11)|(LCD_XSIZE_TFT_320240/1);
	
	rLCDINTMSK|=(3); // MASK LCD Sub Interrupt
	//rLPCSEL&=(~7); // Disable LPC3600
	rTPAL=0; // Disable Temp Palette
}

/**************************************************************/
//LCD视频和控制信号输出或者停止，1开启视频输出
/**************************************************************/
static void Lcd_EnvidOnOff(int onoff)
{
    if(onoff==1)
	rLCDCON1|=1; // ENVID=ON
    else
	rLCDCON1 =rLCDCON1 & 0x3fffe; // ENVID Off
}

/**************************************************************/
//640*240 16Bpp TFT LCD单个象素的显示数据输出
/**************************************************************/
void PutPixel(U16 x0,U16 y0,U16 c)
{
	if ( (x0 < SCR_XSIZE_TFT_320240) && (y0 < SCR_YSIZE_TFT_320240) )
	LCD_BUFER[(y0)][(x0)] = c;
}

/**************************************************************/
//640*240 16Bpp TFT LCD全屏填充特定颜色单元或清屏
/**************************************************************/
static void Lcd_ClearScr(U16 c)
{
	unsigned int x,y ;
		
    for( y = 0 ; y < SCR_YSIZE_TFT_320240; y++ )
    {
    	for( x = 0 ; x < SCR_XSIZE_TFT_320240; x++ )
    	{
			LCD_BUFER[y][x] = c;
    	}
    }
}

/**************************************************************/
void Test_Lcd_HZ(unsigned char* test)
{
    Uart_Printf("\nTest 640*240 TFT LCD HZ display!\n");

    Lcd_Port_Init();
    Lcd_Init();
    Lcd_EnvidOnOff(1);		//turn on vedio
	Lcd_ClearScr(0xffff);   //fill all screen with some color
	Lcd_disp_hz16(300, 250, RED,test);
	getchar();
}
//*************************************************************
