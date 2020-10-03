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



extern AP_Configuration_template apTemplate[max_template];

extern AP_Configuration_template Configure_Template;

extern AP ap_list[max_ap];

extern int legalTemplate[max_template];

extern int legalAP[max_ap];

extern char legalAPConfigureResult[max_ap];//0��δ���ã�1�����óɹ���2������ʧ�ܣ�3�����ó�ʱ

extern int legalAPUpgradeResult[max_ap];//0����������ɹ���1���������ʧ�ܣ�2�����������ʱ

extern int legalAPLogGetResult[max_ap];//0����ȡ��־�ɹ���1����ȡ��־ʧ�ܣ�2����ȡ��־��ʱ

extern AP_upgrade_ack AP_upgrade_log_ack[max_ap];
extern AP_logs_ack AP_log_ack[max_ap];
extern AP_configuration_ack AP_configure_ack[max_ap];
extern int status;



extern AP_timer legalAPTimer[max_ap];

extern char Loacl_Upgrade_FilePath[50];

extern char LocalUsername[50];

extern char LoaclPassWord[20];

extern char Scp_Exe[1024];

extern char LogFilePath[512];

extern char AP_Server_IP[50];

extern char Web_Server_IP[50];

extern int AP_Upgrade_Num;

extern int AP_GetLog_Num;
extern int AP_Configure_Num;


extern AP_socket ap_socket_list[max_ap];

extern int sockSer;



extern int sockWeb;

extern struct sockaddr_in addrWebSer;  //����һ����¼��ַ��Ϣ�Ľṹ�� 

extern socklen_t addrWeblen;



extern int Webtimerflag; //0,��������1,�����У�2,����ͨ����

extern int WebOntimerflag; //0,����ͨ���У�1,����ͨ�������У�2,����ͨ��������,��3������ͨ�Ż�ȡ��־��

extern int webTimer;

extern int webOnTimer;




void template_report();

void ap_report();

int print_log(unsigned int level, const char* filename, unsigned int line, char* format, ...);

void add_template_file(int template_index);

void write_template_set();

int del_template_file(int template_index);

void modify_template_file(int template_index);

void write_ap_set();

void init_legalTemplate();

void init_legalAP();

void print_templates();

void print_aps();

void upgrade_ack_report();

void log_info_report();

void Create_LogDir(char* filename);

void configure_reprot();

void template_assign_value(AP_Configuration_template* destination, AP_Configuration_template* source);


void decode_web_AP_log_requestMsg(char* buf);



/*command  AP_Configuration_template* tempTemplate*/

void send_to_ap(int apIndex, int command, AP_Configuration_template* tempTemplate)//decode����Web���Ȼ�����ݸ�AP

{

	char sendbuf[sizeof(AP) + 1];    

	char* p = sendbuf;

	AP_Configuration_template* pp = &(apTemplate[ap_list[apIndex].configure.templateIndex]);//指向这个AP的模板信息



	*p = command;//sendbuf传入命令

	p++;


	if (legalAP[apIndex] == 1)

		switch (command)

		{

		case APSERVER_AP_REGISTER_ACK://发送AS存储的AP模板信息给AP
			

			memcpy(p, (char*)pp, sizeof(AP_Configuration_template));

			sendto(sockSer, sendbuf, sizeof(AP_Configuration_template) + 2, 0, (struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen);

			break;

		case APSERVER_AP_REGISTER_REJ://注册拒绝，

			sendto(sockSer, sendbuf, 2, 0, (struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen);

			break;

		case APSERVER_AP_CONFIGURE://command + Template，配置信息
			memcpy(p, tempTemplate, sizeof(AP_Configuration_template));
			sendto(sockSer, sendbuf, sizeof(AP_Configuration_template) + 2, 0, (struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen);
			break;

		case APSERVER_AP_SOFTWARE_UPGRADE:

			memcpy(p, tempTemplate, sizeof(AP_Configuration_template));

			sendto(sockSer, sendbuf, sizeof(AP_Configuration_template) + 2, 0, (struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen);

			break;

		case APSERVER_AP_REQUEST_LOGINFO:

			memcpy(p, tempTemplate, sizeof(AP_Configuration_template));

			sendto(sockSer, sendbuf, sizeof(AP_Configuration_template) + 2, 0, (struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen);

			break;

		case APSERVER_AP_HEART_BEAT:

			sendto(sockSer, sendbuf, 2, 0, (struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen);

			break;

		default:

			printf("undefined command AP Server to AP");

		}

}



void send_to_web(char command, char* sendbuf2, int length)//decode��ʹ��   �صĶ���Ack��

{

	switch (command)

	{

	case APSERVER_WEB_TEMPLATE_REPORT:

		template_report();

		break;

	case APSERVER_WEB_AP_REPORT:

		ap_report();

		break;

	case APSERVER_WEB_TEMPLATE_MODIFY:

		sendto(sockWeb, sendbuf2, length, 0, (struct sockaddr*)&addrWebSer, addrWeblen);

		break;

	case APSERVER_WEB_AP_MODIFY:

		sendto(sockWeb, sendbuf2, length, 0, (struct sockaddr*)&addrWebSer, addrWeblen);

		break;

	case APSERVER_WEB_AP_CONFIGURE:

		configure_reprot();

		break;

	case APSERVER_WEB_SOFTWARE_UPGRADE://�����������ϱ�

		upgrade_ack_report();

		break;

	case APSERVER_WEB_REQUEST_LOGINFO://����ȡ����־��·�����û����������ϱ�

		log_info_report();

		break;

	case APSERVER_WEB_HEART_BEAT:

		sendto(sockWeb, sendbuf2, length, 0, (struct sockaddr*)&addrWebSer, addrWeblen);

		break;



	default:

		printf("undefined command AP Server to WEB\n");

	}



}



void configure_reprot()

{
	int i = 0, size = 0;
	size = AP_Configure_Num;

	AP_configuration_ack* ap_configuration;

	char sendbuf[sizeof(AP_configuration_ack) * max_ap + 2] = { "\0" };

	char* p = sendbuf + 2;
	ap_configuration = (AP_configuration_ack*)p;
	for (i = 0; i < AP_Configure_Num; i++)

		{

			if (AP_configure_ack[i].ack == '0')

			{

				ap_configuration->ack = AP_configure_ack[i].ack;

				strcpy(ap_configuration->AP_SN, AP_configure_ack[i].AP_SN);
				printf("Configure reporter  success %d\n",i);

			}

			else

			{

				ap_configuration->ack = '1';

				strcpy(ap_configuration->AP_SN, AP_configure_ack[i].AP_SN);

			}

			ap_configuration++;

		}
	sendbuf[1] = size;

	sendbuf[0] = APSERVER_WEB_AP_CONFIGURE;

	sendto(sockWeb, sendbuf, size * sizeof(AP_configuration_ack) + 2, 0, (struct sockaddr*)&addrWebSer, addrWeblen);

	printf("configure sendbuf[0]  %d sendbuf[1] %d  sendbuf[2]  %d\n",sendbuf[0],sendbuf[1],sendbuf[2]);
	printf("Enter configure report size=%d\n", size);
	
	AP_Configure_Num = 0;
}



/*     commadn  size AP_upgrade_ack *size    */

void upgrade_ack_report()

{

	int i = 0;

	AP_upgrade_ack* ap_upgrade;

	char sendbuf[sizeof(AP_upgrade_ack) * max_ap + 2] = { "\0" };

	char* p = sendbuf + 2;

	ap_upgrade = (AP_upgrade_ack*)p;

	//AP_upgrade_ack AP_upgrade_log_ack[max_ap]

	for (i = 0; i < AP_Upgrade_Num; i++)

	{

		if (AP_upgrade_log_ack[i].ack == '0')

		{

			ap_upgrade->ack = AP_upgrade_log_ack[i].ack;

			strcpy(ap_upgrade->AP_SN, AP_upgrade_log_ack[i].AP_SN);
			printf("Upgrade reporter 1 success\n");

		}

		else

		{

			ap_upgrade->ack = '1';

			strcpy(ap_upgrade->AP_SN, AP_upgrade_log_ack[i].AP_SN);

		}

		ap_upgrade++;

	}

	sendbuf[1] = AP_Upgrade_Num;

	sendbuf[0] = APSERVER_WEB_SOFTWARE_UPGRADE;

	ap_upgrade = (AP_upgrade_ack*)p;

	printf("upgrade APSN %s ACK %c\n",ap_upgrade->AP_SN,ap_upgrade->ack);
	
	sendto(sockWeb, sendbuf, sizeof(AP_upgrade_ack) * max_ap + 2, 0, (struct sockaddr*)&addrWebSer, addrWeblen);

	printf("enter upgrade report size=%d\n", AP_Upgrade_Num);

	memset(AP_upgrade_log_ack, 0, sizeof(AP_upgrade_log_ack));//����ackȷ����Ϣ��0

	AP_Upgrade_Num = 0;

}



void log_info_report()

{

	int i = 0;

	AP_logs_ack* ap_upgrade;

	char sendbuf[sizeof(AP_logs_ack) * max_ap + 2] = { "\0" };

	char* p = sendbuf + 2;

	ap_upgrade = (AP_logs_ack*)p;

	//AP_upgrade_ack AP_upgrade_log_ack[max_ap]

	for (i = 0; i < AP_GetLog_Num; i++)

	{

		if (AP_log_ack[i].ack == '0')

		{
			printf("Send to Web Log ack is %c,i is %d,AP_SN is  %s\n",AP_log_ack[i].ack,i,AP_log_ack[i].AP_SN);
			ap_upgrade->ack = AP_log_ack[i].ack;
			strcpy(ap_upgrade->AP_SN, AP_log_ack[i].AP_SN);
			strcpy(ap_upgrade->Remote_FilePath, LogFilePath);// /var/lib/tftpboot/log_%dy%dm%dd%dh%dm
			strcpy(ap_upgrade->Scp_Username, "xlh@192.168.0.121");//此句无效
			strcpy(ap_upgrade->Scp_PassWord, "xlh");
			printf("Log Reporter Ap_Sn:%s, FilePath : %s,  Username : %s, PassWord : %s\n", AP_log_ack[i].AP_SN, LogFilePath, ap_upgrade->Scp_Username, ap_upgrade->Scp_PassWord);

		}

		else

		{

			ap_upgrade->ack = '1';
			strcpy(ap_upgrade->Scp_Username, "xlh@192.168.153.138");
			strcpy(ap_upgrade->Scp_PassWord, "xlh");
			strcpy(ap_upgrade->Remote_FilePath, LogFilePath);
			strcpy(ap_upgrade->AP_SN, AP_log_ack[i].AP_SN);
			//printf("Log repoter one fail\n");

		}

		ap_upgrade++;

	}

	sendbuf[1] = AP_GetLog_Num;

	sendbuf[0] = APSERVER_WEB_REQUEST_LOGINFO;

	sendto(sockWeb, sendbuf, AP_GetLog_Num * sizeof(AP_upgrade_ack) + 2, 0, (struct sockaddr*)&addrWebSer, addrWeblen);

	printf("Enter loglog report size=%d", AP_GetLog_Num);

	memset(AP_log_ack, 0, sizeof(AP_logs_ack) * AP_GetLog_Num);//��־ackȷ����Ϣ��0

	AP_GetLog_Num = 0;

}





int LegalAP_find(char* buf)

{

	int i = 0;

	for (i = 0; i < max_ap; i++)

	{

		if (strcmp(ap_list[i].AP_SN, buf) == 0)

			return i;

	}

	return -1;



}



int assign_apIndex()//找到一个legalAP[i] == 0

{

	int i = 0;

	for (i = 0; i < max_ap; i++)

	{

		if (legalAP[i] == 0)

			return i;

	}

	return -1;



}
void ap_aprequest_configure_assign_value(AP* destination,  legalAP_operation_req* source)

{


	strcpy(destination->AP_SN,source->AP_SN);
	destination->configure.templateIndex = source->ap_template.templateIndex;

	destination->configure.ap_gateway.gateway = source->ap_template.ap_gateway.gateway;

	destination->configure.ap_gateway.subnetmask = source->ap_template.ap_gateway.subnetmask;

	destination->configure.DHCPOrStatic = source->ap_template.DHCPOrStatic;

	destination->configure.AP_IP = source->ap_template.AP_IP;

	strcpy(destination->configure.ap_login.login, source->ap_template.ap_login.login);

	strcpy(destination->configure.ap_login.login_psw, source->ap_template.ap_login.login_psw);

	strcpy(destination->configure.ap_ssid.ssid, source->ap_template.ap_ssid.ssid);

	strcpy(destination->configure.ap_ssid.ssid_psw, source->ap_template.ap_ssid.ssid_psw);

	destination->configure.NatOrBridge = source->ap_template.NatOrBridge;

	destination->configure.pool.start = source->ap_template.pool.start;

	destination->configure.pool.end = source->ap_template.pool.end;

	destination->configure.AP_Server_IP = source->ap_template.AP_Server_IP;

	strcpy(destination->configure.Remote_FilePath, source->ap_template.Remote_FilePath);

	strcpy(destination->configure.Scp_Username, source->ap_template.Scp_Username);

	strcpy(destination->configure.Scp_PassWord, source->ap_template.Scp_PassWord);



}


void ap_configure_assign_value(AP* destination, AP* source)

{



	destination->configure.templateIndex = source->configure.templateIndex;

	destination->configure.ap_gateway.gateway = source->configure.ap_gateway.gateway;

	destination->configure.ap_gateway.subnetmask = source->configure.ap_gateway.subnetmask;

	destination->configure.DHCPOrStatic = source->configure.DHCPOrStatic;

	destination->configure.AP_IP = source->configure.AP_IP;

	strcpy(destination->configure.ap_login.login, source->configure.ap_login.login);

	strcpy(destination->configure.ap_login.login_psw, source->configure.ap_login.login_psw);

	strcpy(destination->configure.ap_ssid.ssid, source->configure.ap_ssid.ssid);

	strcpy(destination->configure.ap_ssid.ssid_psw, source->configure.ap_ssid.ssid_psw);

	destination->configure.NatOrBridge = source->configure.NatOrBridge;

	destination->configure.pool.start = source->configure.pool.start;

	destination->configure.pool.end = source->configure.pool.end;

	destination->configure.AP_Server_IP = source->configure.AP_Server_IP;

	strcpy(destination->configure.Remote_FilePath, source->configure.Remote_FilePath);

	strcpy(destination->configure.Scp_Username, source->configure.Scp_Username);

	strcpy(destination->configure.Scp_PassWord, source->configure.Scp_PassWord);



}



void ap_status_assign_value(AP* destination, AP* source)

{



	destination->status.online_state = source->status.online_state;

	destination->status.antenna_number = source->status.antenna_number;

	destination->status.model = source->status.model;

	destination->status.user_number = source->status.user_number;

	destination->status.AP_IP = source->status.AP_IP;

	destination->status.ap_gateway.gateway = source->status.ap_gateway.gateway;

	destination->status.ap_gateway.subnetmask = source->status.ap_gateway.subnetmask;

	strcpy(destination->status.version, source->status.version);

}





void  dispatch_apMsg(int ap_index, char* buf)

{
	char* p = buf;
	AP* pp = (AP*)(p + 1);
	int i = 0;
	switch (buf[0])

	{

	case AP_APSERVER_REGISTER_REQ_WITHOUT_CONFIGURATION:

		log_info("Info: receive AP_APSERVER_REGISTER_REQ_WITHOUT_CONFIGURATION AP=%s\n", ap_list[ap_index].AP_SN);

		send_to_ap(ap_index, APSERVER_AP_REGISTER_ACK, NULL);

		log_info("Info: send APSERVER_AP_REGISTER_ACK AP=%s\n", ap_list[ap_index].AP_SN);

		legalAPTimer[ap_index].ap_timer_flag = AP_APSERVER_CONNECTING;

		legalAPTimer[ap_index].ap_on_timer = apServer_apAgent_configure_time;

		break;

	case AP_APSERVER_REGISTER_REQ_WITH_CONFIGURATION:

		ap_configure_assign_value(&ap_list[ap_index], pp);

		ap_status_assign_value(&ap_list[ap_index], pp);

		log_info("Info: receive AP_APSERVER_REGISTER_REQ_WITH_CONFIGURATION AP=%s\n", ap_list[ap_index].AP_SN);

		ap_list[ap_index].status.online_state = AP_ON_LINE;

		//send_to_ap(ap_index, APSERVER_AP_HEART_BEAT, NULL);

		legalAPTimer[ap_index].ap_timer_flag = AP_APSERVER_ON;

		legalAPTimer[ap_index].heart_beat_timer = apServer_apAgent_heart_time;

		legalAPTimer[ap_index].heart_beat_timeout_times = 0;

		log_info("Info: send APSERVER_AP_HEART_BEAT AP=%s\n", ap_list[ap_index].AP_SN);

		break;

	case AP_APSERVER_REGISTER_COMPLETE:

		ap_status_assign_value(&ap_list[ap_index], pp);

		log_info("Info: receive AP_APSERVER_REGISTER_COMPLETE AP=%s\n", ap_list[ap_index].AP_SN);

		//send_to_ap(ap_index, APSERVER_AP_HEART_BEAT, NULL);

		log_info("Info: send APSERVER_AP_HEART_BEAT AP=%s\n", ap_list[ap_index].AP_SN);

		ap_list[ap_index].status.online_state = AP_ON_LINE;

		legalAPTimer[ap_index].ap_timer_flag = AP_APSERVER_ON;

		legalAPTimer[ap_index].heart_beat_timer = apServer_apAgent_heart_time;

		legalAPTimer[ap_index].heart_beat_timeout_times = 0;

		break;

	case AP_APSERVER_CONFIGURE_OK:

		//legalAPTimer[ap_index].ap_on_timer_flag = 0;

		//legalAPConfigureResult[ap_index] = '0';
		
		for (i = 0; i < AP_Configure_Num; i++)
			{
				//printf("dispatch AP Configure ,ap_list[ap_index].AP_SN   %s, AP_upgrade_log_ack[i].AP_SN %s\n",ap_list[ap_index].AP_SN, AP_configure_ack[i].AP_SN);
				if (strcmp(ap_list[ap_index].AP_SN, AP_configure_ack[i].AP_SN) == 0)
				{
					AP_configure_ack[i].ack = '0';
					printf("Get APSN: %s Configure ack is :%c\n",ap_list[ap_index].AP_SN,AP_configure_ack[i].ack);
					template_assign_value(&(ap_list[ap_index].configure), &Configure_Template);
					}
				}
		//send_to_ap(ap_index, APSERVER_AP_HEART_BEAT, NULL);
		ap_list[ap_index].status.online_state = AP_ON_LINE;
		
		legalAPTimer[ap_index].ap_timer_flag = AP_APSERVER_ON;

		legalAPTimer[ap_index].heart_beat_timer = apServer_apAgent_heart_time;

		legalAPTimer[ap_index].heart_beat_timeout_times = 0;
		
		log_info("Info: receive AP_APSERVER_CONFIGURE_OK AP=%s\n", ap_list[ap_index].AP_SN);

		break;

	case AP_APSERVER_CONFIGURE_NOK:

		//legalAPTimer[ap_index].ap_on_timer_flag = 0;
		ap_list[ap_index].status.online_state = AP_ON_LINE;
		
		legalAPTimer[ap_index].ap_timer_flag = AP_APSERVER_ON;

		legalAPTimer[ap_index].heart_beat_timer = apServer_apAgent_heart_time;

		legalAPTimer[ap_index].heart_beat_timeout_times = 0;

		legalAPConfigureResult[ap_index] = CONFIGURE_NOK;

		log_info("Info: receive AP_APSERVER_CONFIGURE_NOK AP=%s\n", ap_list[ap_index].AP_SN);

		break;

	case AP_APSERVER_HEART_BEAT_ACK:
		
		ap_list[ap_index].status.online_state = AP_ON_LINE;
		
		legalAPTimer[ap_index].ap_timer_flag = AP_APSERVER_ON;

		legalAPTimer[ap_index].heart_beat_timer = apServer_apAgent_heart_time;//3s

		legalAPTimer[ap_index].heart_beat_timeout_times = 0;

		ap_status_assign_value(&ap_list[ap_index], pp);

		ap_list[ap_index].status.online_state = AP_ON_LINE;
		for (i = 0; i < AP_Upgrade_Num; i++)

		{
			if (strcmp(ap_list[ap_index].AP_SN, AP_upgrade_log_ack[i].AP_SN) == 0)
			{
				if (strcmp(ap_list[ap_index].status.version, AP_upgrade_log_ack[i].version) == 0)
				{
					AP_upgrade_log_ack[i].ack = '0';
					printf("APSN is %s UPGRADE_SUCCESS\n", AP_upgrade_log_ack[i].AP_SN);
				}

				else
				{
					AP_upgrade_log_ack[i].ack = '1';
					//printf("UPGRADE FAILED\n");
				}

			}

		}

		//log_info("Info: receive AP_APSERVER_HEART_BEAT_ACK AP=%s\n",ap_list[ap_index].AP_SN);
		//printf("AP:%s's version is %s   ", ap_list[ap_index].AP_SN, ap_list[ap_index].status.version);

		//printf("AP:%s's state is %d    ", ap_list[ap_index].AP_SN, ap_list[ap_index].status.online_state);

		//printf("AP:%s's antenna number is %d\n", ap_list[ap_index].AP_SN, ap_list[ap_index].status.antenna_number);
		//send_to_web(APSERVER_WEB_SOFTWARE_UPGRADE, NULL, 0);

		break;

	case AP_APSERVER_REQUEST_LOGINFO:

		//strcpy(ap_ip, inet_ntoa(ap_list[ap_index].configure.AP_IP));

		//strcpy(path, ap_list[ap_index].configure.Remote_FilePath);

		//snprintf(tftp, 256, "tftp %s get %s %s", ap_ip, ap_list[ap_index].configure.Remote_FilePath,LogFilePath); //to do

		//legalAPTimer[ap_index].ap_on_timer_flag = 0;

		//strcpy(AP_upgrade_log_ack[0], ap_list[ap_index].AP_SN);
		ap_list[ap_index].status.online_state = AP_ON_LINE;
		legalAPTimer[ap_index].ap_timer_flag = AP_APSERVER_ON;

		legalAPTimer[ap_index].heart_beat_timer = apServer_apAgent_heart_time;

		legalAPTimer[ap_index].heart_beat_timeout_times = 0;

		for (i = 0; i < AP_GetLog_Num; i++)

		{
			printf("dispatch AP Log message,ap_list[ap_index].AP_SN   %s, AP_upgrade_log_ack[i].AP_SN %s\n",ap_list[ap_index].AP_SN, AP_log_ack[i].AP_SN);

			if (strcmp(ap_list[ap_index].AP_SN, AP_log_ack[i].AP_SN) == 0)

			{

				AP_log_ack[i].ack = '0';
				printf("Get AP  i is %d ,APSN: %s Log ack is :%c\n",i,ap_list[ap_index].AP_SN,AP_log_ack[i].ack);

			}

		}

		log_info("Info: receive AP_APSERVER_REQUEST_LOGINFO_OK AP=%s\n", ap_list[ap_index].AP_SN);

		break;

	case AP_APSERVER_HEART_BEAT_ACK_WITH_STATUS:
		
		ap_list[ap_index].status.online_state = AP_ON_LINE;
		
		legalAPTimer[ap_index].ap_timer_flag = AP_APSERVER_ON;

		legalAPTimer[ap_index].heart_beat_timer = apServer_apAgent_heart_time;

		legalAPTimer[ap_index].heart_beat_timeout_times = 0;

		ap_status_assign_value(&ap_list[ap_index], pp);

		ap_list[ap_index].status.online_state = 1;

		log_info("Info: receive AP_APSERVER_HEART_BEAT_ACK_WITH_STATUS_UPGRADE AP=%s\n", ap_list[ap_index].AP_SN);

		break;



	default:

		//log_error("Error: Unknown protocol type = %d!\n", envelop->bProtocolSap);

		break;

	}





}



void template_assign_value(AP_Configuration_template* destination, AP_Configuration_template* source)

{

	destination->templateIndex = source->templateIndex;

	destination->ap_gateway.gateway = source->ap_gateway.gateway;

	destination->ap_gateway.subnetmask = source->ap_gateway.subnetmask;

	destination->DHCPOrStatic = source->DHCPOrStatic;

	destination->AP_IP = source->AP_IP;

	strcpy(destination->ap_login.login, source->ap_login.login);

	strcpy(destination->ap_login.login_psw, source->ap_login.login_psw);

	strcpy(destination->ap_ssid.ssid, source->ap_ssid.ssid);

	strcpy(destination->ap_ssid.ssid_psw, source->ap_ssid.ssid_psw);

	destination->NatOrBridge = source->NatOrBridge;

	destination->pool.start = source->pool.start;

	destination->pool.end = source->pool.end;

	destination->AP_Server_IP = source->AP_Server_IP;

	strcpy(destination->Remote_FilePath, source->Remote_FilePath);

	strcpy(destination->Scp_Username, source->Scp_Username);

	strcpy(destination->Scp_PassWord, source->Scp_PassWord);

}





void template_report()

{

	int i = 0, size = 0;

	char sendbuf[bufsize_apToWeb1];

	char* p = sendbuf;

	p++; p++;



	sendbuf[0] = APSERVER_WEB_TEMPLATE_REPORT;

	for (i = 0; i < max_template; i++)

	{

		if (legalTemplate[i])

		{

			size++;

			memcpy(p, (char*)&apTemplate[i], sizeof(AP_Configuration_template));

			p += sizeof(AP_Configuration_template);

		}

	}

	sendbuf[1] = size;

	/*

		printf("ip=%s\n",inet_ntoa(ttt));

		printf("sendbuf[0]=0x%x,sendbuf[1]=0x%x,sendbuf[2]=0x%x,sendbuf[3]=0x%x,sendbuf[4]=0x%x,sendbuf[5]=0x%x\n",sendbuf[0],sendbuf[1],sendbuf[2],sendbuf[3],sendbuf[4],sendbuf[5]);

		printf("sendbuf[6]=0x%x,sendbuf[7]=0x%x,sendbuf[8]=0x%x,sendbuf[9]=0x%x,sendbuf[10]=0x%x,sendbuf[11]=0x%x\n",sendbuf[6],sendbuf[7],sendbuf[8],sendbuf[9],sendbuf[10],sendbuf[11]);

		printf("sendbuf[12]=0x%x,sendbuf[13]=0x%x,sendbuf[14]=0x%x,sendbuf[15]=0x%x,sendbuf[16]=0x%x,sendbuf[17]=0x%x\n",sendbuf[12],sendbuf[13],sendbuf[14],sendbuf[15],sendbuf[16],sendbuf[17]);

		printf("sendbuf[18]=0x%x,sendbuf[19]=0x%x,sendbuf[20]=0x%x,sendbuf[21]=0x%x,sendbuf[22]=0x%x,sendbuf[23]=0x%x\n",sendbuf[18],sendbuf[19],sendbuf[20],sendbuf[21],sendbuf[22],sendbuf[23]);*/

	log_info("Info: send_to_web:template_report,size=%d\n", sendbuf[1]);

	sendto(sockWeb, sendbuf, bufsize_apToWeb1, 0, (struct sockaddr*)&addrWebSer, addrWeblen);





}



void ap_report()

{

	int i = 0, size = 0;

	char sendbuf2[bufsize_apToWeb2];

	char* p;

	AP* ppp = ap_list;



	p = sendbuf2;

	sendbuf2[0] = APSERVER_WEB_AP_REPORT;

	p++; p++;

	for (i = 0; i < max_ap; i++)

	{

		if (legalAP[i])

		{

			size++;

			memcpy(p, (char*)ppp, sizeof(AP));

			p = p + sizeof(AP);

		}

		ppp++;

	}

	sendbuf2[1] = size;

	log_info("Info: send_to_web:ap_report,size=%d\n", sendbuf2[1]);

	sendto(sockWeb, sendbuf2, bufsize_apToWeb2, 0, (struct sockaddr*)&addrWebSer, addrWeblen);

}



void  connect_to_web()
{
	template_report();
	ap_report();
	printf("connect web...\n");
	Webtimerflag = WEB_APSERVER_CONNECTING;
	webTimer = web_apServer_heart_out_time;
}





void decode_web_template_modifyMsg(char* buf)//解析模板的操作信息，返回操作的确认信息给WS

{

	int i = 0, size = buf[1];

	template_operation_req* template_Operation;

	template_operation_ack* template_operation;

	char sendbuf[sizeof(template_operation_ack) * max_template + 1] = { "\0" };

	char* p = sendbuf;
	
	sendbuf[0] = APSERVER_WEB_TEMPLATE_MODIFY;
	sendbuf[1] = size;

	template_operation = (template_operation_ack*)(p + 2);

	if (size > 0 && size < max_template)

	{
		template_Operation = (template_operation_req*)&buf[2];
		
		for (i = 0; i < size; i++)
		{
			template_operation[i].templateIndex = template_Operation->ap_template.templateIndex;//模板号到ACK信息填充
			switch (template_Operation->command)
			{
			case TEMPLATE_ADD:
				legalTemplate[template_Operation->ap_template.templateIndex] = 1;
				template_assign_value(&apTemplate[template_Operation->ap_template.templateIndex], &template_Operation->ap_template);
				printf("enter template add\n");
				//print_templates();
				add_template_file(template_Operation->ap_template.templateIndex);
				template_operation[i].ack = '0';
				break;
			case TEMPLATE_DEL:
				printf("enter template del\n");
				if (legalTemplate[template_Operation->ap_template.templateIndex])
				{
					legalTemplate[template_Operation->ap_template.templateIndex] = 0;
					del_template_file(template_Operation->ap_template.templateIndex);
					template_operation[i].ack = '0';
				}
				else
				{
					log_info("Info: del template,but not exist\n");
					template_operation[i].ack = '0';
				}

				break;

			case TEMPLATE_MODIFY:

				printf("enter template modifyl:,%d,%d\n",template_operation[i].templateIndex,template_Operation->ap_template.templateIndex);

				legalTemplate[template_Operation->ap_template.templateIndex] = 1;

				template_assign_value(&apTemplate[template_Operation->ap_template.templateIndex], &template_Operation->ap_template);

				del_template_file(template_Operation->ap_template.templateIndex);

				add_template_file(template_Operation->ap_template.templateIndex);//将修改后的Template信息写入。
				template_operation[i].ack = '0';

				break;

			default:

				log_error("Error: Unknown templdate operation=%d\n", template_Operation->command);

				break;

			}
		template_Operation++;//指针移位到下一条command

		}

		send_to_web(APSERVER_WEB_TEMPLATE_MODIFY, p, 2 + size * sizeof(template_operation_ack));

		log_info("Info: send_to_web:APSERVER_WEB_TEMPLATE_MODIFY\n");
		printf("上报修改模板的确认信息，Info: send_to_web:APSERVER_WEB_TEMPLATE_MODIFY\n");

		write_template_set();//写入Template集合

	}

}

void decode_web_AP_modifyMsg(char* buf)

{

	int i = 0, size = buf[1], apIndex = -1;

	legalAP_operation_req* ap_Operation;

	legalAP_operation_ack* ap_operation;

	char sendbuf[sizeof(legalAP_operation_ack) * max_ap + 2] = { "\0" };

	char* p = sendbuf;//返回给WS
	
	ap_operation = (legalAP_operation_ack*)(p + 2);

	sendbuf[0] = APSERVER_WEB_AP_MODIFY;
	
	sendbuf[1] = size;

	if (size > 0 && size < max_ap)
	{
	
		ap_Operation = (legalAP_operation_req*)&buf[2];
		
		for (i = 0; i < size; i++)
		{
			apIndex = LegalAP_find(ap_Operation->AP_SN);
			printf("apIndex:%d,,size:%d\n",apIndex,size);
			//ap_operation[i].apSeverIndex = -1;


			
			strcpy(ap_operation[i].AP_SN, ap_Operation->AP_SN);//存储APSN，发送给WS



			switch (ap_Operation->command)
			{
			case AP_ADD:
				printf("Enter ap add\n");
				if (apIndex != -1 && legalAP[apIndex] == 1)
				{
					log_info("Info: add AP,but exist,already have this AP\n");
					printf("Info: add AP,but exist,already have this AP\n");
					ap_operation[i].ack = '3';
				}
				else
				{
					apIndex = assign_apIndex();
					if (apIndex != -1)
					{
						legalAP[apIndex] = 1;
						printf("add AP, SN=%s, templateID=%d\n", ap_Operation->AP_SN, ap_Operation->ap_template.templateIndex);
						
						strcpy(ap_list[apIndex].AP_SN, ap_Operation->AP_SN);
						template_assign_value(&(ap_list[apIndex].configure),&(ap_Operation->ap_template));
						printf("AP_ADD: AP_IP：%s\n",ap_list[apIndex].configure.ap_login.login);
						ap_list[apIndex].configure.templateIndex = ap_Operation->ap_template.templateIndex;
						ap_operation[i].ack = '0';
					}
					else
					{
						log_error("Error: no enough apIndex!\n");
						ap_operation[i].ack = '1';
					}
				}
				
				break;
//
			case AP_DEL:

				printf("enter ap del\n");

				if (apIndex != -1)

				{
					legalAP[apIndex] = 0;
					ap_operation[i].ack = '0';
				}
				else

				{
					log_info("Info: del AP,but not exist\n");
					ap_operation[i].ack = '3';
				}
				
				break;
			case AP_MODIFY://这个状态是无效的

				printf("enter ap modify\n");

				if (apIndex == -1)

				{
					log_info("Info: modify AP,but not exist\n");

					ap_operation[i].ack = '1';
				break;

				}
				else{
				ap_operation[i].ack = '0';

				legalAP[apIndex] = 1;

				strcpy(ap_list[apIndex].AP_SN, ap_Operation->AP_SN);
				}

				break;

			default:
				log_error("Error: Unknown AP operation=%d\n", ap_Operation->command);
				break;
			}
			ap_Operation++;
			printf("i %d APSN  %s ACK %c\n",i,ap_operation[i].AP_SN,ap_operation[i].ack);
		}
		ap_operation = (legalAP_operation_ack*)&sendbuf[2];
		printf("ap_operation : APSN %s  ACK %c  buf[1]  %d buf[0] %d  buf[2] %d\n",ap_operation->AP_SN,ap_operation->ack,buf[1],buf[0],buf[2]);
		//send_to_web(APSERVER_WEB_AP_MODIFY, sendbuf,sizeof(legalAP_operation_ack) * max_ap + 2 );
		send_to_web(APSERVER_WEB_AP_MODIFY, p,sizeof(legalAP_operation_ack) * size + 2 );
		log_info("Info: send_to_web:APSERVER_WEB_AP_MODIFY\n");

		write_ap_set();

	}


}



void decode_web_AP_configurationMsg(char* buf)

{

	int i = 0, size = buf[1], apIndex = -1;
	AP_Configuration_template* ap_Operation;
	char* p;
	char sn[APSN_length] = { "\0" };
	
	AP_Configure_Num = size;
	
	p = buf + 2 + sizeof(AP_Configuration_template);
	if (size > 0 && size < max_ap)
	{
		ap_Operation = (AP_Configuration_template*)&buf[2];//指向模板

		template_assign_value(&Configure_Template,ap_Operation);//save configure info
		
		for (i = 0; i < size; i++)
		{
			strncpy(sn, p, APSN_length);
			//printf("Configure APSN:%s\n",sn);
			
			apIndex = LegalAP_find(sn);
			if (apIndex != -1)
			{
				strcpy(AP_configure_ack[i].AP_SN,ap_list[apIndex].AP_SN);//组包返回的确认信息
				
				printf("send to ap configure APSN: %s 返回%s  \n",AP_configure_ack[i].AP_SN,ap_list[apIndex].AP_SN);
				
				send_to_ap(apIndex, APSERVER_AP_CONFIGURE, ap_Operation);
				
				printf("########################################\n");
				printf(" netmask=%s %s\n",  inet_ntoa(ap_Operation->ap_gateway.subnetmask),inet_ntoa(Configure_Template.ap_gateway.subnetmask));
				printf(" gateway=%s %s\n",  inet_ntoa(ap_Operation->ap_gateway.gateway),inet_ntoa(Configure_Template.ap_gateway.gateway));
				
				
				log_info("Info: send_to_ap:APSERVER_AP_CONFIGURE AP:%s\n", ap_list[apIndex].AP_SN);
				legalAPTimer[apIndex].ap_on_timer_flag = 1;
				legalAPTimer[apIndex].ap_on_timer = apServer_apAgent_heart_time * 11;//33s
				
				WebOntimerflag = 1;
				webOnTimer = 30;
			}
			else
			{
				log_error("Error: Configure Unknown AP SN=%s\n", sn);
			}
			p = p + APSN_length;
		}

	}



}

//Scp  username = ap_info->username filepath = ap_info->filepath     ///�������

//   -------------------------------------------------

//   |command|size =10|AP_opertion_req|*size|�ļ�����|

//   -------------------------------------------------

void decode_web_AP_log_requestMsg(char* buf)//done ������ȡ��־��Ϣ��������Ϣת������Ӧ��AP������������Ŀ¼��Ϣ

{

	int i = 0, size = buf[1], apIndex = -1;

	legalAP_operation_req* ap_info;
	char sn[APSN_length] = { "\0" };

	Create_LogDir(LogFilePath);//                                /var/lib/tftpboot/log_%dy%dm%dd%dh%dm

	//strcat(path, &LogFilePath[17]);//      ./rizhi2020                

	AP_GetLog_Num = size;

	if (size > 0 && size < max_ap)

	{

		ap_info = (legalAP_operation_req*)&buf[2];

		for (i = 0; i < size; i++)

		{

			strcpy(sn, ap_info->AP_SN);

			apIndex = LegalAP_find(sn);

			if (apIndex != -1)

			{

				strcpy(AP_log_ack[i].AP_SN, ap_info->AP_SN);  
				printf("decode log AP_log_ack[i].AP_SN: %s,ap_info->AP_SN:%s\n",AP_log_ack[i].AP_SN,ap_info->AP_SN);
				
				strcpy(ap_info->ap_template.Remote_FilePath, &LogFilePath[17]);//  /log_%dy%dm%dd%dh%dm
				
				send_to_ap(apIndex, APSERVER_AP_REQUEST_LOGINFO, &(ap_info->ap_template));
				printf("Send to AP LogFilePtath is %s\n", &LogFilePath[17]);

				log_info("Info: send_to_ap:APSERVER_AP_Request_LogInfo AP:%s\n", ap_list[apIndex].AP_SN);

				legalAPTimer[apIndex].ap_on_timer_flag = 3;

				legalAPTimer[apIndex].ap_on_timer = apServer_apAgent_log_get_time;

				WebOntimerflag = 3;

				webOnTimer = web_apServer_log_get_time +12;

				

			}

			else

			{

				log_error("Error: Send Log Unknown AP SN=%s\n", sn);

			}

			ap_info = ap_info + 1;

		}

	}

}



void decode_web_AP_Soft_UpgradeMsg(char* buf) //done 

{

	int i = 0, size = buf[1], apIndex = -1;
	printf("软件升级的 size %d\n",size);
	legalAP_operation_req* ap_info;
	char sn[APSN_length] = { "\0" };
	AP_Upgrade_Num = size;

	if (size > 0 && size < max_ap)

	{

		ap_info = (legalAP_operation_req*)&buf[2];

		snprintf(Scp_Exe, 512, "sshpass -p \"%s\" scp  -o StrictHostKeyChecking=no  %s:%s %s", ap_info->ap_template.Scp_PassWord, ap_info->ap_template.Scp_Username, ap_info->ap_template.Remote_FilePath, Loacl_Upgrade_FilePath);

		//snprintf(Scp_Exe, 512, "scp %s@%s:%s %s", ap_info->ap_template.Scp_Username, Web_Server_IP, ap_info->ap_template.Remote_FilePath, Loacl_Upgrade_FilePath);

		printf("获取升级文件：scp to do is: %s\n", Scp_Exe);

		status = system(Scp_Exe);                    

		if (-1 == status) {

			printf("scp error\n");

		}

		if (!WIFEXITED(status)) {

			printf("scp error\n");

		}

		if (WEXITSTATUS(status)) {

			printf("scp error\n");

		}

		else {

			printf("scp success\n\n");

		}

		for (i = 0; i < size; i++)

		{

			strcpy(AP_upgrade_log_ack[i].AP_SN, ap_info->AP_SN);

			strcpy(AP_upgrade_log_ack[i].version, ap_info->ap_template.version);//组包确认信息
			//strcpy(ap_info->ap_template.Remote_FilePath, (ap_info->ap_template.Remote_FilePath)+5);
			//printf("version:%s  AP_SN:%s,upgrade file name is %s\n",ap_info->ap_template.version,ap_info->AP_SN,(ap_info->ap_template.Remote_FilePath)[5]);
			printf("get version:%s  AP_SN:%s,upgrade file name is %s\n\n", ap_info->ap_template.version, ap_info->AP_SN, ap_info->ap_template.Remote_FilePath);
			strcpy(sn, ap_info->AP_SN);
			apIndex = LegalAP_find(sn);

			if (apIndex != -1)

			{

				send_to_ap(apIndex, APSERVER_AP_SOFTWARE_UPGRADE, &(ap_info->ap_template));//包含用户名等信息

				log_info("Info: send_to_ap:WEB_APSERVER_SOFTWARE_UPGRADE AP:%s\n", ap_list[apIndex].AP_SN);

				log_info("Info: send_to_ap:WEB_APSERVER_SOFTWARE_UPGRADE APIndex:%d\n", apIndex);

				legalAPTimer[apIndex].ap_on_timer_flag = 2;

				legalAPTimer[apIndex].ap_on_timer = apServer_apAgent_upgrade_time;

				WebOntimerflag = 2;
				Webtimerflag = 2;
				webOnTimer = web_apServer_upgrade_time;
				legalAPTimer[apIndex].ap_on_timer_flag = 2;
				legalAPTimer[apIndex].ap_on_timer = 20;

			}

			else

			{

				log_error("Error: Unknown AP SN=%s\n", sn);

			}

			ap_info = ap_info + 1;

			//p = p + 1;

		}

	}

}



void decode_web_template_reportMsg(char* buf)

{

	int i = 0, size = buf[1];

	AP_Configuration_template* template_operation;

	if (size > 0 && size < max_template)

	{
		template_operation = (AP_Configuration_template*)&buf[2];
		for (i = 0; i < size; i++)

		{
			legalTemplate[template_operation->templateIndex] = 1;

			template_assign_value(&apTemplate[template_operation->templateIndex], template_operation);

			add_template_file(template_operation->templateIndex);//记录模板的详细信息
			template_operation++;

		}

		write_template_set();//将所有Template的序号写入一个txt文件

	}

}



void remove_all_template_file()

{

	int i = 0;

	char name[51] = { 0 };

	for (i = 0; i < max_template; i++)

	{

		snprintf(name, 50, "../templates/%d", i);

		remove(name);

	}



}



void decode_web_AP_reportMsg(char* buf)

{

	int i = 0, size = buf[1];

	legalAP_operation_req* ap_operation;
	printf("test,size : %d\n",size);
	ap_operation = (legalAP_operation_req*)&buf[2];
	if (size > 0 && size < max_ap)

	{

		for (i = 0; i < size; i++)

		{

			legalAP[i] = 1;
			ap_aprequest_configure_assign_value(&ap_list[i], ap_operation);//给AP赋值，并且加载到内存中
			printf("AP REPORTER APSN:ap_list %s,ap_operation:%s\n",ap_list[i].AP_SN,ap_operation->AP_SN);
			//ap_status_assign_value(&ap_list[i], temp);
			ap_operation++;

		}

		write_ap_set();//将AP信息写入APSet，APSerialID = TemplateID

	}

}





void send_web_heartbeat_withstatusMsg()

{

	int i = 0, size = 0;

	char sendbuf[sizeof(AP) * max_ap + 2] = { "\0" };

	AP* ap_operation;

	char* p = sendbuf;

	ap_operation = (AP*)(p + 2);

	sendbuf[0] = APSERVER_WEB_HEART_BEAT;

	for (i = 0; i < max_ap; i++)

	{

		if (legalAP[i])

		{

			size++;

			strcpy(ap_operation->AP_SN, ap_list[i].AP_SN);

			ap_configure_assign_value(ap_operation, &ap_list[i]);

			ap_status_assign_value(ap_operation, &ap_list[i]);

			ap_operation++;

		}

	}

	sendbuf[1] = size;
	//printf(" size:%d,antena_num:%c\n",size,ap_list[--i].status.antenna_number);
	printf("send_web_heartbeat_withstatusMsg size : %d\n",size);
	send_to_web(APSERVER_WEB_HEART_BEAT, p, 2 + size * sizeof(AP));

	//log_info("Info: send_to_web:APSERVER_WEB_HEART_BEAT\n");

}





void  dispatch_webMsg(char* buf)

{
	//printf("dispatch_webMsg buf[0]:%d\n",buf[0]);

	switch (buf[0])

	{

	case APSERVER_WEB_TEMPLATE_REPORT:

		remove_all_template_file();

		init_legalTemplate();

		decode_web_template_reportMsg(buf);//写入内存和TXT文件

		break;

	case APSERVER_WEB_AP_REPORT:
		printf("APSERVER_WEB_AP_REPORT,ago\n");
		init_legalAP();//初始化和清空AP的信息

		decode_web_AP_reportMsg(buf);

		break;
		

	case APSERVER_WEB_TEMPLATE_MODIFY:
		printf("APSERVER_WEB_TEMPLATE_MODIFY,ago\n");
		decode_web_template_modifyMsg(buf);

		break;

	case APSERVER_WEB_AP_MODIFY:
		printf("APSERVER_WEB_AP_MODIFY,ago\n");
		decode_web_AP_modifyMsg(buf);

		break;

	case APSERVER_WEB_AP_CONFIGURE:
		printf("APSERVER_WEB_AP_Configure,ago\n");
		decode_web_AP_configurationMsg(buf);

		break;

	case APSERVER_WEB_HEART_BEAT:
		printf("*********APSERVER_WEB_AP_HeartBeat,ago********\n\n");
		send_web_heartbeat_withstatusMsg();

		Webtimerflag = WEB_APSERVER_ON;

		webTimer = web_apServer_heart_out_time;  //9s

		break;

	case APSERVER_WEB_REQUEST_LOGINFO:

		decode_web_AP_log_requestMsg(buf);

		break;

	case APSERVER_WEB_SOFTWARE_UPGRADE:

		decode_web_AP_Soft_UpgradeMsg(buf);

		break;

	default:

		//log_error("Error: Unknown protocol type = %d!\n", envelop->bProtocolSap);

		break;

	}

}



void Create_LogDir(char* filename)
{

	char file_path[512] = { "\0" };
	char chmodir[512] = "\0";

	time_t timep;

	struct tm* ti;

	time(&timep);

	ti = gmtime(&timep);

	snprintf(file_path, 256, "mkdir /var/lib/tftpboot/log_%dy%dm%dd%dh%dm", 1900 + ti->tm_year, 1 + ti->tm_mon, ti->tm_mday, 8 + ti->tm_hour, ti->tm_min);

	//printf("file_path is:%s\n",file_path);
	
	status = system(file_path);//����Ŀ¼

	if (-1 == status) {

		printf("mkdir error\n");

	}

	if (!WIFEXITED(status)) {

		printf("mkdir error\n");

	}

	if (WEXITSTATUS(status)) {

		printf("mkdir error\n");

	}

	else

	{

		printf("mkdir success\n");

	}
	
	printf("mkdir\n");
	strcpy(filename, &file_path[6]);
	snprintf(chmodir,256,"chmod 777 %s",&file_path[6]);
	status = system(chmodir);//give manager

}




