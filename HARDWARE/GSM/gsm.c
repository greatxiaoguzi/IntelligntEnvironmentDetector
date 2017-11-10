#ifndef GSM__C
#define GSM__C
#include"include.h"	
//GSMģ���ʼ������
//���֣����ö��Ÿ�ʽ������
u8 MsgFlag=0;
extern  GUI_CONST_STORAGE GUI_FONT GUI_FontHZ12;
extern  GUI_CONST_STORAGE GUI_FONT GUI_FontHZ16;
extern  GUI_CONST_STORAGE GUI_FONT GUI_FontHZ24;
void GSMInit(void)
{
	  GUI_SetFont(&GUI_FontHZ16);	
	  GUI_DispStringAt("GSM����������...", 70,60);
	  printf("AT\r\n");
	  Hand("OK",255);
	  if(!Hand("OK",255))		
		{
		  GUI_DispStringAt("GSM��ʼ��ʧ��!", 70,80);
			beep(1,2);
		}
		else
		{
				ClearBuf();	
				GUI_DispStringAt("GSM��ʼ���ɹ�",70,80);
				
				printf("AT+CNMI=2,1\r\n");                     //���ö��ŵ���ʱ����
			  Hand("OK",200);
				GUI_DispStringAt("GSM��Ϣ���ѳɹ�", 70,100);
				ClearBuf();
				delay_ms(2000);
				
				printf("AT+CMGF=1\r\n");                     //���ö����ı�ģʽ
			  Hand("OK",200);
				GUI_DispStringAt("GSM�����ı����óɹ�",70,120);
				ClearBuf();
				delay_ms(2000);
				
				printf("AT+CMGD=1\r\n");                     //ɾ����һ������
			  Hand("OK",200);
				GUI_DispStringAt("����ɾ�����óɹ�", 70,140);
				ClearBuf();
				delay_ms(2000);	
	  }
}
//��ȡ����
void 	RecMesageDeal(void)
{
    if(strstr(Uart2InfBuf,"+CMTI")!=NULL)    //�Ƿ��ж�������
		{
			  printf("AT+CMGR=1\r\n");               // ���Ͷ�ȡָ��
			  delay_ms(1000);		
        Hand("OK",200);			
				if(strstr(Uart2InfBuf,"SOpenJDQ1")!=NULL)
				   JDQ1=0;
				else if(strstr((const char*)Uart2InfBuf,"SClosJDQ1")!=NULL)
				   JDQ1=1;
				else if(strstr((const char*)Uart2InfBuf,"SOpenJDQ2")!=NULL)
				   JDQ2=0;
				else if(strstr((const char*)Uart2InfBuf,"SClosJDQ2")!=NULL)
				   JDQ2=1;
				else if(strstr((const char*)Uart2InfBuf,"SOpenJDQ3")!=NULL)
				   JDQ3=0;
				else if(strstr((const char*)Uart2InfBuf,"SClosJDQ3")!=NULL)
				   JDQ3=1;
				else if(strstr((const char*)Uart2InfBuf,"SOpenJDQ4")!=NULL)
				   JDQ4=0;
				else if(strstr((const char*)Uart2InfBuf,"SClosJDQ4")!=NULL)
				   JDQ4=1;
				else if(strstr((const char*)Uart2InfBuf,"SGetMsg")!=NULL)
				   MsgFlag=1; 
			 ClearBuf();                                                    
			 printf("AT+CMGD=1\r\n");                    // ���Ͷ�ȡָ��
			 delay_ms(2000);
				Hand("OK",200);
			 ClearBuf();
		}
}
void SendMessage(void)
{
	  char D1[3],D2[3],D3[3],D4[3];	  
	  D1[0]=User_data[0]/100+0X30;
	  D1[1]=User_data[0]/10%10+0X30;
	  D1[2]=User_data[0]%10+0X30;
	  D2[0]=User_data[1]/100+0X30;
	  D2[1]=User_data[1]/10%10+0X30;
	  D2[2]=User_data[1]%10+0X30;
	  D3[0]=User_data[2]/100+0X30;
	  D3[1]=User_data[2]/10%10+0X30;
	  D3[2]=User_data[2]%10+0X30;
    D4[0]=User_data[3]/100+0X30;
	  D4[1]=User_data[3]/10%10+0X30;
	  D4[2]=User_data[3]%10+0X30;
	  ClearBuf();                           //������ڻ���
    printf("AT+CMGS=\"%s\"\r\n",PhoneNumber); //Ҫ���͵ĵ绰���� 
	  delay_ms(1000); 
	  Hand(">",200);
	  printf("Envrmnt Status\r\n");
	  printf("Temp1:%s'C\r\n",D1);
	  printf("Temp2:%s'C\r\n",D2);
	  printf("Fire:%s\r\n",D3);
	  printf("Damp:%s%%\r\n",D4);
		printf("JDQ Status\r\n");
		if((GPIOC->ODR&0X01)==0x00)
			printf("JDQ1 Opened!\r\n");
		else
			printf("JDQ1 Close!\r\n");
		if((GPIOC->ODR&0X02)==0x00)
			printf("JDQ2 Opened!\r\n");
		else
			printf("JDQ2 Close!\r\n");
	  if((GPIOC->ODR&0X04)==0x00)
			printf("JDQ3 Opened!\r\n");
		else
			printf("JDQ3 Close!\r\n");
		if((GPIOC->ODR&0X08)==0x00)
			printf("JDQ4 Opened!\r\n");
		else
			printf("JDQ4 Close!\r\n");
		if((GPIOC->ODR&0X10)==0x00)
			printf("JDQ5 Opened!\r\n");
		else
			printf("JDQ5 Close!\r\n");
	  UsartSendDex(3,0X1A);
	  Hand("OK",200);
	  ClearBuf();     	 
}
void CallPhone(void)
{
    ClearBuf(); 
	  printf("ATD%s;\r\n",PhoneNumber);
	  delay_ms(6000);
	  printf("ATH\r\n");
}
//GPRSͨ�Ź������Ӳ��ԣ���Ҫ��������ATָ��
//IP�Ͷ˿ںŵ�ַ
//  AT+CGCLASS="B"
//  AT+CGDCONT=1,"IP","CMNET"
//  AT+CGATT=1
//  AT+CIPCSGP=1,"CMNET"
void GPRSBuiltNet(u8 *IP,u8 *Port)
{
    printf("AT+CGCLASS=\"B\"\r\n");              //�����ƶ���̨���
	  Hand("OK",200);
	  printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n"); //���û�����Э�飨 IP���������Ϊ"CMNET"
	  Hand("OK",200);
	  printf("AT+CGATT=1\r\n");                   //�������ø��źͷ��� GPRS ҵ�񡣷��ͣ� AT+CGATT=1������ GPRS ҵ��
	  Hand("OK",200);
	  printf("AT+CIPCSGP=1,\"CMNET\"\r\n");       //�������� CSD �� GPRS ����ģʽ�����ͣ� AT+CIPCSGP=1, "CMNET"������Ϊ GPRS ���ӣ������Ϊ��CMNET��
	  Hand("OK",200);
	  delay_ms(500);
	  printf("AT+CLPORT=\"TCP\",\"2000\"\r\n");   //�������ñ��ض˿ںš����ͣ� AT+CLPORT="TCP","8888"�������� TCP���ӱ��ض˿ں�Ϊ 8888
	  Hand("OK",200);
	  //printf("%s%s\r\n",IP,Port);
	  printf("AT+CIPSTART=\"TCP\",\"113.111.214.69\",\"8086\"\r\n");//���ڽ��� TCP ���ӻ�ע�� UDP �˿ںš����ͣ� AT+CIPSTART="TCP","113.111.214.69","8086"��ģ�齫����һ�� TCP ���ӣ�����Ŀ���ַΪ�� 113.111.214.69�����Ӷ˿�Ϊ 8086�����ӳɹ��᷵�أ� CONNECT OK
	  Hand("CONNECT OK",200);
}
void GPRSSendMSg(char *p)
{
	  u8 key;
	  static u8 num=5;              //�����һ�η��Ͳ��ɹ�����еڶ��η��ͣ�ָ���������
	  while(num)
		{
				printf("AT+CIPSEND\r\n");
				key=Hand(">",200);        //���յ���Ϣ����д���
				if(key)                   //�л�����˵��ģ����Է�����Ϣ
				{
						//����Ҫ���͵����ݻ�����Ϣ
						printf("%s",p);
						UsartSendDex(3,0X1A);    //�����Ҫ��ʮ��������
						Hand("OK",200);		
						break;			
				}	  	  
				else
				{
						num--;
				}
	   }
		if(num==0)
			num=5;
}
void AdminNowStat(void)
{
    printf("AT+CIPSTATUS\r\n");//���ڲ�ѯ��ǰ����״̬
	  Hand("OK",200);
	  printf("AT+CIPCLOSE\r\n");//���ڹر� TCP/UDP ����
	  Hand("OK",200);
	  printf("AT+CIPSHUT\r\n");//���ڹر��ƶ�����
	  Hand("OK",200);
	
}
//������Ϣ
u8 Hand(char *a,u8 Time)
{
	while(Time--)
	{
    if(strstr(Uart2InfBuf,a)!=NULL)
		{
			  return 1;
		    break;
		}
		delay_ms(10);
	}
	return 0;
}
#endif





