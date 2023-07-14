#define ADDRESS     "139.224.42.2"
#define PORT        "1883"
#define CLIENTID    "114515|securemode=3,signmethod=hmacsha1|"   //设备的名称
#define USERNAME    "TERMINAL&h9xwKgTYzwD" //产品ID
#define PASSWORD    "6FD2EF9E4744001219C5532B234C3CC5A867EEEB"
#define SUBTOPIC    "/h9xwKgTYzwD/TERMINAL/user/get"//订阅topic  XXXXXX是产品ID   ??????是设备名称
#define PUBTOPIC    "/h9xwKgTYzwD/TERMINAL/user/update"      //发布topic

#define TOPIC       "/h9xwKgTYzwD/TERMINAL/user/get"  //订阅topic  XXXXXX是产品ID   ??????是设备名称
int open_nb_socket(const char* addr, const char* port);
int example_mqtt(int argc, const char *argv[]);