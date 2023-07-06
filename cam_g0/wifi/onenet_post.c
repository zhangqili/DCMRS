
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <lwip/api.h>
#include <lwip/arch.h>
#include <lwip/opt.h>
#include <lwip/inet.h>
#include <lwip/errno.h>
#include <netdb.h>

#include "bflb_adc.h"
#include "shell.h"
#include "utils_getopt.h"
#include "bflb_mtimer.h"

#define HOST_NAME "183.230.40.33"
#define ONENET_HTTP_POST_MAX_LEN 1024
#define ONENET_HTTP_POST_CONTENT_MAX_LEN 1024
#define DEV_ID "XXXXXXXX"  //设备ID  更换为自己的设备id后再编译
#define API_KEY "XXXXXXXXXXXXXXXXX"  //API-KEY  更换为自己的API-KEY后再编译


// clang-format off
// static const uint8_t get_buf[] = "GET / HTTP/1.1 \r\nHost: www.gov.cn\r\n\r\n";
uint32_t recv_buf[4 * 1024] = { 0 };
// clang-format on

shell_sig_func_ptr abort_exec;
uint64_t total_cnt = 0;
int sock_client = -1;
struct bflb_device_s *adc;

static void test_close(int sig)
{
    if (sock_client) {
        closesocket(sock_client);
    }
    abort_exec(sig);
    if (total_cnt > 0) {
        printf("Total send data=%lld\r\n", total_cnt);
    }
}

#define PING_USAGE                                \
    "wifi_http_test [hostname] [port]\r\n"        \
    "\t hostname: hostname or dest server ip\r\n" \
    "\t port: dest server listen port, defualt port:80\r\n"

static void wifi_test_http_client_init(int argc, char **argv)
{
    float temp = 0.0;
    float average_filter = 0.0;

    //adc temp
    adc = bflb_device_get_by_name("adc");

    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_2P0V;

    struct bflb_adc_channel_s chan;

    chan.pos_chan = ADC_CHANNEL_TSEN_P;
    chan.neg_chan = ADC_CHANNEL_GND;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, &chan, 1);
    bflb_adc_tsen_init(adc, ADC_TSEN_MOD_INTERNAL_DIODE);

    abort_exec = shell_signal(SHELL_SIGINT, test_close);
    printf("Http client task start ...\r\n");

    char *host_name;
    char *addr;
    char *port;
    struct sockaddr_in remote_addr;

    // if (argc < 2) {
    //     printf("%s", PING_USAGE);
    //     return;
    // }
    

    if (argc > 1) {
    /* get address (argv[1] if present) */
    host_name = argv[1];
#ifdef LWIP_DNS
    ip4_addr_t dns_ip;
    netconn_gethostbyname(host_name, &dns_ip);
    addr = ip_ntoa(&dns_ip);
#endif
    }
    else {
        addr = HOST_NAME;
    }
    /* get port number (argv[2] if present) */
    if (argc > 2) {
        port = argv[2];
    } else {
        port = "80";
    }

    while (1) {
        if ((sock_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("Http Client create socket error\r\n");
            return;
        }
        //准备数据
        //temp
        for (int i = 0; i < 50; i++) {
            average_filter += bflb_adc_tsen_get_temp(adc);
            vTaskDelay(10);
        }
        temp=average_filter/50.0;
        average_filter = 0.0;

        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = htons(atoi(port));
        remote_addr.sin_addr.s_addr = inet_addr(addr);
        memset(&(remote_addr.sin_zero), 0, sizeof(remote_addr.sin_zero));

        printf("Host:%s, Server ip Address : %s:%s\r\n", HOST_NAME, addr, port);

        if (connect(sock_client, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) != 0) {
            printf("Http client connect server falied!\r\n");
            closesocket(sock_client);
            return;
        }

        printf("Http client connect server success!\r\n");
        printf("Press CTRL-C to exit.\r\n");
        memset(recv_buf, 0, sizeof(recv_buf));
        total_cnt = 0;
        //创建post数据
        char post_buf[ONENET_HTTP_POST_MAX_LEN];
        char post_content[ONENET_HTTP_POST_CONTENT_MAX_LEN];
        char post_content_len[4];
        
        memset(post_content, 0, sizeof(post_content));
        memset(post_buf, 0, sizeof(post_buf));

        sprintf(post_content,"{\"datastreams\":["
                                                "{\"id\":\"temp\",\"datapoints\":[{\"value\":%.2f}]},"
                                                "]}",temp);
        sprintf(post_content_len,"%d",strlen(post_content));
            
        strcat(post_buf, "POST /devices/");
        strcat(post_buf, DEV_ID);
        strcat(post_buf, "/datapoints HTTP/1.1\r\n");
        strcat(post_buf, "api-key:");
        strcat(post_buf, API_KEY);
        strcat(post_buf, "\r\n");
        strcat(post_buf, "Host:api.heclouds.com\r\n");
        strcat(post_buf, "Content-Length:");
        strcat(post_buf, post_content_len);
        strcat(post_buf, "\r\n\r\n");
        strcat(post_buf, post_content);
        strcat(post_buf, "\r\n\r\n");
        printf("%s\r\n", post_buf);
        write(sock_client, post_buf, sizeof(post_buf));
        while (1) {
            total_cnt = recv(sock_client, (uint8_t *)recv_buf, sizeof(recv_buf), 0);
            if (total_cnt <= 0)
                break;
            printf("%s\r\n", (uint8_t *)recv_buf);
            vTaskDelay(5000);
        }
        closesocket(sock_client);
        // return;
    }
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_wifi_http_client(int argc, char **argv)
{
    wifi_test_http_client_init(argc, argv);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_http_client, onenet_post, wifi http client test);
#endif
