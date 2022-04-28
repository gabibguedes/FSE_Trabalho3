#include "app.h"

#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "dht11.h"
#include "wifi.h"
#include "http_client.h"
#include "mqtt.h"

#define TAG "APP"
#define MODE CONFIG_ESP_MODE

#define BUTTON_PIN 0
#define LED_PIN 2
#define SENSOR_PIN 4

SemaphoreHandle_t wifi_connection_semaphore;
SemaphoreHandle_t mqtt_conection_semaphore;

void wifi_conected(void *params){
  while (true){
    if (xSemaphoreTake(wifi_connection_semaphore, portMAX_DELAY)){
      mqtt_start();
    }
  }
}

void send_mqtt_message(char *message){
  if (xSemaphoreTake(mqtt_conection_semaphore, portMAX_DELAY)){
    // TODO: Customize topic
    mqtt_envia_mensagem("fse2021/<matricula>/<cômodo>/temperatura", message);
    xSemaphoreGive(mqtt_conection_semaphore);
  }
}

void read_sensor_input(void *params){
  struct dht11_reading sensor_read;
  char message[50];
  float temp_sum=0, hum_sum=0;
  int count=0;

  while (true) {
    // Read sensor
    sensor_read = DHT11_read();
    temp_sum += sensor_read.temperature;
    hum_sum += sensor_read.humidity;
    count++;

    // Send MQTT message with data avrage in every 10 seconds
    if(count == 5){
      // TODO: send 2 mqtt messages, one for temperature, other for humidity
      sprintf(message, "{ \"temperature\": %f, \"humidity\": %f }", temp_sum/5, hum_sum/5);
      send_mqtt_message(message);

      temp_sum=0;
      hum_sum=0;
      count=0;
    }

    // Sleep for 2s
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void read_button_input(void *params){
  int tmp_btn_level, button_level = 1;
  char message[50];

  while (true){
    tmp_btn_level = gpio_get_level(BUTTON_PIN);
    if(tmp_btn_level != button_level){
      button_level = tmp_btn_level;
      ESP_LOGI(TAG, "BUTTON PRESSED");
      sprintf(message, "{ \"button\": %d }", button_level);
      send_mqtt_message(message);
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void set_button_gpio(){
  esp_rom_gpio_pad_select_gpio(BUTTON_PIN);
  gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
}

void set_all_gpios(){
  set_button_gpio();

  esp_rom_gpio_pad_select_gpio(LED_PIN);
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

  esp_rom_gpio_pad_select_gpio(SENSOR_PIN);
  gpio_set_direction(SENSOR_PIN, GPIO_MODE_INPUT);
  DHT11_init(SENSOR_PIN);
}

void battery_loop(){
  set_button_gpio();
  xTaskCreate(&read_button_input, "Read button input", 4096, NULL, 1, NULL);
}

void energy_loop(){
  set_all_gpios();
  xTaskCreate(&read_sensor_input, "Read DHT11 sensor input", 4096, NULL, 1, NULL);
}

void initialize_conections(){
  wifi_connection_semaphore = xSemaphoreCreateBinary();
  mqtt_conection_semaphore = xSemaphoreCreateBinary();

  wifi_start();
  xTaskCreate(&wifi_conected, "Conect to MQTT", 4096, NULL, 1, NULL);
}

void app_loop(){
  initialize_conections();

  // TODO: Add MQTT ping to know the device is ON or OFF
  // TODO: Implement Led as an exit representing lamps and air conditioning
  // TODO: Use MAC Address as device ID

  switch (MODE) {
  case BATERY:
    ESP_LOGI(TAG, "BATERY MODE SELECTED");
    battery_loop();
    break;

  case ENERGY:
    ESP_LOGI(TAG, "ENERGY MODE SELECTED");
    energy_loop();
    break;

  default:
    ESP_LOGE(TAG, "ESP MODE IS UNDEFINED");
    break;
  }
}