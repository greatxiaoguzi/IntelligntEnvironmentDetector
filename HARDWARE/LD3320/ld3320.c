#include "include.h"

uint8_t SpeechReactionFlag = 0;     //������Ӧ��־

uint8_t  nLD_Mode = LD_MODE_IDLE;		//������¼��ǰ���ڽ���ASRʶ�����ڲ���MP3
uint8_t ucRegVal; 						//����ʶ����			
extern uint8_t nAsrStatus;					//��ʾһ��ʶ�����̽��������޽��
static FIL F_mp3data;
uint8_t ucHighInt;
uint8_t ucLowInt;
uint8_t bMp3Play = 0;							//	������¼����MP3��״̬
uint32_t nMp3Size = 0;
uint32_t nMp3Pos = 0;
//uint8_t STR_USER_XX[ITEM_USER_COUNT][50];  //��Ŵ���
//uint8_t MP3FilePath[ITEM_USER_COUNT][50];  //����ļ�·��
uint8_t nAsrRes;
uint8_t nAsrStatus = 0;
uint8_t MP3Flag = 0;
extern OS_EVENT * sem_asr_finish;
uint8_t LD3320InitFlag = 0;
/*
//�����û��Զ��������mp3�ļ�·�����ڴ�
uint8_t LoadSpeechInfo(void)
{
	UINT br;
	FIL *F_MP3data;
	uint8_t *databuf;
	uint8_t res;
	uint8_t  *pDivide1=0;
	uint8_t  *pDivide2=0;
	uint8_t StrLen;
	databuf = (uint8_t *)mymalloc(sizeof(200)); 
	if(databuf==NULL)
		return 1;
	F_MP3data = (FIL *)mymalloc(sizeof(FIL));
	if(F_MP3data==NULL)
	{
		myfree(databuf);
		return 1;
	}
	res=f_open(F_MP3data,(const TCHAR*)"0:/MP3FILE/USERFILE/configfile.ini",FA_READ);
	if(res == 0)  	//�򿪳ɹ�
	{
		if(f_eof(F_MP3data) == 1)
		{
			f_close(F_MP3data);
			myfree(F_MP3data);
			myfree(databuf);
			return 1;
		}
	}
	else
	{
		f_close(F_MP3data);
		myfree(F_MP3data);
		myfree(databuf);
		return 1;
	}
	for(uint8_t i=0;i<ITEM_USER_COUNT;i++)
	{
		if(f_gets((TCHAR*)databuf,200,F_MP3data) != NULL)  //��ȡ��һ��
		{
			StrLen = strstr((const char*)databuf,(const char*)"&%") - (strstr((const char*)databuf,(const char*)"%KEY")+5);
			pDivide1 = strstr((const char*)databuf,(const char*)"%KEY") + 5;
			pDivide2 = strstr((const char*)databuf,(const char*)"&%") + 2;
			memcpy(STR_USER_XX[i],pDivide1,StrLen);
			strcpy((char*)MP3FilePath[i],"0:/MP3FILE/USERFILE/");
			strcat((char*)MP3FilePath[i],(char*)pDivide2);
		}
		else
		{
			f_close(F_MP3data);
			myfree(F_MP3data);
			myfree(databuf);
			return 1;
		}
	}
	f_close(F_MP3data);
	myfree(F_MP3data);
	myfree(databuf);
	return 0;
}*/
//LD3320�Ƿ���������
void Ld3320Test(void)
{
	uint8_t text = 0;
	LD_Reset();
	text = LD_Read(0x06);
	LD_Write(0x35,0x33);
	LD_Write(0x1b,0x55);
	LD_Write(0xb3,0xaa);
	text = LD_Read(0x35);
	text = LD_Read(0x1b);
	text = LD_Read(0xb3);
	LD_Reset();
	text = LD_Read(0x06);
	text = LD_Read(0x06);
	text = LD_Read(0x35);
	text = LD_Read(0xb3);
	LD_Reset();
}
//�ر�LD3320�ж�ʹ��
void DisableLD3320NVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�11
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
}
//LD3320��ʼ��
void LD3320_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	/*SPI3 Configuration */ 
	SPI3_Init();
	//RST
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);			   
	//CS    	Ƭѡ
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
	GPIO_SetBits(GPIOB,GPIO_Pin_6);  				//Ƭѡȡ��
	//LD_IRQ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			  
	GPIO_Init(GPIOC,&GPIO_InitStructure); 			//�ж�	
	//LD_WR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;		 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			  
	GPIO_Init(GPIOA,&GPIO_InitStructure); 			//�ж�
	//LD_IRQ 
	EXTI_ClearITPendingBit(EXTI_Line2);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);	   
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�11
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
	Ld3320Test();   //LD3320�Ƿ���������
//	if(!LoadSpeechInfo())
//		;//PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/niyehao.mp3",2);
	nAsrStatus = LD_ASR_NONE;
}
void EXTI2_IRQHandler(void)
{
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
OSIntEnter();
#endif
	if(EXTI_GetITStatus(EXTI_Line2) != RESET)
	{
		ProcessInt0(); 
		EXTI_ClearFlag(EXTI_Line2);
		EXTI_ClearITPendingBit(EXTI_Line2); 
	}
#ifdef OS_TICKS_PER_SEC
OSIntExit();  					//�˳��жϱ���				 		 
#endif
}

//����ʶ������
void SpeechRecgnizeTask(void)
{
	if(bMp3Play != 0)
	{
		return;
	}
	if(SysParaSetInfo.SpeechRecognize.Speech_Sw == 1)
	{
		if(AlarmCloseFlag != 0)
		{
			if((SysTimeData.Second%20)==0 && Esp8266InitFinishFlag==1)
			{
				SpeechReactionFlag = 1;
				switch(AlarmCloseFlag)
				{
					case 1:
					{
						PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/jiaquanalarm.mp3",SysParaSetInfo.SpeechRecognize.Speech_Volume);
						
					}break;
					case 2:
					{
						PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/pm2_5alarm.mp3",SysParaSetInfo.SpeechRecognize.Speech_Volume);
					}break;
					case 3:
					{
						PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/co2alarm.mp3",SysParaSetInfo.SpeechRecognize.Speech_Volume);
					}break;
					case 4:
					{
						PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/tempalarm.mp3",SysParaSetInfo.SpeechRecognize.Speech_Volume);
					}break;
					case 5:
					{
						PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/shidualarm.mp3",SysParaSetInfo.SpeechRecognize.Speech_Volume);
					}break;
					case 6:
					{
						PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/dianliangdi.mp3",SysParaSetInfo.SpeechRecognize.Speech_Volume);
					}break;
					default:break;
				}
				nAsrStatus = LD_ASR_NONE;
				OSTaskSuspend(MainUI_TASK_PRIO);
				return;
			}
		}
	}
	switch(nAsrStatus)
	{
		case LD_ASR_RUNING:break;
		case LD_ASR_ERROR:break;
		case LD_ASR_NONE:
		{
			nAsrStatus = LD_ASR_RUNING;
			if(RunASR() == 0)															//����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����
			{
				nAsrStatus = LD_ASR_ERROR;
			}
		}break;
		case LD_ASR_FOUNDOK:
		{
			nAsrRes = LD_GetResult();
			switch(nAsrRes)
			{
				case 0:PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/gabeng.mp3",SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
				case 1:PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/nijiaoshenme.mp3",SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
				case 2:PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/daiji.mp3",SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
				case 3:
					PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/guanji.mp3",SysParaSetInfo.SpeechRecognize.Speech_Volume);
					LoadPowerOffUI(1);  //�����ػ���
					SetBeepFreq(AlarmFreqTab[6]);
					Close_FDN304P();
					break;
				case 4:PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/nihaoa.mp3",SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
				case 5:break;
				case 6:break;
				case 7:break;
				case 8:break;
				case 9:break;
				case 10:break;
				case 11:break;
				case 12:break;
				case 13:break;
				case 14:break;
				case 15:break;
				case 16:break;
				case 17:break;
				case 18:break;
				case 19:break;
				case 20:break;
				case 21:break;
				case 22:break;
				case 23:break;
				case 24:break;
				case 25:break;
//				case 26:PlayDemoSound_mp3((char*)MP3FilePath[0],SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
//				case 27:PlayDemoSound_mp3((char*)MP3FilePath[1],SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
//				case 28:PlayDemoSound_mp3((char*)MP3FilePath[2],SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
//				case 29:PlayDemoSound_mp3((char*)MP3FilePath[3],SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
//				case 30:PlayDemoSound_mp3((char*)MP3FilePath[4],SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
//				case 31:PlayDemoSound_mp3((char*)MP3FilePath[5],SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
//				case 32:PlayDemoSound_mp3((char*)MP3FilePath[6],SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
//				case 33:PlayDemoSound_mp3((char*)MP3FilePath[7],SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
//				case 34:PlayDemoSound_mp3((char*)MP3FilePath[8],SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
//				case 35:PlayDemoSound_mp3((char*)MP3FilePath[9],SysParaSetInfo.SpeechRecognize.Speech_Volume);break;
				default:break;
			}
			nAsrStatus = LD_ASR_NONE;
			OSTaskSuspend(MainUI_TASK_PRIO);
		}break;
		case LD_ASR_FOUNDZERO:
		default:nAsrStatus = LD_ASR_NONE;break;
	}
}
//=======================================
//оƬ��λ
void LD_Reset(void)	  
{
	LD_RST = 1;
	delay_us(100);
	LD_RST = 0;
	delay_us(100);
	LD_RST = 1;
	delay_us(10);
	LD_RST = 0;
	delay_us(100);
	LD_RST = 1;
	delay_us(100);
}      
//==========================================================
//�ֽ�д
void LD_Write(uint8_t Address,uint8_t Data)
{
	LD_CS = 0;
	LD_WR = 0;
	SPI3_ReadWriteByte(0x04);
	SPI3_ReadWriteByte(Address);
	SPI3_ReadWriteByte(Data);
	LD_CS = 1;
}
//========================================================
//�ֽڶ�
uint8_t LD_Read(uint8_t Address)
{
	uint8_t i;
	LD_CS = 0;
	LD_WR = 0;
	SPI3_ReadWriteByte(0x05);
	SPI3_ReadWriteByte(Address);
	i=SPI3_ReadWriteByte(0);	
	LD_CS = 1; 
	return(i);
}
//==============================================================================================
//���ź�����ʶ���ж�
void ProcessInt0(void)
{	
	uint8_t err;
	uint8_t nAsrResCount = 0;	
	ucRegVal = LD_Read(0x2B);
	if(nLD_Mode == LD_MODE_ASR_RUN)		//��ǰ��������ʶ��
	{
		// ����ʶ��������ж�
		// �����������룬����ʶ��ɹ���ʧ�ܶ����жϣ�
		LD_Write(0x29,0) ;					//�ж����� FIFO �ж����� 0��ʾ������ 
		LD_Write(0x02,0) ;					// FIFO�ж�����	 FIFO_DATA FIFO_EXT�ж�   ������
		if((ucRegVal & 0x10) &&		  //2b����λΪ1 оƬ�ڲ�FIFO�жϷ��� MP3����ʱ������жϱ�־�����ⲿMCU��FIFO_DATA��Reload��
			LD_Read(0xb2)==0x21 && 	  //��b2�õ�0x21��ʾ�п��Խ�����һ��ASR����
			LD_Read(0xbf)==0x35)		  //������ֵΪ0x35������ȷ����һ������ʶ��������������
		{
			nAsrResCount = LD_Read(0xba);	    //ASR���ж�ʱ���ж�����ʶ���м���ʶ���ѡ
			if(nAsrResCount==1)
				LD_Read(0xc5);
			if(nAsrResCount>0 && nAsrResCount<=4) 	  //1 �C 4: N��ʶ���ѡ 0���ߴ���4��û��ʶ���ѡ
			{
				nAsrStatus = LD_ASR_FOUNDOK;		 //��ʾһ��ʶ�����̽�������һ��ʶ����
				SpeechReactionFlag = 1;
			}
			else
		    {
				nAsrStatus = LD_ASR_FOUNDZERO;	   //��ʾһ��ʶ�����̽�����û��ʶ����
			}	
		}
		else
		{
			nAsrStatus = LD_ASR_FOUNDZERO;	   //��ʾһ��ʶ�����̽�����û��ʶ����
		}
		LD_Write(0x2b, 0);
    	LD_Write(0x1C,0);	  //ADC���ؿ��� д00H ADC������
		return;
	}
//=============================================================================================
	// �������Ų������жϣ������֣�
	// A. ����������ȫ�������ꡣ
	// B. ���������ѷ�����ϡ�
	// C. ����������ʱ��Ҫ���꣬��Ҫ�����µ����ݡ�	
	ucHighInt = LD_Read(0x29); 
	ucLowInt = LD_Read(0x02); 
	LD_Write(0x29,0);
	LD_Write(0x02,0);
	if(LD_Read(0xBA)&CAUSE_MP3_SONG_END)
	{
		// A. ����������ȫ�������ꡣ
		LD_Write(0x2B,  0);
		LD_Write(0xBA, 0);	
		LD_Write(0xBC,0x0);	
		bMp3Play=0;							// ��������ȫ����������޸�bMp3Play�ı���
		LD_Write(0x08,1);

		LD_Write(0x08,0);
		LD_Write(0x33, 0);
		f_close(&F_mp3data);
		nLD_Mode = LD_MODE_ASR_RUN;
		nAsrStatus = LD_ASR_NONE;
		OSTaskResume(MainUI_TASK_PRIO);  	//�ظ�����
		return;
	}
	if(nMp3Pos >= nMp3Size)
	{
		// B. ���������ѷ�����ϡ�
		LD_Write(0xBC, 0x01);
		LD_Write(0x29, 0x10);
		f_close(&F_mp3data);
		nLD_Mode = LD_MODE_ASR_RUN;
		nAsrStatus = LD_ASR_NONE;
		OSTaskResume(MainUI_TASK_PRIO);		//�ظ�����
		return;
	}
	
	// C. ����������ʱ��Ҫ���꣬��Ҫ�����µ����ݡ�	
	LD_ReloadMp3Data_Again();	
	LD_Write(0x29,ucHighInt); 
	LD_Write(0x02,ucLowInt) ;
}
/*  ������ȡmp3�ļ����ݵ�fifo,ֱ��fifo��
 *	��дmp3�ļ����ݵ�fifoʱ,LD3320�����벥��
 *	��Ȼдmp3�ļ����ݵ�fifo��ʱ���̹�������ʱ��
 *	�������첥����ϵ�ʱ������ProcessInt0����
 *	ProcessInt0�����ֻ���ô˺���,����������������
 */
void LD_ReloadMp3Data_Again(void)
{
	uint8_t val;
	uint8_t ucStatus;
	UINT br;
	
	ucStatus = LD_Read(0x06);
	//fifo�Ƿ�����
	while(!(ucStatus&MASK_FIFO_STATUS_AFULL) && nMp3Pos<=nMp3Size)
	{
		nMp3Pos ++;
		f_read(&F_mp3data,&val,1,&br);
		LD_Write(0x01,val);
		ucStatus = LD_Read(0x06);
	}
	if(nMp3Pos>=nMp3Size)
	{
	    LD_Write(0xBC, 0x01);
		LD_Write(0x29, 0x10);

		//�ȴ�MP3�������
		while(!(LD_Read(0xBA)&CAUSE_MP3_SONG_END))

		LD_Write(0x2B, 0);
      	LD_Write(0xBA, 0);	
		LD_Write(0xBC,0x0);	
		bMp3Play=0;					// ��������ȫ����������޸�bMp3Play�ı���
		LD_Write(0x08,1);

		LD_Write(0x08,0);
		LD_Write(0x33, 0);

		f_close(&F_mp3data);
		nLD_Mode = LD_MODE_ASR_RUN;
		nAsrStatus = LD_ASR_NONE;
		OSTaskResume(MainUI_TASK_PRIO);
	}
}
//=========================================================
//ͨ�ó�ʼ��
void LD_Init_Common(void)	
{
	bMp3Play = 0;

	LD_Read(0x06);  
	LD_Write(0x17, 0x35); //��λLD3320
	delay_us(10);
	LD_Read(0x06);  

	LD_Write(0x89, 0x03);  //ģ���·���� ��ʼ��ʱд03H
	delay_us(5);
	LD_Write(0xCF, 0x43);  //�ڲ�ʡ��ģʽ���� ��ʼ��ʱд��43H 
	delay_us(5);
	LD_Write(0xCB, 0x02);
	
	/*PLL setting*/
	LD_Write(0x11, LD_PLL_11);   		//ʱ��Ƶ������1  
	if (nLD_Mode == LD_MODE_MP3)    //MP3����
	{
		LD_Write(0x1E, 0x00); 					 	 //ADCר�ÿ��ƣ�Ӧ��ʼ��Ϊ00H
		LD_Write(0x19, LD_PLL_MP3_19);  	 //ʱ��Ƶ������2
		LD_Write(0x1B, LD_PLL_MP3_1B); 	   //ʱ��Ƶ������3
		LD_Write(0x1D, LD_PLL_MP3_1D);		 //ʱ��Ƶ������4
	}
	else						   									 //����ʶ��
	{
		LD_Write(0x1E,0x00);	 					   //ADCר�ÿ��ƣ�Ӧ��ʼ��Ϊ00H
		LD_Write(0x19, LD_PLL_ASR_19); 		 //ʱ��Ƶ������2
		LD_Write(0x1B, LD_PLL_ASR_1B);		 //ʱ��Ƶ������3	
	    LD_Write(0x1D, LD_PLL_ASR_1D);	 //ʱ��Ƶ������4
	}
	delay_us(10);
	
	LD_Write(0xCD, 0x04);    //DSP�������� ��ʼ��ʱд��04H ����DSP����
	LD_Write(0x17, 0x4C); 	 //д4CH����ʹDSP���ߣ��Ƚ�ʡ��
	delay_us(5);
	LD_Write(0xB9, 0x00);		 //ASR����ǰ���ʶ�����ַ������ȣ�ƴ���ַ�������ʼ��ʱд��00H	 ÿ���һ��ʶ����Ҫ�趨һ��
	LD_Write(0xCF, 0x4F); 	 //�ڲ�ʡ��ģʽ���� MP3��ʼ����ASR��ʼ��ʱд�� 4FH
	LD_Write(0x6F, 0xFF); 	 //��оƬ���г�ʼ��ʱ����Ϊ0xFF
}
//==================================================
//���ų�ʼ��
void LD_Init_MP3(void)	
{
	nLD_Mode = LD_MODE_MP3;	   //��ǰ����MP3����
	LD_Init_Common();		  		 //ͨ�ó�ʼ��

	LD_Write(0xBD,0x02);	 		 	//�ڲ�������� ��ʼ��ʱд��FFH
	LD_Write(0x17, 0x48);				//д48H���Լ���DSP
	delay_us(10);

	LD_Write(0x85, 0x52); 	//�ڲ��������� ��ʼ��ʱд��52H
	LD_Write(0x8F, 0x00);  	//LineOut(��·���)ѡ�� ��ʼ��ʱд��00H
	LD_Write(0x81, 0x00);		//���������� ����Ϊ00HΪ�������
	LD_Write(0x83, 0x00);		//���������� ����Ϊ00HΪ�������
	LD_Write(0x8E, 0xff);		//�����������  ���Ĵ�������Ϊ00HΪ�������	�˴������ر�
	LD_Write(0x8D, 0xff);		//�ڲ�������� ��ʼ��ʱд��FFH
    delay_us(1);
	LD_Write(0x87, 0xff);	   //ģ���·���� MP3���ų�ʼ��ʱд FFH
	LD_Write(0x89, 0xff);    //ģ���·���� MP3����ʱд FFH
	delay_us(1);
	LD_Write(0x22, 0x00);    //FIFO_DATA���޵�8λ
	LD_Write(0x23, 0x00);		 //FIFO_DATA���޸�8λ
	LD_Write(0x20, 0xef);    //FIFO_DATA���޵�8λ
	LD_Write(0x21, 0x07);		 //FIFO_DATA���޸�8λ
	LD_Write(0x24, 0x77);        
	LD_Write(0x25, 0x03);	
	LD_Write(0x26, 0xbb);    
	LD_Write(0x27, 0x01); 
}
//======================================================
//��������
void LD_AdjustMIX2SPVolume(uint8_t val)	  
{
//	ucSPVol = val;
	val = ((15-val)&0x0f) << 2;	
	LD_Write(0x8E, val | 0xc3); 
	LD_Write(0x87, 0x78); 
}
//======================================================
//����fifo�Ӷ�����fifo�ж�,Ϊ����mp3��׼��
void fill_the_fifo(void)
{
	uint8_t ucStatus;
	int i = 0;
	ucStatus = LD_Read(0x06);
	//fifo�Ƿ�����
	while ( !(ucStatus&MASK_FIFO_STATUS_AFULL))
	{
		LD_Write(0x01,0xff);
		i++;
		ucStatus = LD_Read(0x06);
	}
}
//==================================================
//���ź���
void LD_play()	  
{
	nMp3Pos = 0;
	bMp3Play = 1;
	if (nMp3Pos >=  nMp3Size)	  //���������ѷ������
		return ; 
	fill_the_fifo();
	
	LD_Write(0xBA, 0x00);	  //�жϸ�����Ϣ����������Ϊ00
	LD_Write(0x17, 0x48);	  //д48H���Լ���DSP
	LD_Write(0x33, 0x01);	  //MP3���������� ��ʼ����ʱд��01H, ������д��00H
	LD_Write(0x29, 0x04);	  //�ж����� ��2λ��FIFO �ж����� ��4λ��ͬ���ж����� 1����0������
	
	LD_Write(0x02, 0x01); 	  //FIFO�ж����� ��0λ������FIFO_DATA�жϣ� ��2λ������FIFO_EXT�жϣ�
	LD_Write(0x85, 0x5A);	  //�ڲ��������� ��ʼ��ʱд��52H ����MP3ʱд��5AH (�ı��ڲ�����)

	//EX0=1;		//���ⲿ�ж�0�ж�
	EXTI_ClearITPendingBit(EXTI_Line2);
}
//============================================================
//����mp3 //����
uint8_t PlayDemoSound_mp3(uint8_t *path,uint8_t Volume)
{	
	LD_Reset();	
	memset(&F_mp3data,0,sizeof(FIL));
	if(f_open(&F_mp3data, (const TCHAR*)path,FA_READ) == FR_OK)
	{
		bMp3Play = 1;
		nMp3Size = F_mp3data.fsize-1;
		LD_Init_MP3();									//��LD3320��ʼ��Ϊ����MP3ģʽ
		LD_AdjustMIX2SPVolume(Volume);	//���ö�������  
		LD_play();	
		return 0;
	}
	else
		return 1;
}
//=============================================================
//ȡASRʶ����
uint8_t LD_GetResult(void)
{
	uint8_t res;
	res = LD_Read(0xc5);
	return res;
}
//=============================================================
// Return 1: success.
//	���ʶ��ؼ���������߿���ѧϰ"����ʶ��оƬLD3320�߽��ؼ�.pdf"�й��������������մ�����÷�
uint8_t LD_AsrAddFixed(void)	  //��ӹؼ����ﵽLD3320оƬ��
{
	uint8_t k, flag;
	flag = 1;
	for (k=0; k<(ITEM_SYS_COUNT/*+ITEM_USER_COUNT*/); k++)
	{
		if(LD_Check_ASRBusyFlag_b2() == 0)	  //оƬ�ڲ�����
		{
			flag = 0;
			break;
		}
		LD_AsrAddFixed_ByIndex(k);
		delay_us(50);
	}
	return flag;
}
//==========================================================
//����ʶ���ʼ��
void LD_Init_ASR(void)	   //����ʶ���ʼ��
{
	nLD_Mode=LD_MODE_ASR_RUN;	  //��ǰ��������ʶ��
	LD_Init_Common();	   //ͨ�ó�ʼ��

	LD_Write(0xBD, 0x00);	 //
	LD_Write(0x17, 0x48);	 //д48H���Լ���DSP
	delay_us(100);

	LD_Write(0x3C, 0x80);     
	LD_Write(0x3E, 0x07);
	LD_Write(0x38, 0xff);    
	LD_Write(0x3A, 0x07);
	delay_us(10);
	LD_Write(0x40, 0);          
	LD_Write(0x42, 8);
	LD_Write(0x44, 0);    
	LD_Write(0x46, 8); 
	delay_us(10);
}
//==========================================================
//��ʼʶ��
//Return 1: success.
uint8_t LD_AsrRun(void)		
{
	LD_Write(0x35, MIC_VOL);	   //��˷�����
//	LD_Write(0xB3, 0x0D);	// �û��Ķ� �����ֲ� ���B3�Ĵ����ĵ������������Ⱥ�ʶ������Ӱ��

	LD_Write(0x1C, 0x09);	//ADC���ؿ��� д09H Reserve����������
	LD_Write(0xBD, 0x20);	//��ʼ�����ƼĴ��� д��20H��Reserve����������
	LD_Write(0x08, 0x01);	//���FIFO���ݵ�0λ��д��1�����FIFO_DATA ��2λ��д��1�����FIFO_EXT
	delay_us( 10 );
	LD_Write(0x08, 0x00);	//���FIFO���ݵ�0λ�����ָ��FIFO����д��һ��00H�� 
	delay_us( 10 );

	if(LD_Check_ASRBusyFlag_b2() == 0)	   //оƬ�ڲ�����
	{
		return 0;
	}

	LD_Write(0xB2, 0xff);	  //ASR��DSPæ��״̬ 0x21 ��ʾ�У���ѯ��Ϊ��״̬���Խ�����һ�� ??? Ϊʲô����read??
	LD_Write(0x37, 0x06);	  //ʶ����������·��Ĵ��� д06H ֪ͨDSP��ʼʶ������ �·�ǰ����Ҫ���B2�Ĵ���
	delay_us( 50 );
	LD_TEST();
	LD_Write(0x1C, 0x0b);	  // ADC���ؿ���  д0BH ��˷�����ADCͨ������  
	LD_Write(0x29, 0x10);	  //�ж����� ��2λ��FIFO �ж����� ��4λ��ͬ���ж����� 1����0������
	
	LD_Write(0xBD, 0x00);	  //��ʼ�����ƼĴ��� д��00 Ȼ������ ΪASRģ�� 
	//EX0=1;		  //���ⲿ�ж�0
	EXTI_ClearITPendingBit(EXTI_Line2);
	return 1;
}
void LD_TEST(void)
{
	// bw1(0x35, 0x22);
	// bw1(0x1b, 0x44);
	// bw1(0xb3, 0x66);
	LD_Read(0xBF);
	LD_Read(0x06);
	LD_Read(0x35);
	LD_Read(0xb3);
}

//================================================================
//���оƬ�ڲ����޳���
//Return 1: success.
uint8_t LD_Check_ASRBusyFlag_b2(void)	  
{
	uint8_t j;
	uint8_t flag = 0;
	for (j=0; j<10; j++)
	{
		if (LD_Read(0xb2) == 0x21)
		{
			flag = 1;
			break;
		}
		delay_us(10);		
	}
	return flag;
}
//================================================================
//����
void LD_AsrAddFixed_ByIndex(uint8_t nIndex)	 
{
	switch(nIndex)
	{
		case  0: LD_AsrAddFixed_ByString(STR_SYS_00,nIndex); break;
		case  1: LD_AsrAddFixed_ByString(STR_SYS_01,nIndex); break;
		case  2: LD_AsrAddFixed_ByString(STR_SYS_02,nIndex); break;
		case  3: LD_AsrAddFixed_ByString(STR_SYS_03,nIndex); break;
		case  4: LD_AsrAddFixed_ByString(STR_SYS_04,nIndex); break;
		case  5: LD_AsrAddFixed_ByString(STR_SYS_05,nIndex); break;
		case  6: LD_AsrAddFixed_ByString(STR_SYS_06,nIndex); break;
		case  7: LD_AsrAddFixed_ByString(STR_SYS_07,nIndex); break;
		case  8: LD_AsrAddFixed_ByString(STR_SYS_08,nIndex); break;
		case  9: LD_AsrAddFixed_ByString(STR_SYS_09,nIndex); break;
		case 10: LD_AsrAddFixed_ByString(STR_SYS_10,nIndex); break;
		case 11: LD_AsrAddFixed_ByString(STR_SYS_11,nIndex); break;
		case 12: LD_AsrAddFixed_ByString(STR_SYS_12,nIndex); break;
		case 13: LD_AsrAddFixed_ByString(STR_SYS_13,nIndex); break;
		case 14: LD_AsrAddFixed_ByString(STR_SYS_14,nIndex); break;
		case 15: LD_AsrAddFixed_ByString(STR_SYS_15,nIndex); break;
		case 16: LD_AsrAddFixed_ByString(STR_SYS_16,nIndex); break;
		case 17: LD_AsrAddFixed_ByString(STR_SYS_17,nIndex); break;
		case 18: LD_AsrAddFixed_ByString(STR_SYS_18,nIndex); break;
		case 19: LD_AsrAddFixed_ByString(STR_SYS_19,nIndex); break;
		case 20: LD_AsrAddFixed_ByString(STR_SYS_20,nIndex); break;
		case 21: LD_AsrAddFixed_ByString(STR_SYS_21,nIndex); break;
		case 22: LD_AsrAddFixed_ByString(STR_SYS_22,nIndex); break;
		case 23: LD_AsrAddFixed_ByString(STR_SYS_23,nIndex); break;
		case 24: LD_AsrAddFixed_ByString(STR_SYS_24,nIndex); break;
		case 25: LD_AsrAddFixed_ByString(STR_SYS_25,nIndex); break;
//		case 26: LD_AsrAddFixed_ByString(STR_USER_XX[0],nIndex); break;
//		case 27: LD_AsrAddFixed_ByString(STR_USER_XX[1],nIndex); break;
//		case 28: LD_AsrAddFixed_ByString(STR_USER_XX[2],nIndex); break;
//		case 29: LD_AsrAddFixed_ByString(STR_USER_XX[3],nIndex); break;
//		case 30: LD_AsrAddFixed_ByString(STR_USER_XX[4],nIndex); break;
//		case 31: LD_AsrAddFixed_ByString(STR_USER_XX[5],nIndex); break;
//		case 32: LD_AsrAddFixed_ByString(STR_USER_XX[6],nIndex); break;
//		case 33: LD_AsrAddFixed_ByString(STR_USER_XX[7],nIndex); break;
//		case 34: LD_AsrAddFixed_ByString(STR_USER_XX[8],nIndex); break;
//		case 35: LD_AsrAddFixed_ByString(STR_USER_XX[9],nIndex); break;
		default:break;
	}
}
void LD_AsrAddFixed_ByString(char * pRecogString, uint8_t k)
{
	uint8_t nAsrAddLength;
	if (*pRecogString==0)
		return;

	LD_Write(0xc1, k );	   //ASR��ʶ����Index��00H��FFH��
	LD_Write(0xc3, 0 );	   //ASR��ʶ�������ʱд��00
	LD_Write(0x08, 0x04);	 //���FIFO���ݵ�0λ��д��1�����FIFO_DATA ��2λ��д��1�����FIFO_EXT
	delay_us(1);
	LD_Write(0x08, 0x00);	 //���FIFO���ݵ�0λ�����ָ��FIFO����д��һ��00H��
	delay_us(1);
	for (nAsrAddLength=0; nAsrAddLength<50; nAsrAddLength++)
	{
		if (pRecogString[nAsrAddLength] == 0)
			break;
		LD_Write(0x5, pRecogString[nAsrAddLength]);	  //дFIFO_EXT���ݿ�
	}
	
	LD_Write(0xb9, nAsrAddLength);	  //��ǰ���ʶ�����ַ������ȳ�ʼ��ʱд��00H ÿ���һ��ʶ����Ҫ�趨һ��
	LD_Write(0xb2, 0xff);	  //DSPæ��״̬ 0x21 ��ʾ�� ���Խ�����һ��	   ??
	LD_Write(0x37, 0x04);	  //����ʶ����������·��Ĵ��� д04H��֪ͨDSPҪ���һ��ʶ���
}
//==================================================
//����һ��ʶ������
uint8_t RunASR(void)
{
	uint8_t i=0;
	uint8_t j=0;
	uint8_t asrflag=0;
	SPI3_SetSpeed(SPI_BaudRatePrescaler_64);
	for (i=0; i<5; i++)	//	��ֹ����Ӳ��ԭ����LD3320оƬ����������������һ������5������ASRʶ������
	{
		LD_Init_ASR();
		delay_us(500);
		if(LD_AsrAddFixed()==0)
		{
			LD_Reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			delay_us(200);			//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}
		LD_TEST();
		delay_us(100);
		j= LD_AsrRun();
		if (j == 0)
		{
			LD_Reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			delay_us(200);			//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}
		asrflag=1;
		break;						//	ASR���������ɹ����˳���ǰforѭ������ʼ�ȴ�LD3320�ͳ����ж��ź�
	}	
	SPI3_SetSpeed(SPI_BaudRatePrescaler_32);
	return asrflag;
}





