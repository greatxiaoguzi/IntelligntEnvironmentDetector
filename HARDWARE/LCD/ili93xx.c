#include "include.h"
#include "font.h"
//ili9488Һ��������
//ʵ���˻��㺯����Һ�����ĳ�ʼ����
			 
//LCD�Ļ�����ɫ�ͱ���ɫ	   
//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;
	 
//д�Ĵ�������
//regval:�Ĵ���ֵ         0XE0
void LCD_WR_REG(uint8_t regval)
{ 	 
	LCD_CS_CLR; 
	LCD_RS_CLR;//д��ַ
	
	DATAOUT(regval);	
	//GPIOB->ODR = (regval<<8);
	
	LCD_WR_CLR; 
	LCD_WR_SET;
	
 	LCD_CS_SET;
}
//дLCD����
//data:Ҫд���ֵ
void LCD_WR_DATA(uint8_t data)
{				
	LCD_CS_CLR;	
	LCD_RS_SET;
	
	DATAOUT(data);
	//GPIOB->ODR = (data<<8);
	
	LCD_WR_CLR;
	LCD_WR_SET;
	
	LCD_CS_SET;  
}
//��LCD����
//����ֵ:������ֵ
uint8_t LCD_RD_DATA(void)
{										    	   
	uint8_t Temp;
 	GPIOB->CRH=0X88888888; //PB8-15  ��������
	GPIOB->ODR &= 0x0000;     //ȫ�����0

	LCD_RS_SET;
	LCD_CS_CLR;
	LCD_RD_CLR;
	Temp = DATAIN;  
	LCD_RD_SET;
	LCD_CS_SET; 

	GPIOB->CRH = 0X33333333; //PB0-7  �������
	GPIOB->ODR |= 0XFF00;    //ȫ�������
	return (uint8_t)(Temp>>8);   
}					   
//д�Ĵ���
//LCD_Reg:�Ĵ�����ַ
//LCD_RegValue:Ҫд�������
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	   
//���Ĵ���
//LCD_Reg:�Ĵ�����ַ
//����ֵ:����������
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{					
	uint16_t Temp;
	LCD_WR_REG(LCD_Reg);		//д��Ҫ���ļĴ������
	delay_ms(5);	
	Temp = LCD_RD_DATA()|(LCD_RD_DATA()>>8);
//	LCD_WR_REG(LCD_Reg);		//д��Ҫ���ļĴ������
//	delay_ms(5);
//	Temp |= (LCD_RD_DATA()>>8);
	return Temp;		//���ض�����ֵ
}   
//��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
  LCD_WR_REG(lcddev.wramcmd);
}	 
//LCDдGRAM
//RGB_Code:��ɫֵ
void LCD_WriteRAM(uint16_t RGB_Code)
{							    
	LCD_WR_DATA(RGB_Code>>8);//дʮ��λGRAM
	LCD_WR_DATA(RGB_Code&0xff);	
}
//��ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
//ͨ���ú���ת��
//c:GBR��ʽ����ɫֵ
//����ֵ��RGB��ʽ����ɫֵ
uint16_t LCD_BGR2RGB(uint16_t c)
{
	uint16_t  r,g,b,rgb;
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
} 
//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi
void opt_delay(uint8_t i)
{
	while(i--);
}
//��ȡ��ĳ�����ɫֵ
//x,y:����
//����ֵ:�˵����ɫ
uint16_t LCD_ReadPoint(uint16_t x,uint16_t y)
{
 	uint16_t r,g,b;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//�����˷�Χ,ֱ�ӷ���		   
	GPIOB->CRH=0X88888888; //PB8-15 ��������
	GPIOB->ODR=0XFFFF;     //ȫ�������

	LCD_RS_SET;
	LCD_CS_CLR;	    
	//��ȡ����(��GRAMʱ,��һ��Ϊ�ٶ�)	
	LCD_RD_CLR;	
  	delay_us(1);//��ʱ1us					   
	LCD_RD_SET;
 	//dummy READ
	LCD_RD_CLR;					   
	delay_us(1);//��ʱ1us					   
 	r=DATAIN;  	//ʵ��������ɫ
	LCD_RD_SET;
 
	LCD_RD_CLR;					   
	b=DATAIN;//��ȡ��ɫֵ  	  
	LCD_RD_SET;
	g=r&0XFF;//����9341,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ
	g<<=8;
 
	LCD_CS_SET;
	GPIOB->CRH=0X33333333; 		//PB8-15 �������
	//GPIOB->ODR=0XFFFF;    		//ȫ�������  
  return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510��Ҫ��ʽת��һ��
}			 
//LCD������ʾ
void LCD_DisplayOn(void)
{
	LCD_WriteReg(0X07,0x0173);		
}	 
//LCD�ر���ʾ
void LCD_DisplayOff(void)
{	   
	LCD_WriteReg(0X07,0x00);	
}    		
//LCD�Ƿ����˯��ģʽ
void LCD_EnterSleep(uint8_t orSleep)
{
	//uint16_t i=0;
 	//uint16_t j=0;
	
	if(orSleep) 
	{
		LCD_DisplayOff();
		LCD_WR_REG(0x28);
		delay_ms(10); 
		LCD_WR_REG(0x10);     
		delay_ms(120); 
	}
	else
	{
		LCD_DisplayOn();
		LCD_WR_REG(0x11);     
		delay_ms(120); 
		LCD_WR_REG(0x29);
	}
}
//���ù��λ��
//Xpos:������
//Ypos:������
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	    
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA((Xpos>>8)&0xff);
	LCD_WR_DATA(Xpos&0xff); 			 
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA((Ypos>>8)&0xff);
	LCD_WR_DATA(Ypos&0xff); 		
	
	LCD_WriteRAM_Prepare();
} 
//����LCD���Զ�ɨ�跽��
//ע��:�����������ܻ��ܵ��˺������õ�Ӱ��(������9341/6804����������),
//����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
//dir:0~7,����8������(���嶨���lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341/5310/5510��IC�Ѿ�ʵ�ʲ���	   	   
void LCD_Scan_Dir(uint8_t dir)
{
	uint16_t regval=0;
	uint16_t dirreg=0;
	uint16_t temp;  
	if(lcddev.dir==1&&lcddev.id!=0X6804)//����ʱ����6804���ı�ɨ�跽��
	{
		switch(dir)//����ת��
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	}
	switch(dir)
	{
		case L2R_U2D://������,���ϵ���
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://������,���µ���
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://���ҵ���,���ϵ���
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://���ҵ���,���µ���
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://���ϵ���,������
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://���ϵ���,���ҵ���
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://���µ���,������
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://���µ���,���ҵ���
			regval|=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
	dirreg=0X36;
	regval|=0X08;//5310/5510����ҪBGR      
	LCD_WriteReg(dirreg,regval);
	if((regval&0X20)||lcddev.dir==1)
	{
		if(lcddev.width<lcddev.height)//����X,Y
		{
			temp=lcddev.width;
			lcddev.width=lcddev.height;
			lcddev.height=temp;
		}
	}else  
	{
		if(lcddev.width>lcddev.height)//����X,Y
		{
			temp=lcddev.width;
			lcddev.width=lcddev.height;
			lcddev.height=temp;
		}
	}  
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(0);
	LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.width-1)>>8);
	LCD_WR_DATA((lcddev.width-1)&0XFF);
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(0);
	LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.height-1)>>8);
	LCD_WR_DATA((lcddev.height-1)&0XFF);  
}   
//��ȡҺ������ID��
uint32_t Read_LCD_ID(void)
{
	LCD_WR_REG(0xd3);

}
//���ٻ���
//x,y:����
//color:��ɫ
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{	   
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(x>>8); 	
	LCD_WR_DATA(x&0XFF); 
	LCD_WR_DATA(x>>8); 	
	LCD_WR_DATA(x&0XFF); 
	
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(y>>8); 	
	LCD_WR_DATA(y&0XFF);
	LCD_WR_DATA(y>>8); 	
	LCD_WR_DATA(y&0XFF); 	
	
	LCD_WR_REG(lcddev.wramcmd); 
	LCD_WR_DATA((color>>8)&0xff); 	
	LCD_WR_DATA(color&0xff);
}	 


//����LCD��ʾ����
//dir:0,������1,����
void LCD_Display_Dir(uint8_t dir)
{
	//����  				
	lcddev.dir=1;	//����
	lcddev.width=480;
	lcddev.height=320;
	
	lcddev.wramcmd=0X2C;
	lcddev.setxcmd=0X2A;
	lcddev.setycmd=0X2B;  	 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//Ĭ��ɨ�跽��
}
//Һ�����������ȵ��ڳ�ʼ��
void TFT_BK_Light(void)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;       //����Ƚ�
    TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;     //��ʱʱ��
    GPIO_InitTypeDef GPIO_InitStructure;
  
	uint16_t PrescalerValue = 0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;  //3-1=2;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 999;		//��ʱ��Ԥװ��ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;	//pwmʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//���ϼ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/* PWM1 Mode configuration: Channel */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;//��ʼռ�ձ�Ϊ0
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
//	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
//	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
//	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2,ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	//��ʼ��PWM�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PA1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	TIM2->CCR2 = 1;  //��ʼ�Ȳ�������,ռ�ձ�ԽС����ĻԽ��
}
//��ʼ��lcd
//�ó�ʼ���������Գ�ʼ������ILI93XXҺ��,�������������ǻ���ILI9320��!!!
//�������ͺŵ�����оƬ��û�в���! 
void TFTLCD_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //ʹ��PORTB,Cʱ�Ӻ�AFIOʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//����SWD��ʧ��JTAG
	TFT_BK_Light();
	//LCD_RD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//LCD_RS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//LCD_CS  LCD_WR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//LCD_DB
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|
								GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	//����PB11�Ǹ���
	GPIO_Init(GPIOB, &GPIO_InitStructure); //GPIOB
	GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|
						GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
 	delay_ms(50);  
	
	LCD_RS_SET;
	delay_ms(1);
	LCD_RS_CLR;
	delay_ms(10);
	LCD_RS_SET;
	delay_ms(120);
	
	delay_ms(50); // delay 50 ms 
 	LCD_WriteReg(0x0000,0x0001);
	delay_ms(50); // delay 50 ms 
	lcddev.id = LCD_ReadReg(0XD3);
	
	LCD_WR_REG(0xE0); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x0D); 
	LCD_WR_DATA(0x1B); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x3c); 
	LCD_WR_DATA(0x78); 
	LCD_WR_DATA(0x4A); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x0E); 
	LCD_WR_DATA(0x09); 
	LCD_WR_DATA(0x1B); 
	LCD_WR_DATA(0x1e); 
	LCD_WR_DATA(0x0f);  
	
	LCD_WR_REG(0xE1); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x22); 
	LCD_WR_DATA(0x24); 
	LCD_WR_DATA(0x06); 
	LCD_WR_DATA(0x12); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x36); 
	LCD_WR_DATA(0x47); 
	LCD_WR_DATA(0x47); 
	LCD_WR_DATA(0x06); 
	LCD_WR_DATA(0x0a); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x30); 
	LCD_WR_DATA(0x37); 
	LCD_WR_DATA(0x0f); 
	
	LCD_WR_REG(0xC0); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x10); 
	
	LCD_WR_REG(0xC1); 
	LCD_WR_DATA(0x41); 
	
	LCD_WR_REG(0xC5); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x22); 
	LCD_WR_DATA(0x80); 
	
	LCD_WR_REG(0x36); 
	LCD_WR_DATA(0x48); 
	
	LCD_WR_REG(0x3A); //Interface Mode Control
	LCD_WR_DATA(0x55);
		
	LCD_WR_REG(0XB0);  //Interface Mode Control  
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xB1);   //Frame rate 70HZ  
	LCD_WR_DATA(0xB0); 
	LCD_WR_DATA(0x11); 
	LCD_WR_REG(0xB4); 
	LCD_WR_DATA(0x02);   
	LCD_WR_REG(0xB6); //RGB/MCU Interface Control
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x02); 
	
	LCD_WR_REG(0xB7); 
	LCD_WR_DATA(0xC6); ;
	
	LCD_WR_REG(0xE9); 
	LCD_WR_DATA(0x00);
	
	LCD_WR_REG(0XF7);    
	LCD_WR_DATA(0xA9); 
	LCD_WR_DATA(0x51); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x82);
	
	LCD_WR_REG(0x11); 
	delay_ms(120); 
	LCD_WR_REG(0x29); 
	
	//LCD_WR_REG(0xd3);
	LCD_Display_Dir(1);
	//LCD_Clear(WHITE);
}
//��������
//color:Ҫ���������ɫ
void LCD_Clear(uint16_t color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//�õ��ܵ��� 	 			
	LCD_SetCursor(0x00,0x0000);		//���ù��λ��    	 
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		LCD_WR_DATA((color>>8)&0XFF);	
		LCD_WR_DATA(color&0XFF);
	}
}  
void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint8_t hsareg,heareg,vsareg,veareg;
	uint16_t hsaval,heaval,vsaval,veaval;

	hsaval=sy;				
	heaval=height;
	vsaval=lcddev.width-width-1;
	veaval=lcddev.width-sx-1;
	hsareg=0X50;heareg=0X51;//ˮƽ���򴰿ڼĴ���
	vsareg=0X52;veareg=0X53;//��ֱ���򴰿ڼĴ���	

	//���üĴ���ֵ
	LCD_WriteReg(hsareg,hsaval);
	LCD_WriteReg(heareg,heaval);
	LCD_WriteReg(vsareg,vsaval);
	LCD_WriteReg(veareg,veaval);		
	LCD_SetCursor(sx,sy);	//���ù��λ��
}
//��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ     0,150,200,250
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color)
{          
	uint16_t i,j;
	uint16_t xlen=0;
	uint16_t ylen = 0;
	LCD_WR_REG(0x2a);
	if(xsta>xend)
	{
		LCD_WR_DATA(xend>>8);
		LCD_WR_DATA(xend&0xff);
		LCD_WR_DATA(xsta>>8);
		LCD_WR_DATA(xsta&0xff);
	}
	else
	{
		LCD_WR_DATA(xsta>>8);
		LCD_WR_DATA(xsta&0xff);
		LCD_WR_DATA(xend>>8);
		LCD_WR_DATA(xend&0xff);
	}
	LCD_WR_REG(0x2b);
	if(ysta>yend)
	{
		LCD_WR_DATA(yend>>8);
		LCD_WR_DATA(yend&0xff);
		LCD_WR_DATA(ysta>>8);
		LCD_WR_DATA(ysta&0xff);
	}
	else
	{
		LCD_WR_DATA(ysta>>8);
		LCD_WR_DATA(ysta&0xff);
		LCD_WR_DATA(yend>>8);
		LCD_WR_DATA(yend&0xff);
	}
	LCD_WriteRAM_Prepare();
	xlen=abs(xend-xsta)+1;	
	ylen = abs(ysta-yend)+1;
	for(i=0;i<ylen;i++) 
	{
		for(j=0;j<xlen;j++)
		{
			LCD_WR_DATA((color>>8)&0XFF);	//���ù��λ�� 
			LCD_WR_DATA(color&0XFF);
		}
	}
}
//����
//x1,y1:�������
//x2,y2:�յ�����  ,
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_Fast_DrawPoint(uRow,uCol,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}

//������
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}
//����Բ�Ǿ���
void LCD_FillRoundFrame(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t r,uint16_t c)
{
   int16_t  x,y,xd;
   if ( x2 < x1 )
   {
      x = x2;
      x2 = x1;
      x1 = x;
   }
   if ( y2 < y1 )
   {
      y = y2;
      y2 = y1;
      y1 = y;
   }

   if ( r<=0 ) return;

   xd = 3 - (r << 1);
   x = 0;
   y = r;

   LCD_Fill(x1 + r, y1, x2 - r, y2, c);

   while ( x <= y )
   {
     if( y > 0 )
     {
        LCD_DrawLine(x2 + x - r, y1 - y + r, x2+ x - r, y + y2 - r, c);
        LCD_DrawLine(x1 - x + r, y1 - y + r, x1- x + r, y + y2 - r, c);
     }
     if( x > 0 )
     {
        LCD_DrawLine(x1 - y + r, y1 - x + r, x1 - y + r, x + y2 - r, c);
        LCD_DrawLine(x2 + y - r, y1 - x + r, x2 + y - r, x + y2 - r, c);
     }
     if ( xd < 0 )
     {
        xd += (x << 2) + 6;
     }
     else
     {
        xd += ((x - y) << 2) + 10;
        y--;
     }
     x++;
   }
}
//��ָ��λ�û�һ��ָ���Ƕȵ�Բ��
//(x,y):���ĵ�
//r    :�뾶
//Angle:�Ƕ�
void LCDDrawCir(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color,uint8_t xiangxian,uint8_t orFill)
{
	int i,j;
	int a;//�����޵ķָ���
	int b;//
	int di;
	a=0;b=r;
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		if(orFill)
		{
			switch(xiangxian)
			{
				case 0:
				{
					for(i=x0;i<x0+a;i++)
					{
						LCD_Fast_DrawPoint(i,y0-a,color);          //1   
						LCD_Fast_DrawPoint(i,y0-b,color);
					}
					for(i=x0+a;i<x0+b;i++)
					{
						LCD_Fast_DrawPoint(i,y0-a,color);       //2         
					}
					for(i=x0+a;i<x0+b;i++)
					{
						LCD_Fast_DrawPoint(i,y0+a,color);         //3
					}
					for(i=x0;i<x0+a;i++)
					{
						LCD_Fast_DrawPoint(i,y0+a,color);         //4
						LCD_Fast_DrawPoint(i,y0+b,color);
					}
					for(i=x0-a;i<x0;i++)
					{
						LCD_Fast_DrawPoint(i,y0+a,color);         //5
						LCD_Fast_DrawPoint(i,y0+b,color);
					}
					for(i=x0-b;i<=x0-a;i++)
					{
						LCD_Fast_DrawPoint(i,y0+a,color);         //6
					}
					for(i=x0-b;i<x0-a;i++)
					{
						LCD_Fast_DrawPoint(i,y0-a,color);         //7
					}
					for(i=x0-a;i<x0;i++)
					{
						LCD_Fast_DrawPoint(i,y0-a,color);      //8
						LCD_Fast_DrawPoint(i,y0-b,color);
					}
				}break;
				case 1:
				{
					for(i=x0;i<x0+a;i++)
					{
						LCD_Fast_DrawPoint(i,y0-a,color);          //1   
						LCD_Fast_DrawPoint(i,y0-b,color);
					}
					for(i=x0+a;i<x0+b;i++)
					{
						LCD_Fast_DrawPoint(i,y0-a,color);       //2         
					}
				}break;
				case 2:
				{
					for(i=x0+a;i<x0+b;i++)
					{
						LCD_Fast_DrawPoint(i,y0+a,color);         //3
					}
					for(i=x0;i<x0+a;i++)
					{
						LCD_Fast_DrawPoint(i,y0+a,color);         //4
						LCD_Fast_DrawPoint(i,y0+b,color);
					}
				}break;
				case 3:
				{
					for(i=x0-a;i<x0;i++)
					{
						LCD_Fast_DrawPoint(i,y0+a,color);         //5
						LCD_Fast_DrawPoint(i,y0+b,color);
					}
					for(i=x0-b;i<=x0-a;i++)
					{
						LCD_Fast_DrawPoint(i,y0+a,color);         //6
					}
				}break;
				case 4:
				{
					for(i=x0-b;i<x0-a;i++)
					{
						LCD_Fast_DrawPoint(i,y0-a,color);         //7
					}
					for(i=x0-a;i<x0;i++)
					{
						LCD_Fast_DrawPoint(i,y0-a,color);      //8
						LCD_Fast_DrawPoint(i,y0-b,color);
					}
				}break;
			}
		}
		else
		{
			switch(xiangxian)
			{
				case 1:   //��һ����
				LCD_Fast_DrawPoint(x0+a,y0-b,color);             //5
				LCD_Fast_DrawPoint(x0+b,y0-a,color);             //0 
				break;
				case 4:  //��������
				LCD_Fast_DrawPoint(x0+b,y0+a,color);             //4               
				LCD_Fast_DrawPoint(x0+a,y0+b,color);             //6 
				break;
				case 3:  //��������
				LCD_Fast_DrawPoint(x0-a,y0+b,color);             //1       
				LCD_Fast_DrawPoint(x0-b,y0+a,color);             //3
				break;
				case 2:  //�ڶ�����
				LCD_Fast_DrawPoint(x0-a,y0-b,color);             //2             
				LCD_Fast_DrawPoint(x0-b,y0-a,color);             //7
				break;
				case 0:
				LCD_Fast_DrawPoint(x0+a,y0-b,color);             //5
				LCD_Fast_DrawPoint(x0+b,y0-a,color);
				LCD_Fast_DrawPoint(x0+b,y0+a,color);             //4               
				LCD_Fast_DrawPoint(x0+a,y0+b,color);
				LCD_Fast_DrawPoint(x0-a,y0+b,color);             //1       
				LCD_Fast_DrawPoint(x0-b,y0+a,color);
				LCD_Fast_DrawPoint(x0-a,y0-b,color);             //2             
				LCD_Fast_DrawPoint(x0-b,y0-a,color);  
				break;
				default :break;
			}
		}
		a++;
		//ʹ��Bresenham�㷨��Բ
		if(di<0)
			di += 4*a+6;
		else
		{
			di+=10+4*(a-b);
			b--;
		} 						    
	}
}
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16/24
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode,uint16_t point_color,uint16_t backcolor)
{  							  
  uint8_t temp,t1,t;
	uint16_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
	//���ô���
	num=num-' ';//�õ�ƫ�ƺ��ֵ
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp = asc2_1206[num][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[num][t];	//����1608����
		else if(size==24)temp=asc2_2412[num][t];	//����2412����
		else if(size==36)temp=asc2_3636[num][t];
		else if(size==60)
		{
			if(num)
				temp=asc2_6060[num-13][t];
			else 
				temp=asc2_6060[num][t];}//���ﴦ���˷�ֹɾȥ��һЩ�ַ��ĸ���
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
				LCD_Fast_DrawPoint(x,y,point_color);
			else if(mode==0)
				LCD_Fast_DrawPoint(x,y,backcolor);
			temp<<=1;
			y++;
			if(x>=lcddev.width)return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//��������
				break;
			}
		}  	 
	}  	
}

//m^n����
//����ֵ:m^n�η�.
u32 LCD_Pow(uint8_t m,uint8_t n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}
uint8_t GetValueBit(uint32_t Value)
{
	uint8_t Bit;
	if((Value/10000)!=0)
		Bit = 5;
	else if((Value/1000)!=0)
		Bit = 4;
	else if((Value/100)!=0)
		Bit = 3;
	else if((Value/10)!=0)
		Bit = 2;
	else 
		Bit = 1;
	return Bit;
}
//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
void LCD_ShowNum(uint16_t x,uint16_t y,u32 num,uint8_t len,uint8_t size,uint8_t Mode,uint16_t Point_Color,uint16_t Back_Color)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;		
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(Mode)
					LCD_ShowChar(x+(size/2)*t,y,' ',size,0,Point_Color,Back_Color);
				else
					LCD_ShowChar(x+(size/2)*t,y,'0',size,0,Point_Color,Back_Color);
				continue;
			}
			else 
				enshow=1; 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0,Point_Color,Back_Color); 
	}
} 
//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~999999999);	 
//len:����(��Ҫ��ʾ��λ��)
//size:�����С
//mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
/*void LCD_ShowxNum(uint16_t x,uint16_t y,u32 num,uint8_t len,uint8_t size,uint8_t mode)
{
	uint8_t t,temp;
	uint8_t enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)
					LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01,RED,BACK_COLOR);  
				else
					LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01,RED,BACK_COLOR);  
 				continue;
			}
			else
				enshow=1; 		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01,RED,BACK_COLOR); 
	}
}*/
//��ʾ�ַ���
//x,y:�������
//width,height:�����С
//size:�����С
//*p:�ַ�����ʼ��ַ
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p,uint16_t point_color,uint16_t backcolor)
{
	uint8_t x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width)
		{
			x=x0;
			y+=size;
		}
        if(y>=height)
			break;//�˳�
        LCD_ShowChar(x,y,*p,size,0,point_color,backcolor);
        x+=size/2;
        p++;
    }
}
//��ʾһ��ϵͳ��ʼ������,һ����ʮ����
void LCDDispStart(uint16_t x0,uint16_t y0,uint8_t Numpoint)
{
	uint8_t i;
	for(i=0;i<Numpoint;i++)
	{
		LCDDrawCir(x0+i*10,y0,2,LGRAY,0,1);
		delay_ms(150);
	}
}

//OLED��ʾһ����ͼ��ָ��λ��
//������������꣬��ʾ�����С��λͼ����ָ�룬��ʾģʽ(�Ƿ����)
void Show_DrawBMP(uint16_t x,uint16_t y,uint16_t BmpXSize,uint16_t BmpYSize,uint8_t *buf,uint16_t Colour,uint16_t BakColor)
{
	uint16_t temp,t,t1;
	uint16_t y0=y;		
	uint16_t Area_Size = BmpXSize*BmpYSize/8;  //�ܵ��ֽڴ�С
    for(t=0;t<Area_Size;t++)
    {
		temp=buf[t];  //�õ�ͼƬ��һ���ֽ�����	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
				LCD_Fast_DrawPoint(x,y,Colour);	
			else 
				LCD_Fast_DrawPoint(x,y,BakColor);	   
			temp<<=1;
			y++;
			if((y-y0)==BmpYSize)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }  
}


























