/*
 * configureAP.c
 *
 *  Created on: 2020年5月28日
 *      Author: scorpinxia
 */
#include"define.h"
#include<stdio.h>
#include<stdlib.h>
//#include<unistd.h>
#include<string.h>
//#include<pthread.h>
//#include<ctype.h>

extern AP ap_configuration_status;
extern char Ap_States_allaps[][50];

void setAPStatus()
{
	char input_id[100];
	int ret;


	printf("%s=%s\n",Ap_States_allaps[AP_STATES_LAN_IPADDR],inet_ntoa(ap_configuration_status.configure.AP_IP));
		printf("%s=%s\n",Ap_States_allaps[AP_STATES_SSID_NAME],ap_configuration_status.configure.ap_ssid.ssid);
		printf("%s=%s\n",Ap_States_allaps[AP_STATES_SSID_PASSWORD],ap_configuration_status.configure.ap_ssid.ssid_psw);
		printf("%s=%s\n",Ap_States_allaps[AP_STATES_LOGIN_NAME],ap_configuration_status.configure.ap_login.login);
		printf("%s=%s\n",Ap_States_allaps[AP_STATES_LOGIN_PASSWORD],ap_configuration_status.configure.ap_login.login_psw);
		printf("%d\n",ap_configuration_status.configure.NatOrBridge);
		printf("%s=%s\n",Ap_States_allaps[AP_STATES_IP_POOL_START],inet_ntoa(ap_configuration_status.configure.pool.start));
		printf("%s=%s\n",Ap_States_allaps[AP_STATES_IP_POOL_END],inet_ntoa(ap_configuration_status.configure.pool.end));
		printf("%s=%s\n",Ap_States_allaps[AP_STATES_IP_GATEWAY],inet_ntoa(ap_configuration_status.configure.ap_gateway.gateway));
		printf("%s=%s\n",Ap_States_allaps[AP_STATES_IP_SUBNET],inet_ntoa(ap_configuration_status.configure.ap_gateway.subnetmask));
	sprintf(input_id,"nvram set %s=%s",Ap_States_allaps[AP_STATES_LAN_IPADDR],inet_ntoa(ap_configuration_status.configure.AP_IP));
	ret=system(input_id);
	if(ret!=-1||ret!=127)
		puts("Set IP Ok");
	sprintf(input_id,"nvram set %s=%s",Ap_States_allaps[AP_STATES_SSID_NAME],ap_configuration_status.configure.ap_ssid.ssid);
	ret=system(input_id);
	if(ret!=-1||ret!=127)
		puts("Set ssid Ok");
	sprintf(input_id,"nvram set %s=%s",Ap_States_allaps[AP_STATES_SSID_PASSWORD],ap_configuration_status.configure.ap_ssid.ssid_psw);
	ret=system(input_id);
	if(ret!=-1||ret!=127)
		puts("Set ssid_psw Ok");

	sprintf(input_id,"nvram set %s=%s",Ap_States_allaps[AP_STATES_LOGIN_NAME],ap_configuration_status.configure.ap_login.login);
	ret=system(input_id);
	if(ret!=-1||ret!=127)
		puts("Set loginname Ok");
	sprintf(input_id,"nvram set %s=%s",Ap_States_allaps[AP_STATES_LOGIN_PASSWORD],ap_configuration_status.configure.ap_login.login_psw);
	ret=system(input_id);
	if(ret!=-1||ret!=127)
		puts("Set loginpsw Ok");

	if(ap_configuration_status.configure.NatOrBridge==0)
	   sprintf(input_id,"nvram set lan_proto=dhcp");
	else
	   sprintf(input_id,"nvram set lan_proto=static");;
	ret=system(input_id);
	if(ret!=-1||ret!=127)
		puts("Set Nat or Bridge Ok");

	sprintf(input_id,"nvram set %s=%s",Ap_States_allaps[AP_STATES_IP_POOL_START],inet_ntoa(ap_configuration_status.configure.pool.start));
	ret=system(input_id);
	if(ret!=-1||ret!=127)
		puts("Set poolstart Ok");
	sprintf(input_id,"nvram set %s=%s",Ap_States_allaps[AP_STATES_IP_POOL_END],inet_ntoa(ap_configuration_status.configure.pool.end));
	ret=system(input_id);
	if(ret!=-1||ret!=127)
		puts("Set poolend Ok");

	sprintf(input_id,"nvram set %s=%s",Ap_States_allaps[AP_STATES_IP_GATEWAY],inet_ntoa(ap_configuration_status.configure.ap_gateway.gateway));
	ret=system(input_id);
	if(ret!=-1||ret!=127)
		puts("Set gateway Ok");
	sprintf(input_id,"nvram set %s=%s",Ap_States_allaps[AP_STATES_IP_SUBNET],inet_ntoa(ap_configuration_status.configure.ap_gateway.subnetmask));
	ret=system(input_id);
	if(ret!=-1||ret!=127)
		puts("Set submask Ok");

	sprintf(input_id,"nvram commit");
	ret=system(input_id);
	if(ret!=-1||ret!=127)
		puts("commit Ok");

	sprintf(input_id,"/sys-restart");
	ret=system(input_id);
	if(ret!=-1||ret!=127)
		puts("network restart Ok");

}
void setAPStatus1()
{
//	char input_id[100];
//	int ret;

	printf("%s=%s/n",Ap_States_allaps[AP_STATES_LAN_IPADDR],inet_ntoa(ap_configuration_status.configure.AP_IP));
	printf("%s=%s/n",Ap_States_allaps[AP_STATES_SSID_NAME],ap_configuration_status.configure.ap_ssid.ssid);
	printf("%s=%s/n",Ap_States_allaps[AP_STATES_SSID_PASSWORD],ap_configuration_status.configure.ap_ssid.ssid_psw);
	printf("%s=%s/n",Ap_States_allaps[AP_STATES_LOGIN_NAME],ap_configuration_status.configure.ap_login.login);
	printf("%s=%s/n",Ap_States_allaps[AP_STATES_LOGIN_PASSWORD],ap_configuration_status.configure.ap_login.login_psw);
	printf("%d/n",ap_configuration_status.configure.NatOrBridge);
	printf("%s=%s/n",Ap_States_allaps[AP_STATES_IP_POOL_START],inet_ntoa(ap_configuration_status.configure.pool.start));
	printf("%s=%s/n",Ap_States_allaps[AP_STATES_IP_POOL_END],inet_ntoa(ap_configuration_status.configure.pool.end));
	printf("%s=%s/n",Ap_States_allaps[AP_STATES_IP_GATEWAY],inet_ntoa(ap_configuration_status.configure.ap_gateway.gateway));
	printf("%s=%s/n",Ap_States_allaps[AP_STATES_IP_SUBNET],inet_ntoa(ap_configuration_status.configure.ap_gateway.subnetmask));



}
