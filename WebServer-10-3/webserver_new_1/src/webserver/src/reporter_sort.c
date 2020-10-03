/*
auth:	Baojinzhuang
date:	2020-01-07
history:
reporte_sort.c
*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<stdarg.h>
#include<ctype.h>

#include"function.h"

void AP_Roporter_to_APServer_sort() {//将WS存储的AP分类发送给AS
	legalAP_operation_req legalAP_req[256];
	AP ap_list[256];
	int i = 0;
	int j;
	int index;
	int count = 0;
	char sql_insert[1024];
	int ret = -1;
	int k;int ap_num;
	
	typedef struct APServer_IP_Node{
		char AP_ServerIP[30];
		struct APServer_IP_Node *next;
		}APServer_IP_Node;
	typedef struct APServer_IP_Node *LinkList;	
	LinkList L,L2,L3;
	L = (LinkList)malloc(sizeof(APServer_IP_Node));//头结点
	L->next = NULL;
	L2 = L;//指向头结点


	
	ret = mysql_query(conn, "select * from AP");
	if (ret != 0) {
		printf("mysql_select error: %s\n", mysql_error(&mysql));
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("mysql_store_result error: %s\n", mysql_error(&mysql));
	}
	while ((row = mysql_fetch_row(result))) {
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
		(&ap_list[i])->configure.ap_gateway.gateway.s_addr = inet_addr(
				&row[11][0]);
		(&ap_list[i])->configure.AP_Server_IP.s_addr = inet_addr(&row[12][0]);
		(&ap_list[i])->status.online_state = row[13][0];
		(&ap_list[i])->status.user_number = row[14][0];
		(&ap_list[i])->status.model = row[15][0];
		strcpy((&ap_list[i])->status.version, &row[16][0]);
		(&ap_list[i])->status.antenna_number = row[17][0];
		(&ap_list[i])->configure.ap_gateway.subnetmask.s_addr = inet_addr(
				&row[18][0]);
		//printf("Fill_ap_list AP_Template_index:%d\n", row[1][0]);
		i++;
	}
	ap_num = i;
	mysql_free_result(result);


	i = 0;
	snprintf(sql_insert, 512,"select APserver_IP,count(*) as sum from AP group by APserver_IP");
	mysql_query(conn, sql_insert);
	result1 = mysql_store_result(&mysql);
	if (result1 == NULL) {
		printf("mysql_store_result error: %s\n", mysql_error(&mysql));
	} else 
	{

		while ((row1 = mysql_fetch_row(result1))) {
			L3 = (LinkList)malloc(sizeof(APServer_IP_Node));
			strcpy(L3->AP_ServerIP,&row1[0][0]);
			L2->next = L3;
			L2 = L3;
			i++;//get apserver ip to APServer_IP
		}
		L2->next = NULL;
	
	}

	k = i;//APServer数量
	mysql_free_result(result1);
	
	L3 = L;
	L = L->next;//头结点无信息
	//printf("APServerIp num count : %d\n", k);
	
	for (j = 0; j < k; j++) {//选定AP发送给APServer
		count = 0;
		for (i = 0; i < ap_num; i++) 
		{
			//if(memcmp(temp_aplist[i].configure.AP_Server_IP.s_addr,inet_addr(&APServer_IP[j][0]) , sizeof(struct in_addr))==0)
			if (ap_list[i].configure.AP_Server_IP.s_addr == inet_addr(L->AP_ServerIP))
				{
					//== inet_addr(&APServer_IP[j][0])) {
				(&legalAP_req[count])->command = AP_ADD;
				strcpy((&legalAP_req[count])->AP_SN, ap_list[i].AP_SN);

				(&legalAP_req[count])->ap_template.templateIndex =
						ap_list[i].configure.templateIndex;
				strcpy((&legalAP_req[count])->ap_template.ap_ssid.ssid,
						ap_list[i].configure.ap_ssid.ssid);
				strcpy((&legalAP_req[count])->ap_template.ap_ssid.ssid_psw,
						ap_list[i].configure.ap_ssid.ssid_psw);
				strcpy((&legalAP_req[count])->ap_template.ap_login.login,
						ap_list[i].configure.ap_login.login);
				strcpy((&legalAP_req[count])->ap_template.ap_login.login_psw,
						ap_list[i].configure.ap_login.login_psw);
				(&legalAP_req[count])->ap_template.NatOrBridge =
						ap_list[i].configure.NatOrBridge;

				(&legalAP_req[count])->ap_template.pool.start.s_addr =
						ap_list[i].configure.pool.start.s_addr;
				(&legalAP_req[count])->ap_template.pool.end.s_addr =
						ap_list[i].configure.pool.end.s_addr;
				(&legalAP_req[count])->ap_template.DHCPOrStatic =
						ap_list[i].configure.DHCPOrStatic;
				(&legalAP_req[count])->ap_template.AP_IP.s_addr =
						ap_list[i].configure.AP_IP.s_addr;
				(&legalAP_req[count])->ap_template.ap_gateway.gateway.s_addr =
						ap_list[i].configure.ap_gateway.gateway.s_addr;
				(&legalAP_req[count])->ap_template.AP_Server_IP.s_addr =
						ap_list[i].configure.AP_Server_IP.s_addr;
				(&legalAP_req[count])->ap_template.ap_gateway.subnetmask.s_addr =
						ap_list[i].configure.ap_gateway.subnetmask.s_addr;
				//printf("已将AP中的数据发送给AP_Server,AP_serial_ID为:%s\n", &row[0][0]);
				//printf("AP Reporter to APServer APSn : %s %s \n",ap_list[i].AP_SN, (&legalAP_req[count])->AP_SN);
				
				count++;
			}
			
		}
		for (index = 0; index < max_ap_server; index++)	//找出AP所属AP_Server的序列号
		{
			if (inet_addr(L->AP_ServerIP)
					== (&apServer_socket_list[index])->addrAP.sin_addr.s_addr)
				break;
		}

		if (index < 256) {
			send_to_apServer(index, APSERVER_WEB_AP_REPORT, NULL, NULL,
					legalAP_req, count, NULL);
			printf("count nums is %d\n",count);

		} else {
			printf(
					"AP Reporter to APServer error:未找到AP_Server的序列号,AP_Server的IP为：%s\n",L->AP_ServerIP);
		}
		L= L->next;
	}
	
	LinkList freeNode;
    while (NULL != L3){
        freeNode = L3;
        L3 = L3 -> next;
        free(freeNode);
		//printf("free success\n");
    }
	
}

void AP_Upgrade_sort(char *APset, int command, int size, char *username, char *pwd,
		char *filepath, char *version) {
	legalAP_operation_req legalAP_req1[256];
	char APServer_IP[256][20];
	int i = 0;
	int j;
	int index;
	int count = 0;
	int size1 = size;
	char sql_insert[1024];
	snprintf(sql_insert, 512, "select * from AP where  serials_ID in (%s) and state = 1",
			APset);
	mysql_query(conn, sql_insert);
	result1 = mysql_store_result(&mysql);
	if (result1 == NULL) {
		printf("mysql_store_result error: %s\n", mysql_error(&mysql));
	} else {
		row1 = mysql_fetch_row(result1);
		while (size) {
			strcpy(temp_aplist[i].AP_SN, &row1[0][0]);
			printf("temp_aplist[i].AP_SN: %d %s %s\n", i, temp_aplist[i].AP_SN,
					&row1[12][0]);
			temp_aplist[i].configure.AP_Server_IP.s_addr = inet_addr(
					&row1[12][0]);
			row1 = mysql_fetch_row(result1);
			//赋值给aplist
			size--;
			i++;
		}
	}
	mysql_free_result(result1);

	i = 0;
	snprintf(sql_insert, 512,
			"select APserver_IP,count(*) as sum from AP where serials_ID in (%s) and state = 1 group by APserver_IP",
			APset);
	
	//printf("%s\n", sql_insert);
	mysql_query(conn, sql_insert);
	result1 = mysql_store_result(&mysql);
	if (result1 == NULL) {
		printf("mysql_store_result error: %s\n", mysql_error(&mysql));
	} else {

		while ((row1 = mysql_fetch_row(result1))) {
			strcpy(&APServer_IP[i][0], &row1[0][0]);
			//APServerIp_nu == inet_addr(&row1[12][0]);
			//
			printf("i:%d, APServer Ip %s; \n", i, &APServer_IP[i][0]);
			i++;			//get apserver ip to APServer_IP
		}
	}
	mysql_free_result(result1);

	
	int k;
	k = i;
	printf("发送给APServer的数量: %d\n", k);
	for (j = 0; j < k; j++) {
		printf("**********************************************************\n");
		count = 0;
		for (i = 0; i < size1; i++) {
			//if(memcmp(temp_aplist[i].configure.AP_Server_IP.s_addr,inet_addr(&APServer_IP[j][0]) , sizeof(struct in_addr))==0)
			if (temp_aplist[i].configure.AP_Server_IP.s_addr
					== inet_addr(&APServer_IP[j][0])) {
				(&legalAP_req1[count])->command = command;
				strcpy((&legalAP_req1[count])->AP_SN, temp_aplist[i].AP_SN);
				(&legalAP_req1[count])->ap_template.AP_Server_IP.s_addr =
						temp_aplist[i].configure.AP_Server_IP.s_addr;

						
				if (username != NULL) {
					strcpy((&legalAP_req1[count])->ap_template.Scp_Username,
							username);
					strcpy((&legalAP_req1[count])->ap_template.Scp_PassWord,
							pwd);
					strcpy((&legalAP_req1[count])->ap_template.Remote_FilePath,
							filepath);
					strcpy((&legalAP_req1[count])->ap_template.version,
							version);
					printf("软件升级 APSn : %s %s filepath: %s,versiom:%s\n",
							temp_aplist[i].AP_SN, (&legalAP_req1[count])->AP_SN,
							(&legalAP_req1[count])->ap_template.Remote_FilePath,
							(&legalAP_req1[count])->ap_template.version);
				}
				
				count++;
			}
		}
		/*printf(
				"这句不全就是日志请求upgrade APSn : %s   %s filepath: %s,versiom:%s j:%d, APServerIp:%s\n",
				temp_aplist[0].AP_SN, (&legalAP_req1[0])->AP_SN,
				(&legalAP_req1[0])->ap_template.Remote_FilePath,
				(&legalAP_req1[0])->ap_template.version, j, &APServer_IP[j][0]);*/
		
		for (index = 0; index < max_ap_server; index++)	//找出AP所属AP_Server的序列号
				{
			if (inet_addr(&APServer_IP[j][0])
					== (&apServer_socket_list[index])->addrAP.sin_addr.s_addr)
				break;
		}
		if (index < 256) {
			send_to_apServer(index, command, NULL, NULL, legalAP_req1, count,
					NULL);			//调用Socket，将数据发送到ap_server
			printf("send soft upgrade or log size is : %d\n", count);
			snprintf(sql_insert, 100, "%d", index);
			strcat(sendapserverid, sql_insert);
			strcat(sendapserverid, ",");

		} else {
			printf("send upgrade error:未找到AP_Server的序列号,AP_Server的IP为：%s\n",
					&APServer_IP[j][0]);
		}
	}

}

void Send_to_AP_Configure_Sort(char *APset, int command, int size) {//完成分类发送配置
	
	AP_Configuration_template ap_template[10];
	char temp_APSN[25][20];
	char APServer_IP[25][20];
	int i = 0;
	int j;
	int index;
	int count = 0;
	int size1 = size;
	char sql_insert[1024];
	
	snprintf(sql_insert, 512, "select * from AP where  serials_ID in (%s) and state = 1",//从数据库获取需要配置的AP
			APset);
	mysql_query(conn, sql_insert);
	result1 = mysql_store_result(&mysql);
	if (result1 == NULL) {
		printf("mysql_store_result error: %s\n", mysql_error(&mysql));
	} else {
		row1 = mysql_fetch_row(result1);
		while (size) {
			strcpy(temp_aplist[i].AP_SN, &row1[0][0]);
			printf("temp_aplist[i].AP_SN: %d %s %s\n", i, temp_aplist[i].AP_SN,
					&row1[12][0]);
			temp_aplist[i].configure.AP_Server_IP.s_addr = inet_addr(
					&row1[12][0]);
			row1 = mysql_fetch_row(result1);
			//赋值给aplist
			size--;
			i++;
		}
	}
	mysql_free_result(result1);

	i = 0;
	mysql_query(conn, "select * from temporary");//获取配置的数据
	result1 = mysql_store_result(&mysql);
	if (result1 == NULL) {
		printf("mysql_configure_store_result error: %s\n", mysql_error(&mysql));
		printf("sql_insert is:%s\n", sql_insert);
	} else {
		row1 = mysql_fetch_row(result1);
		//template_ID	SSID_name	SSID_psw	Login_name	Login_psw	NatOrBridge	pool_start	pool_end	DhcpOrStatic	ip_address	gateway	APServer_IP
		strcpy((&ap_template[i])->ap_ssid.ssid, &row1[0][0]);
		strcpy((&ap_template[i])->ap_ssid.ssid_psw, &row1[1][0]);
		strcpy((&ap_template[i])->ap_login.login, &row1[2][0]);
		strcpy((&ap_template[i])->ap_login.login_psw, &row1[3][0]);
		(&ap_template[i])->NatOrBridge = atoi(&row1[4][0]);
		(&ap_template[i])->pool.start.s_addr = inet_addr(&row1[5][0]);
		(&ap_template[i])->pool.end.s_addr = inet_addr(&row1[6][0]);
		(&ap_template[i])->DHCPOrStatic = atoi(&row1[7][0]);
		(&ap_template[i])->AP_IP.s_addr = inet_addr(&row1[8][0]);
		(&ap_template[i])->ap_gateway.gateway.s_addr = inet_addr(&row1[9][0]);
		(&ap_template[i])->AP_Server_IP.s_addr = inet_addr(&row1[10][0]);
		(&ap_template[i])->ap_gateway.subnetmask.s_addr = inet_addr(
				&row1[11][0]);
	}
	mysql_free_result(result1);			//get template  then free

	i = 0;
	snprintf(sql_insert, 512,
			"select APserver_IP,count(*) as sum from AP where  serials_ID in (%s) and state = 1 group by APserver_IP",
			APset);//获取需要发送给的APServerIP
	mysql_query(conn, sql_insert);
	result1 = mysql_store_result(&mysql);
	if (result1 == NULL) {
		printf("mysql_store_result error: %s\n", mysql_error(&mysql));
		printf("%s\n", sql_insert);
	} 
	else {
		while ((row1 = mysql_fetch_row(result1))) {
			strcpy(&APServer_IP[i][0], &row1[0][0]);
			i++;			
		}
	}
	mysql_free_result(result1);

	int k;//AS的数量
	k = i;
	for (j = 0; j < k; j++) {
		count = 0;
		for (i = 0; i < size1; i++) 
		{
			if (temp_aplist[i].configure.AP_Server_IP.s_addr
					== inet_addr(&APServer_IP[j][0])) {
				strcpy(&temp_APSN[count][0], temp_aplist[i].AP_SN);
				//printf("Configure APSn : %s\n", &temp_APSN[count][0]);
				count++;
			}
		}

		//printf("**********************************************************\n");
		for (index = 0; index < max_ap_server; index++)	//找出AP所属AP_Server的序列号
				{
			if (inet_addr(&APServer_IP[j][0])
					== (&apServer_socket_list[index])->addrAP.sin_addr.s_addr)
				break;
		}
		if (index < 256) {
			//send_to_apServer(index, command, &ap_template[i], NULL, NULL, 1, &input[i][0]);
			send_to_apServer(index, command, ap_template, NULL, NULL, count,
					&temp_APSN[0][0]);			//调用Socket，将数据发送到ap_server
			printf("send CONFIGURE size is : %d,APSrver index:%d\n", count,
					index);
			snprintf(sql_insert, 100, "%d", index);
			strcat(sendapserverid, sql_insert);
			strcat(sendapserverid, ",");

		} else {
			printf("send upgrade error:未找到AP_Server的序列号,AP_Server的IP为：%s\n",
					&APServer_IP[j][0]);
		}
	}

}

void add_quotation(char *s, char *APset, char *filepath, char *version) //s:前端的content内容  ，返回的结果  5456465,545,6,88  ——>'5456465','545','6','88'
{

	int i = 0, j = 0;
	char input[20][60];
	char *pn;
	pn = strtok(s, ",");
	while (pn) {
		strcpy(&input[i][0], pn);
		pn = strtok(NULL, ",");
		i++;
	}
	for (j = 0; j < i - 2; j++) {
		strcat(APset, "'");
		strcat(APset, &input[j][0]);
		strcat(APset, "'");
		strcat(APset, ",");
	}
	strcpy(version, &input[j][0]);
	j++;
	strcpy(filepath, &input[j][0]);
	APset[strlen(APset) - 1] = '\0';
}

void add_quotae(char *s, char *APset) //s:前端的content内容  ，返回的结果  5456465,545,6,88  ——>'5456465','545','6','88'
{

	int i = 0, j = 0;
	char input[20][60];
	char *pn;
	pn = strtok(s, ",");
	while (pn) {
		strcpy(&input[i][0], pn);
		pn = strtok(NULL, ",");
		i++;
	}
	for (j = 0; j < i; j++) {
		strcat(APset, "'");
		strcat(APset, &input[j][0]);
		strcat(APset, "'");
		strcat(APset, ",");
	}

	APset[strlen(APset) - 1] = '\0';
}
