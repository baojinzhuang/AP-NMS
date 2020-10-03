
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<signal.h>
#include<sys/time.h>


#define AP_ERROR 8
#define AP_INFO 4
#define AP_DEBUG 1


#define log_error(format...) print_log(AP_ERROR, __FILE__, __LINE__, ##format)
#define log_info(format...) print_log(AP_INFO, __FILE__, __LINE__, ##format)
#define log_debug(format...) print_log(AP_DEBUG, __FILE__, __LINE__, ##format)


//长度常量
#define APSN_length 20
#define ssid_length 20
#define ssid_psw_length 20
#define login_length 20
#define login_psw_length 20
#define version_length 20
#define scp_username_length 50
#define scp_psw_length 20
#define remote_file_length 50

#define apServer_apAgent_heart_time 3
#define apServer_apAgent_configure_time 7*apServer_apAgent_heart_time
#define apServer_apAgent_reject_wait_time 3600*24
#define apServer_apAgent_lost_heart_wait_time 3600
#define apServer_apAgent_upgrade_time 60
#define apServer_apAgent_log_get_time 6

#define bufsize_APToServer sizeof(AP)+1


#define AP_APSERVER_OFF 0
#define AP_APSERVER_CONNECTING 1
#define AP_APSERVER_ON 2
#define AP_APSERVER_SOFTWARE_UPGRADE 4
#define AP_APSERVER_REQUEST_LOGINFO  5


//#define AP_APSERVER_ON_IDLE 0
//#define AP_APSERVER_ON_CONFIGURING 1

//#define AP_OFF_LINE 0
//#define AP_ON_LINE  1

//configure result
//#define CONFIGURE_OK   1
//#define CONFIGURE_NOK  2
//#define CONFIGURE_TIMEOUT  3


//消息
#define APSERVER_AP_REGISTER_ACK 0
#define APSERVER_AP_REGISTER_REJ 1
#define APSERVER_AP_CONFIGURE    2
#define APSERVER_AP_HEART_BEAT   3
#define APSERVER_AP_SOFTWARE_UPGRADE 4
#define APSERVER_AP_REQUEST_LOGINFO  5


#define	AP_APSERVER_REGISTER_REQ_WITHOUT_CONFIGURATION                 0
#define	AP_APSERVER_REGISTER_REQ_WITH_CONFIGURATION                    1
#define	AP_APSERVER_REGISTER_COMPLETE                                  2
#define	AP_APSERVER_CONFIGURE_OK                                       3
#define	AP_APSERVER_CONFIGURE_NOK                                      4
#define	AP_APSERVER_HEART_BEAT_ACK                                     5
#define	AP_APSERVER_HEART_BEAT_ACK_WITH_STATUS                         6
#define	AP_APSERVER_REQUEST_LOGINFO_ACK                                    7

#define TRUE	1
#define FALSE	0

#define EOS '\0'

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

//ap_states
#define	AP_STATES_LAN_IPADDR                               0
#define	AP_STATES_SSID_NAME                                1
#define	AP_STATES_SSID_PASSWORD                            2
#define	AP_STATES_LOGIN_NAME                               3
#define	AP_STATES_LOGIN_PASSWORD                           4
#define	AP_STATES_NAT_OR_BRIDGE                            5
#define	AP_STATES_IP_POOL_START                            6
#define	AP_STATES_IP_POOL_END                              7
#define	AP_STATES_IP_GATEWAY                               8
#define	AP_STATES_VERSION                                  9
#define	AP_STATES_ANTENNA_NUMBER                           10
#define	AP_STATES_IP_SUBNET                                11
#define AP_STATES_APSERVER_IP							   12



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
	char Scp_Username[scp_username_length];//用户名
	char Scp_PassWord[scp_psw_length];    //密码
	char Remote_FilePath[remote_file_length]; //远程路径
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
    int ap_timer_flag;//0:off,1:connecting,2:on
	int ap_on_timer_flag;//0:nothing,1:configuring,2:upgrading
    int heart_beat_timer;
	int heart_beat_timeout_times;
	int ap_on_timer;
} AP_timer; 


typedef struct
{
    struct sockaddr_in addrAP;
    socklen_t addrlen;
} AP_socket; 










