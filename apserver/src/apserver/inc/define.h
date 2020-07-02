
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<signal.h>
#include<sys/time.h>


#define APSERVER_ERROR 8
#define APSERVER_INFO 4
#define APSERVER_DEBUG 1


#define log_error(format...) print_log(APSERVER_ERROR, __FILE__, __LINE__, ##format)
#define log_info(format...) print_log(APSERVER_INFO, __FILE__, __LINE__, ##format)
#define log_debug(format...) print_log(APSERVER_DEBUG, __FILE__, __LINE__, ##format)


//常量
#define APSN_length 20
#define ssid_length 20
#define ssid_psw_length 20
#define login_length 20
#define login_psw_length 20
#define version_length 20
#define max_ap 20
#define max_template 200
#define bufsize_webToAp max_ap * sizeof(AP)+2
#define bufsize_apToWeb1 max_template * sizeof(AP_Configuration_template)+2
#define bufsize_apToWeb2 max_ap * sizeof(AP)+2

#define web_apServer_heart_time 3
#define apServer_apAgent_heart_time 3
#define web_apServer_configure_time web_apServer_heart_time+1
#define apServer_apAgent_configure_time apServer_apAgent_heart_time
#define web_apServer_heart_out_time 3*web_apServer_heart_time
#define web_apServer_upgrade_time 60
#define apServer_apAgent_upgrade_time 60
#define web_apServer_log_get_time 6
#define apServer_apAgent_log_get_time 6


//状态
#define WEB_APSERVER_OFF              0
#define WEB_APSERVER_CONNECTING       1
#define WEB_APSERVER_ON               2
#define WEB_APSERVER_CONFIGURING      3//1
#define WEB_APSERVER_SOFTWARE_UPGRADE 4//2
#define WEB_APSERVER_LOGINFO          5//3


#define AP_APSERVER_OFF               0
#define AP_APSERVER_CONNECTING        1
#define AP_APSERVER_ON                2
#define AP_APSERVER_CONFIGURING       3//1
#define AP_APSERVER_SOFTWARE_UPGRADE  4//2
#define AP_APSERVER_GET_LOGINFO       5//3


#define AP_OFF_LINE 0
#define AP_ON_LINE  1

//configure result
#define CONFIGURE_OK       1
#define CONFIGURE_NOK      2
#define CONFIGURE_TIMEOUT  3

//软件升级结果
#define SOFTWARE_UPGRADE_OK       1
#define SOFTWARE_UPGRADE_NOK      2
#define SOFTWARE_UPGRADE_TIMEOUT  3

//请求日志结果
#define REQUEST_LOGINFO_OK       1
#define REQUEST_LOGINFO_NOK      2
#define REQUEST_LOGINFO_TIMEOUT  3



//消息Apserver-ap
#define APSERVER_AP_REGISTER_ACK      0
#define APSERVER_AP_REGISTER_REJ      1
#define APSERVER_AP_CONFIGURE         2
#define APSERVER_AP_HEART_BEAT        3
#define APSERVER_AP_SOFTWARE_UPGRADE  4    //软件升级
#define APSERVER_AP_REQUEST_LOGINFO   5    //请求日志

//Apserver<--->Web
#define APSERVER_WEB_TEMPLATE_REPORT     10
#define APSERVER_WEB_AP_REPORT           1
#define APSERVER_WEB_TEMPLATE_MODIFY     2
#define APSERVER_WEB_AP_MODIFY           3
#define APSERVER_WEB_AP_CONFIGURE        4
#define APSERVER_WEB_HEART_BEAT          5
#define APSERVER_WEB_SOFTWARE_UPGRADE    6  //软件升级
#define APSERVER_WEB_REQUEST_LOGINFO     7  //请求日志

//消息ap--Apserver
#define	AP_APSERVER_REGISTER_REQ_WITHOUT_CONFIGURATION                 0
#define	AP_APSERVER_REGISTER_REQ_WITH_CONFIGURATION                    1
#define	AP_APSERVER_REGISTER_COMPLETE                                  2
#define	AP_APSERVER_CONFIGURE_OK                                       3
#define	AP_APSERVER_CONFIGURE_NOK                                      4
#define	AP_APSERVER_HEART_BEAT_ACK                                     5
#define	AP_APSERVER_HEART_BEAT_ACK_WITH_STATUS                         6
#define	AP_APSERVER_REQUEST_LOGINFO                                    7




#define TRUE	1
#define FALSE	0

#define EOS '\0'

//template
#define	TEMPLATE_SSID_NAME                                0
#define	TEMPLATE_SSID_PASSWORD                            1
#define	TEMPLATE_LOGIN_NAME                               2
#define	TEMPLATE_LOGIN_PASSWORD                           3
#define	TEMPLATE_NAT_OR_BRIDGE                            4
#define	TEMPLATE_IP_POOL_START                            5
#define	TEMPLATE_IP_POOL_END                              6
#define	TEMPLATE_DHCP_OR_STATIC                           7
#define	TEMPLATE_IP_ADDR                                  8
#define	TEMPLATE_IP_GATEWAY                               9
#define	TEMPLATE_IP_SUBNET                                10
#define	TEMPLATE_IP_NMS_IP                                11
#define	TEMPLATE_IP_WEB_IP                                12
#define	TEMPLATE_OPTIONS                                  13

//template operation
#define	TEMPLATE_ADD                                1
#define	TEMPLATE_DEL                                2
#define	TEMPLATE_MODIFY                             3
#define	TEMPLATE_OPERATION_OK                       10
#define	TEMPLATE_OPERATION_NOK                      11



//ap
#define	AP_SERIAL_NUMBER                                  0
#define	AP_TEMPLATE_INDEX                                 1
#define	AP_IP_ADDR                                        2
#define	DHCP_OR_STATIC                                    3
#define	AP_SSID_NAME                                      4
#define	AP_SSID_PASSWORD                                  5
#define	AP_LOGIN_NAME                                     6
#define	AP_LOGIN_PASSWORD                                 7
#define	AP_NAT_OR_BRIDGE                                  8
#define	AP_IP_POOL_START                                  9
#define	AP_IP_POOL_END                                    10
#define	AP_IP_GATEWAY                                     11
#define	AP_IP_SUBNET                                      12
#define	AP_IP_NMS_IP                                      13
#define	AP_USER_NUMBER                                    14
#define	AP_MODE                                           15
#define	AP_VERSION                                        16
#define	AP_ANTENNA_NUMBER                                 17

//AP operation
#define	AP_ADD                                1
#define	AP_DEL                                2
#define	AP_MODIFY                             3
#define	AP_OPERATION_OK                       10
#define	AP_OPERATION_NOK                      11




typedef struct
{
	struct in_addr start;
	struct in_addr end;
}ip_pool;

typedef struct
{
	char ssid[ssid_length];
	char ssid_psw[ssid_psw_length];
}ssid;

typedef struct
{
    char login[login_length];
	char login_psw[login_psw_length];
}login;

typedef struct
{
	struct in_addr gateway;
	struct in_addr subnetmask;
}gateway;

typedef struct
{
	int templateIndex;
	ssid ap_ssid;
	int NatOrBridge;//0:bridge  1:nats
	int DHCPOrStatic;//0:dhcp  1:static
	ip_pool  pool;
	login  ap_login;
	struct in_addr AP_IP;
	gateway ap_gateway;
	struct in_addr AP_Server_IP;
	unsigned char options;
	char Scp_Username[50];//用户名
	char  Scp_PassWord[20];    //密码
	char Remote_FilePath[50]; //远程路径
	char version[version_length];//版本
}AP_Configuration_template;

typedef struct
{
    char online_state;//0:offline  1:online
	char user_number;
	char model;//0:wifi4  1:wifi5  2:wifi6
	char version[version_length];
	char antenna_number;
	struct in_addr AP_IP;
	gateway ap_gateway;
	char options;
}AP_Status;



typedef struct
{
	char AP_SN[APSN_length];
	AP_Configuration_template configure;
	AP_Status status;
}AP;


typedef struct
{
	char command;//0:add  1:del  2:modify  
	char AP_SN[APSN_length];
	AP_Configuration_template  ap_template;
}legalAP_operation_req;

typedef struct
{
	char command;//0:add  1:del  2:modify   
	AP_Configuration_template  ap_template;
} template_operation_req; // 配置模板

typedef struct
{
	char ack;//0:add  1:del  2:modify  
	char AP_SN[APSN_length];
}legalAP_operation_ack;

typedef struct
{
	char ack;//0:sucess  1:fail 
	char AP_SN[APSN_length];
	char Scp_Username[50];//用户名
	char  Scp_PassWord[20];    //密码
	char Remote_FilePath[50]; //远程路径
	char version[version_length];//版本
}AP_upgrade_ack;

typedef struct
{
	char ack; //0:ok,1:nok,2:timeout    
	char AP_SN[APSN_length];
}AP_configuration_ack; //AP配置

typedef struct
{
	char ack;//0:ok  1:nok 
	int templateIndex;
} template_operation_ack; // 配置模板


typedef struct
{
    int ap_timer_flag;//0:off,1:connecting,2:on
	int ap_on_timer_flag;//0:nothing,1:configuring,2:upgrading,3:log get
    int heart_beat_timer;
	int heart_beat_timeout_times;
	int ap_on_timer;
} AP_timer; 

typedef struct
{
    struct sockaddr_in addrAP;
    socklen_t addrlen;
} AP_socket; 
