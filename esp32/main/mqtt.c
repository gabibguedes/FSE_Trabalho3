#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cJSON.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_mac.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "mqtt.h"
#include "nvs.h"
#include "app.h"
#include "gpio.h"

#define TAG "MQTT"

#define MODE CONFIG_ESP_MODE
#define MQTT_URL CONFIG_ESP_MQTT_URL
#define REGISTER CONFIG_ESP_REGISTER

extern SemaphoreHandle_t mqtt_conection_semaphore;
extern char app_room[20];
extern int app_is_registered;

esp_mqtt_client_handle_t client;

void get_mac_addr(char* mac) {
  unsigned char mac_base[6] = {0};
  esp_efuse_mac_get_default(mac_base);
  esp_read_mac(mac_base, ESP_MAC_WIFI_STA);
  sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X\0", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
  ESP_LOGI(TAG,"GET MAC ADDRES: %s", mac);
}

void register_room(cJSON *root){
  cJSON *room_cjson = cJSON_GetObjectItem(root, "room");
  if (room_cjson && room_cjson->valuestring != NULL){
    ESP_LOGI(TAG, "REGISTERED TO ROOM: %s", room_cjson->valuestring);
    strcpy(app_room, room_cjson->valuestring);
    store_room_nvs(app_room);
    app_loop();
    app_is_registered = 1;
  }
}

void handle_led(cJSON *root){
  cJSON *led_cjson = cJSON_GetObjectItem(root, "output");
  if (led_cjson && led_cjson->valueint >=0){
    ESP_LOGI(TAG, "CHANGE OUTPUT LEVEL TO: %d", led_cjson->valueint);
    change_led(led_cjson->valueint);
  }
}

void mqtt_message_received(esp_mqtt_event_handle_t event){
  char data[50];
  sprintf(data, "%.*s\r\0", event->data_len, event->data);
  ESP_LOGI(TAG, "DATA RECEIVED: %s", data);

  cJSON *root = cJSON_Parse(data);
  if(!app_is_registered){
    register_room(root);
  }else{
    handle_led(root);
  }

  cJSON_Delete(root);
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
  esp_mqtt_client_handle_t client = event->client;

  switch (event->event_id)
  {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
    xSemaphoreGive(mqtt_conection_semaphore);
    char mac_addr[20], topic[50];
    get_mac_addr(mac_addr);
    sprintf(topic, "fse2021/%s/dispositivos/%s", REGISTER, mac_addr);
    esp_mqtt_client_subscribe(client, topic, 0);
    break;

  case MQTT_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
    break;

  case MQTT_EVENT_SUBSCRIBED:
    ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
    break;
  case MQTT_EVENT_UNSUBSCRIBED:
    ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
    break;
  case MQTT_EVENT_PUBLISHED:
    ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
    break;
  case MQTT_EVENT_DATA:
    ESP_LOGI(TAG, "MQTT_EVENT_DATA");
    printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    printf("DATA=%.*s\r\n", event->data_len, event->data);
    mqtt_message_received(event);

    break;
  case MQTT_EVENT_ERROR:
    ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
    break;
  default:
    ESP_LOGI(TAG, "Other event id:%d", event->event_id);
    break;
  }
  return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
  ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
  mqtt_event_handler_cb(event_data);
}

void mqtt_start() {
  esp_mqtt_client_config_t mqtt_config = {
      .uri = MQTT_URL,
  };
  client = esp_mqtt_client_init(&mqtt_config);
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
  esp_mqtt_client_start(client);
}

void send_json_mqtt_message(char* topic, char *message){
  char full_topic[50];

  sprintf(full_topic, "fse2021/%s/%s/%s", REGISTER, app_room, topic);
  send_mqtt_message(full_topic, message);
}

void send_int_message(char* topic, int value){
  char message[20];
  sprintf(message, "{ \"value\": %d }", value);
  send_json_mqtt_message(topic, message);
}

void send_float_message(char* topic, float value){
  char message[20];
  sprintf(message, "{ \"value\": %f }", value);
  send_json_mqtt_message(topic, message);
}

void send_mqtt_message(char *topico, char *mensagem){
  if (xSemaphoreTake(mqtt_conection_semaphore, portMAX_DELAY)){
    int message_id = esp_mqtt_client_publish(client, topico, mensagem, 0, 1, 0);
    ESP_LOGI(TAG, "Mesnagem enviada, ID: %d, TOPICO: %s", message_id, topico);
    xSemaphoreGive(mqtt_conection_semaphore);
  }
}

void send_im_alive_signal(){
  char full_topic[50];
  sprintf(full_topic, "fse2021/%s/%s/status", REGISTER, app_room);
  send_mqtt_message(full_topic, "{ \"alive\": true }");
}

void register_to_mqtt(){
  char mac_addr[20], topic[50], message[100];
  get_mac_addr(mac_addr);

  sprintf(topic, "fse2021/%s/dispositivos/%s", REGISTER, mac_addr);
  sprintf(message, "{ \"mode\": %d, \"id\": \"%s\" }", MODE, mac_addr);
  send_mqtt_message(topic, message);
}
