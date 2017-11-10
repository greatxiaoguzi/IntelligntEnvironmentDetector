#include "bmp.h"
#include "Fat_API.h"

//u8 File_Buffer[1024];
FIL f_file;

//小尺寸的bmp解码,解码filename这个BMP文件		
//filename:包含路径的文件名
//x,y,width,height:开窗大小	 
//Mode: 表示是否显示到屏幕的中间
//返回值:0,成功;
//    其他,错误码.
u8 BmpDecode(u8 *filename,u16 x,u16 y,u16 width,u16 height,u8 Mode)//尺寸小于240*320的bmp图片解码.
{
	OS_CPU_SR  cpu_sr;
  u16 br;
	u8  color_byte;
	u16 tx,ty,color;	 
	//tx,ty的实际坐标	
	u8 res;
	u16 i,j;
// 	u8 *databuf;    		//数据读取存放地址
 	u16 readlen=1024;	  //一次从SD卡读取的字节数长度,不能小于240*3!!!

	u8 *bmpbuf;			  	//数据解码地址
	u8 biCompression=0;	//记录压缩方式
	
	u16 rowcnt;				//一次读取的行数
	u16 rowlen;	  		//水平方向字节数  
	u16 rowpix=0;			//水平方向像素数	  
	u8 rowadd;				//每行填充字节数
  _pic_info picinfo;	
	BITMAPINFO *pbmp;   	//临时指针		 
	//得到窗体尺寸
	picinfo.S_Height=height;
	picinfo.S_Width=width;
	
  OS_ENTER_CRITICAL(); 
	res=f_open(&f_file,(const TCHAR*)filename,FA_READ);//打开文件	
  OS_EXIT_CRITICAL();	
	
	if(res==0)//打开成功.
	{ 
		OS_ENTER_CRITICAL();
		f_read(&f_file,File_Buffer,sizeof(BITMAPINFO),(UINT*)&br);//读出BITMAPINFO信息 
		OS_EXIT_CRITICAL();
		
		pbmp=(BITMAPINFO*)File_Buffer;					//得到BMP的头部信息   
		color_byte=pbmp->bmiHeader.biBitCount/8;	//彩色位 16/24/32  
		biCompression=pbmp->bmiHeader.biCompression;//压缩方式
		picinfo.ImgHeight=pbmp->bmiHeader.biHeight;	//得到图片高度
		picinfo.ImgWidth=pbmp->bmiHeader.biWidth;  	//得到图片宽度   
		if(Mode) {
			x = (320 - picinfo.ImgWidth) /2;
			y = (240 - picinfo.ImgHeight) /2;
		}
		//水平像素必须是4的倍数!!
		if((picinfo.ImgWidth*color_byte)%4)rowlen=((picinfo.ImgWidth*color_byte)/4+1)*4;
		else rowlen=picinfo.ImgWidth*color_byte;
		rowadd=rowlen-picinfo.ImgWidth*color_byte;	//每行填充字节数
 		//开始解码BMP   
		color=0;//颜色清空	 													 
		tx=0 ;
		ty=picinfo.ImgHeight-1;
		if(picinfo.ImgWidth<=picinfo.S_Width&&picinfo.ImgHeight<=picinfo.S_Height)
		{  							   
			rowcnt=readlen/rowlen;						//一次读取的行数
			readlen=rowcnt*rowlen;						//一次读取的字节数
			rowpix=picinfo.ImgWidth;					//水平像素数就是宽度 
			
			OS_ENTER_CRITICAL();
			f_lseek(&f_file,pbmp->bmfHeader.bfOffBits);	//偏移到数据起始位置 		
			OS_EXIT_CRITICAL();
			
			while(1)
			{	 
				OS_ENTER_CRITICAL(); 
				res=f_read(&f_file,File_Buffer,readlen,(UINT *)&br);	//读出readlen个字节
				OS_EXIT_CRITICAL();
				
				bmpbuf=File_Buffer;									//数据首地址  
				if(br!=readlen)rowcnt=br/rowlen;				//最后剩下的行数
				if(color_byte==3)  			//24位BMP图片
				{
					for(j=0;j<rowcnt;j++)	//每次读到的行数
					{
						for(i=0;i<rowpix;i++)//写一行像素
						{
							color=(*bmpbuf++)>>3;		   		 	//B
							color+=((u16)(*bmpbuf++)<<3)&0X07E0;	//G
							color+=(((u16)*bmpbuf++)<<8)&0XF800;	//R
 						 	LCD_Fast_DrawPoint(x+tx,y+ty,color);//显示图片	
							tx++;
						}
						bmpbuf+=rowadd;//跳过填充区
						tx=0;
						ty--;
					}
				}else if(color_byte==2)//16位BMP图片
				{
					for(j=0;j<rowcnt;j++)//每次读到的行数
					{
						if(biCompression==BI_RGB)//RGB:5,5,5
						{
							for(i=0;i<rowpix;i++)
							{
								color=((u16)*bmpbuf&0X1F);			//R
								color+=(((u16)*bmpbuf++)&0XE0)<<1; 	//G
		 						color+=((u16)*bmpbuf++)<<9;  	    //R,G	 
							    LCD_Fast_DrawPoint(x+tx,y+ty,color);//显示图片	
								tx++;
							}
						}else  //RGB 565
						{
							for(i=0;i<rowpix;i++)
							{											 
								color=*bmpbuf++;  			//G,B
		 						color+=((u16)*bmpbuf++)<<8;	//R,G	 
							  	LCD_Fast_DrawPoint(x+tx,y+ty,color);//显示图片	
								tx++;
							}
						}
						bmpbuf+=rowadd;//跳过填充区
						tx=0;
						ty--;
					}	
				} else {
					break;
				}
				if(br!=readlen||res)break;	 
			}	 
		}	
		OS_ENTER_CRITICAL();
		f_close(&f_file);//关闭文件
		OS_EXIT_CRITICAL();      
	}else res=1;//图片尺寸错误	
	
	return res;
}


