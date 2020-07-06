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
extern char software_remote_file[50];

extern char sh_log_put[50];
extern char sh_software_upgrade[50];

extern char AP_Server_IP[50];

extern AP ap_configuration_status;
extern AP_timer legalAPTimer;

extern int sockAPServer;
extern struct sockaddr_in addrAPSer;  //创建一个记录地址信息的结构体 
extern socklen_t addrAPServerlen;


extern int APServertimerflag; //0,不启动，1,启动中，2,正常通信中
extern int APServerTimer;

int print_log(unsigned int level, const char* filename, unsigned int line, char* format, ...);
void write_ap_configure_file(const char *filename);
void decode_APServer_log_message(char* buf);
void decode_APServer_upgrade_message(char* buf);

void send_apToServer_message(char* sendbuf)
{
	char* p=sendbuf;
	p++;
	memcpy(p,(char*)&ap_configuration_status,sizeof(AP));
	printf("AP:%s's antenna number is %d\n",ap_configuration_status.AP_SN,ap_configuration_status.status.antenna_number);
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
	
		case AP_APSERVER_REQUEST_LOGINFO_ACK:
		send_apToServer_message(sendbuf);
		APServertimerflag = 5;
		APServerTimer = apServer_apAgent_log_get_time;

		default:
		printf("undefined command AP to AP Server");
	}
	
}


void ap_configure_assign_value(AP* destination,AP_Configuration_template* source)
{
	destination->configure.ap_gateway.gateway=source->ap_gateway.gateway;
    destination->configure.ap_gateway.subnetmask=source->ap_gateway.subnetmask;
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
		send_to_APServer(AP_APSERVER_CONFIGURE_OK);
		log_info("Info: AP_APSERVER_CONFIGURE_OK\n");
        break;
	case APSERVER_AP_HEART_BEAT:
		send_to_APServer(AP_APSERVER_HEART_BEAT_ACK);
		log_info("Info: AP_APSERVER_HEART_BEAT_ACK\n");
        break;

	case APSERVER_AP_SOFTWARE_UPGRADE:

		decode_APServer_upgrade_message(buf);
		break;
	case APSERVER_AP_REQUEST_LOGINFO:

		decode_APServer_log_message(buf);
		send_to_APServer(AP_APSERVER_REQUEST_LOGINFO);
		log_info("Info: AP_APSERVER_REQUEST_LOGINFO_ACK\n");
		break;
	default:
		//log_error("Error: Unknown protocol type = %d!\n", envelop->bProtocolSap);
		break;
	}


}

void decode_APServer_log_message(char* buf){

	char buf1[1024];
	char input_id[1024];
	char* p=buf;
	p++;
	AP_Configuration_template *pp = (AP_Configuration_template*) p;
	log_info("Info: APSERVER_AP_LOG_MESSAGES\n");

	FILE * p_file = NULL;

	sprintf(input_id,"%s %s %s %s",sh_log_put,AP_Server_IP,pp->Remote_FilePath,log_file);
	p_file = popen("%s", "r");
	if (!p_file) {
		fprintf(stderr, "Erro to popen");
	}

	while (fgets(buf1, 1024, p_file) != NULL) {
		fprintf(stdout, "%s", buf);
	}
	pclose(p_file);


}


void decode_APServer_upgrade_message(char* buf){

	char buf1[1024];
	char input_id[1024];
	char* p=buf;
	p++;
	AP_Configuration_template *pp = (AP_Configuration_template*) p;
	log_info("Info: APSERVER_AP_SOFTWARE_UPGRADE\n");

	FILE * p_file = NULL;

	sprintf(input_id,"%s %s %s %s%s",sh_software_upgrade,AP_Server_IP,pp->Remote_FilePath,software_remote_file,pp->Remote_FilePath);
	p_file = popen("%s", "r");
	if (!p_file) {
		fprintf(stderr, "Erro to popen");
	}

	while (fgets(buf1, 1024, p_file) != NULL) {
		fprintf(stdout, "%s", buf);
	}
	pclose(p_file);

	//终止当前程序　重新启动

}


