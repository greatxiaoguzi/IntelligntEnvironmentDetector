/*
���˵��������оƬ�ڲ�flash��д���ܣ����ڲ�������
�����ļ�;eeprom.c   stm32f10x_flash.c

*/

//��������ʵ������Ĵ����Լ��������ʵ��
//�����ջ�Ĵ���
#include "include.h"
extern const unsigned char WifiLogo[32];
//extern const unsigned char NoWifiLogo[32];
extern const unsigned char gImage_123[19494];
//�����ջHH
OS_STK START_TASK_STK[START_STK_SIZE];//��ʼ�����ջ
/*
//�õ�SD����״̬
void GetSDCapcity(void)
{
	BYTE res;
	uint32_t total=0,free=0;
	uint32_t Process;
	BACK_COLOR = BLACK;
	res = exf_getfree("0",&total,&free);  //�õ�SD����������С
	if(res)
		Show_Str(1,10,100,20,"��SD������",16,0,RED,LIGHTBLUE);
	else
	{
		total = total>>10;
		free = free>>10;
		Show_Str(200,10,200,16,"SD��������:     MB",16,0,BLUE,LIGHTBLUE);
		LCD_ShowNum(290,10,total,5,16,BLACK,LIGHTBLUE);
		Show_Str(350,10,200,16,"SD����������:     MB",16,0,BLUE,LIGHTBLUE);
		LCD_ShowNum(355,10,free,5,16,BLACK,LIGHTBLUE);
		
		LCD_DrawRectangle(0,10, 180, 30,GRAY);  //������
		LCD_Fill(1,11,179,29,LGRAY);
		Process = (uint32_t)(total-free)*248/total;
		LCD_Fill(1 ,11 ,1+Process+1,29,DARKBLUE);
	}
	delay_ms(2000);
	LCD_Clear(BLACK);
}
*/
//%SSID=AP_02C1E0&%01004566&#
//%USERKEY=af1c24b429134d6cbcb1c255f021277a&#
//%SVN=101.37.32.173&%9960&#
void InitHardware(void)
{
	SensorOnline.Status = 0x00;  		//�տ�ʼû���豸������״̬λȫ��Ϊ��
	LED_Init();
	Beep_init(7999);
	Micro_Wave_ReacT_Init();
	PMS5003S_Init();
	HCHOSetPasty(HCHO_FORWARDLY);   	//���ü�ȩ������Ϊ����ʽ��������ģʽ
	EncoderInit();                  	//������ʼ��
	Senseair8_Init(0XFFFF,72-1);  		//CO2��������ʼ������1MHzƵ�ʼ���,������ֵΪ65535,���ʱ��Ϊ��0.000001*65535=0.065S > ����������Ƶ������Ϊ0.001S
	RX8025t_Init();
	AT24CXX_Init();							//�ֿ�flash��ʼ��
    if(BMP1820Init())                    	 	//��ѹ��������ʼ��
	{
		SensorOnline.Device.BMP180 = 0;
	}
	if(SHT30_Init)  		//��ʪ�ȴ�������ʼ��
	{
		SensorOnline.Device.SHT30 = 1;
	}
	else
		SensorOnline.Device.SHT30 = 0;
	LCD_Fill(0,0,480,320,PARA_SHOW_INTERFACE_BACKCOLOR);
	SetBeepFreq(AlarmFreqTab[8]);
	delay_ms(400);
	TIM_Cmd(TIM3,DISABLE);
	LD3320_Init();
}

//������
int main(void)
{
//	uint32_t Adcval;
    sys_clock_init(9);   		             	//ϵͳʱ�ӳ�ʼ��
	SCB->VTOR = FLASH_BASE | 0xE000;  			//* APPP������ַ��APP������ʼ����ַΪ��0x8010000 */	
    delay_init();      		                	//��ʱ��ʼ��   
	NVIC_Configuration();                    	//�����жϷ���
	StartMachineInit();							//����������Ƴ�ʼ��
    uart_init(115200,9600,9600);     		  	//����1,3��4��ʼ����ʼ��()	
    TFTLCD_Init();      						//������ʼ��  
    LED_Init();  								//LED��ʼ��
	mem_init();									//�ڴ�س�ʼ��	
	exfuns_init();								//�ļ�ϵͳ��ʼ��
	f_mount(fs[0],"0:",1); 						//����SD�� 
	f_mount(fs[1],"1:",1); 						//����FLASH.    
	Adc_Init(); 								//ADCת����ʼ��
//    ADC_DMAInit(DMA1_Channel1,(u32)&ADC1->DR,(u32)ADC_ChangeValue,4);  //DMA�����ʼ��
	if(Font_Init())							//�ֿ��ʼ������Ƿ�OK
		LCD_ShowString(100,100,100,16,16,"Font Init Fail!",RED,BLACK);
	if(SD_Initialize())							//SD����ʼ��
		LCD_ShowString(100,120,100,16,16,"SD Init Fail!",RED,BLACK);
	InitHardware();   							//��ʼ��������Ӳ�����ּ��洢������
	ReadSetPara();    							//�������ò���*/ 
//	while(1)
//	{
////		Adcval = Get_Adc_Average(ChalTab[1],5);
//		Adcval = bmp180Convert();
//		LCD_ShowNum(100,100,Adcval,6,24,0,BLUE,BLACK);
//		delay_ms(400);
//		LED0 =! LED0;
//	}
    //�������ϵͳ����������
    OSInit();            						//UCOS��ʼ��
    OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);     //������ʼ����
    OSStart();           						//��ʼ����
}

//end of file....




