#include <stdio.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_mac.h"

#include "app.h"
#include "wifi.h"
#include "mqtt.h"
#include "nvs.h"

SemaphoreHandle_t wifi_connection_semaphore;
SemaphoreHandle_t mqtt_conection_semaphore;

void wifi_conected(void *params){
  while (true){
    if (xSemaphoreTake(wifi_connection_semaphore, portMAX_DELAY)){
      mqtt_start();
    }
  }
}

void initialize_conections(){
  wifi_connection_semaphore = xSemaphoreCreateBinary();
  mqtt_conection_semaphore = xSemaphoreCreateBinary();

  wifi_start();
  xTaskCreate(&wifi_conected, "Conect to MQTT", 4096, NULL, 1, NULL);
}

void app_main(void) {
  // Inicializa o NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  initialize_conections();
  init_app();
}
