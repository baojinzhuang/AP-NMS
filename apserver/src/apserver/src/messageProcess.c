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
extern AP ap_list[max_ap];
extern int legalTemplate[max_template];
extern int legalAP[max_ap];
extern int legalAPConfigureResult[max_ap];//0，未配置，1，配置成功，2，配置失败，3，配置超时
extern int legalAPUpgradeResult[max_ap];//0，软件升级成功，1，软件升级失败，2，软件升级超时
extern int legalAPLogGetResult[max_ap];//0，获取日志成功，1，获取日志失败，2，获取日志超时
extern AP_upgrade_ack AP_upgrade_log_ack[max_ap];
extern int status;

extern AP_timer legalAPTimer[max_ap];
extern char Loacl_Upgrade_FilePath[50];
extern char LocalUsername[50];
extern char LoaclPassWord[20];
extern char Scp_Exe[1024];
extern char LogFilePath[50];
extern char AP_Server_IP[50];
extern char Web_Server_IP[50];
extern int AP_Upgrade_Num;
extern int AP_GetLog_Num;

extern AP_socket ap_socket_list[max_ap];
extern int sockSer;

extern int sockWeb;
extern struct sockaddr_in addrWebSer;  //创建一个记录地址信息的结构体 
extern socklen_t addrWeblen;

extern int Webtimerflag; //0,不启动，1,启动中，2,正常通信中
extern int WebOntimerflag; //0,正常通信中，1,正常通信配置中，2,正常通信升级中,，3，正常通信获取日志中
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

void decode_web_AP_log_requestMsg(char* buf);

/*command  AP_Configuration_template* tempTemplate*/
void send_to_ap(int apIndex, int command,AP_Configuration_template* tempTemplate)//decode解析Web命令，然后发数据给AP
{
	char sendbuf[sizeof(AP) + 1];    //申请一个发送数据缓存区
	char* p = sendbuf;
	AP_Configuration_template* pp = &(apTemplate[ap_list[apIndex].configure.templateIndex]);

	*p = command;
	p++;
	printf("after p++ apIndex=%d\n", apIndex);
	//return;

	if (legalAP[apIndex] == 1)
		switch (command)
		{
		case APSERVER_AP_REGISTER_ACK:
			memcpy(p, (char*)pp, sizeof(AP_Configuration_template));
			sendto(sockSer, sendbuf, sizeof(AP_Configuration_template) + 2, 0, (struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen);
			break;
		case APSERVER_AP_REGISTER_REJ:
			sendto(sockSer, sendbuf, 2, 0, (struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen);
			break;
		case APSERVER_AP_CONFIGURE:
			memcpy(p, tempTemplate, sizeof(AP_Configuration_template));
			sendto(sockSer, sendbuf, sizeof(AP_Configuration_template) + 2, 0, (struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen);
			break;
		case AP_APSERVER_SOFTWARE_UPGRADE:
			memcpy(p, tempTemplate, sizeof(AP_Configuration_template));
			sendto(sockSer, sendbuf, sizeof(AP_Configuration_template) + 2, 0, (struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen);
			break;
		case AP_APSERVER_REQUEST_LOGINFO:
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

void send_to_web(char command, char* sendbuf2, int length)//decode后使用   回的都是Ack？
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
	case APSERVER_WEB_SOFTWARE_UPGRADE://软件升级结果上报
		upgrade_ack_report();
		break;
	case APSERVER_WEB_REQUEST_LOGINFO://将获取的日志的路径，用户名，密码上报
		log_info_report();
		break;
	case APSERVER_WEB_HEART_BEAT:
		sendto(sockWeb, sendbuf2, length, 0, (struct sockaddr*)&addrWebSer, addrWeblen);
		break;

	default:
		printf("undefined command AP Server to WEB");
	}

}

void configure_reprot()//配置ack上报到Web
{
    int i=0,size = 0;
	AP_configuration_ack* ap_configuration;
	char sendbuf[sizeof(AP_configuration_ack)*max_ap+2]={"\0"};
	char* p=sendbuf+2;

	ap_configuration = (AP_configuration_ack*)p;

	for (i = 0; i < max_ap; i++)
	{
		if (legalAPConfigureResult[i])
		{
			size++;
			ap_configuration->ack = legalAPConfigureResult[i];
			strcpy(ap_configuration->AP_SN, ap_list[i].AP_SN);
			ap_configuration++;
		}
	}
	sendbuf[1]=size;
	sendbuf[0]=APSERVER_WEB_AP_CONFIGURE;
	sendto(sockWeb,sendbuf,size*sizeof(AP_configuration_ack)+2,0,(struct sockaddr*)&addrWebSer, addrWeblen); 
	printf("enter configure report size=%d",size);
	for(i=0;i<max_ap;i++)
		legalAPConfigureResult[i]=0;
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
		if (AP_upgrade_log_ack[i].ack == 0)
		{
			ap_upgrade->ack = AP_upgrade_log_ack[i].ack;
			strcpy(ap_upgrade->AP_SN, AP_upgrade_log_ack[i].AP_SN);
		}
		else
		{
			ap_upgrade->ack = 1;
			strcpy(ap_upgrade->AP_SN, AP_upgrade_log_ack[i].AP_SN);
		}
		ap_upgrade++;
	}
	sendbuf[1] = AP_Upgrade_Num;
	sendbuf[0] = APSERVER_WEB_SOFTWARE_UPGRADE;
	sendto(sockWeb, sendbuf, AP_Upgrade_Num * sizeof(AP_upgrade_ack) + 2, 0, (struct sockaddr*)&addrWebSer, addrWeblen);
	printf("enter upgrade report size=%d", AP_Upgrade_Num);
	memset(AP_upgrade_log_ack, 0, sizeof(AP_upgrade_ack) * AP_Upgrade_Num);//升级ack确认信息清0
	AP_Upgrade_Num = 0;
}

void log_info_report()
{
	int i = 0;
	AP_upgrade_ack* ap_upgrade;
	char sendbuf[sizeof(AP_upgrade_ack) * max_ap + 2] = { "\0" };
	char* p = sendbuf + 2;
	ap_upgrade = (AP_upgrade_ack*)p;
	//AP_upgrade_ack AP_upgrade_log_ack[max_ap]
	for (i = 0; i < AP_GetLog_Num; i++)
	{
		if (AP_upgrade_log_ack[i].ack == 0 )
		{
			ap_upgrade->ack = AP_upgrade_log_ack[i].ack;
			strcpy(ap_upgrade->AP_SN, AP_upgrade_log_ack[i].AP_SN);
			strcpy(ap_upgrade->Remote_FilePath,LogFilePath);
		}
		else 
		{
			ap_upgrade->ack = 1;
			strcpy(ap_upgrade->AP_SN, AP_upgrade_log_ack[i].AP_SN);
		}
		ap_upgrade++;
	}
	sendbuf[1] = AP_GetLog_Num;
	sendbuf[0] = APSERVER_WEB_REQUEST_LOGINFO;
	sendto(sockWeb, sendbuf, AP_GetLog_Num * sizeof(AP_upgrade_ack) + 2, 0, (struct sockaddr*)&addrWebSer, addrWeblen);
	printf("enter loglog report size=%d", AP_GetLog_Num);
	memset(AP_upgrade_log_ack, 0, sizeof(AP_upgrade_ack) * AP_GetLog_Num);//日志ack确认信息清0
	AP_GetLog_Num = 0;
}


int LegalAP_find(char* buf)
{
    int i=0;
	for(i=0;i<max_ap;i++)
		{
			if(strcmp(ap_list[i].AP_SN,buf)==0)
				return i;
		}
	return -1;

}

int assign_apIndex()
{
    int i=0;
	for(i=0;i<max_ap;i++)
		{
			if(legalAP[i]==0)
				return i;
		}
	return -1;

}

void ap_configure_assign_value(AP* destination,AP* source)
{

	destination->configure.templateIndex=source->configure.templateIndex;
	destination->configure.ap_gateway.gateway=source->configure.ap_gateway.gateway;
    destination->configure.ap_gateway.subnetmask=source->configure.ap_gateway.subnetmask;
	destination->configure.DHCPOrStatic=source->configure.DHCPOrStatic;
	destination->configure.AP_IP=source->configure.AP_IP;
	strcpy(destination->configure.ap_login.login,source->configure.ap_login.login);
	strcpy(destination->configure.ap_login.login_psw,source->configure.ap_login.login_psw);
	strcpy(destination->configure.ap_ssid.ssid,source->configure.ap_ssid.ssid);
	strcpy(destination->configure.ap_ssid.ssid_psw,source->configure.ap_ssid.ssid_psw);
	destination->configure.NatOrBridge = source->configure.NatOrBridge;
	destination->configure.pool.start = source->configure.pool.start;
	destination->configure.pool.end = source->configure.pool.end;
	destination->configure.AP_Server_IP=source->configure.AP_Server_IP;
	strcpy(destination->configure.Remote_FilePath, source->configure.Remote_FilePath);
	strcpy(destination->configure.Scp_Username, source->configure.Scp_Username);
	strcpy(destination->configure.Scp_PassWord, source->configure.Scp_PassWord);

}

void ap_status_assign_value(AP* destination,AP* source)
{

	destination->status.online_state = source->status.online_state;
	destination->status.antenna_number = source->status.antenna_number;
	destination->status.model = source->status.model;
	destination->status.user_number = source->status.user_number;
	destination->status.AP_IP = source->status.AP_IP;
	destination->status.ap_gateway.gateway = source->status.ap_gateway.gateway;
	destination->status.ap_gateway.subnetmask = source->status.ap_gateway.subnetmask;
	strcpy(destination->status.version,source->status.version);
}


void  dispatch_apMsg(int ap_index,char* buf)
{

	char* p=buf;
	AP* pp=(AP*)(p+1);
	int i = 0;
	char tftp[512] = {"\0"};
	char ap_ip[64] = { "\0" };
	char path[256] = { "\0" };
	//some information
	switch(buf[0])
	{
	case AP_APSERVER_REGISTER_REQ_WITHOUT_CONFIGURATION:
		log_info("Info: receive AP_APSERVER_REGISTER_REQ_WITHOUT_CONFIGURATION AP=%s\n",ap_list[ap_index].AP_SN);
		send_to_ap(ap_index, APSERVER_AP_REGISTER_ACK, NULL);
		log_info("Info: send APSERVER_AP_REGISTER_ACK AP=%s\n",ap_list[ap_index].AP_SN);
		legalAPTimer[ap_index].ap_timer_flag = AP_APSERVER_CONNECTING;
		legalAPTimer[ap_index].ap_on_timer = apServer_apAgent_configure_time;
        break;
	case AP_APSERVER_REGISTER_REQ_WITH_CONFIGURATION:
		ap_configure_assign_value(&ap_list[ap_index],pp);
		ap_status_assign_value(&ap_list[ap_index],pp);
		ap_list[ap_index].status.online_state = AP_ON_LINE;
		log_info("Info: receive AP_APSERVER_REGISTER_REQ_WITH_CONFIGURATION AP=%s\n",ap_list[ap_index].AP_SN);
		send_to_ap(ap_index, APSERVER_AP_HEART_BEAT, NULL);
		log_info("Info: send APSERVER_AP_HEART_BEAT AP=%s\n",ap_list[ap_index].AP_SN);
		legalAPTimer[ap_index].ap_timer_flag = AP_APSERVER_ON;
		legalAPTimer[ap_index].heart_beat_timer = apServer_apAgent_heart_time;
		legalAPTimer[ap_index].heart_beat_timeout_times = 0;
        break;
	case AP_APSERVER_REGISTER_COMPLETE:
		ap_list[ap_index].status.online_state = AP_ON_LINE;
		ap_status_assign_value(&ap_list[ap_index],pp);
		log_info("Info: receive AP_APSERVER_REGISTER_COMPLETE AP=%s\n",ap_list[ap_index].AP_SN);
		send_to_ap(ap_index, APSERVER_AP_HEART_BEAT, NULL);
		log_info("Info: send APSERVER_AP_HEART_BEAT AP=%s\n",ap_list[ap_index].AP_SN);
		legalAPTimer[ap_index].ap_timer_flag = AP_APSERVER_ON;
		legalAPTimer[ap_index].heart_beat_timer = apServer_apAgent_heart_time;
		legalAPTimer[ap_index].heart_beat_timeout_times = 0;
        break;
	case AP_APSERVER_CONFIGURE_OK:
		legalAPTimer[ap_index].ap_on_timer_flag = 0;
		legalAPConfigureResult[ap_index] = CONFIGURE_OK;
		log_info("Info: receive AP_APSERVER_CONFIGURE_OK AP=%s\n",ap_list[ap_index].AP_SN);
        break;
	case AP_APSERVER_CONFIGURE_NOK:
		legalAPTimer[ap_index].ap_on_timer_flag = 0;
		legalAPConfigureResult[ap_index] = CONFIGURE_NOK;
		log_info("Info: receive AP_APSERVER_CONFIGURE_NOK AP=%s\n",ap_list[ap_index].AP_SN);
        break;
	case AP_APSERVER_HEART_BEAT_ACK:
		legalAPTimer[ap_index].heart_beat_timer = apServer_apAgent_heart_time;
		legalAPTimer[ap_index].heart_beat_timeout_times = 0;
		ap_status_assign_value(&ap_list[ap_index],pp);
		ap_list[ap_index].status.online_state = 1;
		//log_info("Info: receive AP_APSERVER_HEART_BEAT_ACK AP=%s\n",ap_list[ap_index].AP_SN);
		printf("AP:%s's state is %d\n",ap_list[ap_index].AP_SN,ap_list[ap_index].status.online_state);
		printf("AP:%s's antenna number is %d\n",ap_list[ap_index].AP_SN,ap_list[ap_index].status.antenna_number);
        break;
	case AP_APSERVER_REQUEST_LOGINFO:
		strcpy(ap_ip, inet_ntoa(ap_list[ap_index].configure.AP_IP));
		Create_LogDir(path);
		strcpy(LogFilePath, path);
		//strcpy(path, ap_list[ap_index].configure.Remote_FilePath);
		snprintf(tftp, 256, "tftp %s get %s %s", ap_ip, ap_list[ap_index].configure.Remote_FilePath,LogFilePath); //to do
		status = system(tftp);//获取日志文件，存到本地，定时器结束后直接发送给web？信息的保存sendtoweb,  判断执行完毕，，，，，，to do
		if (-1 == status) {
			printf("tftp error");
		}
		else if (!WIFEXITED(status)) {
			printf("tftp error");
		}
		else if (WEXITSTATUS(status)) {
			printf("tftp error");
		}
		else
		{
			printf("tftp success");
		}
		//legalAPTimer[ap_index].ap_on_timer_flag = 0;
		//strcpy(AP_upgrade_log_ack[0], ap_list[ap_index].AP_SN);
		
		for (i = 0; i < max_ap; i++) 
		{
			if (strcmp(ap_list[ap_index].AP_SN, AP_upgrade_log_ack[i].AP_SN) == 0)
			{
				AP_upgrade_log_ack[i].ack = 0;
			}
		}
		log_info("Info: receive AP_APSERVER_REQUEST_LOGINFO_OK AP=%s\n", ap_list[ap_index].AP_SN);
		break;
	case AP_APSERVER_HEART_BEAT_ACK_WITH_STATUS:
		legalAPTimer[ap_index].heart_beat_timer = apServer_apAgent_heart_time;
		legalAPTimer[ap_index].heart_beat_timeout_times = 0;
		ap_status_assign_value(&ap_list[ap_index],pp);
		ap_list[ap_index].status.online_state = 1;
		for (i = 0; i < max_ap; i++)//获取软件升级的结果
		{
			if (strcmp(ap_list[ap_index].AP_SN, AP_upgrade_log_ack[i].AP_SN) == 0)
			{
				if (strcmp(ap_list[ap_index].status.version, AP_upgrade_log_ack[i].version) == 0)
				{
					AP_upgrade_log_ack[i].ack = 0;
				}
				else
				{
					AP_upgrade_log_ack[i].ack = 1;
				}
			}
		}
		//log_info("Info: receive AP_APSERVER_HEART_BEAT_ACK_WITH_STATUS AP=%s\n",ap_list[ap_index].AP_SN);
        break;
		
	default:
		//log_error("Error: Unknown protocol type = %d!\n", envelop->bProtocolSap);
		break;
	}


}

void template_assign_value(AP_Configuration_template* destination,AP_Configuration_template* source)
{

	destination->templateIndex=source->templateIndex;
	destination->ap_gateway.gateway=source->ap_gateway.gateway;
    destination->ap_gateway.subnetmask=source->ap_gateway.subnetmask;
	destination->DHCPOrStatic=source->DHCPOrStatic;
	destination->AP_IP=source->AP_IP;
	strcpy(destination->ap_login.login,source->ap_login.login);
	strcpy(destination->ap_login.login_psw,source->ap_login.login_psw);
	strcpy(destination->ap_ssid.ssid,source->ap_ssid.ssid);
	strcpy(destination->ap_ssid.ssid_psw,source->ap_ssid.ssid_psw);
	destination->NatOrBridge = source->NatOrBridge;
	destination->pool.start = source->pool.start;
	destination->pool.end = source->pool.end;
	destination->AP_Server_IP = source->AP_Server_IP;
	strcpy(destination->Remote_FilePath, source->Remote_FilePath);
	strcpy(destination->Scp_Username, source->Scp_Username);
	strcpy(destination->Scp_PassWord, source->Scp_PassWord);
}


void template_report()//将模板发送到Web
{
    int i=0,size=0;
    char sendbuf[bufsize_apToWeb1];    //申请一个发送数据缓存区
    char* p=sendbuf;
	p++;p++;

	sendbuf[0]=APSERVER_WEB_TEMPLATE_REPORT;
	for(i=0;i<max_template;i++)
		{
		if(legalTemplate[i])
			{
			size++;
			memcpy(p,(char*)&apTemplate[i],sizeof(AP_Configuration_template));
			p+=sizeof(AP_Configuration_template);
			}
		}
	sendbuf[1]=size;
/*
	printf("ip=%s\n",inet_ntoa(ttt));
	printf("sendbuf[0]=0x%x,sendbuf[1]=0x%x,sendbuf[2]=0x%x,sendbuf[3]=0x%x,sendbuf[4]=0x%x,sendbuf[5]=0x%x\n",sendbuf[0],sendbuf[1],sendbuf[2],sendbuf[3],sendbuf[4],sendbuf[5]);
	printf("sendbuf[6]=0x%x,sendbuf[7]=0x%x,sendbuf[8]=0x%x,sendbuf[9]=0x%x,sendbuf[10]=0x%x,sendbuf[11]=0x%x\n",sendbuf[6],sendbuf[7],sendbuf[8],sendbuf[9],sendbuf[10],sendbuf[11]);
	printf("sendbuf[12]=0x%x,sendbuf[13]=0x%x,sendbuf[14]=0x%x,sendbuf[15]=0x%x,sendbuf[16]=0x%x,sendbuf[17]=0x%x\n",sendbuf[12],sendbuf[13],sendbuf[14],sendbuf[15],sendbuf[16],sendbuf[17]);
	printf("sendbuf[18]=0x%x,sendbuf[19]=0x%x,sendbuf[20]=0x%x,sendbuf[21]=0x%x,sendbuf[22]=0x%x,sendbuf[23]=0x%x\n",sendbuf[18],sendbuf[19],sendbuf[20],sendbuf[21],sendbuf[22],sendbuf[23]);*/
	log_info("Info: send_to_web:template_report,size=%d\n",sendbuf[1]);
	sendto(sockWeb, sendbuf, bufsize_apToWeb1, 0, (struct sockaddr*)&addrWebSer, addrWeblen);  


}

void ap_report()
{
    int i=0,size=0;
    char sendbuf2[bufsize_apToWeb2];    //申请一个发送数据缓存区
    char* p;
	AP* ppp=ap_list;
	
	p=sendbuf2;
	sendbuf2[0]=APSERVER_WEB_AP_REPORT;
	p++;p++;
	for(i=0;i<max_ap;i++)
		{
		if(legalAP[i])
			{
			size++;
			memcpy(p,(char*)ppp,sizeof(AP));
			p=p+sizeof(AP);
			}
		ppp++;
		}
	sendbuf2[1]=size;
	log_info("Info: send_to_web:ap_report,size=%d\n",sendbuf2[1]);
	sendto(sockWeb, sendbuf2, bufsize_apToWeb2, 0, (struct sockaddr*)&addrWebSer, addrWeblen);   
}

void  connect_to_web()
{
    template_report();

	ap_report();
	
	
	Webtimerflag=WEB_APSERVER_CONNECTING;
	webTimer=web_apServer_heart_out_time;
}


void decode_web_template_modifyMsg(char* buf)//修改AP后，产生ACk，将ACK发给Web
{	
	int i=0,size = buf[1];
	template_operation_req* template_Operation;
	template_operation_ack* template_operation;
	char sendbuf[sizeof(template_operation_ack)*max_template+1]={"\0"};
	char* p=sendbuf;
	template_operation = (template_operation_ack*)(p+1);
	sendbuf[0]=APSERVER_WEB_TEMPLATE_MODIFY;
	if(size>0&&size<max_template)
		{
		for(i=0;i<size;i++)
			{
			template_Operation=(template_operation_req*)&buf[2];
			template_operation[i].templateIndex = template_Operation->ap_template.templateIndex;
			switch(template_Operation->command)
				{
				case TEMPLATE_ADD:
					legalTemplate[template_Operation->ap_template.templateIndex]=1;
					template_assign_value(&apTemplate[template_Operation->ap_template.templateIndex],&template_Operation->ap_template);
					printf("enter templdate add\n");
					print_templates();
					add_template_file(template_Operation->ap_template.templateIndex);
					template_operation[i].ack = 0;
					break;
				case TEMPLATE_DEL:
					printf("enter templdate del\n");
					if(legalTemplate[template_Operation->ap_template.templateIndex])
						{
						legalTemplate[template_Operation->ap_template.templateIndex] = 0;
						del_template_file(template_Operation->ap_template.templateIndex);
						template_operation[i].ack = 0;
						}
					else
						{
						log_info("Info: del template,but not exist\n");
						template_operation[i].ack = 1;
						}
					break;
				case TEMPLATE_MODIFY:
					printf("enter templdate modify\n");
					legalTemplate[template_Operation->ap_template.templateIndex]=1;
					template_assign_value(&apTemplate[template_Operation->ap_template.templateIndex],&template_Operation->ap_template);
					del_template_file(template_Operation->ap_template.templateIndex);
					add_template_file(template_Operation->ap_template.templateIndex);
					break;
					break;
				default:
				log_error("Error: Unknown templdate operation=%d\n", template_Operation->command);
				break;
				}
			}
		send_to_web(APSERVER_WEB_TEMPLATE_MODIFY,p,1+size*sizeof(template_operation_ack));
		log_info("Info: send_to_web:APSERVER_WEB_TEMPLATE_MODIFY\n");
		write_template_set();
		}
}
void decode_web_AP_modifyMsg(char* buf)
{
	int i=0,size = buf[1],apIndex=-1;
	legalAP_operation_req* ap_Operation;
	legalAP_operation_ack* ap_operation;
	char sendbuf[sizeof(legalAP_operation_ack)*max_ap+1]={"\0"};
	char* p=sendbuf;
	ap_operation = (legalAP_operation_ack*)(p+1);
	sendbuf[0]=APSERVER_WEB_AP_MODIFY;
	if(size>0&&size<max_ap)
		{
		for(i=0;i<size;i++)
			{
			ap_Operation=(legalAP_operation_req*)&buf[2];
			apIndex=LegalAP_find(ap_Operation->AP_SN);
			strcpy(ap_operation[i].AP_SN,ap_Operation->AP_SN);
			switch(ap_Operation->command)
				{
				case AP_ADD:
					printf("enter ap add\n");
					if(apIndex!=-1)
						{
						log_info("Info: add AP,but exist\n");
						ap_operation[i].ack = 1;
						}
					else
						{
						apIndex = assign_apIndex();
						if(apIndex!=-1)
							{
							legalAP[apIndex]=1;
							printf("add AP, SN=%s, templateID=%d\n",ap_Operation->AP_SN,ap_Operation->ap_template.templateIndex);
							strcpy(ap_list[apIndex].AP_SN,ap_Operation->AP_SN);
							ap_list[apIndex].configure.templateIndex=ap_Operation->ap_template.templateIndex;
							ap_operation[i].ack = 0;
							}
						else
							{
							log_error("Error: no enough apIndex!\n");
							ap_operation[i].ack = 1;
							}
						}
					break;
				case AP_DEL:
					printf("enter ap del\n");
					if(apIndex!=-1)
						{
						legalAP[apIndex]=0;
						ap_operation[i].ack = 0;
						}
					else
						{
						log_info("Info: del AP,but not exist\n");
						ap_operation[i].ack = 1;
						}
					break;
				case AP_MODIFY:
					printf("enter ap modify\n");
					if(apIndex==-1)
						{
						log_info("Info: modify AP,but not exist\n");
						ap_operation[i].ack = 1;
						break;
						}
					ap_operation[i].ack = 0;
					legalAP[apIndex]=1;
					strcpy(ap_list[apIndex].AP_SN,ap_Operation->AP_SN);
					break;

				default:
				log_error("Error: Unknown AP operation=%d\n", ap_Operation->command);
				break;
				}
			}
		send_to_web(APSERVER_WEB_AP_MODIFY,p,1+size*sizeof(legalAP_operation_ack));
		log_info("Info: send_to_web:APSERVER_WEB_AP_MODIFY\n");
		write_ap_set();
		}

}

void decode_web_AP_configurationMsg(char* buf)//把配置发送给AP，ack在main.c里进行回复
{
	int i=0,size = buf[1],apIndex=-1;
	AP_Configuration_template* ap_Operation;
	char* p;
	char sn[APSN_length]={"\0"};
	p=buf+2+sizeof(AP_Configuration_template);
	if(size>0&&size<max_ap)
		{
		for(i=0;i<size;i++)
			{
			ap_Operation=(AP_Configuration_template*)&buf[2];
			strncpy(sn,p,APSN_length);
			apIndex=LegalAP_find(sn);
			if(apIndex!=-1)
				{
				send_to_ap(apIndex,APSERVER_AP_CONFIGURE,ap_Operation);
				log_info("Info: send_to_ap:APSERVER_AP_CONFIGURE AP:%s\n",ap_list[apIndex].AP_SN);
				legalAPTimer[i].ap_on_timer_flag = 1;
				legalAPTimer[i].ap_on_timer = apServer_apAgent_heart_time;
				WebOntimerflag=1;
				webOnTimer=web_apServer_configure_time;
				legalAPTimer[apIndex].ap_on_timer_flag=1;
				}
			else
				{
				log_error("Error: Unknown AP SN=%s\n", sn);
				}

			p=p+APSN_length;

			}
		}

}
//Scp  username = ap_info->username filepath = ap_info->filepath     ///软件升级
//   -------------------------------------------------
//   |command|size =10|AP_opertion_req|*size|文件名？|
//   -------------------------------------------------
void decode_web_AP_log_requestMsg(char* buf)//done 解析获取日志信息，并将消息转发给对应的AP
{
	int i = 0, size = buf[1], apIndex = -1;
	legalAP_operation_req* ap_info;
	//char* p = NULL;
	char sn[APSN_length] = { "\0" };
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
				strcpy(AP_upgrade_log_ack[i].AP_SN, ap_info->AP_SN);//获取需要日志的AP的序列号
				send_to_ap(apIndex, APSERVER_AP_REQUEST_LOGINFO, &(ap_info->ap_template));
				log_info("Info: send_to_ap:APSERVER_AP_Request_LogInfo AP:%s\n", ap_list[apIndex].AP_SN);
				legalAPTimer[i].ap_on_timer_flag = 3;
				legalAPTimer[i].ap_on_timer = apServer_apAgent_log_get_time;
				WebOntimerflag = 3;
				webOnTimer = web_apServer_log_get_time;
				legalAPTimer[apIndex].ap_on_timer_flag = 3;
			}
			else
			{
				log_error("Error: Unknown AP SN=%s\n", sn);
			}
			ap_info = ap_info + 1;
		}
	}
}

void decode_web_AP_Soft_UpgradeMsg(char* buf) //todo 解析软件升级信息，将软件升级的文件转存到本地，并将文件转发给对应的AP
{
	int i = 0, size = buf[1], apIndex = -1;
	legalAP_operation_req* ap_info;
	//char* p = NULL;
	char sn[APSN_length] = { "\0" };
	//p = buf + 2;
	AP_Upgrade_Num = size;//升级AP的数量
	if (size > 0 && size < max_ap)
	{
		ap_info = (legalAP_operation_req*)&buf[2];
		snprintf(Scp_Exe, 512, "scp %s@%s:%s %s", ap_info->ap_template.Scp_Username, Web_Server_IP, ap_info->ap_template.Remote_FilePath, Loacl_Upgrade_FilePath);
		printf("scp to do is: %s", Scp_Exe);
		status = system(Scp_Exe);                    //Scp获取web的软件升级文件,存到本地  done
		if (-1 == status) {
			printf("scp error");
		}
		if (!WIFEXITED(status)) {
			printf("scp error");
		}
		if (WEXITSTATUS(status)) {
			printf("scp error");
		}
		else {
			printf("scp success");
		}
		//
		for (i = 0; i < size; i++)
		{
			//直接把WEB发送过来的软件升级路径传给AP
			strcpy(AP_upgrade_log_ack[i].AP_SN,ap_info->AP_SN);//存取 升级的AP信息用于记录比较是否升级成功,记录版本信息
			strcpy(AP_upgrade_log_ack[i].version, ap_info->ap_template.version);

			strcpy(sn, ap_info->AP_SN);
			apIndex = LegalAP_find(sn);
			if (apIndex != -1)
			{
				send_to_ap(apIndex, APSERVER_AP_SOFTWARE_UPGRADE, &(ap_info->ap_template));//转发给AP
				log_info("Info: send_to_ap:WEB_APSERVER_SOFTWARE_UPGRADE AP:%s\n", ap_list[apIndex].AP_SN);
				legalAPTimer[i].ap_on_timer_flag = 2;
				legalAPTimer[i].ap_on_timer = apServer_apAgent_upgrade_time;
				WebOntimerflag = 2;
				webOnTimer = web_apServer_upgrade_time;
				legalAPTimer[apIndex].ap_on_timer_flag = 2;
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
	int i=0,size = buf[1];
	AP_Configuration_template* template_operation;
	if(size>0&&size<max_template)
		{
		for(i=0;i<size;i++)
			{
			template_operation=(AP_Configuration_template*)&buf[2];
			legalTemplate[template_operation->templateIndex]=1;
			template_assign_value(&apTemplate[template_operation->templateIndex],template_operation);
			add_template_file(template_operation->templateIndex);
			}
		write_template_set();
		}
}

void remove_all_template_file()
{
	int i=0;
	char name[51]={0};
	for(i=0;i<max_template;i++)
		{
		snprintf(name,50,"../templates/%d",i);
		remove(name);
		}

}

void decode_web_AP_reportMsg(char* buf)
{
	int i=0,size = buf[1];
	AP* ap_operation;
	if(size>0&&size<max_ap)
		{
		for(i=0;i<size;i++)
			{
			ap_operation=(AP*)&buf[2];
			legalAP[i]=1;
			ap_configure_assign_value(&ap_list[i],ap_operation);
			ap_status_assign_value(&ap_list[i],ap_operation);
			}
		write_ap_set();//写入文件
		}
}


void send_web_heartbeat_withstatusMsg()
{
	int i=0,size = 0;
	char sendbuf[sizeof(AP)*max_ap+2]={"\0"};
	AP* ap_operation;
	char* p=sendbuf;
	ap_operation = (AP*)(p+2);
	sendbuf[0]=APSERVER_WEB_HEART_BEAT;
	for(i=0;i<max_ap;i++)
		{
		if(legalAP[i])
			{
			size++;
			strcpy(ap_operation->AP_SN,ap_list[i].AP_SN);
			//strncpy(ap_operation.AP_SN,ap_list[i].AP_SN,APSN_length);
			ap_configure_assign_value(ap_operation,&ap_list[i]);
			ap_status_assign_value(ap_operation,&ap_list[i]);
			ap_operation++;
			}
		}
	sendbuf[1]=size;
	send_to_web(APSERVER_WEB_HEART_BEAT,p,2+size*sizeof(AP));
	//log_info("Info: send_to_web:APSERVER_WEB_HEART_BEAT\n");
}


void  dispatch_webMsg(char* buf)
{
	switch(buf[0])
	{
	case APSERVER_WEB_TEMPLATE_REPORT:
		remove_all_template_file();
		init_legalTemplate();
		decode_web_template_reportMsg(buf);
        break;
	case APSERVER_WEB_AP_REPORT:
		init_legalAP();
		decode_web_AP_reportMsg(buf);
        break;
	case APSERVER_WEB_TEMPLATE_MODIFY:
		decode_web_template_modifyMsg(buf);
        break;
	case APSERVER_WEB_AP_MODIFY:
		decode_web_AP_modifyMsg(buf);
        break;
	case APSERVER_WEB_AP_CONFIGURE:
		decode_web_AP_configurationMsg(buf);
        break;
	case APSERVER_WEB_HEART_BEAT:
		send_web_heartbeat_withstatusMsg();
		Webtimerflag=WEB_APSERVER_ON;
		webTimer=web_apServer_heart_out_time;
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
	time_t timep;
	struct tm* ti;
	time(&timep);
	ti = gmtime(&timep);
	snprintf(file_path, 256, "mkdir /home/book/日志信息_%d年%d月%d日%d时%d分%d秒", 1900 + ti->tm_year, 1 + ti->tm_mon, ti->tm_mday, 8 + ti->tm_hour, ti->tm_min, ti->tm_sec);
	//printf("file_path is:%s\n",file_path);
	status = system(file_path);//建立目录
	if (-1 == status) {
		printf("mkdir error");
	}
	else if (!WIFEXITED(status)) {
		printf("mkdir error");
	}
	else if (WEXITSTATUS(status)) {
		printf("mkdir error");
	}
	else 
	{
		printf("mkdir success");
	}
	strcpy(filename, &file_path[6]);
}

