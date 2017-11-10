#include "sys.h"
#include "sd.h"
#include  "ff.h"
#include "ucos_ii.h"
#include "Fat_API.h"
#include "malloc.h"
//#include "include.h"
FATFS *fs[2];  //为逻辑磁盘申请工作区
//为文件系统申请内存
uint8_t exfuns_init(void)
{
	fs[0] =  (FATFS*)mymalloc(sizeof(FATFS)); //为sd卡工作区申请内存	
	fs[1] =  (FATFS*)mymalloc(sizeof(FATFS));//为FLASH工作区申请内存	
	if(fs[0]&&fs[1])
		return 0;
	else
		return 1;
}
//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
uint8_t exf_getfree(uint8_t *drv,uint32_t *total,uint32_t *free)
{
	FATFS *fs1;
	uint8_t res;
    uint32_t fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res =(uint32_t)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//单位为KB
		*free=fre_sect>>1;	//单位为KB 
 	}
	return res;
}	
//寻找文件路径过程
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

