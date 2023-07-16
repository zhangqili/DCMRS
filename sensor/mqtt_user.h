#include"stdint.h"

extern uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
extern uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */
extern uint8_t message[2048];

#define ADDRESS     "183.230.40.96"
#define PORT        "1883"
#define CLIENTID    "SENSOR"   //设备的名称
#define USERNAME    "m7UHmcDGzz" //产品ID
#define PASSWORD    "version=2018-10-31&res=products%2Fm7UHmcDGzz%2Fdevices%2FSENSOR&et=1786363584&method=sha1&sign=dhcayat4Ssfo0BWmzhnElHMnq8E%3D"  //使用onennet的token工具生成密码 工具地址 https://open.iot.10086.cn/doc/v5/fuse/detail/242
#define SUBTOPIC    "$sys/m7UHmcDGzz/SENSOR/thing/property/post/reply"//订阅topic  XXXXXX是产品ID   ??????是设备名称
#define PUBTOPIC    "$sys/m7UHmcDGzz/SENSOR/thing/property/post"      //发布topic

int open_nb_socket(const char* addr, const char* port);
int example_mqtt(int argc, const char *argv[]);