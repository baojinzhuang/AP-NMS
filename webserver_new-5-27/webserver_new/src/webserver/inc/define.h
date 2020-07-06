
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<signal.h>
#include<sys/time.h>
#include<mysql/mysql.h>

#define WEBSERVER_ERROR 8
#define WEBSERVER_INFO 4
#define WEBSERVER_DEBUG 1



#define log_error(format...) print_log(WEBSERVER_ERROR, __FILE__, __LINE__, ##format)
#define log_info(format...) print_log(WEBSERVER_INFO, __FILE__, __LINE__, ##format)
#define log_debug(format...) print_log(WEBSERVER_DEBUG, __FILE__, __LINE__, ##format)



#define Template 0
#define Ap 1
#define configuring 3
#define update_status 4
#define add 0
#define del 1
#define modify 2





//常量
#define APSN_length 20
#define ssid_length 20
#define ssid_psw_length 20
#define login_length 20
#define login_psw_length 20
#define version_length 20
#define max_ap_server 20
#define max_ap_per_server 20
#define max_ap max_ap_server*max_ap_per_server
#define max_template 200
#define bufsize_webToAp max_ap_per_server * sizeof(AP)+2
#define bufsize_apToWeb1 max_template * sizeof(AP_Configuration_template)+2
#define bufsize_apToWeb2 max_ap_per_server * sizeof(AP)+2

#define web_apServer_heart_time 3
#define web_apServer_configure_time 4*web_apServer_heart_time
#define web_apServer_heart_out_time 3*web_apServer_heart_time


//状态
#define WEB_APSERVER_OFF 0
#define WEB_APSERVER_CONNECTING 1
#define WEB_APSERVER_ON 2
#define WEB_APSERVER_CONFIGURING 3


#define AP_OFF_LINE 0
#define AP_ON_LINE  1

//configure result
#define CONFIGURE_OK   1
#define CONFIGURE_NOK  2
#define CONFIGURE_TIMEOUT  3



//消息


#define APSERVER_WEB_TEMPLATE_REPORT     10
#define APSERVER_WEB_AP_REPORT           1
#define APSERVER_WEB_TEMPLATE_MODIFY     2
#define APSERVER_WEB_AP_MODIFY           3
#define APSERVER_WEB_AP_CONFIGURE        4
#define APSERVER_WEB_HEART_BEAT          5

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
    int apServer_timer_flag;//0:off,1:connecting,2:on
	int apServer_on_timer_flag;//0:nothing,1:configuring,2:upgrading
    int heart_beat_timer;
	int heart_beat_timeout_times;
	int apServer_on_timer;
} APServer_timer; 

typedef struct
{
    struct sockaddr_in addrAP;
    socklen_t addrlen;
} APServer_socket; 









