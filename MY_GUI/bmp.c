#include "bmp.h"
#include "Fat_API.h"

//u8 File_Buffer[1024];
FIL f_file;

//С�ߴ��bmp����,����filename���BMP�ļ�		
//filename:����·�����ļ���
//x,y,width,height:������С	 
//Mode: ��ʾ�Ƿ���ʾ����Ļ���м�
//����ֵ:0,�ɹ�;
//    ����,������.
u8 BmpDecode(u8 *filename,u16 x,u16 y,u16 width,u16 height,u8 Mode)//�ߴ�С��240*320��bmpͼƬ����.
{
	OS_CPU_SR  cpu_sr;
  u16 br;
	u8  color_byte;
	u16 tx,ty,color;	 
	//tx,ty��ʵ������	
	u8 res;
	u16 i,j;
// 	u8 *databuf;    		//���ݶ�ȡ��ŵ�ַ
 	u16 readlen=1024;	  //һ�δ�SD����ȡ���ֽ�������,����С��240*3!!!

	u8 *bmpbuf;			  	//���ݽ����ַ
	u8 biCompression=0;	//��¼ѹ����ʽ
	
	u16 rowcnt;				//һ�ζ�ȡ������
	u16 rowlen;	  		//ˮƽ�����ֽ���  
	u16 rowpix=0;			//ˮƽ����������	  
	u8 rowadd;				//ÿ������ֽ���
  _pic_info picinfo;	
	BITMAPINFO *pbmp;   	//��ʱָ��		 
	//�õ�����ߴ�
	picinfo.S_Height=height;
	picinfo.S_Width=width;
	
  OS_ENTER_CRITICAL(); 
	res=f_open(&f_file,(const TCHAR*)filename,FA_READ);//���ļ�	
  OS_EXIT_CRITICAL();	
	
	if(res==0)//�򿪳ɹ�.
	{ 
		OS_ENTER_CRITICAL();
		f_read(&f_file,File_Buffer,sizeof(BITMAPINFO),(UINT*)&br);//����BITMAPINFO��Ϣ 
		OS_EXIT_CRITICAL();
		
		pbmp=(BITMAPINFO*)File_Buffer;					//�õ�BMP��ͷ����Ϣ   
		color_byte=pbmp->bmiHeader.biBitCount/8;	//��ɫλ 16/24/32  
		biCompression=pbmp->bmiHeader.biCompression;//ѹ����ʽ
		picinfo.ImgHeight=pbmp->bmiHeader.biHeight;	//�õ�ͼƬ�߶�
		picinfo.ImgWidth=pbmp->bmiHeader.biWidth;  	//�õ�ͼƬ���   
		if(Mode) {
			x = (320 - picinfo.ImgWidth) /2;
			y = (240 - picinfo.ImgHeight) /2;
		}
		//ˮƽ���ر�����4�ı���!!
		if((picinfo.ImgWidth*color_byte)%4)rowlen=((picinfo.ImgWidth*color_byte)/4+1)*4;
		else rowlen=picinfo.ImgWidth*color_byte;
		rowadd=rowlen-picinfo.ImgWidth*color_byte;	//ÿ������ֽ���
 		//��ʼ����BMP   
		color=0;//��ɫ���	 													 
		tx=0 ;
		ty=picinfo.ImgHeight-1;
		if(picinfo.ImgWidth<=picinfo.S_Width&&picinfo.ImgHeight<=picinfo.S_Height)
		{  							   
			rowcnt=readlen/rowlen;						//һ�ζ�ȡ������
			readlen=rowcnt*rowlen;						//һ�ζ�ȡ���ֽ���
			rowpix=picinfo.ImgWidth;					//ˮƽ���������ǿ�� 
			
			OS_ENTER_CRITICAL();
			f_lseek(&f_file,pbmp->bmfHeader.bfOffBits);	//ƫ�Ƶ�������ʼλ�� 		
			OS_EXIT_CRITICAL();
			
			while(1)
			{	 
				OS_ENTER_CRITICAL(); 
				res=f_read(&f_file,File_Buffer,readlen,(UINT *)&br);	//����readlen���ֽ�
				OS_EXIT_CRITICAL();
				
				bmpbuf=File_Buffer;									//�����׵�ַ  
				if(br!=readlen)rowcnt=br/rowlen;				//���ʣ�µ�����
				if(color_byte==3)  			//24λBMPͼƬ
				{
					for(j=0;j<rowcnt;j++)	//ÿ�ζ���������
					{
						for(i=0;i<rowpix;i++)//дһ������
						{
							color=(*bmpbuf++)>>3;		   		 	//B
							color+=((u16)(*bmpbuf++)<<3)&0X07E0;	//G
							color+=(((u16)*bmpbuf++)<<8)&0XF800;	//R
 						 	LCD_Fast_DrawPoint(x+tx,y+ty,color);//��ʾͼƬ	
							tx++;
						}
						bmpbuf+=rowadd;//���������
						tx=0;
						ty--;
					}
				}else if(color_byte==2)//16λBMPͼƬ
				{
					for(j=0;j<rowcnt;j++)//ÿ�ζ���������
					{
						if(biCompression==BI_RGB)//RGB:5,5,5
						{
							for(i=0;i<rowpix;i++)
							{
								color=((u16)*bmpbuf&0X1F);			//R
								color+=(((u16)*bmpbuf++)&0XE0)<<1; 	//G
		 						color+=((u16)*bmpbuf++)<<9;  	    //R,G	 
							    LCD_Fast_DrawPoint(x+tx,y+ty,color);//��ʾͼƬ	
								tx++;
							}
						}else  //RGB 565
						{
							for(i=0;i<rowpix;i++)
							{											 
								color=*bmpbuf++;  			//G,B
		 						color+=((u16)*bmpbuf++)<<8;	//R,G	 
							  	LCD_Fast_DrawPoint(x+tx,y+ty,color);//��ʾͼƬ	
								tx++;
							}
						}
						bmpbuf+=rowadd;//���������
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
		f_close(&f_file);//�ر��ļ�
		OS_EXIT_CRITICAL();      
	}else res=1;//ͼƬ�ߴ����	
	
	return res;
}


