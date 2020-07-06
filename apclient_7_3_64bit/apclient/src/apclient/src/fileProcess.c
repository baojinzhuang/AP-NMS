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
extern char AP_Server_IP[50];

extern int g_level;
extern char Ap_allaps[][50];
extern int Ap_Maptable[];


void write_log_file(char* logs);


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
	//printf("%d\n",p->tm_mday);/*获取当前月份日数,范围是1-31%d月%d日%时%分%秒*/
	//printf("%d\n",1+p->tm_mon);/*获取当前月份,范围是0-11,所以要加1*/
	//printf("%d\n",1900+p->tm_year);/*获取当前年份,从1900开始，所以要加1900*/
	
	if((file = fopen(log_file, "a")) == NULL)
	{
		printf("no log_file found\n");
		return ;
	}
	//printf("enter write_log!\n");
	snprintf(line, 512, "[%d年%d月%d日%d时%d分%d秒]:%s\n",1900+p->tm_year,1+p->tm_mon,p->tm_mday,8+p->tm_hour,p->tm_min,p->tm_sec,logs);
	fputs(line, file);
	
	fclose(file);

}



void write_ap_configure_file(const char *filename)
{
	FILE* file = NULL;
    char data[513]={0};
	char whole_file[10000]={0};

	file = fopen(filename, "w+");

	//snprintf(data, 512, "\nTimes: %d\n",times+1);
	snprintf(data, 512, "%s=%s\n",Ap_allaps[AP_IP_ADDR],inet_ntoa(ap_configuration_status.configure.AP_IP));
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%d\n",Ap_allaps[DHCP_OR_STATIC],ap_configuration_status.configure.DHCPOrStatic);
	strcat(whole_file,data);
    snprintf(data, 512, "%s=%s\n",Ap_allaps[AP_SSID_NAME],ap_configuration_status.configure.ap_ssid.ssid);
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%s\n",Ap_allaps[AP_SSID_PASSWORD],ap_configuration_status.configure.ap_ssid.ssid_psw);
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%s\n",Ap_allaps[AP_LOGIN_NAME],ap_configuration_status.configure.ap_login.login);
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%s\n",Ap_allaps[AP_LOGIN_PASSWORD],ap_configuration_status.configure.ap_login.login_psw);
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%d\n",Ap_allaps[AP_NAT_OR_BRIDGE],ap_configuration_status.configure.NatOrBridge);
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%s\n",Ap_allaps[AP_IP_POOL_START],inet_ntoa(ap_configuration_status.configure.pool.start));
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%s\n",Ap_allaps[AP_IP_POOL_END],inet_ntoa(ap_configuration_status.configure.pool.end));
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%s\n",Ap_allaps[AP_IP_GATEWAY],inet_ntoa(ap_configuration_status.configure.ap_gateway.gateway));
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%s\n",Ap_allaps[AP_IP_SUBNET],inet_ntoa(ap_configuration_status.configure.ap_gateway.subnetmask));
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%s\n",Ap_allaps[AP_IP_NMS_IP],inet_ntoa(ap_configuration_status.configure.AP_Server_IP));
	strcat(whole_file,data);
	/*snprintf(data, 512, "%s=%d\n",Ap_allaps[AP_USER_NUMBER],ap_configuration_status.status.user_number);
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%d\n",Ap_allaps[AP_MODE],ap_configuration_status.status.model);
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%s\n",Ap_allaps[AP_VERSION],ap_configuration_status.status.version);
	strcat(whole_file,data);
	snprintf(data, 512, "%s=%d\n",Ap_allaps[AP_ANTENNA_NUMBER],ap_configuration_status.status.antenna_number);
	strcat(whole_file,data);*/
	fputs(whole_file, file);

	fclose(file);
}



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
    	if(!strcmp(Ap_allaps[i],"\0"))
			return -1;
		else if(!strcmp(Ap_allaps[i],strToCmp))
			return i;
		else
			i++;
    	}
    return position;
}


int read_ap_file(const char *filename)
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

	//strcpy(ap_configuration_status.AP_SN,filename);

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
						switch(Ap_Maptable[configure_pos])
							{
								case AP_SERIAL_NUMBER:
									strcpy(ap_configuration_status.AP_SN,p);
									break;
								case AP_TEMPLATE_INDEX:
									ap_configuration_status.configure.templateIndex = value;
									break;
								case AP_IP_ADDR:
									ap_configuration_status.configure.AP_IP.s_addr = inet_addr(p);
									break;
								case AP_SSID_NAME:
									strcpy(ap_configuration_status.configure.ap_ssid.ssid,p);
									break;
								case AP_SSID_PASSWORD:
									strcpy(ap_configuration_status.configure.ap_ssid.ssid_psw,p);
									break;
								case AP_LOGIN_NAME:
									strcpy(ap_configuration_status.configure.ap_login.login,p);
									break;
								case AP_LOGIN_PASSWORD:
									strcpy(ap_configuration_status.configure.ap_login.login_psw,p);
									break;
								case AP_NAT_OR_BRIDGE:
									ap_configuration_status.configure.NatOrBridge = value;
									break;
								case AP_IP_POOL_START:
									ap_configuration_status.configure.pool.start.s_addr = inet_addr(p);
									break;
								case AP_IP_POOL_END:
									ap_configuration_status.configure.pool.end.s_addr = inet_addr(p);
									break;
								case AP_IP_GATEWAY:
									ap_configuration_status.configure.ap_gateway.gateway.s_addr = inet_addr(p);
									break;
								case AP_IP_SUBNET:
									ap_configuration_status.configure.ap_gateway.subnetmask.s_addr = inet_addr(p);
									break;
								case AP_IP_NMS_IP:
									ap_configuration_status.configure.AP_Server_IP.s_addr = inet_addr(p);
									break;
								case AP_USER_NUMBER:
									ap_configuration_status.status.user_number = value;
									break;
								case AP_MODE:
									ap_configuration_status.status.model = value;
									break;
								case AP_VERSION:
									strcpy(ap_configuration_status.status.version,p);
									break;
								case AP_ANTENNA_NUMBER:
									ap_configuration_status.status.antenna_number = value;
									break;
							
								default:
									log_error("Error: not defined configure: %d\n",Ap_Maptable[configure_pos]);

							}
					}
				}
            }
        }
	
    }

	fclose(file);
	return 0;
		
}
int read_AP_Server_configure_file(const char *filename)
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
						switch(Ap_Maptable[configure_pos])
							{
								case AP_IP_NMS_IP:
									//apTemplate[atoi(filename)].AP_Server_IP.s_addr = inet_addr(p);
									strcpy(AP_Server_IP,p);
									break;

								default:
									log_error("Error: not defined configure: %d\n",Ap_Maptable[configure_pos]);

							}
					}
				}
            }
        }

    }

	fclose(file);
	return 0;

}





