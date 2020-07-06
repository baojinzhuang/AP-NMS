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

int read_Web_Server_Configure_file(const char *filename)
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



