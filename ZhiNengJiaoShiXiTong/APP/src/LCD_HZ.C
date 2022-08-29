
#include "def.h"

extern const INT8U g_ucHZK16[];
extern int strlen(const char*s);
extern void PutPixel(U16 x0,U16 y0,U16 c);
/*********************************************************************************************
* name:		lcd_disp_hz16()
* func:		display chinese character string in 16x16 dot array
* para:		usX0,usY0 -- ASCII character string's start point coordinate
*			ForeColor -- appointed color value
*			pucChar   -- ASCII character string
* ret:		none
* modify:
* comment:		
*********************************************************************************************/

void Lcd_disp_hz16(INT16U x00, INT16U y00, INT8U HzColor, INT8U *s)
{
	INT16U i,j,k,x,y,xx;
	INT8U qm,wm;
	INT32U ulOffset;
	INT8U hzbuf[32],temp[2];
	 Uart_Printf("%s\n",s);
	for( i = 0; i < strlen((const char*)s); i++ )
	{
		if( ((INT8U)(*(s+i))) < 161 )
		{
			temp[0] = *(s+i);
			temp[1] = '\0';
			break;
		}
		else
		{
			qm = *(s+i) - 161;
    		wm = *(s + i + 1) - 161;
       		ulOffset = (INT32U)(qm * 94 + wm) * 32;
			for( j = 0; j < 32; j ++ )
            {
            	hzbuf[j] = g_ucHZK16[ulOffset + j];
            }
            for( y = 0; y < 16; y++ )
            {
	        	for( x = 0; x < 16; x++ ) 
	            {
                	k = x % 8;
				   	if( hzbuf[y * 2 + x / 8]  & (0x80 >> k) )
				    {
				    	xx = x00 + x + i * 8;
				    	PutPixel(xx,y + y00,HzColor);
				    }
			   	}
           	}
		    i++;
		}
	}
}


/**************************************************************
*The end
***************************************************************/
