
/**
 * @file
 * A simple program that subscribes to a topic.
 */
#include "main.h"

#include "FreeRTOS_POSIX.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <lwip/errno.h>
#include <netdb.h>

#include "utils_getopt.h"
#include "bflb_adc.h"

#include "mqtt.h"
#include "shell.h"

#include "cJSON.h"
#include "mqtt_user.h"
uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */
uint8_t message[2048];

shell_sig_func_ptr abort_exec;
static TaskHandle_t client_daemon;
int test_sockfd;
const char *addr;
uint8_t carstate_send_flag;
/*
    A template for opening a non-blocking POSIX socket.
*/
int open_nb_socket(const char *addr, const char *port);

int open_nb_socket(const char *addr, const char *port)
{
    struct addrinfo hints = { 0 };

    hints.ai_family = AF_UNSPEC;     /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Must be TCP */
    int sockfd = -1;
    int rv;
    struct addrinfo *p, *servinfo;

    /* get address information */
    rv = getaddrinfo(addr, port, &hints, &servinfo);
    if (rv != 0) {
        printf("Failed to open socket (getaddrinfo): %s\r\n", rv);
        return -1;
    }

    /* open the first possible socket */
    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
            continue;

        /* connect to server */
        rv = connect(sockfd, p->ai_addr, p->ai_addrlen);
        if (rv == -1) {
            close(sockfd);
            sockfd = -1;
            continue;
        }
        break;
    }

    /* free servinfo */
    freeaddrinfo(servinfo);

    /* make non-blocking */
    if (sockfd != -1) {
        int iMode = 1;
        ioctlsocket(sockfd, FIONBIO, &iMode);
    }

    return sockfd;
}

/**
 * @brief The function will be called whenever a PUBLISH message is received.
 */
static void publish_callback_1(void **unused, struct mqtt_response_publish *published);

/**
 * @brief The client's refresher. This function triggers back-end routines to
 *        handle ingress/egress traffic to the broker.
 *
 * @note All this function needs to do is call \ref __mqtt_recv and
 *       \ref __mqtt_send every so often. I've picked 100 ms meaning that
 *       client ingress/egress traffic will be handled every 100 ms.
 */
static void client_refresher(void *client);

/**
 * @brief Safelty closes the \p sockfd and cancels the \p client_daemon before \c exit.
 */
static void test_close(int sig)
{
    if (test_sockfd) {
        close(test_sockfd);
    }
    printf("mqtt_sub disconnecting from %s\r\n", addr);

    abort_exec(sig);

    vTaskDelete(client_daemon);
}

int example_mqtt(int argc, const char *argv[])
{
    const char *port;
    const char *topic;
    const char *username;
    const char *password;

    int ret = 0;

    float temp = 0.0;
    float average_filter = 0.0;

    abort_exec = shell_signal(1, test_close);

    /* get address (argv[1] if present) */
    if (argc > 1) {
        addr = argv[1];
    } else {
        addr = ADDRESS;
    }

    /* get port number (argv[2] if present) */
    if (argc > 2) {
        port = argv[2];
    } else {
        port = PORT;
    }

    /* get the topic name to publish */
    if (argc > 3) {
        topic = argv[3];
    } else {
        topic = SUBTOPIC;
    }

    /* get the user name to publish */
    if (argc > 4) {
        username = argv[4];
    } else {
        username = USERNAME;
    }

    /* get the password to publish */
    if (argc > 5) {
        password = argv[5];
    } else {
        password = PASSWORD;
    }

    /* open the non-blocking TCP socket (connecting to the broker) */
    test_sockfd = open_nb_socket(addr, port);

    if (test_sockfd < 0) {
        printf("Failed to open socket: %d\r\n", test_sockfd);
        test_close(SHELL_SIGINT);
    }

    /* setup a client */
    struct mqtt_client client;

    mqtt_init(&client, test_sockfd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback_1);
    /* Create an anonymous session */
    const char *client_id = CLIENTID;
    /* Ensure we have a clean session */
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    /* Send connection request to the broker. */
    ret = mqtt_connect(&client, client_id, NULL, NULL, 0, username, password, connect_flags, 400);

    if (ret != MQTT_OK) {
        printf("fail \r\n");
    }
    /* check that we don't have any errors */
    if (client.error != MQTT_OK) {
        printf("error: %s\r\n", mqtt_error_str(client.error));
        test_close(SHELL_SIGINT);
    }

    /* start a thread to refresh the client (handle egress and ingree client traffic) */
    xTaskCreate(client_refresher, (char *)"client_ref", 1024, &client, 10, &client_daemon);

    /* subscribe */
    topic = SUBTOPIC;
    mqtt_subscribe(&client, topic, 0);

    /* start publishing the time */
    printf("%s listening for '%s' messages.\r\n", argv[0], topic);
    printf("Press CTRL-C to exit.\r\n");
    char adc_str[20];
    /* block wait CTRL-C exit */
    while (1) {
        /*
        //准备数据
        //temp
        for (int i = 0; i < 50; i++) {
            average_filter += bflb_adc_tsen_get_temp(adc);
            vTaskDelay(10);
        }
        temp=average_filter/50.0;
        average_filter = 0.0;
        */
        /* publisher*/
        if(carstate_send_flag)
        {
            carstate_send_flag=0;
        topic = PUBTOPIC;
        memset(message, 0, sizeof(message));
        sprintf(message, "{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"VehicleState\":{\"value\":%d},\"TargetDevice\":{\"value\":\"VEHICLE\"}},\"method\":\"thing.event.property.post\"}",
                carstate);
        printf("%s\n", message);

        ret = mqtt_publish(&client, topic,
                           message, strlen(message) + 1,
                           MQTT_PUBLISH_QOS_0);
        if (ret != MQTT_OK) {
            printf("ERROR! mqtt_publish() %s\r\n", mqtt_error_str(client.error));
        }
        }
        vTaskDelay(1000);
    }

    /* disconnect */
    /* exit */
    test_close(SHELL_SIGINT);
}

static void publish_callback_1(void **unused, struct mqtt_response_publish *published)
{
    /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
    char *topic_name = (char *)malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    char *topic_msg = (char *)malloc(published->application_message_size + 1);
    memcpy(topic_msg, published->application_message, published->application_message_size);
    topic_msg[published->application_message_size] = '\0';

    printf("Received publish('%s'): %s\r\n", topic_name, topic_msg);

    cJSON *json;
    json = cJSON_Parse(topic_msg);
    cJSON *subobject;
    if (json != NULL) {
        cJSON_ArrayForEach(subobject, json)
        {
            printf("Key: %s\n", subobject->string);
            if (!strcmp(subobject->string, "EnvironmentHumidity")) {
                humidity = subobject->valuedouble;
                lefl_loop_array_push_back(&humi_history, subobject->valuedouble);
            }
            if (!strcmp(subobject->string, "EnvironmentTemperature")) {
                temperature = subobject->valuedouble;
                lefl_loop_array_push_back(&temp_history, subobject->valuedouble);
            }
            if (!strcmp(subobject->string, "CO2Content")) {
                co2content = subobject->valuedouble;
                lefl_loop_array_push_back(&co2_history, subobject->valuedouble);
            }
            if (!strcmp(subobject->string, "LightLux")) {
                lightlux = subobject->valuedouble;
                lefl_loop_array_push_back(&light_history, subobject->valuedouble);
            }
        }
    }
    cJSON_Delete(json);
    free(topic_name);
    free(topic_msg);
}

static void client_refresher(void *client)
{
    while (1) {
        mqtt_sync((struct mqtt_client *)client);
        vTaskDelay(100);
    }
}

#ifdef CONFIG_SHELL
#include <shell.h>

extern uint32_t wifi_state;
static int check_wifi_state(void)
{
    if (wifi_state == 1) {
        return 0;
    } else {
        return 1;
    }
}

int cmd_mqtt_publish(int argc, const char **argv)
{
    uint32_t ret = 0;

    ret = check_wifi_state();
    if (ret != 0) {
        printf("your wifi not connected!\r\n");
        return 0;
    }

    // xTaskCreate(example_mqtt,(char*)"test_mqtt", 8192, argv, 10, NULL);
    example_mqtt(argc, argv);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_mqtt_publish, mqtt_pub, mqtt publish);
#endif
