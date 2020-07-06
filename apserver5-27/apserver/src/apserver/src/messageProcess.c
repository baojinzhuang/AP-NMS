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
extern AP_timer legalAPTimer[max_ap];

extern AP_socket ap_socket_list[max_ap];
extern int sockSer;

extern int sockWeb;
extern struct sockaddr_in addrWebSer;  //创建一个记录地址信息的结构体 
extern socklen_t addrWeblen;

extern int Webtimerflag; //0,不启动，1,启动中，2,正常通信中
extern int WebOntimerflag; //0,正常通信中，1,正常通信配置中，2,正常通信升级中
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





void send_to_ap(int apIndex, int command,AP_Configuration_template* tempTemplate)
{
    char sendbuf[sizeof(AP)+1];    //申请一个发送数据缓存区
    char* p=sendbuf;
	AP_Configuration_template* pp=&(apTemplate[ap_list[apIndex].configure.templateIndex]);

    *p=command;
	p++;
	printf("after p++ apIndex=%d\n",apIndex);
	//return;

	if(legalAP[apIndex]==1)
		switch(command)
			{
			case APSERVER_AP_REGISTER_ACK:
				memcpy(p,(char *)pp,sizeof(AP_Configuration_template));
				sendto(sockSer,sendbuf,sizeof(AP_Configuration_template)+2,0,(struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen); 
				break;
			case APSERVER_AP_REGISTER_REJ:
				sendto(sockSer,sendbuf,2,0,(struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen); 
				break;
			case APSERVER_AP_CONFIGURE:
				memcpy(p,tempTemplate,sizeof(AP_Configuration_template));
				sendto(sockSer,sendbuf,sizeof(AP_Configuration_template)+2,0,(struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen); 
				break;
			case APSERVER_AP_HEART_BEAT:
				sendto(sockSer,sendbuf,2,0,(struct sockaddr*)&ap_socket_list[apIndex].addrAP, ap_socket_list[apIndex].addrlen); 
				break;

			default:
				printf("undefined command AP Server to AP");
		}


}

void configure_reprot()
{
    int i=0,size = 0;
	AP_configuration_ack* ap_configuration;
	char sendbuf[sizeof(AP_configuration_ack)*max_ap+1]={"\0"};
	char* p=sendbuf+2;

	ap_configuration = (AP_configuration_ack*)p;

	for(i=0;i<max_ap;i++)
		if(legalAPConfigureResult[i])
			{
			size++;
			ap_configuration->ack=legalAPConfigureResult[i];
			strcpy(ap_configuration->AP_SN,ap_list[i].AP_SN);
			}
			
	sendbuf[1]=size;
	sendbuf[0]=APSERVER_WEB_AP_CONFIGURE;
	sendto(sockWeb,sendbuf,size*sizeof(AP_configuration_ack)+2,0,(struct sockaddr*)&addrWebSer, addrWeblen); 
	printf("enter configure report size=%d",size);

	for(i=0;i<max_ap;i++)
		legalAPConfigureResult[i]=0;

}


void send_to_web(char command,char* sendbuf2,int length)
{

	switch(command)
	{
		case APSERVER_WEB_TEMPLATE_REPORT:
		template_report();
		break;
		case APSERVER_WEB_AP_REPORT:
		ap_report();
		break;
		case APSERVER_WEB_TEMPLATE_MODIFY:
		sendto(sockWeb,sendbuf2,length,0,(struct sockaddr*)&addrWebSer, addrWeblen); 
		break;
		case APSERVER_WEB_AP_MODIFY:
		sendto(sockWeb,sendbuf2,length,0,(struct sockaddr*)&addrWebSer, addrWeblen); 
		break;
		case APSERVER_WEB_AP_CONFIGURE:
		configure_reprot(); 
		break;
		case APSERVER_WEB_HEART_BEAT:
		sendto(sockWeb,sendbuf2,length,0,(struct sockaddr*)&addrWebSer, addrWeblen); 
		break;
	
		default:
		printf("undefined command AP Server to WEB");
	}
	
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
}

void ap_status_assign_value(AP* destination,AP* source)
{
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
		printf("AP:%s's user number is %d\n",ap_list[ap_index].AP_SN,ap_list[ap_index].status.user_number);
		printf("AP:%s's antenna number is %d\n",ap_list[ap_index].AP_SN,ap_list[ap_index].status.antenna_number);
        break;
	case AP_APSERVER_HEART_BEAT_ACK_WITH_STATUS:
		legalAPTimer[ap_index].heart_beat_timer = apServer_apAgent_heart_time;
		legalAPTimer[ap_index].heart_beat_timeout_times = 0;
		ap_status_assign_value(&ap_list[ap_index],pp);
		ap_list[ap_index].status.online_state = 1;
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
}


void template_report()
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



void decode_web_template_modifyMsg(char* buf)
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
void decode_web_AP_configurationMsg(char* buf)
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
		write_ap_set();
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

	default:
		//log_error("Error: Unknown protocol type = %d!\n", envelop->bProtocolSap);
		break;
	}


}


