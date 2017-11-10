#include "sys.h"
#include "sd.h"
#include  "ff.h"
#include "ucos_ii.h"
#include "Fat_API.h"
#include "malloc.h"
//#include "include.h"
FATFS *fs[2];  //Ϊ�߼��������빤����
//Ϊ�ļ�ϵͳ�����ڴ�
uint8_t exfuns_init(void)
{
	fs[0] =  (FATFS*)mymalloc(sizeof(FATFS)); //Ϊsd�������������ڴ�	
	fs[1] =  (FATFS*)mymalloc(sizeof(FATFS));//ΪFLASH�����������ڴ�	
	if(fs[0]&&fs[1])
		return 0;
	else
		return 1;
}
//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
uint8_t exf_getfree(uint8_t *drv,uint32_t *total,uint32_t *free)
{
	FATFS *fs1;
	uint8_t res;
    uint32_t fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res =(uint32_t)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//�õ���������
	    fre_sect=fre_clust*fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//��λΪKB
		*free=fre_sect>>1;	//��λΪKB 
 	}
	return res;
}	
//Ѱ���ļ�·������
void FilePath_process(char * source,char *addstring)
{
	uint8_t count = 0;
	while(*source != '\0')
	{
		source++;
		count++;
	}
	if(*addstring == '.' && *(addstring+1) == '.')
	{
		if(*(source-1) == ':' && *(source-2) == '0' && count == 2) return;
		while(*source != '/') 
		{
			*source = '\0';
			source--;
		}
		*source = '\0';
		source--;
	} 
	else
	{
		*source = '/';
		source++;
		while(*addstring != '\0') 
		{
			*source = *addstring;
			source++;
			addstring++;
		}
  }
}
void ClearPath(char *path)
{
	path += 2;
	while(*path)
	{
		*path = '\0';
		path++;
	}
}

