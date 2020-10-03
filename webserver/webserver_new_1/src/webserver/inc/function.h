#include"define.h"
void send_to_apServer(int apServerIndex, int command,
		AP_Configuration_template *tempTemplate,
		template_operation_req *templateOperation,
		legalAP_operation_req *APOperation, int size, char *apConfiguredList);
int print_log(unsigned int level, const char *filename, unsigned int line,
		char *format, ...);
void print_info(void);
void update_legalAP_add_o(AP *AP_list);
void update_legalAP_add(legalAP_operation_req *AP_req);
void update_legalAP_del(char *index);
void update_legalTemplate_del(int index);
void update_legalTemplate_add(template_operation_req *template_req);
void update_legalTemplate_add_o(AP_Configuration_template *ap_Template);
void add_quotation(char *s, char *APset, char *filepath, char *version);
int Fill_ap_list();
int Fill_apTemplate();
void AP_sort(char *APset, int command, int size, char *username, char *pwd,
		char *filepath, char *version);
void AP_Roporter_to_APServer_sort();
void Send_to_AP_Configure_Sort(char *APset, int command, int size);
void add_quotae(char *s, char *APset);
int mysql_connect();
int Fill_apTemplate();
int Fill_ap_list();
int LegalAPServer_find(struct sockaddr_in *apserver_socket_addr);
int assign_apIndex();
void ap_modify_report_assign_value(legalAP_operation_ack *destination,
		legalAP_operation_ack *source);

extern AP_Configuration_template apTemplate[max_template];
extern AP ap_list[max_ap];
extern AP temp_aplist[max_ap];
extern int legalTemplate[max_template];
extern int legalAP[max_ap];
extern int legalAPServerConfigureResult[max_ap]; //0，未配置，1，配置成功，2，配置失败，3，配置超时
extern char Web_Server_IP[50];

extern APServer_timer legalAPServerTimer[max_ap_server];
extern int Temp_GetLog_Nums;
extern int Configure_Nums;
extern int SoftWare_Nums;
extern int GetLog_Nums;
extern int Num_i1;
extern int Num_i2;
extern int Num_i3;
extern int flag_begin;


extern Gather_timer SumTimer[5];
extern template_operation_ack Template_Ack[max_template];
extern legalAP_operation_ack legalAP_Ack[max_ap];
extern AP_upgrade_ack legalAP_upgrade_Ack[max_ap];
extern AP_log_ack legalAP_log_Ack[max_ap];
extern AP_configuration_ack AP_Configure_Ack[max_ap];

extern APServer_socket apServer_socket_list[max_ap_server];
extern int sockSer;

extern int sockWeb;
extern struct sockaddr_in addrWebSer;  //创建一个记录地址信息的结构体
extern socklen_t addrWeblen;

extern char  Local_Save_AP_LOG_PATH[50];
extern char sql_insert[1024];
extern MYSQL mysql;
extern MYSQL *conn;
extern MYSQL_RES resultt;
extern MYSQL_RES *result;
extern MYSQL_ROW row;
extern MYSQL_ROW row1;
extern MYSQL_RES r;
extern MYSQL_RES *result1;
extern char template_log[256 * 20];
extern char template_temp[500];
extern char template_tempindex[500];
extern char sendapserverid[500];
extern char reciveapserverid[500];
extern char reciveupgradecontent[5000];
extern char reciveapack[500];
void ap_upgrade_report_assign_value(AP_upgrade_ack *destination,
		AP_upgrade_ack *source);
void ap_log_report_assign_value(AP_log_ack *destination, AP_log_ack *source);
void ap_configure_report_assign_value(AP_configuration_ack *destination,
		AP_configuration_ack *source);
void template_modify_report_assign_value(template_operation_ack *destination,
		template_operation_ack *source);
void ap_configure_assign_value(AP *destination, AP *source);
void template_assign_value(AP_Configuration_template *destination,
		AP_Configuration_template *source);
void ap_status_assign_value(AP *destination, AP *source);
void decode_template_report(char *buf, AP_Configuration_template *temp);
void decode_ap_report(char *buf, AP *temp);
void decode_template_modify_report(char *buf,int apServer_index);
void decode_ap_status_reporter(char *buf, AP *temp);
void decode_ap_modify_report(char *buf,int apServer_index);
void decode_ap_upgrade_report(char *buf,int apServer_index);
void decode_ap_log_report(char *buf,int apServer_index);
void decode_ap_configure_report(char *buf,int apServerindex);
void Fill_APServer_socket_list();
void print_info(void);
void update_legalTemplate_del(int index);
void update_legalTemplate_add(template_operation_req *template_req);
void update_legalTemplate_add_o(AP_Configuration_template *ap_Template);
void update_legalAP_del(char *index);
void update_legalAP_add(legalAP_operation_req *AP_req);
void update_legalAP_add_o(AP *AP_list);


void write_software_log();
void write_configure_log();
void write_ap_log();
void write_template_log();
void write_getlog_log();


int print_log(unsigned int level, const char *filename, unsigned int line,
		char *format, ...);
int read_template_set();
int read_AP_set();
int LegalAPServer_find(struct sockaddr_in *apserver_socket_addr);
void dispatch_APServerMsg(int ap_index, char *buf);
void send_to_apServer(int apServerIndex, int command,
		AP_Configuration_template *tempTemplate,
		template_operation_req *templateOperation,
		legalAP_operation_req *APOperation, int size, char*);
int mysql_connect();
void Fill_APServer_socket_list();
int Fill_apTemplate();
int Fill_ap_list();
int control_command_to_APserver();
void add_quotation(char *s, char *APset, char *filepath, char *version);
void AP_Upgrade_sort(char *APset, int command, int size, char *username, char *pwd,
		char *filepath, char *version);
void print_info(void);
int read_Web_Server_Configure_file(const char *filename);

int read_clear_control_file(const char *filename);


