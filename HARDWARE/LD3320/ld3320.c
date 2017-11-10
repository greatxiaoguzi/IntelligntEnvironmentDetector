#include "include.h"

uint8_t SpeechReactionFlag = 0;     //声音感应标志

uint8_t  nLD_Mode = LD_MODE_IDLE;		//用来记录当前是在进行ASR识别还是在播放MP3
uint8_t ucRegVal; 						//有无识别结果			
extern uint8_t nAsrStatus;					//表示一次识别流程结束后有无结果
static FIL F_mp3data;
uint8_t ucHighInt;
uint8_t ucLowInt;
uint8_t bMp3Play = 0;							//	用来记录播放MP3的状态
uint32_t nMp3Size = 0;
uint32_t nMp3Pos = 0;
//uint8_t STR_USER_XX[ITEM_USER_COUNT][50];  //存放词条
//uint8_t MP3FilePath[ITEM_USER_COUNT][50];  //存放文件路径
uint8_t nAsrRes;
uint8_t nAsrStatus = 0;
uint8_t MP3Flag = 0;
extern OS_EVENT * sem_asr_finish;
uint8_t LD3320InitFlag = 0;
/*
//加载用户自定义词条和mp3文件路径到内存
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
	if(res == 0)  	//打开成功
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
		if(f_gets((TCHAR*)databuf,200,F_MP3data) != NULL)  //读取第一行
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
//LD3320是否正常测试
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
//关闭LD3320中断使能
void DisableLD3320NVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级11
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
}
//LD3320初始化
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
	//CS    	片选
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
	GPIO_SetBits(GPIOB,GPIO_Pin_6);  				//片选取消
	//LD_IRQ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			  
	GPIO_Init(GPIOC,&GPIO_InitStructure); 			//中断	
	//LD_WR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;		 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			  
	GPIO_Init(GPIOA,&GPIO_InitStructure); 			//中断
	//LD_IRQ 
	EXTI_ClearITPendingBit(EXTI_Line2);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);	   
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级11
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;					//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
	Ld3320Test();   //LD3320是否正常测试
//	if(!LoadSpeechInfo())
//		;//PlayDemoSound_mp3("0:/MP3FILE/SYSFILE/niyehao.mp3",2);
	nAsrStatus = LD_ASR_NONE;
}
void EXTI2_IRQHandler(void)
{
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
OSIntEnter();
#endif
	if(EXTI_GetITStatus(EXTI_Line2) != RESET)
	{
		ProcessInt0(); 
		EXTI_ClearFlag(EXTI_Line2);
		EXTI_ClearITPendingBit(EXTI_Line2); 
	}
#ifdef OS_TICKS_PER_SEC
OSIntExit();  					//退出中断保护				 		 
#endif
}

//语音识别任务
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
			if(RunASR() == 0)															//启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算
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
					LoadPowerOffUI(1);  //正常关机操
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
//芯片复位
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
//字节写
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
//字节读
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
//播放和语音识别中断
void ProcessInt0(void)
{	
	uint8_t err;
	uint8_t nAsrResCount = 0;	
	ucRegVal = LD_Read(0x2B);
	if(nLD_Mode == LD_MODE_ASR_RUN)		//当前进行语音识别
	{
		// 语音识别产生的中断
		// （有声音输入，不论识别成功或失败都有中断）
		LD_Write(0x29,0) ;					//中断允许 FIFO 中断允许 0表示不允许 
		LD_Write(0x02,0) ;					// FIFO中断允许	 FIFO_DATA FIFO_EXT中断   不允许
		if((ucRegVal & 0x10) &&		  //2b第四位为1 芯片内部FIFO中断发生 MP3播放时会产生中断标志请求外部MCU向FIFO_DATA中Reload数
			LD_Read(0xb2)==0x21 && 	  //读b2得到0x21表示闲可以进行下一步ASR动作
			LD_Read(0xbf)==0x35)		  //读到数值为0x35，可以确定是一次语音识别流程正常结束
		{
			nAsrResCount = LD_Read(0xba);	    //ASR：中断时，判断语音识别有几个识别候选
			if(nAsrResCount==1)
				LD_Read(0xc5);
			if(nAsrResCount>0 && nAsrResCount<=4) 	  //1 – 4: N个识别候选 0或者大于4：没有识别候选
			{
				nAsrStatus = LD_ASR_FOUNDOK;		 //表示一次识别流程结束后，有一个识别结果
				SpeechReactionFlag = 1;
			}
			else
		    {
				nAsrStatus = LD_ASR_FOUNDZERO;	   //表示一次识别流程结束后，没有识别结果
			}	
		}
		else
		{
			nAsrStatus = LD_ASR_FOUNDZERO;	   //表示一次识别流程结束后，没有识别结果
		}
		LD_Write(0x2b, 0);
    	LD_Write(0x1C,0);	  //ADC开关控制 写00H ADC不可用
		return;
	}
//=============================================================================================
	// 声音播放产生的中断，有三种：
	// A. 声音数据已全部播放完。
	// B. 声音数据已发送完毕。
	// C. 声音数据暂时将要用完，需要放入新的数据。	
	ucHighInt = LD_Read(0x29); 
	ucLowInt = LD_Read(0x02); 
	LD_Write(0x29,0);
	LD_Write(0x02,0);
	if(LD_Read(0xBA)&CAUSE_MP3_SONG_END)
	{
		// A. 声音数据已全部播放完。
		LD_Write(0x2B,  0);
		LD_Write(0xBA, 0);	
		LD_Write(0xBC,0x0);	
		bMp3Play=0;							// 声音数据全部播放完后，修改bMp3Play的变量
		LD_Write(0x08,1);

		LD_Write(0x08,0);
		LD_Write(0x33, 0);
		f_close(&F_mp3data);
		nLD_Mode = LD_MODE_ASR_RUN;
		nAsrStatus = LD_ASR_NONE;
		OSTaskResume(MainUI_TASK_PRIO);  	//回复任务
		return;
	}
	if(nMp3Pos >= nMp3Size)
	{
		// B. 声音数据已发送完毕。
		LD_Write(0xBC, 0x01);
		LD_Write(0x29, 0x10);
		f_close(&F_mp3data);
		nLD_Mode = LD_MODE_ASR_RUN;
		nAsrStatus = LD_ASR_NONE;
		OSTaskResume(MainUI_TASK_PRIO);		//回复任务
		return;
	}
	
	// C. 声音数据暂时将要用完，需要放入新的数据。	
	LD_ReloadMp3Data_Again();	
	LD_Write(0x29,ucHighInt); 
	LD_Write(0x02,ucLowInt) ;
}
/*  继续读取mp3文件数据到fifo,直到fifo满
 *	边写mp3文件数据到fifo时,LD3320会变解码播放
 *	当然写mp3文件数据到fifo的时间会短过声音的时间
 *	当声音快播放完毕的时候会进入ProcessInt0函数
 *	ProcessInt0函数又会调用此函数,所以声音得以连续
 */
void LD_ReloadMp3Data_Again(void)
{
	uint8_t val;
	uint8_t ucStatus;
	UINT br;
	
	ucStatus = LD_Read(0x06);
	//fifo是否满了
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

		//等待MP3播放完毕
		while(!(LD_Read(0xBA)&CAUSE_MP3_SONG_END))

		LD_Write(0x2B, 0);
      	LD_Write(0xBA, 0);	
		LD_Write(0xBC,0x0);	
		bMp3Play=0;					// 声音数据全部播放完后，修改bMp3Play的变量
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
//通用初始化
void LD_Init_Common(void)	
{
	bMp3Play = 0;

	LD_Read(0x06);  
	LD_Write(0x17, 0x35); //软复位LD3320
	delay_us(10);
	LD_Read(0x06);  

	LD_Write(0x89, 0x03);  //模拟电路控制 初始化时写03H
	delay_us(5);
	LD_Write(0xCF, 0x43);  //内部省电模式设置 初始化时写入43H 
	delay_us(5);
	LD_Write(0xCB, 0x02);
	
	/*PLL setting*/
	LD_Write(0x11, LD_PLL_11);   		//时钟频率设置1  
	if (nLD_Mode == LD_MODE_MP3)    //MP3播放
	{
		LD_Write(0x1E, 0x00); 					 	 //ADC专用控制，应初始化为00H
		LD_Write(0x19, LD_PLL_MP3_19);  	 //时钟频率设置2
		LD_Write(0x1B, LD_PLL_MP3_1B); 	   //时钟频率设置3
		LD_Write(0x1D, LD_PLL_MP3_1D);		 //时钟频率设置4
	}
	else						   									 //语音识别
	{
		LD_Write(0x1E,0x00);	 					   //ADC专用控制，应初始化为00H
		LD_Write(0x19, LD_PLL_ASR_19); 		 //时钟频率设置2
		LD_Write(0x1B, LD_PLL_ASR_1B);		 //时钟频率设置3	
	    LD_Write(0x1D, LD_PLL_ASR_1D);	 //时钟频率设置4
	}
	delay_us(10);
	
	LD_Write(0xCD, 0x04);    //DSP休眠设置 初始化时写入04H 允许DSP休眠
	LD_Write(0x17, 0x4C); 	 //写4CH可以使DSP休眠，比较省电
	delay_us(5);
	LD_Write(0xB9, 0x00);		 //ASR：当前添加识别句的字符串长度（拼音字符串）初始化时写入00H	 每添加一条识别句后要设定一次
	LD_Write(0xCF, 0x4F); 	 //内部省电模式设置 MP3初始化和ASR初始化时写入 4FH
	LD_Write(0x6F, 0xFF); 	 //对芯片进行初始化时设置为0xFF
}
//==================================================
//播放初始化
void LD_Init_MP3(void)	
{
	nLD_Mode = LD_MODE_MP3;	   //当前进行MP3播放
	LD_Init_Common();		  		 //通用初始化

	LD_Write(0xBD,0x02);	 		 	//内部增益控制 初始化时写入FFH
	LD_Write(0x17, 0x48);				//写48H可以激活DSP
	delay_us(10);

	LD_Write(0x85, 0x52); 	//内部反馈设置 初始化时写入52H
	LD_Write(0x8F, 0x00);  	//LineOut(线路输出)选择 初始化时写入00H
	LD_Write(0x81, 0x00);		//耳机左音量 设置为00H为最大音量
	LD_Write(0x83, 0x00);		//耳机右音量 设置为00H为最大音量
	LD_Write(0x8E, 0xff);		//喇叭输出音量  本寄存器设置为00H为最大音量	此处声音关闭
	LD_Write(0x8D, 0xff);		//内部增益控制 初始化时写入FFH
    delay_us(1);
	LD_Write(0x87, 0xff);	   //模拟电路控制 MP3播放初始化时写 FFH
	LD_Write(0x89, 0xff);    //模拟电路控制 MP3播放时写 FFH
	delay_us(1);
	LD_Write(0x22, 0x00);    //FIFO_DATA下限低8位
	LD_Write(0x23, 0x00);		 //FIFO_DATA下限高8位
	LD_Write(0x20, 0xef);    //FIFO_DATA上限低8位
	LD_Write(0x21, 0x07);		 //FIFO_DATA上限高8位
	LD_Write(0x24, 0x77);        
	LD_Write(0x25, 0x03);	
	LD_Write(0x26, 0xbb);    
	LD_Write(0x27, 0x01); 
}
//======================================================
//音量调整
void LD_AdjustMIX2SPVolume(uint8_t val)	  
{
//	ucSPVol = val;
	val = ((15-val)&0x0f) << 2;	
	LD_Write(0x8E, val | 0xc3); 
	LD_Write(0x87, 0x78); 
}
//======================================================
//填满fifo从而触发fifo中断,为播放mp3做准备
void fill_the_fifo(void)
{
	uint8_t ucStatus;
	int i = 0;
	ucStatus = LD_Read(0x06);
	//fifo是否满了
	while ( !(ucStatus&MASK_FIFO_STATUS_AFULL))
	{
		LD_Write(0x01,0xff);
		i++;
		ucStatus = LD_Read(0x06);
	}
}
//==================================================
//播放函数
void LD_play()	  
{
	nMp3Pos = 0;
	bMp3Play = 1;
	if (nMp3Pos >=  nMp3Size)	  //声音数据已发送完毕
		return ; 
	fill_the_fifo();
	
	LD_Write(0xBA, 0x00);	  //中断辅助信息，（读或设为00
	LD_Write(0x17, 0x48);	  //写48H可以激活DSP
	LD_Write(0x33, 0x01);	  //MP3播放用设置 开始播放时写入01H, 播放完写入00H
	LD_Write(0x29, 0x04);	  //中断允许 第2位：FIFO 中断允许 第4位：同步中断允许 1允许；0不允许
	
	LD_Write(0x02, 0x01); 	  //FIFO中断允许 第0位：允许FIFO_DATA中断； 第2位：允许FIFO_EXT中断；
	LD_Write(0x85, 0x5A);	  //内部反馈设置 初始化时写入52H 播放MP3时写入5AH (改变内部增益)

	//EX0=1;		//开外部中断0中断
	EXTI_ClearITPendingBit(EXTI_Line2);
}
//============================================================
//播放mp3 //音量
uint8_t PlayDemoSound_mp3(uint8_t *path,uint8_t Volume)
{	
	LD_Reset();	
	memset(&F_mp3data,0,sizeof(FIL));
	if(f_open(&F_mp3data, (const TCHAR*)path,FA_READ) == FR_OK)
	{
		bMp3Play = 1;
		nMp3Size = F_mp3data.fsize-1;
		LD_Init_MP3();									//将LD3320初始化为播放MP3模式
		LD_AdjustMIX2SPVolume(Volume);	//设置耳机音量  
		LD_play();	
		return 0;
	}
	else
		return 1;
}
//=============================================================
//取ASR识别结果
uint8_t LD_GetResult(void)
{
	uint8_t res;
	res = LD_Read(0xc5);
	return res;
}
//=============================================================
// Return 1: success.
//	添加识别关键词语，开发者可以学习"语音识别芯片LD3320高阶秘籍.pdf"中关于垃圾词语吸收错误的用法
uint8_t LD_AsrAddFixed(void)	  //添加关键词语到LD3320芯片中
{
	uint8_t k, flag;
	flag = 1;
	for (k=0; k<(ITEM_SYS_COUNT/*+ITEM_USER_COUNT*/); k++)
	{
		if(LD_Check_ASRBusyFlag_b2() == 0)	  //芯片内部出错
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
//语音识别初始化
void LD_Init_ASR(void)	   //语音识别初始化
{
	nLD_Mode=LD_MODE_ASR_RUN;	  //当前进行语音识别
	LD_Init_Common();	   //通用初始化

	LD_Write(0xBD, 0x00);	 //
	LD_Write(0x17, 0x48);	 //写48H可以激活DSP
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
//开始识别
//Return 1: success.
uint8_t LD_AsrRun(void)		
{
	LD_Write(0x35, MIC_VOL);	   //麦克风音量
//	LD_Write(0xB3, 0x0D);	// 用户阅读 开发手册 理解B3寄存器的调整对于灵敏度和识别距离的影响

	LD_Write(0x1C, 0x09);	//ADC开关控制 写09H Reserve保留命令字
	LD_Write(0xBD, 0x20);	//初始化控制寄存器 写入20H；Reserve保留命令字
	LD_Write(0x08, 0x01);	//清除FIFO内容第0位：写入1→清除FIFO_DATA 第2位：写入1→清除FIFO_EXT
	delay_us( 10 );
	LD_Write(0x08, 0x00);	//清除FIFO内容第0位（清除指定FIFO后再写入一次00H） 
	delay_us( 10 );

	if(LD_Check_ASRBusyFlag_b2() == 0)	   //芯片内部出错
	{
		return 0;
	}

	LD_Write(0xB2, 0xff);	  //ASR：DSP忙闲状态 0x21 表示闲，查询到为闲状态可以进行下一步 ??? 为什么不是read??
	LD_Write(0x37, 0x06);	  //识别控制命令下发寄存器 写06H 通知DSP开始识别语音 下发前，需要检查B2寄存器
	delay_us( 50 );
	LD_TEST();
	LD_Write(0x1C, 0x0b);	  // ADC开关控制  写0BH 麦克风输入ADC通道可用  
	LD_Write(0x29, 0x10);	  //中断允许 第2位：FIFO 中断允许 第4位：同步中断允许 1允许；0不允许
	
	LD_Write(0xBD, 0x00);	  //初始化控制寄存器 写入00 然后启动 为ASR模块 
	//EX0=1;		  //开外部中断0
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
//检测芯片内部有无出错
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
//索引
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

	LD_Write(0xc1, k );	   //ASR：识别字Index（00H—FFH）
	LD_Write(0xc3, 0 );	   //ASR：识别字添加时写入00
	LD_Write(0x08, 0x04);	 //清除FIFO内容第0位：写入1→清除FIFO_DATA 第2位：写入1→清除FIFO_EXT
	delay_us(1);
	LD_Write(0x08, 0x00);	 //清除FIFO内容第0位（清除指定FIFO后再写入一次00H）
	delay_us(1);
	for (nAsrAddLength=0; nAsrAddLength<50; nAsrAddLength++)
	{
		if (pRecogString[nAsrAddLength] == 0)
			break;
		LD_Write(0x5, pRecogString[nAsrAddLength]);	  //写FIFO_EXT数据口
	}
	
	LD_Write(0xb9, nAsrAddLength);	  //当前添加识别句的字符串长度初始化时写入00H 每添加一条识别句后要设定一次
	LD_Write(0xb2, 0xff);	  //DSP忙闲状态 0x21 表示闲 可以进行下一步	   ??
	LD_Write(0x37, 0x04);	  //语音识别控制命令下发寄存器 写04H：通知DSP要添加一项识别句
}
//==================================================
//启动一次识别流程
uint8_t RunASR(void)
{
	uint8_t i=0;
	uint8_t j=0;
	uint8_t asrflag=0;
	SPI3_SetSpeed(SPI_BaudRatePrescaler_64);
	for (i=0; i<5; i++)	//	防止由于硬件原因导致LD3320芯片工作不正常，所以一共尝试5次启动ASR识别流程
	{
		LD_Init_ASR();
		delay_us(500);
		if(LD_AsrAddFixed()==0)
		{
			LD_Reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			delay_us(200);			//	并从初始化开始重新ASR识别流程
			continue;
		}
		LD_TEST();
		delay_us(100);
		j= LD_AsrRun();
		if (j == 0)
		{
			LD_Reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			delay_us(200);			//	并从初始化开始重新ASR识别流程
			continue;
		}
		asrflag=1;
		break;						//	ASR流程启动成功，退出当前for循环。开始等待LD3320送出的中断信号
	}	
	SPI3_SetSpeed(SPI_BaudRatePrescaler_32);
	return asrflag;
}





