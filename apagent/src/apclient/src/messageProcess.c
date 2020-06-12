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

extern AP ap_configuration_status;
extern AP_timer legalAPTimer;

extern int sockAPServer;
extern struct sockaddr_in addrAPSer;  //创建一个记录地址信息的结构体 
extern socklen_t addrAPServerlen;


extern int APServertimerflag; //0,不启动，1,启动中，2,正常通信中
extern int APServerTimer;


int print_log(unsigned int level, const char* filename, unsigned int line, char* format, ...);
void write_ap_configure_file(const char *filename);
int read_ap_file(const char *filename);
int getUpdate();
void setAPStatus();
void setAPStatus1();

void send_apToServer_message(char* sendbuf)
{
	char* p=sendbuf;
	p++;
	memcpy(p,(char*)&ap_configuration_status,sizeof(AP));
	sendto(sockAPServer,sendbuf,bufsize_APToServer,0,(struct sockaddr*)&addrAPSer, addrAPServerlen); 
}

void send_to_APServer(char command)
{
	char sendbuf[bufsize_APToServer]={"\0"};
	sendbuf[0]=command;


	switch(command)
	{
		case AP_APSERVER_REGISTER_REQ_WITHOUT_CONFIGURATION:
		send_apToServer_message(sendbuf);
		APServertimerflag = 1;
		APServerTimer = apServer_apAgent_heart_time;
		break;
		case AP_APSERVER_REGISTER_REQ_WITH_CONFIGURATION:
		send_apToServer_message(sendbuf);
		APServertimerflag = 1;
		APServerTimer = apServer_apAgent_heart_time;
		break;
		case AP_APSERVER_REGISTER_COMPLETE:
		send_apToServer_message(sendbuf);
		APServertimerflag = 1;
		APServerTimer = apServer_apAgent_heart_time;
		break;
		case AP_APSERVER_CONFIGURE_OK:
		send_apToServer_message(sendbuf);
		break;
		case AP_APSERVER_CONFIGURE_NOK:
		send_apToServer_message(sendbuf);
		log_error("Error: AP_APSERVER_CONFIGURE_NOK\n");
		break;
		case AP_APSERVER_HEART_BEAT_ACK:
		send_apToServer_message(sendbuf);
		APServertimerflag = 2;
		APServerTimer = apServer_apAgent_heart_time;
		break;
		case AP_APSERVER_HEART_BEAT_ACK_WITH_STATUS:
		send_apToServer_message(sendbuf);
		APServertimerflag = 2;
		APServerTimer = apServer_apAgent_heart_time;
		break;
	
		default:
		printf("undefined command AP to AP Server");
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

	ap_configure_assign_value(&ap_configuration_status,pp);
}

void  dispatch_APServerMsg(char* buf)
{


	switch(buf[0])
	{
	case APSERVER_AP_REGISTER_ACK:
		log_info("Info: Receive APSERVER_AP_REGISTER_ACK\n");
		decode_APServer_message(buf);
		write_ap_configure_file(ap_remote_file);

		setAPStatus();

		sleep(8);
		printf("sleep ok");
		send_to_APServer(AP_APSERVER_REGISTER_COMPLETE);
		log_info("Info: AP_APSERVER_REGISTER_COMPLETE\n");
        break;
	case APSERVER_AP_REGISTER_REJ:
		APServertimerflag = 0;
		APServerTimer = apServer_apAgent_reject_wait_time;
		log_error("Error: Reject SN = %s!\n", ap_configuration_status.AP_SN);
        break;
	case APSERVER_AP_CONFIGURE:
		decode_APServer_message(buf);


		write_ap_configure_file(ap_local_file);
		setAPStatus();

		sleep(8);
		send_to_APServer(AP_APSERVER_CONFIGURE_OK);
		log_info("Info: AP_APSERVER_CONFIGURE_OK\n");
        break;
	case APSERVER_AP_HEART_BEAT:

		getUpdate();
		send_to_APServer(AP_APSERVER_HEART_BEAT_ACK);
		log_info("Info: AP_APSERVER_HEART_BEAT_ACK\n");
        break;

	default:
		//log_error("Error: Unknown protocol type = %d!\n", envelop->bProtocolSap);
		break;
	}


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

