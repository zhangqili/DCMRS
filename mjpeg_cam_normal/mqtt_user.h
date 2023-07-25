#define ADDRESS     "139.224.42.2"
#define PORT        "1883"
#define CLIENTID    "114516|securemode=3,signmethod=hmacsha1|"   //设备的名称
#define USERNAME    "VEHICLE&h9xwKgTYzwD" //产品ID
#define PASSWORD    "82CCEE5034AB6AA0D86F09AD83FC1ECDA1CC8981"  //使用onennet的token工具生成密码 工具地址 https://open.iot.10086.cn/doc/v5/fuse/detail/242
#define SUBTOPIC    "$sys/m7UHmcDGzz/TERMINAL/thing/property/post/reply"//订阅topic  XXXXXX是产品ID   ??????是设备名称
#define PUBTOPIC    "$sys/m7UHmcDGzz/TERMINAL/thing/property/post"      //发布topic

#define TOPIC       "/h9xwKgTYzwD/VEHICLE/user/get"  //订阅topic  XXXXXX是产品ID   ??????是设备名称
int open_nb_socket(const char* addr, const char* port);
int example_mqtt(int argc, const char *argv[]);

extern uint8_t carstate;