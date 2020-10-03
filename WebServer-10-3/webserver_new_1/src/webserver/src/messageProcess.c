/*
auth:	Baojinzhuang
date:	2020-01-07
history:	
messageProcess.c
*/

#include<stdio.h>
#include<stdlib.h> 
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<stdarg.h>
#include<ctype.h>

#include"function.h"

int mysql_connect()//连接数据库
{
	int ret = -1;
	conn = mysql_init(&mysql);
	if (conn == NULL) 
	{
		ret = mysql_errno(&mysql);
		printf("mysql_init error: %s\n", mysql_error(&mysql));
		return ret;
	}
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");//编码设置
	conn = mysql_real_connect(conn, "127.0.0.1", "root", "xlh123xlh", "nmsdatabase", 0, NULL, 0);
	if (conn == NULL) 
	{
		ret = mysql_errno(&mysql);
		printf("mysql_real_connect error, err is: %s\n", mysql_error(&mysql));
		return ret;
	}
	printf("mysql_real_connect ok...\n");//链接数据库完成
	return ret;
}



int Fill_apTemplate()  //将模板数据写入到apTemplate数组中
{
	int i = 0;
	int ret = -1;
	ret = mysql_query(conn, "select * from config");
	if (ret != 0) {
		printf("mysql_select error: %s\n", mysql_error(&mysql));
	}
	result = mysql_store_result(&mysql);
	if (result == NULL)
	{
		printf("mysql_store_result error_97: %s\n", mysql_error(&mysql));
	}
	while ((row = mysql_fetch_row(result)))
	{
		(&apTemplate[i])->templateIndex = atoi(&row[0][0]);
		strcpy((&apTemplate[i])->ap_ssid.ssid, &row[1][0]);
		strcpy(((&apTemplate[i]))->ap_ssid.ssid_psw, &row[2][0]);
		strcpy((&apTemplate[i])->ap_login.login, &row[3][0]);
		strcpy((&apTemplate[i])->ap_login.login_psw, &row[4][0]);
		(&apTemplate[i])->NatOrBridge = atoi(&row[5][0]);
		(&apTemplate[i])->pool.start.s_addr = inet_addr(&row[6][0]);
		(&apTemplate[i])->pool.end.s_addr = inet_addr(&row[7][0]);
		(&apTemplate[i])->DHCPOrStatic = atoi(&row[8][0]);
		(&apTemplate[i])->AP_IP.s_addr = inet_addr(&row[9][0]);
		(&apTemplate[i])->ap_gateway.gateway.s_addr = inet_addr(&row[10][0]);
		(&apTemplate[i])->AP_Server_IP.s_addr = inet_addr(&row[11][0]);
		(&apTemplate[i])->ap_gateway.subnetmask.s_addr = inet_addr(&row[12][0]);
		printf("Fill_apTemplate AP_SSID:%s\n",&row[1][0]);
		i++;
	}
	mysql_free_result(result);
	return (i);
}

int Fill_ap_list()//将数据库的AP数据写入ap数组
{
	int i = 0;
	int ret = -1;
	ret = mysql_query(conn, "select * from AP");
	if (ret != 0) 
	{
		printf("mysql_select error: %s\n", mysql_error(&mysql));
	}
	result = mysql_store_result(&mysql);
	if (result == NULL)
	{
		printf("mysql_store_result error: %s\n", mysql_error(&mysql));
	}
	while ((row = mysql_fetch_row(result)))
	{
		strcpy((&ap_list[i])->AP_SN, &row[0][0]);
		(&ap_list[i])->configure.templateIndex = atoi(&row[1][0]);
		strcpy((&ap_list[i])->configure.ap_ssid.ssid, &row[2][0]);
		strcpy((&ap_list[i])->configure.ap_ssid.ssid_psw, &row[3][0]);
		strcpy((&ap_list[i])->configure.ap_login.login, &row[4][0]);
		strcpy((&ap_list[i])->configure.ap_login.login_psw, &row[5][0]);
		(&ap_list[i])->configure.NatOrBridge = atoi(&row[6][0]);
		(&ap_list[i])->configure.pool.start.s_addr = inet_addr(&row[7][0]);
		(&ap_list[i])->configure.pool.end.s_addr = inet_addr(&row[8][0]);
		(&ap_list[i])->configure.DHCPOrStatic = atoi(&row[9][0]);
		(&ap_list[i])->configure.AP_IP.s_addr = inet_addr(&row[10][0]);
		(&ap_list[i])->configure.ap_gateway.gateway.s_addr = inet_addr(&row[11][0]);
		(&ap_list[i])->configure.AP_Server_IP.s_addr = inet_addr(&row[12][0]);
		(&ap_list[i])->status.online_state = row[13][0];
		(&ap_list[i])->status.user_number = row[14][0];
		(&ap_list[i])->status.model = row[15][0];
		strcpy((&ap_list[i])->status.version, &row[16][0]);
		(&ap_list[i])->status.antenna_number = row[17][0];
		(&ap_list[i])->configure.ap_gateway.subnetmask.s_addr = inet_addr(&row[18][0]);
		//printf("Fill_ap_list AP_Template_index:%d\n", row[1][0]);
		i++;
	}
	mysql_free_result(result);
	return (i);
}


int LegalAPServer_find(struct      sockaddr_in* apserver_socket_addr)//查找ap_sever的序列号
{
    int i=0;
	for(i=0;i<max_ap_server;i++)
		{
			if(apServer_socket_list[i].addrAP.sin_addr.s_addr==apserver_socket_addr->sin_addr.s_addr)
				return i;
		}
	return -1;
}

int assign_apIndex()//初始化ap
{
    int i=0;
	for(i=0;i<max_ap;i++)
		{
			if(legalAP[i]==0)
				return i;
		}
	return -1;

}

void ap_modify_report_assign_value(legalAP_operation_ack* destination, legalAP_operation_ack* source)//ack的赋值
{
	destination->ack = source->ack;
	strcpy(destination->AP_SN, source->AP_SN);
	//printf("ap_modify_report_assign_value APSN %s  ack %c\n",source->AP_SN,source->ack);
}
void ap_upgrade_report_assign_value(AP_upgrade_ack* destination, AP_upgrade_ack* source)//ack的赋值
{
	destination->ack = source->ack;
	strcpy(destination->AP_SN, source->AP_SN);
	printf("ap_upgrade_report_assign_value APSN %s ACK %d\n",source->AP_SN,source->ack);
	strcpy(destination->Scp_Username, source->Scp_Username);
	strcpy(destination->Scp_PassWord, source->Scp_PassWord);
	strcpy(destination->Remote_FilePath, source->Remote_FilePath);	
}
void ap_log_report_assign_value(AP_log_ack* destination, AP_log_ack* source)//ack的赋值
{
	destination->ack = source->ack;
	strcpy(destination->AP_SN, source->AP_SN);
	strcpy(destination->Scp_Username, source->Scp_Username);
	strcpy(destination->Scp_PassWord, source->Scp_PassWord);
	strcpy(destination->Remote_FilePath, source->Remote_FilePath);	
}

void ap_configure_report_assign_value(AP_configuration_ack* destination,AP_configuration_ack* source)//配置赋值 把source的东西付给destination
{
	destination->ack = source->ack;
	strcpy(destination->AP_SN, source->AP_SN);
	printf("ap_configure_report_assign_value:%s %c  %s %c\n",source->AP_SN,source->ack,destination->AP_SN,destination->ack);

}

void template_modify_report_assign_value(template_operation_ack* destination,template_operation_ack* source)
{
	destination->ack = source->ack;
	destination->templateIndex = source->templateIndex;
}

void ap_configure_assign_value(AP* destination,AP* source)
{
	strcpy(destination->AP_SN, source->AP_SN);
	destination->configure.ap_gateway.gateway=source->configure.ap_gateway.gateway;
    destination->configure.ap_gateway.subnetmask=source->configure.ap_gateway.subnetmask;
	strcpy(destination->configure.ap_login.login,source->configure.ap_login.login);
	strcpy(destination->configure.ap_login.login_psw,source->configure.ap_login.login_psw);
	strcpy(destination->configure.ap_ssid.ssid,source->configure.ap_ssid.ssid);
	strcpy(destination->configure.ap_ssid.ssid_psw,source->configure.ap_ssid.ssid_psw);
	destination->configure.NatOrBridge = source->configure.NatOrBridge;
	destination->configure.pool.start = source->configure.pool.start;
	destination->configure.pool.end = source->configure.pool.end;
	destination->configure.templateIndex = source->configure.templateIndex;
	destination->configure.DHCPOrStatic = source->configure.DHCPOrStatic;
	destination->configure.AP_IP = source->configure.AP_IP;
	destination->configure.AP_Server_IP = source->configure.AP_Server_IP;
	destination->configure.options = source->configure.options;
	strcpy(destination->configure.Scp_Username, source->configure.Scp_Username);
	strcpy(destination->configure.Scp_PassWord, source->configure.Scp_PassWord);
    strcpy(destination->configure.Remote_FilePath, source->configure.Remote_FilePath);	
}


void template_assign_value(AP_Configuration_template* destination,AP_Configuration_template* source)
{
	destination->ap_gateway.gateway=source->ap_gateway.gateway;
    destination->ap_gateway.subnetmask=source->ap_gateway.subnetmask;
	strcpy(destination->ap_login.login,source->ap_login.login);
	strcpy(destination->ap_login.login_psw,source->ap_login.login_psw);
	strcpy(destination->ap_ssid.ssid,source->ap_ssid.ssid);
	strcpy(destination->ap_ssid.ssid_psw,source->ap_ssid.ssid_psw);
	destination->NatOrBridge = source->NatOrBridge;
	destination->pool.start = source->pool.start;
	destination->pool.end = source->pool.end;
	destination->templateIndex = source->templateIndex;
	destination->DHCPOrStatic = source->DHCPOrStatic;
	destination->AP_IP = source->AP_IP;
	destination->AP_Server_IP = source->AP_Server_IP;
	destination->options = source->options;
	strcpy(destination->Scp_Username, source->Scp_Username);
	strcpy(destination->Scp_PassWord, source->Scp_PassWord);
    stpcpy(destination->Remote_FilePath, source->Remote_FilePath);	
}//修改完成


void ap_status_assign_value(AP* destination,AP* source)//status 的赋值
{
	
	strcpy(destination->AP_SN, source->AP_SN);
	destination->status.antenna_number = source->status.antenna_number;
	destination->status.model = source->status.model;
	destination->status.user_number = source->status.user_number;
	destination->status.online_state = source->status.online_state;
	strcpy(destination->status.version,source->status.version);
	destination->status.options = source->status.options;
}


void decode_template_report(char* buf, AP_Configuration_template* temp)//解析apserver发送过来的模板
{
	int i=0,size=buf[1];
	char* p=buf;
	p++;p++;
	AP_Configuration_template* pp=(AP_Configuration_template*)p;
	for(i=0;i<size;i++)
		{
		template_assign_value(temp+i,pp+i);
		}
}

void decode_ap_report(char* buf, AP* temp)//解析apserver发送过来的ap
{
	
	int i=0,size=buf[1];
	char* p=buf;
	p++;p++;
	AP* pp=(AP*)p;
	for(i=0;i<size;i++)
	{
	  ap_status_assign_value(temp + i, pp + i);
	  ap_configure_assign_value(temp + i, pp + i);
	}
}

void decode_template_modify_report(char* buf,int apServer_index)//解析apserver发送过来的更改模板确认信息
{

	int i=0,size=buf[1];
	char* p=buf;
	p++;
	p++;
	template_operation_ack* pp=(template_operation_ack*)p;
	for(i=0;i<size;i++)
		{
		template_modify_report_assign_value(&Template_Ack[Num_i1], pp + i);
		Template_Ack[Num_i1].apServer_index = apServer_index;
		//printf("Template_Ack[Num_i] templateindex %d\n",Template_Ack[Num_i1].templateIndex);
		Num_i1++;
		}
}

void decode_ap_status_reporter(char* buf,AP* temp)//解析apserver发送过来的状态信息
{

	int i = 0, size = buf[1];
	char* p = buf;
	p++; p++;
	AP* pp = (AP*)p;
	for (i = 0; i < size; i++)
	{
		ap_status_assign_value(temp + i, pp + i);
	}
	printf(" Decode_ap_status_reporter size:%d\n",size);
}

void decode_ap_modify_report(char* buf,int apServerindex)//解析apserver发送过来的更改ap确认信息 修改过
{
	int i=0,size=buf[1];
	char* p=buf;
	p++;p++;
	legalAP_operation_ack* pp=(legalAP_operation_ack*)&buf[2];
	for(i=0;i<size;i++)
		{
		ap_modify_report_assign_value(&legalAP_Ack[Num_i2],pp+i);
		legalAP_Ack[Num_i2].apServerindex = apServerindex;
		Num_i2++;
		}
}
void decode_ap_upgrade_report(char* buf,int apServer_index)//解析apserver发送过来的更改ap确认信息 修改过
{
	int i=0,size=buf[1];
	char* p=buf;
	p++;p++;
	AP_upgrade_ack* pp=(AP_upgrade_ack*)p;
	for(i=0;i<size;i++)
		{
		ap_upgrade_report_assign_value(&legalAP_upgrade_Ack[SoftWare_Nums],pp+i);
		legalAP_upgrade_Ack[SoftWare_Nums].apServer_index = apServer_index;
		SoftWare_Nums++;
		}
}
void decode_ap_log_report(char* buf,int apServer_index)//解析apserver发送过来的更改ap确认信息 修改过
{
	int i=0,size=buf[1];
	char* p=buf;
	p++;p++;
	AP_log_ack* pp=(AP_log_ack*)p;
	for(i=0;i<size;i++)
		{
		ap_log_report_assign_value(&legalAP_log_Ack[GetLog_Nums],pp+i);
		legalAP_log_Ack[GetLog_Nums].apserver_index = apServer_index;
		GetLog_Nums++;
		}
	printf(" decode_ap_log_report  APSN[0]:%s, FILEPATH:%s, ack : %c\n",legalAP_log_Ack[0].AP_SN,legalAP_log_Ack[0].Remote_FilePath,legalAP_log_Ack[0].ack);
}

void decode_ap_configure_report(char* buf,int apServer_index)//解析apserver发送过来的配置信息
{
	int i=0,size=buf[1];
	char* p=buf;
	p++;p++;
	AP_configuration_ack* pp=(AP_configuration_ack*)p;
	for(i=0;i<size;i++)
		{
		ap_configure_report_assign_value(&AP_Configure_Ack[Configure_Nums],pp+i);
		AP_Configure_Ack[Configure_Nums].apServer_index = apServer_index;
		printf("AP_Configure_Ack[Configure_Nums] apsn %s ack %d\n",AP_Configure_Ack[Configure_Nums].AP_SN,AP_Configure_Ack[Configure_Nums].ack);
		Configure_Nums++;
		}
}



//AP_Configuration_template apTemplate[max_template];
//AP ap_list[max_ap];
//int legalTemplate[max_template];
//int legalAP[max_ap];
void Fill_APServer_socket_list()//将apserver的信息写入apServer_socket_list数组
{
	int i = 0;
	int ret = -1;
	ret = mysql_query(conn, "select * from ap_server");
	if (ret != 0) {
		printf("mysql_select error: %s\n", mysql_error(&mysql));
	}
	result = mysql_store_result(&mysql);
	if (result == NULL)
	{
		printf("mysql_store_result error: %s\n", mysql_error(&mysql));
	}
	while ((row = mysql_fetch_row(result)))
	{
		(&apServer_socket_list[i])->addrAP.sin_addr.s_addr = inet_addr(&row[1][0]);
		i++;
	}
	mysql_free_result(result);
}

void print_info(void)//打印信息
{
	int i = 0;
	printf("==========================================================\n");
	while ((&apServer_socket_list[i])->addrAP.sin_addr.s_addr)
	{
		printf("AP_server_list:%s\n", inet_ntoa((&apServer_socket_list[i])->addrAP.sin_addr));
		i++;
	}
	i = 0;
	printf("==========================================================\n");
	for (i = 0; i < 8; i++)
	{
		printf("apTemplate:tenplateIndex,ap_ssid:%d, %s\n", apTemplate[i].templateIndex, apTemplate[i].ap_ssid.ssid);

	}
	i = 0;
	printf("==========================================================\n");
	for (i = 0; i < 8; i++)
	{
		printf("ap_list:AP_SN,apTemplate:%s, %d\n", ap_list[i].AP_SN, ap_list[i].configure.templateIndex);
	
	}
}

void update_legalTemplate_del(int index)//删除模板后，更新数组apTemplate
{
	int i= 0;
	for (i = 0; i < max_template; i++)
	{
		if (apTemplate[i].templateIndex == index)
		{
			break;
		}
	}
	memset(&apTemplate[i], 0, sizeof(AP_Configuration_template));
	legalTemplate[i] = 0;
}//AP_Configuration_template* apTemplate 

void update_legalTemplate_add(template_operation_req *template_req)//添加模板后，更新数组apTemplate
{
	int i= 0;
		for (i = 0; i < max_template; i++)
		{
			if (legalTemplate[i] == 0)
				break;
		}
		legalTemplate[i] = 1;
		template_assign_value(&apTemplate[i], (&template_req->ap_template));
}

void update_legalTemplate_add_o(AP_Configuration_template* ap_Template)//删除模板后，更新数组apTemplate
{
	int i= 0;
	for (i = 0; i < max_template; i++)
	{
		if (legalTemplate[i] == 0)
			break;
	}
	legalTemplate[i] = 1;
	template_assign_value(&apTemplate[i],ap_Template);
}

void update_legalAP_del(char* index)//删除ap后，更新数组ap_list
{
	int i= 0;
	for (i = 0; i < max_ap; i++)
	{
		if (strcmp(ap_list[i].AP_SN,index) == 0)
		{
			break;
		}
	}
	memset(&ap_list[i], 0, sizeof(AP));
	legalAP[i] = 0;
}
void update_legalAP_add(legalAP_operation_req* AP_req)
{
	int i= 0;
	for (i = 0; i < max_ap; i++)
	{
		if (legalAP[i] == 0)
			break;
	}
	legalAP[i] = 1;
	strcpy(ap_list[i].AP_SN , AP_req->AP_SN);
	template_assign_value((&ap_list[i].configure),(&AP_req->ap_template));
}
void update_legalAP_add_o(AP* AP_list)
{//添加ap后，更新数组ap_list
	int i= 0;
	for (i = 0; i < max_ap; i++)
	{
		if (legalAP[i] == 0)
			break;
	}
	legalAP[i] = 1;
	strcpy(ap_list[i].AP_SN,AP_list->AP_SN);
	ap_configure_assign_value(&ap_list[i], AP_list);
	ap_status_assign_value(&ap_list[i], AP_list);
}

