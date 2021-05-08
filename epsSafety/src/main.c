#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "driver/gpio.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "addr_from_stdin.h"

static const char *TAG = "SafetySustem";

//Данные для подключения к сети
#define EXAMPLE_ESP_WIFI_SSID      "Keenetic-5504"
#define EXAMPLE_ESP_WIFI_PASS      "Cn3aaY3b"
#define EXAMPLE_ESP_MAXIMUM_RETRY  20

//Объявляем рабочие порты
gpio_num_t SD1 = GPIO_NUM_15;
gpio_num_t SD2 = GPIO_NUM_2;
gpio_num_t SD3 = GPIO_NUM_0;
gpio_num_t SD4 = GPIO_NUM_4;
gpio_num_t SD5 = GPIO_NUM_13;
gpio_num_t SD6 = GPIO_NUM_12;

//-----------------------WIFI-------------------------------//
/* Группа событий FreeRTOS для сигнализации, когда есть подключение */
static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0 //Успешное подключение
#define WIFI_FAIL_BIT      BIT1  //Ошибка

static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) 
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else 
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
	        .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    //Ожидание установки соединения
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {

        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } 
    else if (bits & WIFI_FAIL_BIT) 
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } 
    else 
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}

//--------------------------------------END WI FI-------------------------------------//

//Данные для подключения к серверу
#define HOST_IP_ADDR "192.168.1.52"

#define PORT 1111
static const int payloadSize = 255;
static const char *payload = "NEW_CLIENT::SafetySystem";
char* res1;

void InitPort() //Инициализация портов
{
    gpio_reset_pin(SD1);
    gpio_reset_pin(SD2);
    gpio_reset_pin(SD3);
    gpio_reset_pin(SD4);
    gpio_reset_pin(SD5);
    gpio_reset_pin(SD6);

    gpio_set_direction(SD1, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(SD2, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(SD3, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(SD4, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(SD5, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(SD6, GPIO_MODE_INPUT_OUTPUT);

    gpio_set_level(SD1, 0);
    gpio_set_level(SD2, 0);
    gpio_set_level(SD3, 0);
    gpio_set_level(SD4, 0);
    gpio_set_level(SD5, 0);
    gpio_set_level(SD6, 0);
}

void app_main(void)
{
    InitPort(); //Инициализация портов
    
    //Подключение к беспроводной сети
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    //Подключение к серверу
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;    

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(host_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    int sock =  socket(addr_family, SOCK_STREAM, ip_protocol); //Создание сокета

    if (sock >= 0) //Если успешно
    {
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT); 
        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6)); //Связывание созданного сокета с сокетом сервера
        if (err != 0) 
        {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        }  
        else //Если успешно, отправляем серверу команду подключения
        {
            ESP_LOGI(TAG, "Successfully connected");
            send(sock, (char*)&payloadSize, sizeof(int), 0);
            send(sock, payload, payloadSize, 0);
        }
    }
    else
    {
         ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    }

    while(1) //В цикле принимаем сообщения от сервера
    {
        int sizeR;
        if (recv(sock, (char*)&sizeR, sizeof(int), 0) > 0)
        {
            char msgR[sizeR];
            msgR[sizeR] = 0;
            if (recv(sock, msgR, sizeR, 0) > 0)
            {
                ESP_LOGI(TAG, "%s", msgR);

                if (strcmp(msgR, "GIVE") == 0) //Если от сервера получена команда на отправку данных
                {
                    char res1[45] = "SAFETY::";
                    char *temp;

                    if (gpio_get_level(SD1) == 1)
                    {
                        temp = "SD1:1;";
                        strcat(res1, temp);
                        //ESP_LOGI(TAG, "%s", "SD1:HIGH");
                    }
                    else
                    {
                        temp = "SD1:0;";
                        strcat(res1, temp);
                        //ESP_LOGI(TAG, "%s", "SD1:LOW");
                    }

                    if (gpio_get_level(SD2) == 1)
                    {
                        temp = "SD2:1;";
                        strcat(res1, temp);
                        //ESP_LOGI(TAG, "%s", "SD2:HIGH");
                    }
                    else
                    {
                        temp = "SD2:0;";
                        strcat(res1, temp);
                        //ESP_LOGI(TAG, "%s", "SD2:LOW");
                    }     

                    if (gpio_get_level(SD3) == 1)
                    {
                        temp = "SD3:1;";
                        strcat(res1, temp);
                        //ESP_LOGI(TAG, "%s", "SD3:HIGH");
                    }
                    else
                    {
                        temp = "SD3:0;";
                        strcat(res1, temp);
                        //ESP_LOGI(TAG, "%s", "SD3:LOW");
                    }

                    if (gpio_get_level(SD4) == 1)
                    {
                        temp = "SD4:1;";
                        strcat(res1, temp);
                        //ESP_LOGI(TAG, "%s", "SD4:HIGH");
                    }
                    else
                    {
                        temp = "SD4:0;";
                        strcat(res1, temp);
                        //ESP_LOGI(TAG, "%s", "SD4:LOW");
                    }

                    if (gpio_get_level(SD5) == 1)
                    {
                        temp = "SD5:1;";
                        strcat(res1, temp);
                        //ESP_LOGI(TAG, "%s", "SD5:HIGH");
                    }
                    else
                    {
                        temp = "SD5:0;";
                        strcat(res1, temp);
                        //ESP_LOGI(TAG, "%s", "SD5:LOW");
                    }

                    if (gpio_get_level(SD6) == 1)
                    {
                        temp = "SD6:1;";
                        strcat(res1, temp);
                        //ESP_LOGI(TAG, "%s", "SD6:HIGH");
                    }
                    else
                    {
                        temp = "SD6:0;";
                        strcat(res1, temp);
                        //SP_LOGI(TAG, "%s", "SD6:LOW");
                    }
                    res1[45] = '\0';  
                    ESP_LOGI(TAG, "%s", res1);  

                    int size = 45;
                    send(sock, (char*)&size, sizeof(int), 0);
                    send(sock, res1, size, 0);
                }
            }
        }
    }    

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    if (sock != -1) 
    {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }
}