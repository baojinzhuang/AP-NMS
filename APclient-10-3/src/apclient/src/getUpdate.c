/*
 * getUpdate1.c
 *
 *  Created on: 2020年5月27日
 *      Author: scorpinxia
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<stdarg.h>
#include<ctype.h>
#include"define.h"


extern AP ap_configuration_status;
int read_states_file(const char *filename);
int get_serialnumber();
int  get_usernumber();
int htoi(char s[]);
void split(char *src,const char *separator,char **dest,int *num);
char* itoa(int num,char* str,int radix);

int getUpdate()
{
	char* states_file="/data/.kernel_nvram.setting";
	printf("正在获取状态\n");

	if(read_states_file(states_file)==-1)
		printf("error read states file");


	if(get_serialnumber()==-1)
		printf("error get user_serial_number");

	if(get_usernumber()==-1)
			printf("error get user_number");

	ap_configuration_status.status.model = 0;

	return 0;
}

int get_serialnumber()
{
	    FILE *file = NULL;
	    char *filename="/sys/class/net/eth7/address";

		if((file = fopen(filename, "r")) == NULL)
		{
			printf("Error: failed to open file: %s\n", filename);
			return -1;
		}
		char line[128];
		char input_id[50];
		while( fgets(line,1024,file)!=NULL )
	    {
			char *revbuf[8] = {0}; //存放分割后的子字符串

				//分割后子字符串的个数
			int num = 0;
			//int i;
			int j;
			split(line,":",revbuf,&num); //调用函数进行分割


			//输出返回的每个内容
//			for(i = 0;i < num; i ++) {
//					//lr_output_message("%s\n",revbuf[i]);
//					printf("%s\n",revbuf[i]);
//	//				j=htoi(revbuf[i]);
//	//				printf("%d\n",j);
//				}
			sprintf(input_id,"%s%s%s",revbuf[3],revbuf[4],revbuf[5]);
			j=htoi(input_id);

			itoa(j,ap_configuration_status.AP_SN,10);
	        printf("%s\n",ap_configuration_status.AP_SN);


	    }

		fclose(file);

		return 0;
}

int  get_usernumber()
{
	char line[1024];
//	char input_id[1000];
//	int ret;

	FILE *fp=popen("wl -i eth7 assoclist", "r");
	if(fp==NULL)
		return -1;
	int num=0;
	while( fgets(line,1024,fp)!=NULL )
		{
		    num++;
			    }

	ap_configuration_status.status.user_number = num;
	printf("在线人书：%d",num);
	pclose(fp);
	return 0;
}



int htoi(char s[])
{
    int i = 0;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
    {
        i = 2;
    }
    else
    {
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)
    {
        if (tolower(s[i]) > '9')
        {
            n = 16 * n + (10 + tolower(s[i]) - 'a');
        }
        else
        {
            n = 16 * n + (tolower(s[i]) - '0');
        }
    }
    return n;
}

void split(char *src,const char *separator,char **dest,int *num) {
	/*
		src 源字符串的首地址(buf的地址)
		separator 指定的分割字符
		dest 接收子字符串的数组
		num 分割后子字符串的个数
	*/
     char *pNext;
     int count = 0;
     if (src == NULL || strlen(src) == 0) //如果传入的地址为空或长度为0，直接终止
        return;
     if (separator == NULL || strlen(separator) == 0) //如未指定分割的字符串，直接终止
        return;
     pNext = (char *)strtok(src,separator); //必须使用(char *)进行强制类型转换(虽然不写有的编译器中不会出现指针错误)
     while(pNext != NULL) {
          *dest++ = pNext;
          ++count;
         pNext = (char *)strtok(NULL,separator);  //必须使用(char *)进行强制类型转换
    }
    *num = count;
}


char* itoa(int num,char* str,int radix)
{
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//索引表
    unsigned unum;//存放要转换的整数的绝对值,转换的整数可能是负数
    int i=0,j,k;//i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况，k用来指示调整顺序的开始位置;j用来指示调整顺序时的交换。

    //获取要转换的整数的绝对值
    if(radix==10&&num<0)//要转换成十进制数并且是负数
    {
        unum=(unsigned)-num;//将num的绝对值赋给unum
        str[i++]='-';//在字符串最前面设置为'-'号，并且索引加1
    }
    else unum=(unsigned)num;//若是num为正，直接赋值给unum

    //转换部分，注意转换后是逆序的
    do
    {
        str[i++]=index[unum%(unsigned)radix];//取unum的最后一位，并设置为str对应位，指示索引加1
        unum/=radix;//unum去掉最后一位

    }while(unum);//直至unum为0退出循环

    str[i]='\0';//在字符串最后添加'\0'字符，c语言字符串以'\0'结束。

    //将顺序调整过来
    if(str[0]=='-') k=1;//如果是负数，符号不用调整，从符号后面开始调整
    else k=0;//不是负数，全部都要调整

    char temp;//临时变量，交换两个值时用到
    for(j=k;j<=(i-1)/2;j++)//头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
    {
        temp=str[j];//头部赋值给临时变量
        str[j]=str[i-1+k-j];//尾部赋值给头部
        str[i-1+k-j]=temp;//将临时变量的值(其实就是之前的头部值)赋给尾部
    }

    return str;//返回转换后的字符串

}

