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
AP ap_configuration_status;
int APServertimerflag = 0; //0,不启动，1,启动中，2,正常通信中
int APServerTimer = 0;

char ap_remote_file[50]="./ap/ap_remote";
char ap_local_file[50]="./ap/ap_local";
char log_file[50]="./logs/logs.txt";
char ap_serial[APSN_length]="1234567890";
//ap_configuration_status.configure.NMS_IP.s_addr = inet_addr("192.168.0.151");	//设置通信ip



//socket with apserver
int sockAPServer = -1;
struct sockaddr_in addrAPSer;  //创建一个记录地址信息的结构体 
socklen_t addrAPServerlen = sizeof(struct sockaddr);
char AP_Server_IP[50]="192.168.0.160";

char Ap_allaps[][50]={"serial_number","template_index","ip_addr","DHCP_Static","ssid_name","ssid_passwd","login_name","login_passwd","NatOrBridge",
		"ip_pool_start","ip_pool_end","ip_gateway","ip_subnet","ip_NMSip","user_number","mode","version","antenna_number","\0"}; 

int Ap_Maptable[]={AP_SERIAL_NUMBER,AP_TEMPLATE_INDEX,AP_IP_ADDR,DHCP_OR_STATIC,AP_SSID_NAME,AP_SSID_PASSWORD,AP_LOGIN_NAME,AP_LOGIN_PASSWORD,
	AP_NAT_OR_BRIDGE,AP_IP_POOL_START,AP_IP_POOL_END,AP_IP_GATEWAY,AP_IP_SUBNET,AP_IP_NMS_IP,AP_USER_NUMBER,AP_MODE,AP_VERSION,AP_ANTENNA_NUMBER};

char Ap_States_allaps[][50]={"lan_ipaddr","wl1_ssid","wl1_wpa_psk","http_username","http_passwd","lan_proto",
		"dhcp_start","dhcp_end","lan_gateway","os_version","wl1_hw_txchain","lan_netmask","\0"};
int Ap_States_Maptable[]={AP_STATES_LAN_IPADDR,AP_STATES_SSID_NAME,AP_STATES_SSID_PASSWORD,AP_STATES_LOGIN_NAME,AP_STATES_LOGIN_PASSWORD,
		AP_STATES_NAT_OR_BRIDGE,AP_STATES_IP_POOL_START,AP_STATES_IP_POOL_END,AP_STATES_IP_GATEWAY,AP_STATES_VERSION,AP_STATES_ANTENNA_NUMBER,AP_STATES_IP_SUBNET};


int print_log(unsigned int level, const char* filename, unsigned int line, char* format, ...);
void write_ap_file(const char *filename);
int read_ap_file(const char *filename);
void  dispatch_APServerMsg(char* buf);
void send_to_APServer(char command);
void write_log_file(char* logs);
int read_AP_Server_configure_file(const char *filename);
int getUpdate();

void* socket_with_APServer(int* recvFlag)
{
	pthread_detach(pthread_self());

	char recvbufFromAPServer[bufsize_APToServer];	  //申请一个接收数据缓存区

	//printf("%d",*apIndex);
	//apIndex=1;
	
    //创建一个套接字，并检测是否创建成功
    sockAPServer = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockAPServer == -1){
        perror("socket");
    }
	addrAPSer.sin_family = AF_INET;	 //使用AF_INET协议族 
	addrAPSer.sin_port = htons(5050);	//设置地址结构体中的端口号
	addrAPSer.sin_addr.s_addr = inet_addr(AP_Server_IP);

	//connect_to_APServer();

    while(1){

        //接收来自客户端的数据
        if(recvfrom(sockAPServer, recvbufFromAPServer, bufsize_APToServer, *recvFlag, (struct sockaddr*)&addrAPSer, &addrAPServerlen)==-1)
			printf("receive error from APServer");
		else
			{
			dispatch_APServerMsg(recvbufFromAPServer);
			}
    }
    return 0;



	pthread_exit(0);
}



void prompt_info(int signo)

{
    //printf("%d\n",signo);
    //int i=0;

    //webTimerHandle
	switch(APServertimerflag)
	{
		case AP_APSERVER_OFF:
		APServerTimer--;
		if(APServerTimer==0)
		{
			APServertimerflag=AP_APSERVER_CONNECTING;
			APServerTimer=apServer_apAgent_configure_time;

			send_to_APServer(AP_APSERVER_REGISTER_REQ_WITH_CONFIGURATION);
			log_info("Info: start with configuration\n");
	
		}
		break;
		case AP_APSERVER_CONNECTING:
		APServerTimer--;
		if(APServerTimer==0)
		{
			APServertimerflag=AP_APSERVER_OFF;
			APServerTimer=apServer_apAgent_lost_heart_wait_time;
		}
		break;
		case AP_APSERVER_ON:
		APServerTimer--;
		if(APServerTimer==0)
		{
			APServertimerflag=AP_APSERVER_OFF;
			APServerTimer=apServer_apAgent_lost_heart_wait_time;
		}
		break;

		default:
		printf("undefined state of APServertimerflag");

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


int main()
{

	int taskID = 0, temp = 0;
	pthread_t tid;




	init_sigaction();

    init_time();


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

//	strcpy(ap_configuration_status.AP_SN,ap_serial);
//	ap_configuration_status.status.antenna_number=6;
//	ap_configuration_status.status.model=0;
//	strcpy(ap_configuration_status.status.version,"1.03");
//	ap_configuration_status.status.user_number=5;
    getUpdate();
	printf("APSerial=%s\n",ap_configuration_status.AP_SN);
	read_AP_Server_configure_file("./apserver.txt");
	printf("APServerIP=%s\n",AP_Server_IP);



	taskID = pthread_create(&tid, NULL, (void *)socket_with_APServer, (void *)(&(temp)));
	printf("%d",taskID);

	sleep(1);
	

	if(read_ap_file(ap_local_file)==-1)
	{
		if(read_ap_file(ap_remote_file)==-1)
			{
			log_info("Info: no local configuration,start with AP_APSERVER_REGISTER_REQ_WITHOUT_CONFIGURATION\n");
			send_to_APServer(AP_APSERVER_REGISTER_REQ_WITHOUT_CONFIGURATION);
			}
		else
			{
			send_to_APServer(AP_APSERVER_REGISTER_REQ_WITH_CONFIGURATION);
			log_info("Info: no remote configuration,start with remote configuration\n");
			}
	}
	else
		{
		send_to_APServer(AP_APSERVER_REGISTER_REQ_WITH_CONFIGURATION);
		write_log_file("Info: start with remote configuration\n");
		log_info("Info: start with local configuration\n");
		}
	printf("APSerial=%s\n",ap_configuration_status.AP_SN);

	while(1)
	{
	}

    return 0;


}



