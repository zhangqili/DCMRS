#define ADDRESS     "183.230.40.96"
#define PORT        "1883"
#define CLIENTID    "TERMINAL"   //设备的名称
#define USERNAME    "m7UHmcDGzz" //产品ID
#define PASSWORD    "version=2018-10-31&res=products%2Fm7UHmcDGzz%2Fdevices%2FTERMINAL&et=1786363584&method=sha1&sign=VobwcY%2FVvZRqmbiYtmZoCvyEDGg%3D"  //使用onennet的token工具生成密码 工具地址 https://open.iot.10086.cn/doc/v5/fuse/detail/242
#define SUBTOPIC    "$sys/m7UHmcDGzz/TERMINAL/thing/property/post/reply"//订阅topic  XXXXXX是产品ID   ??????是设备名称
#define PUBTOPIC    "$sys/m7UHmcDGzz/TERMINAL/thing/property/post"      //发布topic

#define TOPIC       "$sys/m7UHmcDGzz/TERMINAL/thing/property/set"  //订阅topic  XXXXXX是产品ID   ??????是设备名称
int open_nb_socket(const char* addr, const char* port);
int example_mqtt(int argc, const char *argv[]);