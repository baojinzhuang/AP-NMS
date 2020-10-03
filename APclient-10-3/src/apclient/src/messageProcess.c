/*
auth:	Baojinzhuang
date:	2020-01-07
history:	
*/

#include<stdio.h>
#include<stdlib.h> 
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<stdarg.h>
#include<ctype.h>

#include"define.h"

extern char ap_remote_file[50];
extern char ap_local_file[50];
extern char log_file[50];
extern char log_file1[50];
extern char software_remote_file[50];


extern char sh_log_put[50];
extern char sh_software_upgrade[50];

extern char AP_Server_IP[50];

extern AP ap_configuration_status;
extern AP_timer legalAPTimer;

extern int sockAPServer;
extern struct sockaddr_in addrAPSer;  //创建一个记录地址信息的结构体 
extern socklen_t addrAPServerlen;


extern int APServertimerflag; //0,不启动，1,启动中，2,正常通信中,4 软件升级，５去日志
extern int APServerTimer;


int print_log(unsigned int level, const char* filename, unsigned int line, char* format, ...);
void write_ap_configure_file(const char *filename);
int read_ap_file(const char *filename);
int getUpdate();
void setAPStatus();
void setAPStatus1();
void decode_APServer_log_message(char* buf);
void decode_APServer_upgrade_message(char* buf);


void send_apToServer_message(char* sendbuf)//上报sendbuf，顺便将AP的信息，序列号，天线数量上报等
{
	char* p=sendbuf;
	p++;
	memcpy(p,(char*)&ap_configuration_status,sizeof(AP));
	printf("AP:%s's antenna number is %d\n",ap_configuration_status.AP_SN,ap_configuration_status.status.antenna_number);
	sendto(sockAPServer,sendbuf,bufsize_APToServer,0,(struct sockaddr*)&addrAPSer, addrAPServerlen); 
}

void send_to_APServer(char command)//返回command  +            AP_info
{
	char sendbuf[bufsize_APToServer]={"\0"};
	sendbuf[0]=command;


	switch(command)
	{
		case AP_APSERVER_REGISTER_REQ_WITHOUT_CONFIGURATION:
		send_apToServer_message(sendbuf);
		APServertimerflag = 1; //connect
		APServerTimer = apServer_apAgent_heart_time;//3s
		break;
		case AP_APSERVER_REGISTER_REQ_WITH_CONFIGURATION:
		send_apToServer_message(sendbuf);
		APServertimerflag = 1;
		APServerTimer = apServer_apAgent_heart_time;//3s
		break;
		case AP_APSERVER_REGISTER_COMPLETE:
		send_apToServer_message(sendbuf);
		APServertimerflag = 1;
		APServerTimer = apServer_apAgent_heart_time;
		break;
		case AP_APSERVER_CONFIGURE_OK://配置成功上报
		send_apToServer_message(sendbuf);
		break;
		case AP_APSERVER_CONFIGURE_NOK:
		send_apToServer_message(sendbuf);
		log_error("Error: AP_APSERVER_CONFIGURE_NOK\n");
		printf("Error: AP_APSERVER_CONFIGURE_NOK\n");
		break;
		case AP_APSERVER_HEART_BEAT_ACK://回复WS的心跳
		send_apToServer_message(sendbuf);//上报状态
		APServertimerflag = 2;//正常通信中
		APServerTimer = apServer_apAgent_heart_time;
		break;
		case AP_APSERVER_HEART_BEAT_ACK_WITH_STATUS://携带状态信息通过心跳上报，一举两得
		send_apToServer_message(sendbuf);
		APServertimerflag = 2;//正常通信中
		APServerTimer = apServer_apAgent_heart_time;
		break;
	
		case AP_APSERVER_REQUEST_LOGINFO_ACK://请求日志确认
		APServertimerflag = 5;
		send_apToServer_message(sendbuf);
		break;

		default:
		printf("Undefined command AP to AP Server");
	}
	
}


void ap_configure_assign_value(AP* destination,AP_Configuration_template* source)
{

	destination->configure.ap_gateway.gateway=source->ap_gateway.gateway;
    destination->configure.ap_gateway.subnetmask.s_addr=inet_addr("255.255.255.0");
	destination->configure.DHCPOrStatic=source->DHCPOrStatic;
	destination->configure.AP_IP=source->AP_IP;
	strcpy(destination->configure.ap_login.login,source->ap_login.login);
	strcpy(destination->configure.ap_login.login_psw,source->ap_login.login_psw);
	strcpy(destination->configure.ap_ssid.ssid,source->ap_ssid.ssid);
	strcpy(destination->configure.ap_ssid.ssid_psw,source->ap_ssid.ssid_psw);
	destination->configure.NatOrBridge = source->NatOrBridge;
	destination->configure.pool.start = source->pool.start;
	destination->configure.pool.end = source->pool.end;
	destination->configure.AP_Server_IP=source->AP_Server_IP;
	strcpy(destination->configure.Remote_FilePath, source->Remote_FilePath);
	strcpy(destination->configure.Scp_Username, source->Scp_Username);
	strcpy(destination->configure.Scp_PassWord, source->Scp_PassWord);

}

void ap_status_assign_value(AP* destination,AP* source)
{

	destination->status.antenna_number = source->status.antenna_number;
	destination->status.model = source->status.model;
	destination->status.user_number = source->status.user_number;
	strcpy(destination->status.version,source->status.version);
}


void decode_APServer_message(char* buf)
{
	char* p=buf;
	p++;
	AP_Configuration_template*   pp=(AP_Configuration_template*)p;

	ap_configure_assign_value(&ap_configuration_status,pp);//更新AP的模板信息
}

void  dispatch_APServerMsg(char* buf)//处理APServer的消息
{


	switch(buf[0])//判断command
	{
	case APSERVER_AP_REGISTER_ACK:
		log_info("Info: Receive APSERVER_AP_REGISTER_ACK\n");
		decode_APServer_message(buf);
		write_ap_configure_file(ap_remote_file);

		setAPStatus();

		sleep(28);
		printf("sleep ok\n");
		send_to_APServer(AP_APSERVER_REGISTER_COMPLETE);
		log_info("Info: AP_APSERVER_REGISTER_COMPLETE\n");
        break;
	case APSERVER_AP_REGISTER_REJ://注册拒绝
		APServertimerflag = 0;
		APServerTimer = apServer_apAgent_reject_wait_time;//24h后，发起重连
		log_error("Error: Reject SN = %s!\n", ap_configuration_status.AP_SN);
		printf("Error: Reject SN = %s!\n", ap_configuration_status.AP_SN);
        break;
	case APSERVER_AP_CONFIGURE://处理配置命令
		decode_APServer_message(buf);//更新AP的模板信息，跟新AP的配置

		write_ap_configure_file(ap_local_file);//将接收到的配置信息写入ap_loacl文件

		APServerTimer=apServer_apAgent_configure_time;//21s

		setAPStatus();//将打印配置信息并将配置信息写入开发板，大概需要28s

		sleep(28);
		printf("sleep ok\n");

		send_to_APServer(AP_APSERVER_CONFIGURE_OK);
		log_info("Info: AP_APSERVER_CONFIGURE_OK\n");
		printf("Info: AP_APSERVER_CONFIGURE_OK\n");
        break;
	case APSERVER_AP_HEART_BEAT:
		
		getUpdate();//获取AP的信息
		send_to_APServer(AP_APSERVER_HEART_BEAT_ACK);
		log_info("Info: AP_APSERVER_HEART_BEAT_ACK\n");
        break;
	case APSERVER_AP_SOFTWARE_UPGRADE:

		decode_APServer_upgrade_message(buf);//解析升级的消息
	    break;
	case APSERVER_AP_REQUEST_LOGINFO:

		getUpdate();
		decode_APServer_log_message(buf);//解析日志消息，并且利用tftp上传日志
		send_to_APServer(AP_APSERVER_REQUEST_LOGINFO_ACK);//返回确认信息
		log_info("Info: AP_APSERVER_REQUEST_LOGINFO_ACK\n");
		printf("Info: AP_APSERVER_REQUEST_LOGINFO_ACK\n");
		break;

	default:
		log_error("Error: Unknown protocol type = %d!\n", buf[0]);
		break;
	}


}

void decode_APServer_log_message(char* buf){//解析日志消息，并且利用tftp上传日志

	char input_id[1024];

	char* p=buf;
	char remotefile[100] = "/";
	strcat(remotefile,ap_configuration_status.configure.ap_ssid.ssid);
	strcat(remotefile,".txt");//remotefile: /172111.txt
	p++;

	AP_Configuration_template *pp = (AP_Configuration_template*) p;
	
	log_info("Info: APSERVER_AP_LOG_MESSAGES\n");
	printf("Info: APSERVER_AP_LOG_MESSAGES\n");
	
		
	
	printf("tftp -p -r %s%s -l  %s %s\n",pp->Remote_FilePath,remotefile,log_file,AP_Server_IP);

	sprintf(input_id,"tftp -p -r %s%s -l  %s %s\n",pp->Remote_FilePath,remotefile,log_file,AP_Server_IP);

	int status = system(input_id);//将日志传给AS

	if (-1 == status) {

		printf("get log error\n");

	}

	 if (!WIFEXITED(status)) {

		printf("get log error\n");

	}

	 if (WEXITSTATUS(status)) {

		printf("get log error\n");

	}

	else

	{

		printf("get log  success\n");

	}
}





void decode_APServer_upgrade_message(char* buf){

	char input_id[1024];

	char* p=buf;

	p++;

	AP_Configuration_template *pp = (AP_Configuration_template*) p;

	log_info("Info: APSERVER_AP_SOFTWARE_UPGRADE\n");


	printf("sh_software_upgrade：%s，AP_Server_IP：%s，远程路径：%s，software_remote_file：%s\n",sh_software_upgrade,AP_Server_IP,pp->Remote_FilePath,software_remote_file);

	sprintf(input_id,"%s %s %s %s%s",sh_software_upgrade,AP_Server_IP,pp->Remote_FilePath,software_remote_file,pp->Remote_FilePath);


	

	
	int status = system(input_id);//获取升级文件

	if (-1 == status) {

		printf("upgrade error\n");

	}

	 if (!WIFEXITED(status)) {

		printf("upgrade error\n");

	}

	 if (WEXITSTATUS(status)) {

		printf("upgrade error\n");

	}

	else

	{

		printf("upgrade success\n");

	}



	//终止当前程序　重新启动



}



//void setAPStatus()
//{
//	char input_id[100];
//	int back;
//
//	sprintf(input_id,"nvram set wl0_ssid=%s",ap_configuration_status.configure.ap_ssid.ssid);
//	back=system(input_id);
//	if(back!=-1||back!=127)
//		puts("Set ssid Ok");
//	sprintf(input_id,"nvram set wl0_wpa_psk=%s",ap_configuration_status.configure.ap_ssid.ssid_psw);
//	back=system(input_id);
//	if(back!=-1||back!=127)
//		puts("Set ssid_psw Ok");
//
//	sprintf(input_id,"nvram set http_username=%s",ap_configuration_status.configure.ap_login.login);
//	back=system(input_id);
//	if(back!=-1||back!=127)
//		puts("Set loginname Ok");
//	sprintf(input_id,"nvram set http_passwd=%s",ap_configuration_status.configure.ap_login.login_psw);
//	back=system(input_id);
//	if(back!=-1||back!=127)
//		puts("Set loginpsw Ok");
//
//	if(ap_configuration_status.configure.NatOrBridge==1)
//	   sprintf(input_id,"nvram set nat_type=sym");
//	else
//		;
//	back=system(input_id);
//	if(back!=-1||back!=127)
//		puts("Set nat Ok");
//
//	sprintf(input_id,"nvram set dhcp_start=%s",inet_ntoa(ap_configuration_status.configure.pool.start));
//	back=system(input_id);
//	if(back!=-1||back!=127)
//		puts("Set poolstart Ok");
//	sprintf(input_id,"nvram set dhcp_end=%s",inet_ntoa(ap_configuration_status.configure.pool.end));
//	back=system(input_id);
//	if(back!=-1||back!=127)
//		puts("Set poolend Ok");
//
//	sprintf(input_id,"nvram set lan_gateway=%s",inet_ntoa(ap_configuration_status.configure.ap_gateway.gateway));
//	back=system(input_id);
//	if(back!=-1||back!=127)
//		puts("Set gateway Ok");
//	sprintf(input_id,"nvram set lan_netmask=%s",inet_ntoa(ap_configuration_status.configure.ap_gateway.subnetmask));
//	back=system(input_id);
//	if(back!=-1||back!=127)
//		puts("Set submask Ok");
//
//	sprintf(input_id,"nvram commit");
//	back=system(input_id);
//	if(back!=-1||back!=127)
//		puts("commit Ok");
//
//	sprintf(input_id,"nvram commit");
//	back=system(input_id);
//	if(back!=-1||back!=127)
//		puts("commit Ok");
//}
//
//void apStatusUpdate()
//{
//	char input_id[100];
//	int back;
//
//	sprintf(input_id,"/program/Apstatus");
//	back=system(input_id);
//	if(back!=-1||back!=127)
//		puts("finish");
//
//	read_ap_file("/program/result.txt");
//
//
//}

