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
#include<mysql/mysql.h>
#include"define.h"


//全局变量
int g_level = 0;
AP_Configuration_template apTemplate[max_template];
AP ap_list[max_ap];
int legalTemplate[max_template];
int legalAP[max_ap];
int legalAPServerConfigureResult[max_ap_server];//0，未配置，1，配置成功，2，配置失败，3，配置超时
APServer_timer legalAPServerTimer[max_ap_server];

//mysql
char sql_insert[1024];
MYSQL mysql;
MYSQL* conn =&mysql;
MYSQL_RES resultt;
MYSQL_RES* result=&resultt;
MYSQL_ROW row;
MYSQL_ROW row1;
MYSQL_RES r;
MYSQL_RES *result1 = &r;
//ack
template_operation_ack Template_Ack[max_template];
legalAP_operation_ack legalAP_Ack[max_ap];
AP_configuration_ack AP_Configure_Ack[max_ap];


//socket with APServer
APServer_socket apServer_socket_list[max_ap_server];
int sockSer;
struct sockaddr_in addrSer;  //创建一个记录地址信息的结构体 
socklen_t addrlen = sizeof(struct sockaddr);
char Web_Server_IP[50]="192.168.3.78";



char Webserver_allaps[][50]={"serial_number","template_index","ip_addr","DHCP_Static","ssid_name","ssid_passwd","login_name","login_passwd","NatOrBridge",
	"ip_pool_start","ip_pool_end","ip_gateway","ip_subnet","ip_NMSip","user_number","mode","version","antenna_number","\0"}; 

char Webserver_alltemplates[][50]={"ssid_name","ssid_passwd","login_name","login_passwd","NatOrBridge","ip_pool_start","ip_pool_end","DHCP_Static","ip_addr","ip_gateway",
	"ip_subnet","ip_NMSip","ip_WEBip","options","\0"}; 

int Webserver_apMaptable[]={AP_SERIAL_NUMBER,AP_TEMPLATE_INDEX,AP_IP_ADDR,DHCP_OR_STATIC,AP_SSID_NAME,AP_SSID_PASSWORD,AP_LOGIN_NAME,AP_LOGIN_PASSWORD,
	AP_NAT_OR_BRIDGE,AP_IP_POOL_START,AP_IP_POOL_END,AP_IP_GATEWAY,AP_IP_SUBNET,AP_IP_NMS_IP,AP_USER_NUMBER,AP_MODE,AP_VERSION,AP_ANTENNA_NUMBER};
int Webserver_templateMaptable[]={TEMPLATE_SSID_NAME,TEMPLATE_SSID_PASSWORD,TEMPLATE_LOGIN_NAME,TEMPLATE_LOGIN_PASSWORD,TEMPLATE_NAT_OR_BRIDGE,
	TEMPLATE_IP_POOL_START,TEMPLATE_IP_POOL_END,TEMPLATE_DHCP_OR_STATIC,TEMPLATE_IP_ADDR,TEMPLATE_IP_GATEWAY,TEMPLATE_IP_SUBNET,TEMPLATE_IP_NMS_IP,TEMPLATE_IP_WEB_IP,TEMPLATE_OPTIONS};


int print_log(unsigned int level, const char* filename, unsigned int line, char* format, ...);
int read_template_set();
int read_AP_set();
int LegalAPServer_find(struct      sockaddr_in* apserver_socket_addr);
void  dispatch_APServerMsg(int ap_index,char* buf);
void send_to_apServer(int apServerIndex, int command, AP_Configuration_template* tempTemplate, template_operation_req* templateOperation, legalAP_operation_req* APOperation, int size, char*);
int mysql_connect();
void Fill_APServer_socket_list();
int Fill_apTemplate();
int Fill_ap_list();
int control_command_to_APserver();
void print_info(void);
int read_Web_Server_Configure_file(const char *filename);


void* socket_with_APServer(int* apIndex)
{
	pthread_detach(pthread_self());

	int apServer_index=0;

	
    //创建一个套接字，并检测是否创建成功
    sockSer = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockSer == -1)
        perror("socket");


    addrSer.sin_family = AF_INET;    //使用AF_INET协议族 
    addrSer.sin_port = htons(5050);     //设置地址结构体中的端口号
    addrSer.sin_addr.s_addr = inet_addr(Web_Server_IP);  //设置通信ip

    //将套接字地址与所创建的套接字号联系起来，并检测是否绑定成功

    int res = bind(sockSer,(struct sockaddr*)&addrSer, addrlen);

	
    if(res == -1)
        perror("bind");

    //char sendbuf[256];    //申请一个发送数据缓存区
    char recvbuf[bufsize_apToWeb2];    //申请一个接收数据缓存区
    struct sockaddr_in addrCli;
    while(1)  //服务器一直循环接受客户端的请求
    {
        if(recvfrom(sockSer,recvbuf,bufsize_apToWeb2,0,(struct  sockaddr*)&addrCli, &addrlen)==-1)
			log_error("Error: receive error from ap server!\n");
			else
				{
				apServer_index = LegalAPServer_find(&addrCli);
				if(apServer_index!=-1)
					{
					apServer_socket_list[apServer_index].addrAP=addrCli;
					apServer_socket_list[apServer_index].addrlen=addrlen;	
					dispatch_APServerMsg(apServer_index,recvbuf);

					}
				else
					{
					log_error("Error: unconfigured APServer!\n");
					}
				}

    }
    return 0;


	pthread_exit(0);
}


void prompt_info(int signo)

{
    //printf("%d\n",signo);
    control_command_to_APserver();

    int i=0;

	//apServerTimerHandle
	for(i=0;i<max_ap_server;i++)
	{
		switch(legalAPServerTimer[i].apServer_timer_flag)
			{
			case WEB_APSERVER_OFF:
				break;
				
			case WEB_APSERVER_CONNECTING:
				legalAPServerTimer[i].apServer_on_timer--;
				if(legalAPServerTimer[i].apServer_on_timer==0)
				{
					legalAPServerTimer[i].apServer_timer_flag = WEB_APSERVER_OFF;
				}
				break;
			case WEB_APSERVER_ON:
				legalAPServerTimer[i].heart_beat_timer--;
				if(legalAPServerTimer[i].heart_beat_timer==0)
				{
					send_to_apServer(i,APSERVER_WEB_HEART_BEAT,NULL,NULL,NULL,1,NULL);
					legalAPServerTimer[i].heart_beat_timer = web_apServer_heart_time;
					legalAPServerTimer[i].heart_beat_timeout_times++;
					if(legalAPServerTimer[i].heart_beat_timeout_times==3)
						{
						legalAPServerTimer[i].apServer_timer_flag = WEB_APSERVER_OFF;
						}
				}
				switch(legalAPServerTimer[i].apServer_on_timer_flag)
					{
					case 0:	//idle
						break;
					case 1:	//configuration
						legalAPServerTimer[i].apServer_on_timer--;
						if(legalAPServerTimer[i].apServer_on_timer==0)
							{
							//configure time out, give response to database
							legalAPServerTimer[i].apServer_on_timer_flag=0;
						}
						break;
					case 2:	//template modify
						legalAPServerTimer[i].apServer_on_timer--;
						if(legalAPServerTimer[i].apServer_on_timer==0)
							{
							//template modify time out, give response to database
							legalAPServerTimer[i].apServer_on_timer_flag=0;
						}
						break;
					case 3:	//AP modify
						legalAPServerTimer[i].apServer_on_timer--;
						if(legalAPServerTimer[i].apServer_on_timer==0)
							{
							//AP modify time out, give response to database
							legalAPServerTimer[i].apServer_on_timer_flag=0;
						}
						break;
					default:
						log_error("Error: undefined state of apServer_on_timer_flag!\n");
					}
				break;

			default:
				log_error("Error: undefined state of apServer_timer_flag!\n");
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

void init_legalTemplate_legalAP(void)

{
    int i=0;
	
	for(i=0;i<max_template;i++)
		legalTemplate[i] = 0;
	for(i=0;i<max_ap;i++)
		legalAP[i] = 0;
	for(i=0;i<max_ap_server;i++)
		{
		legalAPServerTimer[i].apServer_timer_flag = 0;
		legalAPServerTimer[i].apServer_on_timer = 0;
		legalAPServerTimer[i].heart_beat_timer = 0;
		legalAPServerTimer[i].heart_beat_timeout_times = 0;
		legalAPServerTimer[i].apServer_on_timer_flag = 0;
		legalAPServerConfigureResult[i]=0;
		}
		
}



int main()
{

	mysql_connect();
	int taskID = 0, temp = 0;
	int a = 0,b = 0;

	pthread_t tid;
	
	init_legalTemplate_legalAP();


	//add read database here:configured APServer to apServer_socket_list     APServer_socket apServer_socket_list[max_ap_server];
	Fill_APServer_socket_list();
         

	//add read database here:configured template to apTemplate and legalTemplate   AP_Configuration_template apTemplate[max_template];   legalTemplate[max_template];
	a = Fill_apTemplate();
	for (b = 0; b < a; b++)
	{
		legalTemplate[b] = 1;
	}
	//add read database here:configured AP to ap_list and legalAP
	a = Fill_ap_list();
	for (b = 0; b < a; b++)
	{
		legalAP[b] = 1;
	}

//	dispatch_APServerMsg(11, recvbuf);



	init_sigaction();

    init_time();

	print_info();


	read_Web_Server_Configure_file("../webserver.txt");


	taskID = pthread_create(&tid, NULL, (void *)socket_with_APServer, (void *)(&(temp)));
	printf("%d",taskID);

	while(1)
	{
	}
	mysql_close(conn);
    return 0;


}



