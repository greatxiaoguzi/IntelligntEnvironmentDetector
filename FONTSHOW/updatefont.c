#include "sys.h"
#include "ff.h"
#include "w25q64.h"
#include "malloc.h"
#include "ili93xx.h"
#include "updatefont.h"
#define FONTINFOADDR 	(4916+100)*1024 
//�ֿ���Ϣ�ṹ��. 
//���������ֿ������Ϣ����ַ����С��
_font_info ftinfo;

//����ֿ�
//����ֵ 1 �ֿⶪʧ����
//       0 �ֿ�ɹ�
uint8_t Font_Init(void)
{
	W25Q64_Init();	//��ʼ��SPI flash
	SPI_Flash_Read((uint8_t*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//�����ṹ�������
	if(ftinfo.fontok!=0XAA)
		return 1;
	else
		return 0;
}












































