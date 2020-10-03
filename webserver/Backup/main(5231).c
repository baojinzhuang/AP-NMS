/*
 auth:	Baojinzhuang
 date:	2019-12-10
 history:
 main.c
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<ctype.h>
#include<mysql/mysql.h>
#include"function.h"

 //全局变量
int flag_begin = 0;
int flag_end = 0;
int g_level = 0;
AP_Configuration_template apTemplate[max_template];
AP ap_list[max_ap];
AP temp_aplist[max_ap]; //global
int legalTemplate[max_template];
int legalAP[max_ap];
int legalAPServerConfigureResult[max_ap]; //0，未配置，1，配置成功，2，配置失败，3，配置超时


APServer_timer legalAPServerTimer[max_ap_server];


//总定时器
Gather_timer SumTimer[5];



//mysqls
char sql_insert[1024];
MYSQL mysql;
MYSQL* conn = &mysql;
MYSQL_RES resultt;
MYSQL_RES* result = &resultt;
MYSQL_ROW row;
MYSQL_ROW row1;
MYSQL_RES r;
MYSQL_RES* result1 = &r;
//num
int Configure_Nums = 0;
int Temp_GetLog_Nums = 0;
int SoftWare_Nums = 0;
int GetLog_Nums = 0;
int Num_i1 = 0;
int Num_i2 =0;
int Num_i3 = 0;
char Local_Save_AP_LOG_PATH[50] = "/var/";
//ack
template_operation_ack Template_Ack[max_template];
legalAP_operation_ack legalAP_Ack[max_ap];
AP_configuration_ack AP_Configure_Ack[max_ap];
AP_upgrade_ack legalAP_upgrade_Ack[max_ap];
AP_log_ack legalAP_log_Ack[max_ap];

//socket with APServer
APServer_socket apServer_socket_list[max_ap_server];
int sockSer;
struct sockaddr_in addrSer;  //创建一个记录地址信息的结构体
socklen_t addrlen = sizeof(struct sockaddr);
char Web_Server_IP[50] = "0.0.0.0";//从配置中读取
char cleardate[50] = "2020.12.31";
char template_log[256 * 20] = "Apserver_ID:";
char template_temp[500] = "Ack:";
char template_tempindex[500];
char sendapserverid[500];
char reciveapserverid[500];
char reciveupgradecontent[5000];
char reciveapack[500];

char Webserver_allaps[][50] = { "serial_number", "template_index", "ip_addr",
		"DHCP_Static", "ssid_name", "ssid_passwd", "login_name", "login_passwd",
		"NatOrBridge", "ip_pool_start", "ip_pool_end", "ip_gateway",
		"ip_subnet", "ip_NMSip", "user_number", "mode", "version",
		"antenna_number", "\0" };

char Webserver_alltemplates[][50] = { "ssid_name", "ssid_passwd", "login_name",
		"login_passwd", "NatOrBridge", "ip_pool_start", "ip_pool_end",
		"DHCP_Static", "ip_addr", "ip_gateway", "ip_subnet", "ip_NMSip",
		"ip_WEBip", "options", "\0" };

int Webserver_apMaptable[] = { AP_SERIAL_NUMBER, AP_TEMPLATE_INDEX, AP_IP_ADDR,
		DHCP_OR_STATIC, AP_SSID_NAME, AP_SSID_PASSWORD, AP_LOGIN_NAME,
		AP_LOGIN_PASSWORD,
		AP_NAT_OR_BRIDGE, AP_IP_POOL_START, AP_IP_POOL_END, AP_IP_GATEWAY,
		AP_IP_SUBNET, AP_IP_NMS_IP, AP_USER_NUMBER, AP_MODE, AP_VERSION,
		AP_ANTENNA_NUMBER };
int Webserver_templateMaptable[] = { TEMPLATE_SSID_NAME, TEMPLATE_SSID_PASSWORD,
		TEMPLATE_LOGIN_NAME, TEMPLATE_LOGIN_PASSWORD, TEMPLATE_NAT_OR_BRIDGE,
		TEMPLATE_IP_POOL_START, TEMPLATE_IP_POOL_END, TEMPLATE_DHCP_OR_STATIC,
		TEMPLATE_IP_ADDR, TEMPLATE_IP_GATEWAY, TEMPLATE_IP_SUBNET,
		TEMPLATE_IP_NMS_IP, TEMPLATE_IP_WEB_IP, TEMPLATE_OPTIONS };

int print_log(unsigned int level, const char* filename, unsigned int line,
	char* format, ...);
int read_template_set();
int read_AP_set();
int LegalAPServer_find(struct sockaddr_in* apserver_socket_addr);
void dispatch_APServerMsg(int ap_index, char* buf);
void send_to_apServer(int apServerIndex, int command,
	AP_Configuration_template* tempTemplate,
	template_operation_req* templateOperation,
	legalAP_operation_req* APOperation, int size, char*);
int mysql_connect();
void Fill_APServer_socket_list();
int Fill_apTemplate();
int Fill_ap_list();
int control_command_to_APserver();
void add_quotation(char* s, char* APset, char* filepath, char* version);
void AP_sort(char* APset, int command, int size, char* username, char* pwd,
	char* filepath, char* version);
void Send_to_AP_Configure(char* APset, int command, int size);
void print_info(void);
int read_Web_Server_Configure_file(const char* filename);

int read_clear_control_file(const char* filename);

void* socket_with_APServer(int* apIndex) {
	pthread_detach(pthread_self());

	int apServer_index = 0;

	//创建一个套接字，并检测是否创建成功
	sockSer = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockSer == -1)
		perror("socket");

	addrSer.sin_family = AF_INET;    //使用AF_INET协议族
	addrSer.sin_port = htons(5050);     //设置地址结构体中的端口号
	addrSer.sin_addr.s_addr = inet_addr(Web_Server_IP);  //设置通信ip

	//将套接字地址与所创建的套接字号联系起来，并检测是否绑定成功

	int res = bind(sockSer, (struct sockaddr*)&addrSer, addrlen);

	if (res == -1)
		perror("bind");

	//char sendbuf[256];    //申请一个发送数据缓存区
	char recvbuf[bufsize_apToWeb2];    //申请一个接收数据缓存区
	struct sockaddr_in addrCli;
	while (1)  //服务器一直循环接受客户端的请求
	{
		if (recvfrom(sockSer, recvbuf, bufsize_apToWeb2, 0,
			(struct sockaddr*)&addrCli, &addrlen) == -1)
			log_error("Error: receive error from ap server!\n");
		else {
			apServer_index = LegalAPServer_find(&addrCli);
			if (apServer_index != -1) {
				apServer_socket_list[apServer_index].addrAP = addrCli;
				apServer_socket_list[apServer_index].addrlen = addrlen;
				dispatch_APServerMsg(apServer_index, recvbuf);

			}
			else {
				log_error("一个未知的AS，Error: unconfigured APServer!\n");
			}
		}

	}
	return 0;

	pthread_exit(0);
}

void prompt_info()

{
	//printf("%d\n",signo);
	control_command_to_APserver();

	int i = 0;
	int	j =0;
	int ret = -1;
	//SumTimerHandle
	for(j = 0;j<5;j++){
	switch(SumTimer[j].flag)
		{
	case 0:
		SumTimer[j].timer--;
		if(SumTimer[j].timer==0){
			write_template_log();
			flag_begin = 0;
		}
		break;
	case 1:
		SumTimer[j].timer--;
		if(SumTimer[j].timer==0){
			write_ap_log();
			flag_begin = 0;
				}
		break;
	case 2:
		SumTimer[j].timer--;
		if(SumTimer[j].timer==0){
			write_configure_log();
			flag_begin = 0;
			
			}
		break;
	case 3:
		SumTimer[j].timer--;
		if(SumTimer[j].timer==0){
			write_software_log();
			flag_begin = 0;
			}
		break;
	case 4:
		SumTimer[j].timer--;
		if(SumTimer[j].timer==0){
			write_getlog_log();
			flag_begin = 0;
			}
		break;
			
	}
		}
	
	//apServerTimerHandle  //nearly no use
	for (i = 0; i < max_ap_server; i++) {
		switch (legalAPServerTimer[i].apServer_timer_flag) {
		case WEB_APSERVER_OFF:
			break;

		case WEB_APSERVER_CONNECTING:
			legalAPServerTimer[i].apServer_on_timer--;
			if (legalAPServerTimer[i].apServer_on_timer == 0) {
				legalAPServerTimer[i].apServer_timer_flag = WEB_APSERVER_OFF;
				//renew status  done
				snprintf(sql_insert,512,"update AP set state = 0 where APServer_IP = '%s'",inet_ntoa((&apServer_socket_list[i])->addrAP.sin_addr));
				ret = mysql_query(conn, sql_insert);
				printf("APServer off update AP state : %s\n",sql_insert);
				if (ret != 0) {
				printf("mysql_query error is:%s\n", mysql_error(&mysql));
				}
				
				}
			break;
			
		case WEB_APSERVER_ON:
			legalAPServerTimer[i].heart_beat_timer--;//每隔3s发送一次心跳,如果掉线就更新状态
			if (legalAPServerTimer[i].heart_beat_timer == 0) {
				printf("********************Heart beat****************\n\n");
				
				send_to_apServer(i, APSERVER_WEB_HEART_BEAT, NULL, NULL, NULL,1, NULL);
				legalAPServerTimer[i].heart_beat_timer =web_apServer_heart_time;
				legalAPServerTimer[i].heart_beat_timeout_times++;
				if (legalAPServerTimer[i].heart_beat_timeout_times == 3) {
				legalAPServerTimer[i].apServer_timer_flag =WEB_APSERVER_OFF;
				snprintf(sql_insert,512,"update AP set state = 0 where APServer_IP = '%s'",inet_ntoa((&apServer_socket_list[i])->addrAP.sin_addr));
				ret = mysql_query(conn, sql_insert);
				printf("APServer off update AP state : %s\n",sql_insert);
				if (ret != 0) {
				printf("mysql_query error is:%s\n", mysql_error(&mysql));
				}
				
				}
			}
			
			switch (legalAPServerTimer[i].apServer_on_timer_flag) {
			case 0:	//idle
				break;
			
			case 1:	//configuration
				legalAPServerTimer[i].apServer_on_timer--;
				if (legalAPServerTimer[i].apServer_on_timer == 0) {
					//configure time out, give response to database
					legalAPServerTimer[i].apServer_on_timer_flag = 0;
				}
				break;
				
			case 2:	//template modify
				break;
				
			case 3:	//AP modify
				legalAPServerTimer[i].apServer_on_timer--;
				if (legalAPServerTimer[i].apServer_on_timer == 0) {
					//AP modify time out, give response to database
					legalAPServerTimer[i].apServer_on_timer_flag = 0;
				}
				break;
				
			case 4:	//AP upgrade
				legalAPServerTimer[i].apServer_on_timer--;
				printf("legalAPServerTimer[%d].apServer_on_timer is %d\n", i, legalAPServerTimer[i].apServer_on_timer);
				if (legalAPServerTimer[i].apServer_on_timer == 0) {
					legalAPServerTimer[i].apServer_on_timer_flag = 0;
				}
				break;
				
			case 5:	//AP log request
				legalAPServerTimer[i].apServer_on_timer--;
				if (legalAPServerTimer[i].apServer_on_timer == 0) {
					legalAPServerTimer[i].apServer_on_timer_flag = 0;
				}
				break;
				
			case 6:	//AP configure request
				legalAPServerTimer[i].apServer_on_timer--;
				if (legalAPServerTimer[i].apServer_on_timer == 0) {

					legalAPServerTimer[i].apServer_on_timer_flag = 0;
			
				}
				break;
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

	value.it_value.tv_sec = 2;

	value.it_value.tv_usec = 0;

	/*设定初始时间计数也为2秒0微秒*/

	value.it_interval = value.it_value;

	/*设置计时器ITIMER_REAL*/

	setitimer(ITIMER_REAL, &value, NULL);

}
void prompt_control_txt() {
	char s[512];
	int status;
	snprintf(s, 512,
		"mysql -h 127.0.0.1 -u root -pxlh123xlh 2>/dev/null -P 3306 -D nmsdatabase -e 'select * from control;' > /var/control.txt");
	//printf("s is : %s\n", s);
	status = system(s);
	if (-1 == status) {
		printf("control.txt save error\n");
	}
	if (!WIFEXITED(status)) {
		printf("control.txt save error\n");
	}
	if (WEXITSTATUS(status)) {
		printf("control.txt save error\n");
	}
	else {
		printf("control.txt save success\n");
	}

}
// 建立信号处理机制

void controltxt_sigaction(void)

{

	struct sigaction cact;

	/*信号到了要执行的任务处理函数为prompt_info*/

	cact.sa_handler = prompt_control_txt;

	cact.sa_flags = 14;

	/*初始化信号集*/

	sigemptyset(&cact.sa_mask);

	/*建立信号处理机制*/

	sigaction(SIGALRM, &cact, NULL);

}

void controltxt_time()

{

	struct itimerval value;

	/*设定执行任务的时间间隔为2秒0微秒*/

	value.it_value.tv_sec = 5;

	value.it_value.tv_usec = 0;

	/*设定初始时间计数也为2秒0微秒*/

	value.it_interval = value.it_value;

	/*设置计时器ITIMER_REAL*/

	setitimer(ITIMER_REAL, &value, NULL);

}
void prompt_control_del() {
	char s[100];
	int ret = -1;
	int status;
	char sdate[50];
	char sname[50];
	time_t t;
	struct tm* lt;
	time(&t);	//获取Unix时间戳。
	lt = localtime(&t);	//转为时间结构。
	sprintf(sdate, "%d.%d.%d", lt->tm_year + 1900, lt->tm_mon, lt->tm_mday);
	sprintf(sname, "%d%d%d", lt->tm_year + 1900, lt->tm_mon, lt->tm_mday);
	printf("sdate is %s\n", sdate);
	printf("cleardate is %s\n", cleardate);
	if (strcmp(sdate, cleardate) == 0) {
		snprintf(s, 512, "cp -r /var/control.txt /home/xlh/'%s'control.txt",
			sname);
		status = system(s);
		if (-1 == status) {
			printf("control.txt save error\n");
		}
		if (!WIFEXITED(status)) {
			printf("control.txt save error\n");
		}
		if (WEXITSTATUS(status)) {
			printf("control.txt save error\n");
		}
		else {
			printf("control.txt success\n");
			snprintf(sql_insert, 512, "DELETE FROM control");
			ret = mysql_query(conn, sql_insert);
			if (ret != 0) {
				printf("mysql_query error is:%s\n", mysql_error(&mysql));
			}

		}
	}

	else {
		printf("it is not time to clear the table of control \n");
		log_error("it is not time to clear the table of control \n");

	}
}
// 建立信号处理机制

void controldel_sigaction(void)

{

	struct sigaction dact;

	/*信号到了要执行的任务处理函数为prompt_info*/

	dact.sa_handler = prompt_control_del;

	dact.sa_flags = 14;

	/*初始化信号集*/

	sigemptyset(&dact.sa_mask);

	/*建立信号处理机制*/

	sigaction(SIGALRM, &dact, NULL);

}

void controldel_time()

{

	struct itimerval value;

	/*设定执行任务的时间间隔为2秒0微秒*/

	value.it_value.tv_sec = 10;	//86400

	value.it_value.tv_usec = 0;

	/*设定初始时间计数也为2秒0微秒*/

	value.it_interval = value.it_value;

	/*设置计时器ITIMER_REAL*/

	setitimer(ITIMER_REAL, &value, NULL);

}

void init_legalTemplate_legalAP(void)

{
	int i = 0;

	for (i = 0; i < max_template; i++)
		legalTemplate[i] = 0;
	for (i = 0; i < max_ap; i++)
		legalAP[i] = 0;
	for (i = 0; i < max_ap_server; i++) {
		legalAPServerTimer[i].apServer_timer_flag = 0;
		legalAPServerTimer[i].apServer_on_timer = 0;
		legalAPServerTimer[i].heart_beat_timer = 0;
		legalAPServerTimer[i].heart_beat_timeout_times = 0;
		legalAPServerTimer[i].apServer_on_timer_flag = 0;
		legalAPServerConfigureResult[i] = 0;
	}

}

struct itimerval one_timer;

struct timers

{

	int interval; //定时时间

	void (*handler)(); //处理函数

};

struct timers two_timer;

struct timers five_timer;

struct timers ten_timer;

void multi_timer_manage()

{

	//printf("\n---");

	two_timer.interval--;

	if (two_timer.interval == 0)

	{

		two_timer.interval = 2;

		two_timer.handler();

	}

	five_timer.interval--;

	if (five_timer.interval == 0)

	{

		five_timer.interval = 300;

		five_timer.handler();

	}

	ten_timer.interval--;

	if (ten_timer.interval == 0)

	{

		ten_timer.interval = 300000;

		ten_timer.handler();

	}

}

void init()

{

	one_timer.it_interval.tv_sec = 1; //设置单位定时器定时时间

	one_timer.it_value.tv_sec = 1; //设置单位定时器初始值

	setitimer(ITIMER_REAL, &one_timer, NULL); //初始化单位定时器

	signal(SIGALRM, multi_timer_manage); //指定单位定时器定时时间到时执行的函数

	two_timer.interval = 2;

	two_timer.handler = prompt_info;

	five_timer.interval = 300;

	five_timer.handler = prompt_control_txt;

	ten_timer.interval = 300000;

	ten_timer.handler = prompt_control_del;

}

void uninit_time()

{

	struct itimerval value;

	value.it_value.tv_sec = 0;

	value.it_value.tv_usec = 0;

	value.it_interval = value.it_value;

	setitimer(ITIMER_REAL, &value, NULL);

}

int main() {

	mysql_connect();
	int taskID = 0, temp = 0;
	int a = 0, b = 0;

	pthread_t tid;

	init_legalTemplate_legalAP();

	//add read database here:configured APServer to apServer_socket_list     APServer_socket apServer_socket_list[max_ap_server];
	Fill_APServer_socket_list();

	//add read database here:configured template to apTemplate and legalTemplate   AP_Configuration_template apTemplate[max_template];   legalTemplate[max_template];
	a = Fill_apTemplate();
	for (b = 0; b < a; b++) {
		legalTemplate[apTemplate[b].templateIndex] = 1;
	}
	//add read database here:configured AP to ap_list and legalAP
	a = Fill_ap_list();
	for (b = 0; b < a; b++) {
		legalAP[b] = 1;
	}

	init();

	print_info();

	read_Web_Server_Configure_file("../webserver.txt");
	read_clear_control_file("../clearcontrol.txt");
	printf("WebServer IP:%s\n",Web_Server_IP);
	
	taskID = pthread_create(&tid, NULL, (void*)socket_with_APServer,
		(void*)(&(temp)));
	printf("%d", taskID);

	while (1) {
	}
	mysql_close(conn);
	return 0;

}


