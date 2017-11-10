#include "sys.h"
#include "ff.h"
#include "w25q64.h"
#include "malloc.h"
#include "ili93xx.h"
#include "updatefont.h"
#define FONTINFOADDR 	(4916+100)*1024 
//字库信息结构体. 
//用来保存字库基本信息，地址，大小等
_font_info ftinfo;

//检查字库
//返回值 1 字库丢失出错
//       0 字库成功
uint8_t Font_Init(void)
{
	W25Q64_Init();	//初始化SPI flash
	SPI_Flash_Read((uint8_t*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//读出结构体的数据
	if(ftinfo.fontok!=0XAA)
		return 1;
	else
		return 0;
}












































