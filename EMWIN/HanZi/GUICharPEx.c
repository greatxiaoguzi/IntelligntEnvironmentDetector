#include <stddef.h>           /* needed for definition of NULL */ 
#include "GUI_Private.h" 
#include "ff.h" 
#include "string.h"


/* ��ģ���ݵ��ݴ�����,�Ե�����ģ������ֽ���Ϊ�趨ֵ */ 
#define BYTES_PER_FONT      1024 
static U8 GUI_FontDataBuf[BYTES_PER_FONT];

/* 
   ����12*12��16*16���Ӻ��ֵ�·�����֣�Ҫ����������·�������� 
   ʹ��sizeof��ʱ�򣬵�ָ������Ĵ�С��
*/
const char FontEx_HZ[22] = {"0:/system/gui_font/HZ1"};
 
FIL fsrc;        // �����ļ�������  
FRESULT res;     // ��������������  
UINT bw1;         // �����д�������� 
/*---------------------------------------------------------------------------*/ 
/*�ֿ��ⲿ��������-----------------------------------------------------------*/ 
void GUI_X_GetFontData(char* font, U32 oft, U8 *ptr, U16 bytes) 
{ 
    res = f_open(&fsrc, font, FA_OPEN_EXISTING | FA_READ);   //���ֿ��ļ�   
    if(res != FR_OK)   
    {   
       
    }   
    res = f_lseek(&fsrc,oft); //�ҵ��׵�ַ  
    res = f_read(&fsrc, ptr, bytes, &bw1); //��ȡ32���ֿ�������� 
	res = f_close(&fsrc); //�ر�����  
 
} 

static void GUI_GetDataFromMemory(const GUI_FONT_PROP GUI_UNI_PTR *pProp, U16P c) 
{ 
    U16 BytesPerFont;
    U32 oft; 
	char *font = (char *)pProp->paCharInfo->pData; 

	/* ÿ����ģ�������ֽ��� */
    BytesPerFont = GUI_pContext->pAFont->YSize * pProp->paCharInfo->BytesPerLine; 
    if (BytesPerFont > BYTES_PER_FONT)
	{
		BytesPerFont = BYTES_PER_FONT;
	}
	/* Ӣ���ַ���ַƫ���㷨 */ 
    if (c < 0x80)                                                                
    { 
		oft = (c-0x20) * BytesPerFont; 
    } 
    else                                                                           
    { 

		if(strncmp(FontEx_HZ, font, sizeof(FontEx_HZ)) == 0)
		{
			/* �����ַ���ַƫ���㷨�������� */
			oft = ((((c >> 8)-0xA1)) + ((c & 0xFF)-0xA1) * 94)* BytesPerFont; 
		}
		else
		{
			/* �����ַ���ַƫ���㷨�������� */
			oft = ((((c >> 8)-0xA1)) + ((c & 0xFF)-0xB0) * 94)* BytesPerFont;
		}

    }


	GUI_X_GetFontData(font, oft, GUI_FontDataBuf, BytesPerFont); 
     
} 
 
void GUIPROP_X_DispChar(U16P c)  
{ 
    int BytesPerLine; 
    GUI_DRAWMODE DrawMode = GUI_pContext->TextMode; 
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUI_pContext->pAFont->p.pProp; 
    //������λ�ֿ�������Ϣ  
    for (; pProp; pProp = pProp->pNext)                                          
    { 
        if ((c >= pProp->First) && (c <= pProp->Last))break; 
    } 
    if (pProp) 
    { 
        GUI_DRAWMODE OldDrawMode;
        const GUI_CHARINFO GUI_UNI_PTR * pCharInfo = pProp->paCharInfo;
        GUI_GetDataFromMemory(pProp, c);//ȡ����ģ����
        BytesPerLine = pCharInfo->BytesPerLine;                
        OldDrawMode  = LCD_SetDrawMode(DrawMode);
        LCD_DrawBitmap(GUI_pContext->DispPosX, GUI_pContext->DispPosY,
                       pCharInfo->XSize, GUI_pContext->pAFont->YSize,
                       GUI_pContext->pAFont->XMag, GUI_pContext->pAFont->YMag,
                       1,     /* Bits per Pixel */
                       BytesPerLine,
                       &GUI_FontDataBuf[0],
                       &LCD_BKCOLORINDEX
                       );
        /* Fill empty pixel lines */
        if (GUI_pContext->pAFont->YDist > GUI_pContext->pAFont->YSize) 
        {
            int YMag = GUI_pContext->pAFont->YMag;
            int YDist = GUI_pContext->pAFont->YDist * YMag;
            int YSize = GUI_pContext->pAFont->YSize * YMag;
            if (DrawMode != LCD_DRAWMODE_TRANS) 
            {
                LCD_COLOR OldColor = GUI_GetColor();
                GUI_SetColor(GUI_GetBkColor());
                LCD_FillRect(GUI_pContext->DispPosX, GUI_pContext->DispPosY + YSize, 
                             GUI_pContext->DispPosX + pCharInfo->XSize, 
                             GUI_pContext->DispPosY + YDist);
                GUI_SetColor(OldColor);
            }
        }
        LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
//      if (!GUI_MoveRTL)
        GUI_pContext->DispPosX += pCharInfo->XDist * GUI_pContext->pAFont->XMag;
    }
} 

/*********************************************************************
*
*       GUIPROP_GetCharDistX
*/
int GUIPROP_X_GetCharDistX(U16P c) 
{
    const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUI_pContext->pAFont->p.pProp;  
    for (; pProp; pProp = pProp->pNext)                                         
    {
        if ((c >= pProp->First) && (c <= pProp->Last))break;
    }
    return (pProp) ? (pProp->paCharInfo)->XSize * GUI_pContext->pAFont->XMag : 0;
}

