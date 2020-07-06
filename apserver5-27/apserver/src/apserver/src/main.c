/*
auth:	Baojinzhuang
date:	2019-12-10
history:	
*/

#include<stdio.h>
#include<stdlib.h> 
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<ctype.h>

#include"define.h"


//全局变量
int g_level = 0;
AP_Configuration_template apTemplate[max_template];
AP ap_list[max_ap];
int legalTemplate[max_template];
int legalAP[max_ap];
int legalAPConfigureResult[max_ap];//0，配置成功，1，配置失败，2，配置超时
AP_timer legalAPTimer[max_ap];
int Webtimerflag = 0; //0,不启动，1,启动中，2,正常通信中
int WebOntimerflag = 0; //0,正常通信中，1,正常通信配置中，2,正常通信升级中
int webTimer = 0;
int webOnTimer = 0;

//socket with web
int sockWeb = -1;
struct sockaddr_in addrWebSer;  //创建一个记录地址信息的结构体 
socklen_t addrWeblen = sizeof(struct sockaddr);


//socket with AP
AP_socket ap_socket_list[max_ap];
int sockSer;
struct sockaddr_in addrAPSer;  //创建一个记录地址信息的结构体 
socklen_t addrlen = sizeof(struct sockaddr);
char AP_Server_IP[50]="192.168.0.160";
char Web_Server_IP[50]="192.168.0.151";




char Apserver_allaps[][50]={"serial_number","template_index","ip_addr","DHCP_Static","ssid_name","ssid_passwd","login_name","login_passwd","NatOrBridge",
	"ip_pool_start","ip_pool_end","ip_gateway","ip_subnet","ip_NMSip","user_number","mode","version","antenna_number","\0"}; 

char Apserver_alltemplates[][50]={"ssid_name","ssid_passwd","login_name","login_passwd","NatOrBridge","ip_pool_start","ip_pool_end","DHCP_Static","ip_addr","ip_gateway",
	"ip_subnet","ip_NMSip","ip_WEBip","options","\0"}; 

int Apserver_apMaptable[]={AP_SERIAL_NUMBER,AP_TEMPLATE_INDEX,AP_IP_ADDR,DHCP_OR_STATIC,AP_SSID_NAME,AP_SSID_PASSWORD,AP_LOGIN_NAME,AP_LOGIN_PASSWORD,
	AP_NAT_OR_BRIDGE,AP_IP_POOL_START,AP_IP_POOL_END,AP_IP_GATEWAY,AP_IP_SUBNET,AP_IP_NMS_IP,AP_USER_NUMBER,AP_MODE,AP_VERSION,AP_ANTENNA_NUMBER};
int Apserver_templateMaptable[]={TEMPLATE_SSID_NAME,TEMPLATE_SSID_PASSWORD,TEMPLATE_LOGIN_NAME,TEMPLATE_LOGIN_PASSWORD,TEMPLATE_NAT_OR_BRIDGE,
	TEMPLATE_IP_POOL_START,TEMPLATE_IP_POOL_END,TEMPLATE_DHCP_OR_STATIC,TEMPLATE_IP_ADDR,TEMPLATE_IP_GATEWAY,TEMPLATE_IP_SUBNET,TEMPLATE_IP_NMS_IP,TEMPLATE_IP_WEB_IP,TEMPLATE_OPTIONS};


void template_report();
void ap_report();
int print_log(unsigned int level, const char* filename, unsigned int line, char* format, ...);
void add_template_file(int template_index);
void write_template_set();
int del_template_file(int template_index);
void modify_template_file(int template_index);
void write_ap_set();
int read_template_set();
int read_AP_set();
int LegalAP_find(char* buf);
void  dispatch_apMsg(int ap_index,char* buf);
void  dispatch_webMsg(char* buf);
void send_to_ap(int apIndex, int command,AP_Configuration_template* tempTemplate);
void send_to_web(char command,char* sendbuf2,int length);
void  connect_to_web();
int read_AP_Server_configure_file(const char *filename);


void* socket_with_ap(int* apIndex)
{
	pthread_detach(pthread_self());

	int ap_index=0;
	AP* pp;

	printf("apsocket:%d\n",*apIndex);
	
    //创建一个套接字，并检测是否创建成功
    sockSer = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockSer == -1)
        perror("socket");


    addrAPSer.sin_family = AF_INET;    //使用AF_INET协议族 
    addrAPSer.sin_port = htons(5050);     //设置地址结构体中的端口号
    addrAPSer.sin_addr.s_addr = inet_addr(AP_Server_IP);  //设置通信ip

    //将套接字地址与所创建的套接字号联系起来，并检测是否绑定成功

    int res = bind(sockSer,(struct sockaddr*)&addrAPSer, addrlen);

	
    if(res == -1)
        perror("bind");

    char sendbuf[256];    //申请一个发送数据缓存区
    char recvbuf[sizeof(AP)+1];    //申请一个接收数据缓存区
    struct sockaddr_in addrCli;
    while(1)  //服务器一直循环接受客户端的请求
    {
        if(recvfrom(sockSer,recvbuf,sizeof(recvbuf),0,(struct  sockaddr*)&addrCli, &addrlen)==-1)
				printf("receive error from ap");
			else
				{
				//printf("receive one from ap\n");
				pp=(AP*)(recvbuf+1);
				ap_index = LegalAP_find(pp->AP_SN);
				if(ap_index!=-1)
					{
					ap_socket_list[ap_index].addrAP=addrCli;
					ap_socket_list[ap_index].addrlen=addrlen;	
					if(legalAP[ap_index])
						dispatch_apMsg(ap_index,recvbuf);
					else
						{
						send_to_ap(ap_index, APSERVER_AP_REGISTER_REJ, NULL);
						log_info("Info: invalid AP:%s\n",ap_list[ap_index].AP_SN);
						}
					}
				else
					{
					sendbuf[0]=APSERVER_AP_REGISTER_REJ;
					sendto(sockSer,sendbuf,1,0,(struct sockaddr*)&addrCli, addrlen);    //向客户端发送数据
					log_info("Info: unconfigured AP:%s\n",pp->AP_SN);
					}
				}

        //printf("Cli:>%s\n",recvbuf);

        //printf("Ser:>");    
        //scanf("%s",sendbuf);
        //sendto(sockSer,sendbuf,strlen(sendbuf)+1,0,(struct sockaddr*)&addrCli, addrlen);    //向客户端发送数据
    }
    return 0;


	pthread_exit(0);
}



void* socket_with_web(int* apIndex)
{
	pthread_detach(pthread_self());

	char recvbufFromWeb[bufsize_webToAp];	  //申请一个接收数据缓存区

	printf("websocket:%d\n",*apIndex);
	
    //创建一个套接字，并检测是否创建成功
    sockWeb = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockWeb == -1){
        perror("socket");
    }
	addrWebSer.sin_family = AF_INET;	 //使用AF_INET协议族 
	addrWebSer.sin_port = htons(5050);	//设置地址结构体中的端口号
	addrWebSer.sin_addr.s_addr = inet_addr(Web_Server_IP);	//设置通信ip

	connect_to_web();

    while(1){

        //接收来自客户端的数据
        if(recvfrom(sockWeb, recvbufFromWeb, bufsize_webToAp, 0, (struct sockaddr*)&addrWebSer, &addrWeblen)==-1)
			printf("receive error from web");
		else
			dispatch_webMsg(recvbufFromWeb);
    }
    return 0;



	pthread_exit(0);
}



void prompt_info(int signo)

{
    //printf("%d\n",signo);
    int i=0;

    //webTimerHandle
	switch(Webtimerflag)
	{
		case WEB_APSERVER_OFF:
		webTimer--;
		if(webTimer==0)
		{
			Webtimerflag=WEB_APSERVER_CONNECTING;
			webTimer=web_apServer_heart_time*3;

			send_to_web(APSERVER_WEB_TEMPLATE_REPORT,NULL,0);
			log_info("Info: APSERVER_WEB_TEMPLATE_REPORT\n");
			send_to_web(APSERVER_WEB_AP_REPORT,NULL,0);
			log_info("Info: APSERVER_WEB_AP_REPORT\n");
	
		}
		break;
		case WEB_APSERVER_CONNECTING:
		webTimer--;
		if(webTimer==0)
		{
			Webtimerflag=WEB_APSERVER_OFF;
			webTimer=3600;
		}
		break;
		case WEB_APSERVER_ON:
		webTimer--;
		if(webTimer==0)
		{
			Webtimerflag=WEB_APSERVER_OFF;
			webTimer=3600;
		}
		switch(WebOntimerflag)
			{
			case 0:	
				break;
			case 1:	//configuration
				webOnTimer--;
				if(webOnTimer==0)
					{
					send_to_web(APSERVER_WEB_AP_CONFIGURE,NULL,0);
					log_info("Info: APSERVER_WEB_AP_CONFIGURE time out\n");
					WebOntimerflag=0;
				}
				break;
				default:
					printf("undefined state of WebOntimerflag");
			}
		break;

		default:
		printf("undefined state of Webtimerflag");

	}

	//apTimerHandle
	for(i=0;i<max_ap;i++)
	{
	if(legalAP[i])
			switch(legalAPTimer[i].ap_timer_flag)
			{
			case AP_APSERVER_OFF:
				break;
				
			case AP_APSERVER_CONNECTING:
				legalAPTimer[i].ap_on_timer--;
				if(legalAPTimer[i].ap_on_timer==0)
				{
					legalAPTimer[i].ap_timer_flag = AP_APSERVER_OFF;
					ap_list[i].status.online_state = AP_APSERVER_OFF;
				}
				break;
			case AP_APSERVER_ON:
				legalAPTimer[i].heart_beat_timer--;
				if(legalAPTimer[i].heart_beat_timer==0)
				{
					send_to_ap(i,APSERVER_AP_HEART_BEAT,NULL);
					log_info("Info: send APSERVER_AP_HEART_BEAT AP:%s\n",ap_list[i].AP_SN);
					legalAPTimer[i].heart_beat_timer = apServer_apAgent_heart_time;
					legalAPTimer[i].heart_beat_timeout_times++;
					if(legalAPTimer[i].heart_beat_timeout_times==3)
						{
						legalAPTimer[i].ap_timer_flag = AP_APSERVER_OFF;
						ap_list[i].status.online_state = AP_APSERVER_OFF;
						}
				}
				switch(legalAPTimer[i].ap_on_timer_flag)
					{
					case 0:	
						break;
					case 1:	//configuration
						legalAPTimer[i].ap_on_timer--;
						if(legalAPTimer[i].ap_on_timer==0)
							{
							legalAPConfigureResult[i]=CONFIGURE_TIMEOUT;
							legalAPTimer[i].ap_on_timer_flag=0;
						}
						break;
					default:
							printf("undefined state of APOntimerflag");
				}
		break;

			default:
				printf("undefined state of APtimerflag");
			}
	}

}

// 建立信号处理机制

void init_sigaction(void)

{

struct sigaction tact;

/*信号到了要执行的任务处理函数为prompt_info*/

tact.sa_handler = prompt_info;

tact.sa_flags = 14;

/*初始化信号集*/

sigemptyset(&tact.sa_mask);

/*建立信号处理机制*/

sigaction(SIGALRM, &tact, NULL);

}

void init_time()

{

struct itimerval value;

/*设定执行任务的时间间隔为2秒0微秒*/

value.it_value.tv_sec = 1;

value.it_value.tv_usec = 0;

/*设定初始时间计数也为2秒0微秒*/

value.it_interval = value.it_value;

/*设置计时器ITIMER_REAL*/

setitimer(ITIMER_REAL, &value, NULL);

}

void init_legalTemplate(void)

{
    int i=0;
	
	for(i=0;i<max_template;i++)
		{
		legalTemplate[i] = 0;
		apTemplate[i].templateIndex = i;
		}
}

void init_legalAP(void)

{
    int i=0;
	
	for(i=0;i<max_ap;i++)
		legalAP[i] = 0;
	for(i=0;i<max_ap;i++)
		{
		legalAPTimer[i].ap_timer_flag = 0;
		legalAPTimer[i].ap_on_timer = 0;
		legalAPTimer[i].heart_beat_timer = 0;
		legalAPTimer[i].heart_beat_timeout_times = 0;
		legalAPTimer[i].ap_on_timer_flag = 0;
		legalAPConfigureResult[i]=0;
		}
		
}


void print_templates()
{
	int i=0;
	for(i=0;i<max_template;i++)
		{
		if(legalTemplate[i])
			{
			printf("templateID=%d's ssid=%s\n",i,apTemplate[i].ap_ssid.ssid);
			printf("templateID=%d's ssid_psw=%s\n",i,apTemplate[i].ap_ssid.ssid_psw);
			printf("templateID=%d's login=%s\n",i,apTemplate[i].ap_login.login);
			printf("templateID=%d's login_psw=%s\n",i,apTemplate[i].ap_login.login_psw);
			printf("templateID=%d's NatOrBridge=%d\n",i,apTemplate[i].NatOrBridge);
			printf("templateID=%d's pool.start=%s\n",i,inet_ntoa(apTemplate[i].pool.start));
			printf("templateID=%d's pool.end=%s\n",i,inet_ntoa(apTemplate[i].pool.end));
			printf("templateID=%d's DHCPOrStatic=%d\n",i,apTemplate[i].DHCPOrStatic);
			printf("templateID=%d's ipaddr=%s\n",i,inet_ntoa(apTemplate[i].AP_IP));
			printf("templateID=%d's netmask=%s\n",i,inet_ntoa(apTemplate[i].ap_gateway.subnetmask));
			printf("templateID=%d's gateway=%s\n",i,inet_ntoa(apTemplate[i].ap_gateway.gateway));
			printf("templateID=%d's NMSIP=%s\n",i,inet_ntoa(apTemplate[i].AP_Server_IP));
			}
		}
}

void print_aps()
{
	int i=0;
		for(i=0;i<max_ap;i++)
			{
			if(legalAP[i])
				printf("APSN=%s's templateIndex=%d\n",ap_list[i].AP_SN,ap_list[i].configure.templateIndex);
			}

}



int main()
{

	int taskID = 0, temp = 0;
	pthread_t tid;

	init_legalTemplate();
	init_legalAP();

	read_template_set();
	read_AP_set();

	print_templates();
	print_aps();

	init_sigaction();

    init_time();

	//AP_Server_IP[50]="192.168.0.160";
	//strcpy(AP_Server_IP,"192.168.0.160");
	read_AP_Server_configure_file("../apserver.txt");
	printf("APServerIP=%s\n",AP_Server_IP);
/*
	AP ap1;
	AP* pp;

	ap1.templateIndex=100;
	ap1.configure.NatOrBridge=4;
	char data[10000]={0};
	char* p=data;
	pp=(AP*)p;
	memcpy(p,(char*)&ap1,sizeof(AP));
	
	printf("pp->configure.NatOrBridge=%d\n",pp->configure.NatOrBridge);
	return 0;*/





	taskID = pthread_create(&tid, NULL, (void *)socket_with_ap, (void *)(&(temp)));
	printf("%d",taskID);
	taskID = pthread_create(&tid, NULL, (void *)socket_with_web, (void *)(&(temp)));
	printf("%d",taskID);

	while(1)
	{
	}

    return 0;

}



