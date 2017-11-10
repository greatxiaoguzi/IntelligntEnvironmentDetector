#include "include.h"
//////////////////////////////////////////////////////////////////////////////////	    
u8 User_data[6];
const u8 IP_MaT[5]={'0','L','O','N','G'};    //�������͵�ַ���Զ���
const u8 IP_SLR0[5]={'0','L','O','N','G'};    //�ӻ�0���ܵ�ַ
const u8 IP_SLR1[5]={'2','L','O','N','G'};     //�ӻ�1���ܵ�ַ
const u8 IP_SLR2[5]={'3','L','O','N','G'};     //�ӻ�2���ܵ�ַ
const u8 IP_SLR3[5]={'4','L','O','N','G'};     //�ӻ�3���ܵ�ַ
const u8 IP_SLR4[5]={'5','L','O','N','G'};     //�ӻ�4���ܵ�ַ
const u8 IP_SLR5[5]={'6','L','O','N','G'};     //�ӻ�5���ܵ�ַ
//u8 Che_TAddr[5]={0xff,0xff,0xff,0xff,0xff};  //���ڼ�鷢�͵�ַ
//u8 Che_RAddr1[5]={0xff,0xff,0xff,0xff,0xff}; //���ڼ����ܵ�ַ1
//u8 Che_RAddr2[5]={0xff,0xff,0xff,0xff,0xff}; //���ڼ����ܵ�ַ2
//u8 Che_RAddr3[5]={0xff,0xff,0xff,0xff,0xff}; //���ڼ����ܵ�ַ3
//u8 Che_RAddr4[5]={0xff,0xff,0xff,0xff,0xff}; //���ڼ����ܵ�ַ4
//u8 Che_RAddr5[5]={0xff,0xff,0xff,0xff,0xff}; //���ڼ����ܵ�ַ5
//u8 Che_RAddr6[5]={0xff,0xff,0xff,0xff,0xff}; //���ڼ����ܵ�ַ6
u8 Che_StaReg[2]={0xff,0x00};   //���ڲ鿴״̬�Ĵ�����ֵ
u8 Chanl=0;                     //��Ҫ�õ���ͨ��ֵ��ȫ�ֱ�����ʾ
//u8  Rx_Succ=0;                //�ն˽��ձ�־����
//��ʼ��24L01��IO��
void NRF24L01_Init(void)        //���߳�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure; 
	
 	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE );	
	//����ΪCE��CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//SPI  IRQ
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //��������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB,GPIO_Pin_2|GPIO_Pin_8);
	SPI1_Init();    		//��ʼ��SPI
	
	SPI_Cmd(SPI1,DISABLE);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//ѡ���˴���ʱ�ӵ���̬:ʱ�����յ͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//���ݲ����ڵ�һ��ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

	
	NRF24L01_CE=0; 	//ʹ��24L01
	NRF24L01_CSN=1;	//SPIƬѡȡ��	 	
}
//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	SPI1_SetSpeed(SPI_BaudRatePrescaler_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//���24L01����	
	return 0;		 //��⵽24L01
}
//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;
  NRF24L01_CSN=0;                 //ʹ��SPI����
  status =SPI1_ReadWriteByte(reg);//���ͼĴ�����
  SPI1_ReadWriteByte(value);      //д��Ĵ�����ֵ
  NRF24L01_CSN=1;                 //��ֹSPI����	   
  return(status);       			//����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;
 	NRF24L01_CSN = 0;          //ʹ��SPI����
  SPI1_ReadWriteByte(reg);   //���ͼĴ�����
  reg_val=SPI1_ReadWriteByte(0XFF);//��ȡ�Ĵ�������
  NRF24L01_CSN = 1;          //��ֹSPI����
  return(reg_val);           //����״ֵ̬
}	
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)//�����ַ�����Ӧ�ĳ���
{
	u8 status,u8_ctr;	       
  NRF24L01_CSN = 0;           //ʹ��SPI����
  status=SPI1_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI1_ReadWriteByte(0XFF);//��������
  NRF24L01_CSN=1;       //�ر�SPI����
  return status;        //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;	    
 	NRF24L01_CSN = 0;          //ʹ��SPI����
  status = SPI1_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI1_ReadWriteByte(*pBuf++); //д������	 
  NRF24L01_CSN = 1;       //�ر�SPI����
  return status;          //���ض�����״ֵ̬
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_8);//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	NRF24L01_CE=0;
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_USER_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CE=1;//��������	   
	while(NRF24L01_IRQ!=0);//�ȴ��������
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		  NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		  return MAX_TX; 
	}
	if(sta&TX_OK)//�������
	{
		  return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����յ������׵�ַ
//����ֵ:0��������ɣ��������������
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;
	SPI1_SetSpeed(SPI_BaudRatePrescaler_8); //spi�ٶ�Ϊ9Mhz(24L01�����SPIʱ��Ϊ10Mhz)
	sta=NRF24L01_Read_Reg(STATUS);          //��ȡ״̬�Ĵ�����ֵ
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)//���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_USER_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ���
		return 0;
	}
	return 1;//û�յ��κ�����
}					    
//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void NRF24L01_RX_Mode(u8 chanl)
{
	  //����24L01+�رգ������޸���������
	  NRF24L01_CE=0;
	  //��������Ӧ��ʹ�ܣ�ͨ��Ƶ���趨
	  NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG,0x0f);//���ý���ģʽ�Ļ�������;PWR_UP,EN_CRC,16BIT_CRC,
	  switch(chanl)       //ѡ�����ͨ��
		{
			case 0:
			{
	        NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA,0x01);//Ӧ�����ã�0x3f����543210ȫ��Ӧ��   11 1111
	        NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR,0x01);//����ͨ��ʹ��0x3f����543210ͨ��ȫ��������� 11 1111
			}break;
			case 1:
			{
	        NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA,0x02);//Ӧ�����ã�0x3f����543210ȫ��Ӧ��   11 1111
	        NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR,0x02);//����ͨ��ʹ��0x3f����543210ͨ��ȫ��������� 11 1111
			}break;
			case 2:
			{
	        NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA,0x04);//Ӧ�����ã�0x3f����543210ȫ��Ӧ��   11 1111
	        NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR,0x04);//����ͨ��ʹ��0x3f����543210ͨ��ȫ��������� 11 1111
			}break;
			case 3:
			{
	        NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA,0x08);//Ӧ�����ã�0x3f����543210ȫ��Ӧ��   11 1111
	        NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR,0x08);//����ͨ��ʹ��0x3f����543210ͨ��ȫ��������� 11 1111
			}break;
			case 4:
			{
	        NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA,0x10);//Ӧ�����ã�0x3f����543210ȫ��Ӧ��   11 1111
	        NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR,0x10);//����ͨ��ʹ��0x3f����543210ͨ��ȫ��������� 11 1111
			}break;
			case 5:
			{
	        NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA,0x20);//Ӧ�����ã�0x3f����543210ȫ��Ӧ��   11 1111
	        NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR,0x20);//����ͨ��ʹ��0x3f����543210ͨ��ȫ��������� 11 1111
			}break;
			default: break;
		}
	  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH ,50);  //����RFͨ��Ƶ��2.4G+����ֵMhz����Χ0x00��0x7f
	  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP,0x0f);//����TX�������,0db����,2Mbps,���������濪��  
	  //���ôӻ��Ľ��յ�
	  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0,(u8*)IP_SLR0,5);//дRX0���յ�ַ���ӻ���ַ
	  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P1,(u8*)IP_SLR1,5);//дRX1���յ�ַ
	  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P2,(u8*)IP_SLR2,1);//дRX2���յ�ַ
	  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P3,(u8*)IP_SLR3,1);//дRX3���յ�ַ
	  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P4,(u8*)IP_SLR4,1);//дRX4���յ�ַ
	  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P5,(u8*)IP_SLR5,1);//дRX5���յ�ַ
	  //������Ч���û����ݿ��Ϊ32�ֽ�
	  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P0,RX_USER_WIDTH);//ѡ��ͨ��0���û���Ч���ݿ��32�ֽ� 
	  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P1,RX_USER_WIDTH);//ѡ��ͨ��1���û���Ч���ݿ��32�ֽ� 
	  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P2,RX_USER_WIDTH);//ѡ��ͨ��2���û���Ч���ݿ��32�ֽ� 
	  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P3,RX_USER_WIDTH);//ѡ��ͨ��3���û���Ч���ݿ��32�ֽ� 
	  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P4,RX_USER_WIDTH);//ѡ��ͨ��4���û���Ч���ݿ��32�ֽ� 
	  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P5,RX_USER_WIDTH);//ѡ��ͨ��5���û���Ч���ݿ��32�ֽ� 
		//CEΪ��,�������ģʽ 
  	NRF24L01_CE = 1; 
}						 
//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������
//CEΪ�ߴ���10us,����������.
void NRF24L01_TX_Mode(void)
{
    //����ģ�����ģʽ���ȴ�����	
	  NRF24L01_CE=0;	    
  	//NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR,(u8*)IP_MaT,TX_ADR_WIDTH);//дTX�ڵ��ַ������ ��5���ֽ�
  	//NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0,(u8*)IP_SLR0,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK��5���ֽ�	  

  	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA,0x3f);     //ʹ��ͨ��543210���Զ�Ӧ��,ȫ��Ӧ��    
  	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR,0x3f); //ʹ��ͨ��543210�Ľ��յ�ַ,ȫ������
    //�����Զ��ط�
  	NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_RETR,0xff);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:15��
  	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH,50);       //����RFͨ��Ϊ40
  	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
  	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	  NRF24L01_CE=1;//CEΪ��,10us����������
}
void NRF_Set_Mode(u8 mode)    //NRF���ù���ģʽ,ͨ���趨
{
   NRF24L01_TX_Mode();	
	 NRF24L01_CE=0;
	 switch(mode)
	 {
		 case 1:
		 {
		    NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR,(u8*)IP_SLR1,5);//дTX�ڵ��ַ
	      NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P1,(u8*)IP_SLR1,5);//дRX1���յ�ַ
		 }break;
		 case 2:
		 {
		    NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR,(u8*)IP_SLR2,5);//дTX�ڵ��ַ
	      NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P2,(u8*)IP_SLR2,5);//дRX2���յ�ַ
		 }break;
		 case 3:
		 {
		    NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR,(u8*)IP_SLR3,5);//дTX�ڵ��ַ
	      NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P3,(u8*)IP_SLR3,1);//дRX3���յ�ַ
		 }break;
		 case 4:
		 {
		    NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR,(u8*)IP_SLR4,5);//дTX�ڵ��ַ
	      NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P4,(u8*)IP_SLR4,1);//дRX4���յ�ַ
		 }break;
		 case 5:
		 {
		    NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR,(u8*)IP_SLR5,5);//дTX�ڵ��ַ
	      NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P5,(u8*)IP_SLR5,1);//дRX5���յ�ַ
		 }break;
		 default : LED0=!LED0;break;
	 }
	  NRF24L01_CE=1;
}
void Chec_NRF(void)   //���NRF24L01�Ƿ���λ.	���ڴ�ӡ���
{
	  u8 Succe=10;
    while(Succe--)	
	  {
				 if(NRF24L01_Check())
				 {
					   printf("NRF24L01 Error\n");
						 delay_ms(500);					 
				 }
				 else
				 {
					  printf("NRF24L01 OK\n");
						break;
				 }
	  }
}






