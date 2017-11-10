#include "sys.h"	 		   	    
#include "tcp_demo.h"
#include "include.h"
//TCP应用接口函数(UIP_APPCALL)
//完成TCP服务(包括server和client)和HTTP服务
void tcp_demo_appcall(void)
{	
  	
	switch(uip_conn->lport)//本地监听端口80和1200 
	{
		case HTONS(80):
			//httpd_appcall(); 
			break;
		case HTONS(1200):
		    //tcp_server_demo_appcall(); 
			break;
		default:						  
		    break;
	}		    
	switch(uip_conn->rport)	//远程连接1400端口
	{
	    case HTONS(1400):
			tcp_client_demo_appcall();
	       break;
	    default: 
	       break;
	}   
}
//打印日志用
void uip_log(char *m)
{			    
	printf("uIP log:%s\r\n",m);
}

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
//uip事件处理函数
//必须将该函数插入用户主循环,循环调用.
void uip_polling(void)
{
	u8 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok=0;	 
	if(timer_ok==0)                //仅初始化一次
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //创建1个0.5秒的定时器 
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//创建1个10秒的定时器 
	}				 
	uip_len=tapdev_read();	       //从网络设备读取一个IP包,得到数据长度.uip_len在uip.c中定义
	if(uip_len>0) 			           //有数据
	{   
		//处理IP数据包(只有校验通过的IP包才会被接收) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))//是否是IP包? 
		{
			uip_arp_ipin();	         //去除以太网头结构，更新ARP表
			uip_input();   	         //IP包处理
			//当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
			//需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)		    
			if(uip_len>0)//需要回应数据
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//处理arp报文,是否是ARP请求包?
		{
			uip_arp_arpin();
 			//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len(这是2个全局变量)
 			if(uip_len>0)tapdev_send();//需要发送数据,则通过tapdev_send发送	 
		}
	}else if(timer_expired(&periodic_timer))	//0.5秒定时器超时
	{
		timer_reset(&periodic_timer);		//复位0.5秒定时器 
		//轮流处理每个TCP连接, UIP_CONNS缺省是40个  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//处理TCP通信事件  
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
	 		if(uip_len>0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		} 
		//每隔10秒调用1次ARP定时器函数 用于定期ARP处理,ARP表10秒更新一次，旧的条目会被抛弃
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}























