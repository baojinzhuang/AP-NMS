/*
auth:	Baojinzhuang
date:	2020-01-07
histor:
communicate_with_apserver.c
*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<stdarg.h>
#include<ctype.h>
#include"function.h"

int  control_command_to_APserver()//读取Control表的命令，进行解析判断，并发送给对应的AP_Server
{
	template_operation_req template_req[max_template];
	legalAP_operation_req  legalAP_req[256];
	//AP_Configuration_template ap_template[256];

	
	char username[50] = "xlh@";//xlh@Web_Server_IP,Web_Server_IP的ip改变而改变
	strcat(username,Web_Server_IP);
	//printf("username:%s\n",username);

	
	char pwd[20] = "xlh";
	int size = -1;
	int i = 0, k = 0, j = 0;
	int obj = -1;
	int index = -1;
	int operation = -1;
	int ret = -1;
	int command = -1;
	char res[500] = { 0 };
	char input[15][30];
	
	char* pn = NULL;
	char tn[200] = "\0";
	char gn[50] = "\0";
	//printf("flag_begin = %d\n",flag_begin);

	if(flag_begin == 0)
	{
	ret = mysql_query(conn, "select * from control where id = ((select id from (select min(id) id from  control c1 where state = 0) t1))");
	if (ret != 0) {
		printf("mysql_select error: %s\n", mysql_error(&mysql));
		mysql_connect();//重连
	}
	result = mysql_store_result(&mysql);
	if (result == NULL)
	{
		printf("There is no command to APServer...\n");
	}
	//command_rows = mysql_num_rows(result);//取出的命令的行数
	//int field_num = mysql_field_count(&mysql); //列数
	// row[0]命令的序号  row[1] 对象 row[2] 数量 row[4] 时间  row[3] 操作 row[5] 执行状态  row[6] content
	else
	{
		
		i = 0, k = 0, j = 0, index = -1;
		while ((row = mysql_fetch_row(result)))
		{
			flag_begin = 1;	
			obj = atoi(&row[1][0]);
			size = atoi(&row[2][0]);
			operation = atoi(&row[3][0]);
			strcpy(template_tempindex, &row[6][0]);
			switch (obj)
			{
			case Template:
				printf("Template send modify\n");
				command = APSERVER_WEB_TEMPLATE_MODIFY;
				switch (operation)
				{
//template_ID	SSID_name	SSID_psw	Login_name	Login_psw	NatOrBridge	pool_start	pool_end	DhcpOrStatic	ip_address	gateway	APServer_IP
				case add:
					snprintf(sql_insert, 512, "select * from config where template_ID in (%s)", &row[6][0]);
					mysql_query(conn, sql_insert);
					result1 = mysql_store_result(&mysql);
					if (result1 == NULL)
					{
						printf("mysql_store_result1 error: %s\n", mysql_error(&mysql));
						printf("Template表中没有ID:%s\n",&row[6][0]);
					}
					else
					{
						printf("Template_ADD:sql_insert : %s\n", sql_insert);
						row1 = mysql_fetch_row(result1);
						while (size)
						{
							(&template_req[i])->command = TEMPLATE_ADD;
							(&template_req[i])->ap_template.templateIndex = atoi(&row1[0][0]);
							strcpy((&template_req[i])->ap_template.ap_ssid.ssid, &row1[1][0]);
							strcpy((&template_req[i])->ap_template.ap_ssid.ssid_psw, &row1[2][0]);
							strcpy((&template_req[i])->ap_template.ap_login.login, &row1[3][0]);
							strcpy((&template_req[i])->ap_template.ap_login.login_psw, &row1[4][0]);
							(&template_req[i])->ap_template.NatOrBridge = atoi(&row1[5][0]);
							(&template_req[i])->ap_template.pool.start.s_addr = inet_addr(&row1[6][0]);
							(&template_req[i])->ap_template.pool.end.s_addr = inet_addr(&row1[7][0]);
							(&template_req[i])->ap_template.DHCPOrStatic = atoi(&row1[8][0]);
							(&template_req[i])->ap_template.AP_IP.s_addr = inet_addr(&row1[9][0]);
							(&template_req[i])->ap_template.ap_gateway.gateway.s_addr = inet_addr(&row1[10][0]);
							(&template_req[i])->ap_template.AP_Server_IP.s_addr = inet_addr(&row1[11][0]);
							(&template_req[i])->ap_template.ap_gateway.subnetmask.s_addr = inet_addr(&row1[12][0]);
							for (index = 0; index < max_ap_server; index++)
							{
								send_to_apServer(index, command, NULL, &template_req[i], NULL, 1, NULL);
								legalAPServerTimer[index].apServer_timer_flag = WEB_APSERVER_ON;
								//legalAPServerTimer[index].apServer_on_timer = web_apServer_configure_time;
								legalAPServerTimer[index].apServer_on_timer_flag = 0;
							}
//							printf("AP_Server的IP为:%s\n", &row1[11][0]);
							update_legalTemplate_add(&template_req[i]);
							row1 = mysql_fetch_row(result1);
							size--; i++;
						}
						SumTimer[0].flag = 0;//开启定时器
						SumTimer[0].timer= 3;//wait 3s
					}
					mysql_free_result(result1);
					break;

				case del:
					(&template_req[i])->command = TEMPLATE_DEL;
					(&template_req[i])->ap_template.templateIndex = atoi(&row[6][0]);
					for (index = 0; index < max_ap_server; index++)
					{
						send_to_apServer(index, command, NULL, &template_req[i], NULL, 1, NULL);
					}
					snprintf(sql_insert, 512, "update control set state = 2 where id = %d", atoi(&row[0][0]));
					printf("del template sql_insert is:%s\n", sql_insert);//直接成功
					mysql_query(conn, sql_insert);
					update_legalTemplate_del((&template_req[i])->ap_template.templateIndex);
					SumTimer[0].flag = 0;//开启定时器
					SumTimer[0].timer= 3; //wait 3s
					break;

				case modify:
					snprintf(sql_insert, 512, "select * from config where template_ID in (%s)", &row[6][0]);
					mysql_query(conn, sql_insert);
					result1 = mysql_store_result(&mysql);
					if (result1 == NULL)
					{
						printf("mysql_store_result1 error_276: %s\n", mysql_error(&mysql));
						printf("sql_insert is:%s\n", sql_insert);
					}
					else
					{
						printf("sql_insert is:%s\n", sql_insert);
						row1 = mysql_fetch_row(result1);
						while (size)
						{
							(&template_req[i])->command = TEMPLATE_MODIFY;
							(&template_req[i])->ap_template.templateIndex = atoi(&row1[0][0]);
							strcpy((&template_req[i])->ap_template.ap_ssid.ssid, &row1[1][0]);
							strcpy((&template_req[i])->ap_template.ap_ssid.ssid_psw, &row1[2][0]);
							strcpy((&template_req[i])->ap_template.ap_login.login, &row1[3][0]);
							strcpy((&template_req[i])->ap_template.ap_login.login_psw, &row1[4][0]);
							(&template_req[i])->ap_template.NatOrBridge = atoi(&row1[5][0]);
							(&template_req[i])->ap_template.pool.start.s_addr = inet_addr(&row1[6][0]);
							(&template_req[i])->ap_template.pool.end.s_addr = inet_addr(&row1[7][0]);
							(&template_req[i])->ap_template.DHCPOrStatic = atoi(&row1[8][0]);
							(&template_req[i])->ap_template.AP_IP.s_addr = inet_addr(&row1[9][0]);
							(&template_req[i])->ap_template.ap_gateway.gateway.s_addr = inet_addr(&row1[10][0]);
							(&template_req[i])->ap_template.AP_Server_IP.s_addr = inet_addr(&row1[11][0]);
							(&template_req[i])->ap_template.ap_gateway.subnetmask.s_addr = inet_addr(&row1[12][0]);
							for (index = 0; index < max_ap_server; index++)//找出AP所属的AP_Server的序列号
							{
								send_to_apServer(index, command, NULL, &template_req[i], NULL, 1, NULL);
							}
							update_legalTemplate_del((&template_req[i])->ap_template.templateIndex);
							update_legalTemplate_add(&template_req[i]);
							row1 = mysql_fetch_row(result1);
							size--; i++;
						}
					}
					SumTimer[0].flag = 0;//开启Template定时器
					SumTimer[0].timer= 3;//wait 3s
					mysql_free_result(result1);
					break;
				}
				break;

			case Ap:
				command = APSERVER_WEB_AP_MODIFY;
				switch (operation)
				{
				case add:
					
					//AP是单个增加的，所以每次只发送一个增加的AP信息给指定的APServer
					add_quotae(&row[6][0],res);
					snprintf(sql_insert, 512, "select * from AP where serials_ID in (%s)", res);
					mysql_query(conn, sql_insert);
					result1 = mysql_store_result(&mysql);

					
					if (result1 == NULL)
					{
						printf("mysql_store_result error: %s\n", mysql_error(&mysql));
					}
					else
					{
						printf("sql_add_ap is:%s\n", sql_insert);
						
						while ((row1 = mysql_fetch_row(result1)))
						{
							(&legalAP_req[i])->command = AP_ADD;
							strcpy((&legalAP_req[i])->AP_SN, &row1[0][0]);
							(&legalAP_req[i])->ap_template.templateIndex = atoi(&row1[1][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_ssid.ssid, &row1[2][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_ssid.ssid_psw, &row1[3][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_login.login, &row1[4][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_login.login_psw, &row1[5][0]);
							(&legalAP_req[i])->ap_template.NatOrBridge = atoi(&row1[6][0]);
							(&legalAP_req[i])->ap_template.pool.start.s_addr = inet_addr(&row1[7][0]);
							(&legalAP_req[i])->ap_template.pool.end.s_addr = inet_addr(&row1[8][0]);
							(&legalAP_req[i])->ap_template.DHCPOrStatic = atoi(&row1[9][0]);
							(&legalAP_req[i])->ap_template.AP_IP.s_addr = inet_addr(&row1[10][0]);
							(&legalAP_req[i])->ap_template.ap_gateway.gateway.s_addr = inet_addr(&row1[11][0]);
							(&legalAP_req[i])->ap_template.AP_Server_IP.s_addr = inet_addr(&row1[12][0]);
							(&legalAP_req[i])->ap_template.ap_gateway.subnetmask.s_addr = inet_addr(&row1[18][0]);
							
							for (index = 0; index < max_ap_server; index++)//找出APServer的序列号
							{

								if (inet_addr(&row1[12][0]) == (&apServer_socket_list[index])->addrAP.sin_addr.s_addr)
									break;
							}
							if (index < 256)
							{
								send_to_apServer(index, command, NULL, NULL, &legalAP_req[i], 1, NULL);//调用Socket，将数据发送到ap_server
								SumTimer[1].flag = 1;//开启AP定时器
								SumTimer[1].timer = 5;//时间3s
								update_legalAP_add(&legalAP_req[i]);//更新AP数组
							}
							else {
								printf("Add AP failed can't find legal APServer,Please check APServerIP\n");
							}
							printf("Add_AP_Server的IP为：%s\n", &row1[12][0]);
							i++;
							
						}
					}
					mysql_free_result(result1);
					break;
				case del://可以进行批量删除
					//strcpy(delapsn, &row[6][0]);
					pn = strtok(&row[6][0], ",");
					k = 0;
					int count = 0;
					while (pn)
					{
					
						strcpy(&input[k][0], pn);
						snprintf(sql_insert,128,"select * from  AP where serials_ID = '%s'",&input[k][0]);
						printf("Sql_insert:%s\n",sql_insert);
						ret = mysql_query(conn, sql_insert);
						result1 = mysql_store_result(&mysql);
						
						if ((row1 = mysql_fetch_row(result1)) == NULL)    //(result1 == NULL)
						{
						printf("DEL SUCCESSED\n");
						(&legalAP_req[count])->command = AP_DEL;
						strcpy((&legalAP_req[count])->AP_SN, &input[k][0]);
						printf("AP:DEL SUCCESS:%s\n",&input[k][0]);
						update_legalAP_del(legalAP_req[count].AP_SN);
						count++;
						}	
						pn = strtok(NULL, ",");
						k++;
						mysql_free_result(result1);
						
					}
					
				
					for (index = 0; index < max_ap_server; index++)//没有AP的APServerIP，只能将删除AP的消息进行广播发送
					{
						send_to_apServer(index, command, NULL, NULL, legalAP_req, count, NULL);
					}
					SumTimer[1].flag = 1;
					SumTimer[1].timer = 5;//开启AP定时器，等待3s
					printf("Del succeed!row[0][0]=%s  command %d\n", &row[0][0],command);
					snprintf(sql_insert, 512, "	update control set state = 2 where id = %d", atoi(&row[0][0]));
					printf("sql_insert is:%s\n", sql_insert);
					mysql_query(conn, sql_insert);
					
					break;
				}
				memset(res, 0, sizeof(res));
				
				break;

			case configuring:
				command = APSERVER_WEB_AP_CONFIGURE;
				add_quotae(&row[6][0], res);
				Send_to_AP_Configure_Sort(res, command, size);
				SumTimer[2].flag = 2;
				SumTimer[2].timer = 33;
				break;
			case update_status:
				switch (operation)
				{
				case add://软件升级

					//template_ID	SSID_name	SSID_psw	Login_name	Login_psw	NatOrBridge	pool_start	pool_end	DhcpOrStatic	ip_address	gateway	APServer_IP
					//pn = strtok(&row[6][0], ",");
					add_quotation(&row[6][0], res, tn, gn);     //tn:filepath gn:version
					command = APSERVER_WEB_SoftWare_Upgrade;
					AP_Upgrade_sort(res, command, size, username, pwd, tn, gn);
					printf("upgrade res is %s  filepath: %s  version: %s\n", res,tn,gn);
					SumTimer[3].flag = 3;
					SumTimer[3].timer = 30;
					break;
				case del://获取日志
					pn = strtok(&row[6][0], ",");
					//int size1 = size;
					command = APSERVER_WEB_Request_LogInfo;
					while (pn)
					{
						strcpy(&input[k][0], pn);
						pn = strtok(NULL, ",");
						k++;
					}

					for (j = 0; j < k; j++)
					{
						strcat(res, "'");
						strcat(res, &input[j][0]);
						strcat(res, "'");
						strcat(res, ",");
					}
					res[strlen(res) - 1] = '\0';
					printf("log res is %s \n", res);
					AP_Upgrade_sort(res, command, size, NULL, NULL, NULL, NULL);
					SumTimer[4].flag = 4;
					SumTimer[4].timer = 20;
					
					
					break;
				}

				break;
			default:
				printf("undefined command AP Server to AP\n");
			}
		}//加一个大的case代表升级未修改完。
	}

	if(obj == 2){
		flag_begin = 0;
		mysql_query(conn, "update control set state = 2 where id = ((select id from (select min(id) id from  control c1 where state = 0) t1))");
		}
	
	mysql_free_result(result);//释放内存
	ret = mysql_query(conn, "update control set state = 1 where id = ((select id from (select min(id) id from  control c1 where state = 0) t1))");
	//更新id最小且state=0的command
	if (ret != 0)
	{
		printf("control_update error: %s\n", mysql_error(&mysql));
	}

}
	return ret;
}

//调用Socket，将数据发送到ap_server
void send_to_apServer(int apServerIndex, int command, AP_Configuration_template* tempTemplate, template_operation_req* templateOperation, legalAP_operation_req* APOperation, int size, char* apConfiguredList)
{
	int i = 0;
	char sendbuf[sizeof(AP) * 256 + 2];    //申请一个发送数据缓存区   模板数据结构            配置模板数据结构
	char* p = sendbuf;
	*p = command;
	p++;
	*p = size;
	p++;
	switch (command)
	{
	case APSERVER_WEB_TEMPLATE_REPORT:     //10
		for (i = 0; i < size; i++)
		{
				memcpy(p, (char*)&tempTemplate[i], sizeof(AP_Configuration_template));
				p += sizeof(AP_Configuration_template);
		}
		printf("APSERVER_WEB_TEMPLATE_REPORT:size:%d\n",size);
		sendto(sockSer, sendbuf, size * sizeof(AP_Configuration_template) + 2, 0, (struct sockaddr*)&apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_AP_REPORT:
		for (i = 0; i < size; i++)
		{
		memcpy(p + sizeof(legalAP_operation_req) * i, APOperation + i, sizeof(legalAP_operation_req));
		printf("AP Reporter APSN:%s\n",(APOperation+i)->AP_SN);
		}
		sendbuf[1] = size;
		sendto(sockSer, sendbuf, size * sizeof(legalAP_operation_req) + 2, 0, (struct sockaddr*)&apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_TEMPLATE_MODIFY:
		for (i = 0; i < size; i++)
			memcpy(p + sizeof(template_operation_req) * i, templateOperation + i, sizeof(template_operation_req));
		sendto(sockSer, sendbuf, size * sizeof(template_operation_req) + 2, 0, (struct sockaddr*)&apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_AP_MODIFY:
		for (i = 0; i < size; i++)
			{memcpy(p + sizeof(legalAP_operation_req) * i, APOperation + i, sizeof(legalAP_operation_req));
			//printf("AP Modify APSN:%s\n",(APOperation+i)->AP_SN);
			}
		sendto(sockSer, sendbuf, size * sizeof(legalAP_operation_req) + 2, 0, (struct sockaddr*)&apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_AP_CONFIGURE:
		memcpy(p, tempTemplate, sizeof(AP_Configuration_template));
		p = p + sizeof(AP_Configuration_template);
		for (i = 0; i < size; i++) {
			memcpy(p + APSN_length * i, apConfiguredList + APSN_length * i, APSN_length);
		}
		sendto(sockSer, sendbuf, sizeof(AP_Configuration_template) + 2 + size * APSN_length, 0, (struct sockaddr*)&apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_HEART_BEAT:
		sendto(sockSer, sendbuf, 2, 0, (struct sockaddr*)&apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_SoftWare_Upgrade:
		for (i = 0; i < size; i++)
		{
			memcpy(p + sizeof(legalAP_operation_req) * i, APOperation + i, sizeof(legalAP_operation_req));
			printf("send to apserver apsn %s,filepath %s\n", (APOperation + i)->AP_SN, (APOperation + i)->ap_template.Remote_FilePath);
		}
		sendto(sockSer, sendbuf, size * sizeof(legalAP_operation_req) + 2, 0, (struct sockaddr*)&apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_Request_LogInfo:
		for (i = 0; i < size; i++)
			memcpy(p + sizeof(legalAP_operation_req) * i, APOperation + i, sizeof(legalAP_operation_req));
		sendto(sockSer, sendbuf, size * sizeof(legalAP_operation_req) + 2, 0, (struct sockaddr*)&apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	default:
		printf("undefined command AP Server to AP");
	}


}//这也要修改加case

void  dispatch_APServerMsg(int apServer_index, char* buf)//处理apserver 发送过来的信息，将结果写入数据库
{
	//char s[100] = "\0";
	char Scp_Exe[200] = "\0";
	//char temp[500] = { 0 };
	//char temp1[5000] = { 0 };
	//char *p1,*p2,*p3,*p4,*p5,*p6;
	char ip1[25] = "\0";char ip2[25] = "\0"; char ip3[25] = "\0"; char ip4[25] = "\0"; char ip5[25] = "\0"; char ip6[25] = "\0";
	int ret = -1;int i = 0;int k = 0;
	int status;
	//char apserver_index[5];
	//char ack[5];
	AP_Configuration_template template_operation[max_template];
	AP list[256];
	AP_Configuration_template apTemplate[max_template];
	//printf("buf[0]is:%d\n", buf[0]);
	
	switch (buf[0])
	{
	case APSERVER_WEB_TEMPLATE_REPORT:
		printf("Template Reporter\n");
		legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_CONNECTING;
		legalAPServerTimer[apServer_index].apServer_on_timer = web_apServer_configure_time;
		decode_template_report(buf, apTemplate);//解析apserver发送过来的模板到Local           apTemplate   
		for (i = 0; i < buf[1]; i++)	//write templdate to database
		{
			/*apTemplate[i].ap_ssid.ssid  apTemplate[i].ap_ssid.ssid_psw  apTemplate[i].ap_login.login  apTemplate[i].ap_login.login_psw  apTemplate[i].options
			apTemplate[i].DHCPOrStatic  apTemplate[i].NatOrBridge    apTemplate[i].pool.start  apTemplate[i].pool.end  apTemplate[i].ap_gateway.gateway变量*/
			strcpy(ip1, inet_ntoa(apTemplate[i].pool.start));
			strcpy(ip2, inet_ntoa(apTemplate[i].pool.end));
			strcpy(ip3, inet_ntoa(apTemplate[i].AP_IP));
			strcpy(ip4, inet_ntoa(apTemplate[i].ap_gateway.gateway));
			strcpy(ip5, inet_ntoa(apTemplate[i].AP_Server_IP));
			strcpy(ip6, inet_ntoa(apTemplate[i].ap_gateway.subnetmask));
			snprintf(sql_insert, 512, "INSERT INTO config (template_ID, SSID_name, SSID_psw, login_name, login_psw,\
			NATOrBride, pool_start, pool_end, dhcporstatic, ip_adress, gateway,APServer_IP,netmask) Values (%d,'%s','%s','%s','%s',%d,'%s','%s',%d,'%s','%s','%s','%s')", \
				apTemplate[i].templateIndex, apTemplate[i].ap_ssid.ssid, apTemplate[i].ap_ssid.ssid_psw, apTemplate[i].ap_login.login, apTemplate[i].ap_login.login_psw, \
				apTemplate[i].NatOrBridge, ip1, ip2, apTemplate[i].DHCPOrStatic, ip3, ip4, ip5, ip6);
			ret = mysql_query(conn, sql_insert);     //将数据写入数据库中
			if (ret == 0)
			{
				printf("template reporter insert success\n");
				update_legalTemplate_add_o(&apTemplate[i]);
			}
			memset(ip1, 0, sizeof(ip1)); memset(ip2, 0, sizeof(ip2)); memset(ip3, 0, sizeof(ip3)); memset(ip4, 0, sizeof(ip4)); memset(ip5, 0, sizeof(ip5)); memset(ip6, 0, sizeof(ip6));
		}



		mysql_query(conn, "select * from config");//WS的模板发送给AP_Server
		result = mysql_store_result(&mysql);
		if (result == NULL)
		{
			printf("mysql_store_result error: %s\n", mysql_error(&mysql));
		}
		else
		{	i = 0;
			while ((row = mysql_fetch_row(result)))
			{
				template_operation[i].templateIndex = atoi(&row[0][0]);
				strcpy((&template_operation[i])->ap_ssid.ssid, &row[1][0]);
				strcpy((&template_operation[i])->ap_ssid.ssid_psw, &row[2][0]);
				strcpy((&template_operation[i])->ap_login.login, &row[3][0]);
				strcpy((&template_operation[i])->ap_login.login_psw, &row[4][0]);
				(&template_operation[i])->NatOrBridge = atoi(&row[5][0]);
				(&template_operation[i])->pool.start.s_addr = inet_addr(&row[6][0]);
				(&template_operation[i])->pool.end.s_addr = inet_addr(&row[7][0]);
				(&template_operation[i])->DHCPOrStatic = atoi(&row[8][0]);
				(&template_operation[i])->AP_IP.s_addr = inet_addr(&row[9][0]);
				(&template_operation[i])->ap_gateway.gateway.s_addr = inet_addr(&row[10][0]);
				(&template_operation[i])->AP_Server_IP.s_addr = inet_addr(&row[11][0]);
				(&template_operation[i])->ap_gateway.subnetmask.s_addr = inet_addr(&row[12][0]);
				/*printf("templateID=%d's ssid=%s\n", template_req[i].ap_template.templateIndex, template_req[i].ap_template.ap_ssid.ssid);
				printf("templateID=%d's ssid_psw=%s\n", template_req[i].ap_template.templateIndex, template_req[i].ap_template.ap_ssid.ssid_psw);
				printf("templateID=%d's login=%s\n", template_req[i].ap_template.templateIndex, template_req[i].ap_template.ap_login.login);
				printf("templateID=%d's login_psw=%s\n", template_req[i].ap_template.templateIndex, template_req[i].ap_template.ap_login.login_psw);
				printf("templateID=%d's NatOrBridge=%d\n", template_req[i].ap_template.templateIndex, template_req[i].ap_template.NatOrBridge);
				printf("templateID=%d's pool.start=%s\n", template_req[i].ap_template.templateIndex, inet_ntoa(template_req[i].ap_template.pool.start));
				printf("templateID=%d's pool.end=%s\n", template_req[i].ap_template.templateIndex, inet_ntoa(template_req[i].ap_template.pool.end));
				printf("templateID=%d's DHCPOrStatic=%d\n", template_req[i].ap_template.templateIndex, template_req[i].ap_template.DHCPOrStatic);
				printf("templateID=%d's ipaddr=%s\n", template_req[i].ap_template.templateIndex, inet_ntoa(template_req[i].ap_template.AP_IP));
				printf("templateID=%d's netmask=%s\n", template_req[i].ap_template.templateIndex, inet_ntoa(template_req[i].ap_template.ap_gateway.subnetmask));
				printf("templateID=%d's gateway=%s\n", template_req[i].ap_template.templateIndex, inet_ntoa(template_req[i].ap_template.ap_gateway.gateway));
				printf("templateID=%d's NMSIP=%s\n", template_req[i].ap_template.templateIndex, inet_ntoa(template_req[i].ap_template.AP_Server_IP));*/
				i++;
			}
		}
		send_to_apServer(apServer_index, APSERVER_WEB_TEMPLATE_REPORT, template_operation, NULL, NULL, i, NULL);
		//legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_ON;
		//legalAPServerTimer[apServer_index].apServer_on_timer_flag = 0;
		mysql_free_result(result);
		break;//done

	case APSERVER_WEB_AP_REPORT:
		//printf("AP Reporter\n");
		legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_CONNECTING;
		legalAPServerTimer[apServer_index].apServer_on_timer = web_apServer_configure_time;
		decode_ap_report(buf, list);
		
		//write AP to database
		for (i = 0; i < buf[1]; i++)
		{
			strcpy(ip1, inet_ntoa(list[i].configure.pool.start));
			strcpy(ip2, inet_ntoa(list[i].configure.pool.end));
			strcpy(ip3, inet_ntoa(list[i].configure.AP_IP));
			strcpy(ip4, inet_ntoa(list[i].configure.ap_gateway.gateway));
			strcpy(ip5, inet_ntoa(list[i].configure.AP_Server_IP));
			strcpy(ip6, inet_ntoa(list[i].configure.ap_gateway.subnetmask));
			
			snprintf(sql_insert, 512, "INSERT INTO AP (serials_ID, template_ID, SSID_name, SSID_psw, login_name, login_psw,\
			NATOrBride, pool_start, pool_end, dhcporstatic, ip_adress, gateway, APServer_IP, state, user_number, mode,version, antenna_number,netmask) \
			Values ('%s',%d,'%s','%s','%s','%s',%d,'%s','%s',%d,'%s','%s','%s',%d,%d,%d,'%s',%d,'%s')", \
				list[i].AP_SN, list[i].configure.templateIndex, list[i].configure.ap_ssid.ssid, list[i].configure.ap_ssid.ssid_psw, list[i].configure.ap_login.login, \
				list[i].configure.ap_login.login_psw, list[i].configure.NatOrBridge, ip1, ip2, list[i].configure.DHCPOrStatic, ip3, ip4, ip5, \
				list[i].status.online_state, list[i].status.user_number, list[i].status.model, list[i].status.version, list[i].status.antenna_number, ip6);
			ret = mysql_query(conn, sql_insert);//将AP_Server的AP数据写入数据库中
			if (ret == 0)
			{
				printf("AP_reporte insert success\n");
				update_legalAP_add_o(&list[i]);//更新ap_list
				
			}
			memset(ip1, 0, sizeof(ip1)); memset(ip2, 0, sizeof(ip2)); memset(ip3, 0, sizeof(ip3)); memset(ip4, 0, sizeof(ip4)); memset(ip5, 0, sizeof(ip5)); memset(ip6, 0, sizeof(ip6));
		}
		
		send_to_apServer(apServer_index, APSERVER_WEB_HEART_BEAT, NULL, NULL, NULL, 0, NULL);
		legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_ON;
		legalAPServerTimer[apServer_index].apServer_on_timer_flag = 0;//idle
		legalAPServerTimer[apServer_index].heart_beat_timer = web_apServer_heart_time;//3s
		legalAPServerTimer[apServer_index].heart_beat_timeout_times = 0;

		AP_Roporter_to_APServer_sort();
		
		break;
	case APSERVER_WEB_TEMPLATE_MODIFY:
		decode_template_modify_report(buf,apServer_index);//解析apserver发送过来的更改模板确认信息，这个函数有打印信息
		//write result to database. update control set ack = %d where templateIndex = %d
		break;
	case APSERVER_WEB_AP_MODIFY:
		//legalAP_operation_ack* pp1=(legalAP_operation_ack*)&buf[2];
		//printf("buf[0] %d  buf[1] %d buf[2] %d buf APSN  %s ACK %c\n",buf[0],buf[1],buf[2],pp1->AP_SN,pp1->ack);
		decode_ap_modify_report(buf,apServer_index);
		//printf("AP_Ack[i].ack is :%c  APSN: %s APSERRINDEX:%d\n", legalAP_Ack[0].ack,legalAP_Ack[0].AP_SN,legalAP_Ack[0].apServerindex);
		//write result to database
		break;
	case APSERVER_WEB_AP_CONFIGURE:
		//printf("sendbuf[0]:%d  sendbuf[1]: %d  sendbuf[2]  %d \n",buf[0],buf[1],buf[2]);
		decode_ap_configure_report(buf,apServer_index);
		// write to logfile
		//write result to database
		mysql_query(conn, "select * from temporary");
		result1 = mysql_store_result(&mysql);
		if (result1 == NULL) {
			printf("mysql_store_result error: %s\n", mysql_error(&mysql));
			printf("sql_insert is:%s\n", sql_insert);
		}
		else
		{
			row1 = mysql_fetch_row(result1);
			strcpy((&apTemplate[0])->ap_ssid.ssid, &row1[0][0]);
			strcpy((&apTemplate[0])->ap_ssid.ssid_psw, &row1[1][0]);
			strcpy((&apTemplate[0])->ap_login.login, &row1[2][0]);
			strcpy((&apTemplate[0])->ap_login.login_psw, &row1[3][0]);
			(&apTemplate[0])->NatOrBridge = atoi(&row1[4][0]);
			(&apTemplate[0])->pool.start.s_addr = inet_addr(&row1[5][0]);
			(&apTemplate[0])->pool.end.s_addr = inet_addr(&row1[6][0]);
			(&apTemplate[0])->DHCPOrStatic = atoi(&row1[7][0]);
			(&apTemplate[0])->AP_IP.s_addr = inet_addr(&row1[8][0]);
			(&apTemplate[0])->ap_gateway.gateway.s_addr = inet_addr(&row1[9][0]);
			(&apTemplate[0])->AP_Server_IP.s_addr = inet_addr(&row1[10][0]);
			(&apTemplate[0])->ap_gateway.subnetmask.s_addr = inet_addr(&row1[11][0]);
			for (i = 0; i < Configure_Nums; i++)
			{
				if (AP_Configure_Ack[i].ack == '0')
				{
					snprintf(sql_insert, 512, "update AP set SSID_name='%s',SSID_psw='%s',login_name='%s',login_psw='%s', NATorBride=%d,pool_start='%s', pool_end='%s',\
				dhcporstatic=%d,ip_adress='%s',gateway='%s',APserver_IP='%s',netmask = '%s' where serials_ID = '%s' and  APServer_IP = '%s'", (&apTemplate[0])->ap_ssid.ssid, (&apTemplate[0])->ap_ssid.ssid_psw,
						(&apTemplate[0])->ap_login.login, (&apTemplate[0])->ap_login.login_psw, (&apTemplate[0])->NatOrBridge, &row1[5][0], &row1[6][0], (&apTemplate[0])->DHCPOrStatic,
						&row1[8][0], &row1[9][0], &row1[10][0], &row1[11][0], AP_Configure_Ack[i].AP_SN,inet_ntoa((&apServer_socket_list[apServer_index])->addrAP.sin_addr));

						
					printf("Update AP_SN %s\n",AP_Configure_Ack[i].AP_SN);
					ret = mysql_query(conn, sql_insert);//更新AP的配置
					if (ret == 0)
						printf("AP_temp_configure insert success\n");
				}
			}
		}
		for (i = 0; i < Configure_Nums; i++)
		{
			if (AP_Configure_Ack[i].ack == '0')
			{
				for (k = 0; k < max_ap; k++)
				{
					if (strcmp(AP_Configure_Ack[i].AP_SN, ap_list[k].AP_SN) == 0)
					{
						strcpy(ap_list[k].configure.ap_ssid.ssid, &row1[0][0]);
						strcpy(ap_list[k].configure.ap_ssid.ssid_psw, &row1[1][0]);
						strcpy(ap_list[k].configure.ap_login.login, &row1[2][0]);
						strcpy(ap_list[k].configure.ap_login.login_psw, &row1[3][0]);
						ap_list[k].configure.NatOrBridge = atoi(&row1[4][0]);
						ap_list[k].configure.pool.start.s_addr = inet_addr(&row1[5][0]);
						ap_list[k].configure.pool.end.s_addr = inet_addr(&row1[6][0]);
						ap_list[k].configure.DHCPOrStatic = atoi(&row1[7][0]);
						ap_list[k].configure.AP_IP.s_addr = inet_addr(&row1[8][0]);
						ap_list[k].configure.ap_gateway.gateway.s_addr = inet_addr(&row1[9][0]);
						ap_list[k].configure.AP_Server_IP.s_addr = inet_addr(&row1[10][0]);
						ap_list[k].configure.ap_gateway.subnetmask.s_addr = inet_addr(&row1[11][0]);
						//update_legalAP_del(ap_list[k].AP_SN);
						//update_legalAP_add_o(&ap_list[k]);//更新数组
					}
				}
			}
		}
		i = 0;
		mysql_free_result(result1);
		break;
	case APSERVER_WEB_HEART_BEAT:
		legalAPServerTimer[apServer_index].heart_beat_timer = web_apServer_heart_time;//每隔3s向APServer发送一个心跳
		legalAPServerTimer[apServer_index].heart_beat_timeout_times = 0;
		decode_ap_status_reporter(buf, list);
		
		//state	user_number	mode	 char version	antenna_number

		for (i = 0; i < buf[1]; i++)//更新AP表的status
		{
			snprintf(sql_insert, 512, "update AP set state = %d,user_number = %d,mode = %d,version = '%s',antenna_number = %d where serials_ID = '%s' and APServer_IP = '%s'",
				list[i].status.online_state, list[i].status.user_number, list[i].status.model, list[i].status.version, list[i].status.antenna_number, list[i].AP_SN,
				inet_ntoa((&apServer_socket_list[apServer_index])->addrAP.sin_addr));
			ret = mysql_query(conn, sql_insert);
			if (ret == 0)
			{
				//printf("update Ap：%s success!state=%d\n",list[i].AP_SN,list[i].status.online_state);
				update_legalAP_del(list[i].AP_SN);//更新AP数组的status
				update_legalAP_add_o(&list[i]);
			}

		}
		legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_ON;
		//legalAPServerTimer[apServer_index].apServer_on_timer_flag = 0; //idle状态
		break;
	case APSERVER_WEB_SoftWare_Upgrade:
		//legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_SoftWare_Upgrade;
		//legalAPServerTimer[apServer_index].apServer_on_timer = web_apServer_upgrade_time;
		//legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_ON;
		//legalAPServerTimer[apServer_index].apServer_on_timer = 15;//web_apServer_upgrade_time;
		//legalAPServerTimer[apServer_index].apServer_on_timer_flag = 4;
		decode_ap_upgrade_report(buf,apServer_index);
		//write result to database
		break;
	case APSERVER_WEB_Request_LogInfo://第二个操作SCp
		//printf("temp1 is:%s\n", temp1);
		//legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_Request_LogInfo;
		//legalAPServerTimer[apServer_index].apServer_on_timer = web_apServer_upgrade_time;

		
		decode_ap_log_report(buf,apServer_index);//解析apserver发送过来的模板
		

		/*apTemplate[i].ap_ssid.ssid  apTemplate[i].ap_ssid.ssid_psw  apTemplate[i].ap_login.login  apTemplate[i].ap_login.login_psw  apTemplate[i].options
apTemplate[i].DHCPOrStatic  apTemplate[i].NatOrBridge    apTemplate[i].pool.start  apTemplate[i].pool.end  apTemplate[i].ap_gateway.gateway变量*/

		
		for(;Temp_GetLog_Nums<GetLog_Nums;Temp_GetLog_Nums++)//根据数量分段，每个APServer只进行一次SCP命令，获取日志文件
		{
		if(legalAP_log_Ack[Temp_GetLog_Nums].ack == '0')
		{

		
		/*snprintf(Scp_Exe,180,"chmod 777 /var");
		status = system(Scp_Exe);
		if (-1 == status) {
			printf("scp error");
		}
		if (!WIFEXITED(status)) {
			printf("scp error");
		}
		if (WEXITSTATUS(status)) {
			printf("scp error");
		}
		else 
		{
		printf("scp success");
		}*/
		
		snprintf(Scp_Exe, 180, "sshpass -p \"%s\" scp -r -o StrictHostKeyChecking=no  xlh@%s:%s  %s", \
			legalAP_log_Ack[Temp_GetLog_Nums].Scp_PassWord, inet_ntoa((&apServer_socket_list[apServer_index])->addrAP.sin_addr),\
			legalAP_log_Ack[Temp_GetLog_Nums].Remote_FilePath,Local_Save_AP_LOG_PATH);
		printf("Scp_Exe:%s\n",Scp_Exe);
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
		else 
		{
		printf("scp success\n");
		}
		Temp_GetLog_Nums = GetLog_Nums;
		break;
		}
		
		}
		
		//legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_ON;
		//legalAPServerTimer[apServer_index].apServer_on_timer_flag = 0;
		break;
	default:
		log_error("Error: Unknown command from APServer = %d!\n", buf[0]);
		break;
	}

}

