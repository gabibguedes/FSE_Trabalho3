#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

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

#define TAG "MQTT"
#define MQTT_URL CONFIG_ESP_MQTT_URL

extern SemaphoreHandle_t mqtt_conection_semaphore;
esp_mqtt_client_handle_t client;

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
  esp_mqtt_client_handle_t client = event->client;
  int msg_id;

  switch (event->event_id)
  {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
    xSemaphoreGive(mqtt_conection_semaphore);
    // TODO: Subscribe correctly fse2021/<matricula>/dispositivos/<ID_do_dispositivo>
    // TODO: wait for return message informing the room name, fse2021/<matricula>/<cômodo>
    // TODO: This step must persist in memory (NVS)
    msg_id = esp_mqtt_client_subscribe(client, "fse2021/<matricula>/dispositivos/<ID_do_dispositivo>", 0);

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
  sprintf(full_topic, "fse2021/<matricula>/<cômodo>/%s", topic);
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
