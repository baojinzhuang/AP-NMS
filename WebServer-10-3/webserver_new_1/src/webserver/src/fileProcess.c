/*
auth:	Baojinzhuang
date:	2020-01-07
history:
fileProcess.c
*/

#include<stdio.h>
#include<stdlib.h> 
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<stdarg.h>
#include<ctype.h>

#include"function.h"

char template_set[50]="../templates/template_set.txt";
char ap_set[50]="../aps/ap_set.txt";
char log_file[50]="../logs/logs.txt";
extern AP_Configuration_template apTemplate[max_template];
extern AP ap_list[max_ap];
extern int legalTemplate[max_template];
extern int legalAP[max_ap];
extern int g_level;
extern char Webserver_allaps[][50];
extern char Webserver_alltemplates[][50];
extern int Webserver_apMaptable[];
extern int Webserver_templateMaptable[];
extern char Web_Server_IP[50];
extern char cleardate[50];


void write_log_file(char* logs);
int assign_apIndex();


int print_log(unsigned int level, const char* filename, unsigned int line, char* format, ...)
{
    va_list va;
    char info[1024 + 1] = {0};
	char data[1024 + 1] = {0};
        
    if(format == NULL)
    {
        return -1;
    }
    
	//judge level
	
    if(g_level >= level)
    {
        return 1;
    }
	

    va_start(va, format);
    vsnprintf(info, 1024, format, va);
    va_end(va);

    snprintf(data,1024,"%s[%u] : %s", filename, line, info);
	write_log_file(data);

	return 0;

}//打印日志


void write_getlog_log(){
	int i;
	int ret;
	printf("Get_Log_Nums %d\n",GetLog_Nums);
	log_info("\n");
	log_info("******************Get_Log_Result****************\n");
	log_info("    APServerIndex          AP_SN           Result	\n");
	
	for(i = 0;i<GetLog_Nums;i++){

	if(legalAP_log_Ack[i].ack=='0')
	{
	log_info("---------------------------------------\n");
	log_info("%10d  %20s          成功\n",legalAP_log_Ack[i].apserver_index,legalAP_log_Ack[i].AP_SN);
		}
	//if(legalAP_log_Ack[i].ack == '1')
	else{
	log_info("---------------------------------------\n");
	log_info("%10d  %20s          失败\n",legalAP_log_Ack[i].apserver_index,legalAP_log_Ack[i].AP_SN);
		}
	}
	log_info("未显示的APServerID均为未回应\n");


	if(GetLog_Nums==0){
	snprintf(sql_insert, 512, "update control set state = 3 where object = 4 \
		and id = (select ID from (select min(id) as ID from control c1 where state = 1) t1)");
		printf("Sql:%s\n",sql_insert);
	ret = mysql_query(conn, sql_insert);//将数据写入数据库中
	GetLog_Nums = 0;
	Temp_GetLog_Nums = 0;
	SumTimer[4].flag = -1;//关闭定时器
	memset(legalAP_log_Ack,-1,sizeof(legalAP_log_Ack));
	printf("Get AP Get_Log ACK message，详情请查看日志\n");
	return;
	}

	
	i = 0;
	while (legalAP_log_Ack[i].ack == '0')
	{
		i++;
	}
	if (i == GetLog_Nums)												//((select id from (select min(id) id from  control c1 where state = 1) t1))
	{
		snprintf(sql_insert, 512, "update control set state = 2 where content like '%%%s%%' and object = 4 \
			and id = (select ID from (select min(id) as ID from control c1 where state = 1) t1)", legalAP_log_Ack[0].AP_SN);
			printf("Sql:%s\n",sql_insert);
		ret = mysql_query(conn, sql_insert);//将数据写入数据库中
		if (ret != 0)
		{
			printf("update log control mysql_query error is: %s\n", mysql_error(&mysql));
		}
	}
	else
	{

	snprintf(sql_insert, 512, "update control set state = 5 where content like '%%%s%%' and object = 4 and state = 1  \
		and id = (select ID from (select min(id) as ID from control c1 where state = 1) t1)",legalAP_log_Ack[0].AP_SN);

		printf("Sql:%s\n",sql_insert);
	ret = mysql_query(conn, sql_insert);
	if (ret != 0)
	{
		printf("mysql_query error is_115:%s\n", mysql_error(&mysql));
	}
	}

	GetLog_Nums = 0;
	Temp_GetLog_Nums = 0;
	SumTimer[4].flag = -1;//关闭定时器
	memset(legalAP_log_Ack,-1,sizeof(legalAP_log_Ack));
	printf("Get AP Get_Log ACK message，详情请查看日志\n");

}


void write_software_log(){
	int i;
	int ret;
	printf("SoftWare_Nums %d\n",SoftWare_Nums);
	log_info("\n");
	log_info("******************SoftWare_Upgrade_Result****************\n");
	log_info("    APServerIndex          AP_SN           Result	\n");
	
	for(i = 0;i<SoftWare_Nums;i++){
	if(legalAP_upgrade_Ack[i].ack=='0')
	{
	log_info("---------------------------------------\n");
	log_info("%10d  %20s          成功\n",legalAP_upgrade_Ack[i].apServer_index,legalAP_upgrade_Ack[i].AP_SN);
		}
	if(legalAP_upgrade_Ack[i].ack == '1')
		{
	log_info("---------------------------------------\n");
	log_info("%10d  %20s          失败\n",legalAP_upgrade_Ack[i].apServer_index,legalAP_upgrade_Ack[i].AP_SN);
		}
	}
	log_info("未显示的APServerID均为未回应\n");

	if(SoftWare_Nums==0){
	snprintf(sql_insert, 512, "update control set state = 3 where  object = 4 and operation = 0 \
		and id = (select ID from (select min(id) as ID from control c1 where state = 1) t1)");
	ret = mysql_query(conn, sql_insert);//将数据写入数据库中
	}

	i = 0;
	while (legalAP_upgrade_Ack[i].ack == '0')
	{
		i++;
	}
	if (i == SoftWare_Nums)
	{
		//printf("test upgrade!");
		snprintf(sql_insert, 512, "update control set state = 2 where content like '%%%s%%' and object = 4 \
		and id = (select ID from (select min(id) as ID from control c1 where state = 1) t1)", legalAP_upgrade_Ack[0].AP_SN);
		ret = mysql_query(conn, sql_insert);//将数据写入数据库中
		if (ret != 0)
		{
			printf("mysql_query error is: %s\n", mysql_error(&mysql));
		}
	}
	else
	{

	snprintf(sql_insert, 512, "update control set state = 5 where content like '%%%s%%' and object = 4 \
		and id = (select ID from (select min(id) as ID from control c1 where state = 1) t1)",legalAP_upgrade_Ack[0].AP_SN);
	//printf("some upgrade success \n ");
	//printf("this is upgrade info: %s", sql_insert);
	printf("Sql:%s\n",sql_insert);
	ret = mysql_query(conn, sql_insert);
	if (ret != 0)
	{
		printf("mysql_query error is_182:%s\n", mysql_error(&mysql));
	}
	}

	
	SoftWare_Nums = 0;
	SumTimer[3].flag = -1;//关闭定时器
	memset(legalAP_upgrade_Ack,-1,sizeof(legalAP_upgrade_Ack));
	printf("Get AP Upgrade ACK message，详情请查看日志\n");
	}


void write_configure_log()
{
	int i;
	int ret;
	printf("Configure_Nums %d\n",Configure_Nums);
	log_info("\n");
	log_info("******************Configure_Result****************\n");
	log_info("    APServerIndex          AP_SN           Result	\n");
	for(i=0;i<Configure_Nums;i++)
	{
	
	if(AP_Configure_Ack[i].ack == '0'){
	log_info("---------------------------------------\n");
	log_info("%10d  %20s          成功\n",AP_Configure_Ack[i].apServer_index,AP_Configure_Ack[i].AP_SN);
		}
	if(AP_Configure_Ack[i].ack == '1')
		{
	log_info("---------------------------------------\n");
	log_info("%10d  %20s          失败\n",AP_Configure_Ack[i].apServer_index,AP_Configure_Ack[i].AP_SN);
		}
	}
	log_info("未显示的APServerID均为未回应\n");

	
	if(Configure_Nums==0){
	snprintf(sql_insert, 512, "update control set state = 3 where object = 3 \
		and id = (select ID from (select min(id) as ID from control c1 where state = 1) t1)");
		printf("Sql:%s\n",sql_insert);
	ret = mysql_query(conn, sql_insert);//将数据写入数据库中
	Configure_Nums = 0;
	SumTimer[2].flag = -1;//关闭定时器
	memset(AP_Configure_Ack,-1,sizeof(AP_Configure_Ack));
	printf("Get AP Configure ACK message，详情请查看日志\n");
	return;
	}

	i= 0;
	while (AP_Configure_Ack[i].ack == '0')//更新Control表
	{
		i++;
	}
	if (i == Configure_Nums)
	{
	snprintf(sql_insert, 512, "update control set state = 2 where content like '%%%s%%' and object = 3 and state = 1\
		and id = (select ID from (select MAX(id) as ID from control) t1)", AP_Configure_Ack[0].AP_SN);
		ret = mysql_query(conn, sql_insert);//将确认信息写入control表
	if (ret != 0)
	{
	printf("mysql_query error is: %s\n", mysql_error(&mysql));
	}
	}
	
	else
	{
	snprintf(sql_insert, 512, "update control set state = 5 where content like '%%%s%%' and object = 3 and state = 1\
		and id = (select ID from (select MAX(id) as ID from control) t1)",AP_Configure_Ack[0].AP_SN);
	ret = mysql_query(conn, sql_insert);
	
	if (ret != 0)
	{
	printf("mysql_update_result error is:%s\n", mysql_error(&mysql));}
	
	}
	Configure_Nums = 0;
	SumTimer[2].flag = -1;//关闭定时器
	memset(AP_Configure_Ack,-1,sizeof(AP_Configure_Ack));
	printf("Get AP Configure ACK message，详情请查看日志\n");

}
void write_template_log()
{
	int i;
	int ret;
	printf("File Template_Ack[0].ack is :%c num_i:%d\n", Template_Ack[0].ack,Num_i1);
	log_info("\n");
	log_info("******************Modify_Template_Result****************\n");
	log_info("APServerIndex    TemplateIndex     Result	\n");
	for(i=0;i<Num_i1;i++)
	{
	if(Template_Ack[i].ack == '0'){
	log_info("---------------------------------------\n");
	log_info("%d              %2d              成功\n",Template_Ack[i].apServer_index,Template_Ack[i].templateIndex);
		}
	 if(Template_Ack[i].ack == '1')
		{
	log_info("---------------------------------------\n");
	log_info("%d              %2d               失败\n",Template_Ack[i].apServer_index,Template_Ack[i].templateIndex);
		}
	}
	log_info("未显示的APServerID均未回应\n");


	if(Num_i1==0){
	snprintf(sql_insert, 512, "update control set state = 3 where object = 0 \
		and id = (select ID from (select min(id) as ID from control c1 where state = 1) t1)");
		printf("Sql:%s\n",sql_insert);
	ret = mysql_query(conn, sql_insert);//将数据写入数据库中
	Num_i1 = 0;
	SumTimer[0].flag = -1;//关闭定时器
	memset(Template_Ack,-1,sizeof(Template_Ack));
	printf("Get Template modify ACK message，详情请查看日志\n");
	return;
	}

	
	i = 0;
	while (Template_Ack[i].ack == '0')
	{
		i++;
	}
	if (i == Num_i1)//将数据写入数据库中
	{
		snprintf(sql_insert, 512, "update control set state = 2 where content = '%d'and object = 0 and state = 1\
			and id = (select ID from (select MAX(id) as ID from control) t1)", Template_Ack[0].templateIndex);//更新control表数据
		//printf("sql:%s",sql_insert);
		ret = mysql_query(conn, sql_insert);
		if (ret != 0) {
			printf("mysql_query error is: %s\n", mysql_error(&mysql));
		}
	}
	else
	{
		snprintf(sql_insert, 512, "update control set state = 5 where content like '%%%d%%' and object = 0 and state = 1\
			and id = (select ID from (select MAX(id) as ID from control) t1)",Template_Ack[0].templateIndex);
		ret = mysql_query(conn, sql_insert);
		if (ret != 0)
		{
			printf("mysql_query error is:%s\n", mysql_error(&mysql));
		}
	}
	Num_i1 = 0;
	SumTimer[0].flag = -1;//关闭定时器
	memset(Template_Ack,-1,sizeof(Template_Ack));
	printf("Get Template modify ACK message，详情请查看日志\n");
}

void write_ap_log(){
	int i;
	int ret;
	printf("File legalAP_Ack[0].ack is :%c  legalAP_Ack[1].ack %c  num_i:%d\n", legalAP_Ack[0].ack,legalAP_Ack[1].ack,Num_i2);
	log_info("\n");
	log_info("******************Modify_AP_Result****************\n");
	log_info("    APServerIndex          AP_SN           Result	\n");
	for(i=0;i<Num_i2;i++)
	{
	if(legalAP_Ack[i].ack == '0'){
	log_info("---------------------------------------\n");
	log_info("%10d  %20s          成功\n",legalAP_Ack[i].apServerindex,legalAP_Ack[i].AP_SN);
		}
	if(legalAP_Ack[i].ack == '1')
		{
		log_info("---------------------------------------\n");
	log_info("%10d  %20s          失败\n",legalAP_Ack[i].apServerindex,legalAP_Ack[i].AP_SN);
		}
	}
	log_info("未显示的APServerID均为未回应\n");

	if(Num_i2==0){
	snprintf(sql_insert, 512, "update control set state = 3 where object = 1 \
		and id = (select ID from (select min(id) as ID from control c1 where state = 1) t1)");
		printf("Sql:%s\n",sql_insert);
	ret = mysql_query(conn, sql_insert);//将数据写入数据库中
	Num_i2 = 0;
	SumTimer[1].flag = -1;//关闭定时器
	memset(legalAP_Ack,-1,sizeof(legalAP_Ack));
	printf("Get AP modify ACK message，详情请查看日志\n");
	return;
	}	


	i = 0;
	while (legalAP_Ack[i].ack == '0')
	{
		i++;
	}
	if (i == Num_i2)
	{

		snprintf(sql_insert, 512, "update control set state = 2 where content like '%%%s%%' and object = 1 and state = 1  and id = (select ID from (select MAX(id) as ID from control) t1)", legalAP_Ack[0].AP_SN);
		ret = mysql_query(conn, sql_insert);//将ACK写入数据库中
		if (ret != 0)
		{
			printf("mysql_query error is: %s\n", mysql_error(&mysql));
			//mysql_connect();
		}
	}
	else
	{
		snprintf(sql_insert, 512, "update control set state = 5 where content like '%%%s%%' and object = 1 and state = 1 and id = (select ID from (select MAX(id) as ID from control) t1)",legalAP_Ack[0].AP_SN);
		//printf("sql_insert:%s\n",sql_insert);
		ret = mysql_query(conn, sql_insert);
		if (ret != 0)
		{
			printf("mysql_query error :%s\n", mysql_error(&mysql));
			//mysql_connect();
		}
	}



	
	Num_i2 = 0;
	SumTimer[1].flag = -1;//关闭定时器
	memset(legalAP_Ack,-1,sizeof(legalAP_operation_ack));
	printf("Get AP modify ACK message，详情请查看日志\n");
}

void write_log_file(char* logs)
{
	FILE* file = NULL;
	char line[1024];

	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);
	//printf("%d\n",p->tm_sec); /*获取当前秒*/
	//printf("%d\n",p->tm_min); /*获取当前分*/
	//printf("%d\n",8+p->tm_hour);/*获取当前时,这里获取西方的时间,刚好相差八个小时*/
	//printf("%d\n",p->tm_mday);/*获取当前月份日数,范围是1-31*/%d月%d日%时%分%秒
	//printf("%d\n",1+p->tm_mon);/*获取当前月份,范围是0-11,所以要加1*/
	//printf("%d\n",1900+p->tm_year);/*获取当前年份,从1900开始，所以要加1900*/
	
	if((file = fopen(log_file, "a")) == NULL)
	{
		log_error("Error: failed to open log file: %s\n", log_file);
		return ;
	}
	
	snprintf(line, 512, "[%d年%d月%d日%d时%d分%d秒]:%s\n",1900+p->tm_year,1+p->tm_mon,8+p->tm_hour,8+p->tm_hour,p->tm_min,p->tm_sec,logs);
	fputs(line, file);
	
	fclose(file);

}//将日志写入文件


int isBlankLine (char *line)
{
	int i;
	for( i=0; line[i] && line[i]!='\r' && line[i]!='\n'; i++)
	{
		if(line[i] != ' ' && line[i] != '\t')
		{
			return FALSE;
		}
	}
	return TRUE;
}
void stringTrimRight(char *  strToTrim)
{
    register char * strCursor = NULL;   /* string cursor */

    strCursor = strToTrim + strlen(strToTrim) - 1;

    while (strCursor > strToTrim)
    {
    if (isspace ((int)(*strCursor)))
        strCursor--;
    else
        break;
    }

    if (strCursor == strToTrim)
    {

    if (isspace ((int)(*strCursor)))   /* whole string is white space */
        {
        *strCursor = EOS;
        return;
        }
    }

    /* Normal return, non-empty string */

    *(strCursor+1) = EOS;
    return;
}




int tccDealConf(char *  strToCmp)
{
    int i=0,position;

    while(1)
    {
    	if(!strcmp(Webserver_alltemplates[i],"\0"))
			return -1;
		else if(!strcmp(Webserver_alltemplates[i],strToCmp))
			return i;
		else
			i++;
    	}
    return position;
}


int read_template_file(const char *filename)//读出配置文件的信息并赋值
{
	FILE* file = NULL;
	int i=0,j=0,value=0,configure_pos=-1;
    char line[1024];
	char temp[50];

	if((file = fopen(filename, "r")) == NULL)
	{
		printf("Error: failed to open file: %s\n", filename);
		return -1;
	}

	while( fgets(line,1024,file)!=NULL )
    {
    	char *wadestr;
		char *p;
		
		wadestr=line;
		if(isBlankLine(wadestr) )
		{
			continue;
		}

		
        if (line [i] != '#' && line [i] != EOS)
        {
            /* Eliminate trailing space */
            stringTrimRight (&line[i]);
        
            if (line[i] != EOS)
            {
            	p= strchr(wadestr,'=');
				if(p)
				{
					j=strcspn(wadestr,"=");
					if(j)
					{
						strncpy(temp,wadestr,j);
						temp[j]=EOS;

						p++;
						value=atoi(p);
						configure_pos=tccDealConf(temp);
						if(configure_pos==-1)
						{
							log_error("Error: not defined configure: %s\n",temp);
							continue;
							}
						switch(Webserver_templateMaptable[configure_pos])
							{
								case TEMPLATE_SSID_NAME:
									strcpy(apTemplate[atoi(filename)].ap_ssid.ssid,p);
									break;
								case TEMPLATE_SSID_PASSWORD:
									strcpy(apTemplate[atoi(filename)].ap_ssid.ssid_psw,p);
									break;
								case TEMPLATE_LOGIN_NAME:
									strcpy(apTemplate[atoi(filename)].ap_login.login,p);
									break;
								case TEMPLATE_LOGIN_PASSWORD:
									strcpy(apTemplate[atoi(filename)].ap_login.login_psw,p);
									break;
								case TEMPLATE_NAT_OR_BRIDGE:
									apTemplate[atoi(filename)].NatOrBridge = value;
									break;
								case TEMPLATE_IP_POOL_START:
									apTemplate[atoi(filename)].pool.start.s_addr = inet_addr(p);
									break;
								case TEMPLATE_IP_POOL_END:
									apTemplate[atoi(filename)].pool.end.s_addr = inet_addr(p);
									break;
								case AP_IP_GATEWAY:
									apTemplate[atoi(filename)].ap_gateway.gateway.s_addr = inet_addr(p);
									break;
								case AP_IP_SUBNET:
									apTemplate[atoi(filename)].ap_gateway.subnetmask.s_addr = inet_addr(p);
									break;
								case AP_IP_NMS_IP:
									apTemplate[atoi(filename)].AP_Server_IP.s_addr = inet_addr(p);
									break;
							
								default:
									log_error("Error: not defined configure: %d\n",Webserver_templateMaptable[configure_pos]);

							}
					}
				}
            }
        }
	
    }

	fclose(file);
	return 0;
		
}



int read_template_set()
{
	FILE* file = NULL;
	int i=0,j=0;
    char line[1024];

	if((file = fopen(template_set, "r")) == NULL)
	{
		log_error("Error: failed to open template_set file: %s\n", template_set);
		return -1;
	}

	while( fgets(line,1024,file)!=NULL )
    {
    	char *wadestr;
	    char templates[50]="../templates/";
	
		wadestr=line;
		if(isBlankLine(wadestr) )
		{
			continue;
		}

		
        if (line [i] != '#' && line [i] != EOS)
        {
            /* Eliminate trailing space */
            stringTrimRight (&line[i]);
        
            if (line[i] != EOS)
            {
				strcat(templates,line);
				legalTemplate[atoi(line)] = 1;
            	read_template_file(templates);
				//strcpy(tcc_test_case[j],line);
				if(j<99)
					j++;
				else
					break;
			}
        }
	
    }

	fclose(file);
	return 0;
		
}



int read_AP_set()//读Ap的设置，复制序列号
{
	FILE* file = NULL;
	int i=0,j=0,value=0,apIndex=-1;
    char line[1024];
	char temp[500]={0};

	if((file = fopen(ap_set, "r")) == NULL)
	{
		log_error("Error: failed to open ap_set file: %s\n", ap_set);
		
		return -1;
	}

	while( fgets(line,1024,file)!=NULL )
    {
    	char *wadestr;
		char *p;
		
		wadestr=line;
		if(isBlankLine(wadestr) )
		{
			continue;
		}

		
        if (line [i] != '#' && line [i] != EOS)
        {
            /* Eliminate trailing space */
            stringTrimRight (&line[i]);
        
            if (line[i] != EOS)
            {
            	p= strchr(wadestr,'=');
				if(p)
				{
					j=strcspn(wadestr,"=");
					if(j)
					{
						strncpy(temp,wadestr,j);
						temp[j]=EOS;
						
						p++;
						value=atoi(p);
						apIndex=assign_apIndex();
						if(apIndex!=-1)
							{
							strcpy(ap_list[apIndex].AP_SN,temp);
							if(value<max_ap&&value>0)
								ap_list[apIndex].configure.templateIndex=value;
							else
								{
								log_error("Error:invalid templateIndex\n");
								continue;
								}
							legalAP[apIndex]=1;
							}
						else
							log_error("Error:too many APs\n");
					}
				}
            }
        }
	
    }

	fclose(file);
	return 0;
		
}

int read_Web_Server_Configure_file(const char *filename)//读取WebServer的IP
{
	FILE* file = NULL;
	int i=0,j=0,configure_pos=-1;
    char line[1024];
	char temp[50];

	if((file = fopen(filename, "r")) == NULL)
	{
		printf("Error: failed to open file: %s\n", filename);
		return -1;
	}

	while( fgets(line,1024,file)!=NULL )
    {
    	char *wadestr;
		char *p;
		
		wadestr=line;
		if(isBlankLine(wadestr) )
		{
			continue;
		}

		
        if (line [i] != '#' && line [i] != EOS)
        {
            /* Eliminate trailing space */
            stringTrimRight (&line[i]);
        
            if (line[i] != EOS)
            {
            	p= strchr(wadestr,'=');
				if(p)
				{
					j=strcspn(wadestr,"=");
					if(j)
					{
						strncpy(temp,wadestr,j);
						temp[j]=EOS;

						p++;
						//value=atoi(p);
						configure_pos=tccDealConf(temp);
						if(configure_pos==-1)
						{
							log_error("Error: not defined configure: %s\n",temp);
							continue;
							}
						switch(Webserver_templateMaptable[configure_pos])
							{
								case TEMPLATE_IP_WEB_IP:
									//apTemplate[atoi(filename)].AP_Server_IP.s_addr = inet_addr(p);
									strcpy(Web_Server_IP,p);
									break;
							
								default:
									log_error("Error: not defined configure: %d\n",Webserver_templateMaptable[configure_pos]);

							}
					}
				}
            }
        }
	
    }

	fclose(file);
	return 0;
		
}


int read_clear_control_file(const char *filename)
{
	FILE* file = NULL;
	int i=0,j=0;
    char line[1024];
	char temp[50];


	if((file = fopen(filename, "r")) == NULL)
	{
		printf("Error: failed to open file: %s\n", filename);
		return -1;
	}

	while( fgets(line,1024,file)!=NULL )
    {
    	char *wadestr;
		char *p;
		
		wadestr=line;
		if(isBlankLine(wadestr) )
		{
			continue;
		}

		
        if (line [i] != '#' && line [i] != EOS)
        {
            /* Eliminate trailing space */
            stringTrimRight (&line[i]);
        
            if (line[i] != EOS)
            {
            	p= strchr(wadestr,'=');
				if(p)
				{
					j=strcspn(wadestr,"=");
					if(j)
					{
						strncpy(temp,wadestr,j);
						temp[j]=EOS;

						p++;
						strcpy(cleardate,p);
					}
				}
            }
        }
	
    }

	fclose(file);
	return 0;
		
}



