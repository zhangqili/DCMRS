#include"stdint.h"

extern uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
extern uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */
extern uint8_t message[2048];

#define ADDRESS     "139.224.42.2"
#define PORT        "1883"
#define CLIENTID    "114514|securemode=3,signmethod=hmacsha1|"   //设备的名称
#define USERNAME    "SENSOR&h9xwKgTYzwD" //产品ID
#define PASSWORD    "A916123B955055DEB08645687F9FA6AD986A467B"  //使用onennet的token工具生成密码 工具地址 https://open.iot.10086.cn/doc/v5/fuse/detail/242
#define SUBTOPIC    "/sys/h9xwKgTYzwD/SENSOR/thing/event/property/post/reply"//订阅topic  XXXXXX是产品ID   ??????是设备名称
#define PUBTOPIC    "/sys/h9xwKgTYzwD/SENSOR/thing/event/property/post"      //发布topic

int open_nb_socket(const char* addr, const char* port);
int example_mqtt(int argc, const char *argv[]);