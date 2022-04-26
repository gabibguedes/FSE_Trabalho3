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

#include "freertos/semphr.h"

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

#define LED_PIN 2
#define SENSOR_PIN 4

void conectadoWifi(void *params)
{
  while (true)
  {
    if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      // Processamento Internet
      mqtt_start();
    }
  }
}

void trataComunicacaoComServidor(void *params)
{
  char mensagem[50];
  if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {
    while (true)
    {
      float temperatura = 20.0 + (float)rand() / (float)(RAND_MAX / 10.0);
      sprintf(mensagem, "temperatura1: %f", temperatura);
      mqtt_envia_mensagem("sensores/temperatura", mensagem);
      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }
}

void app_main(void) {
  int led_mode = 0;
  struct dht11_reading sensor_read;

  printf("Measuring Temperature and humidity:\n");
  fflush(stdout);

  esp_rom_gpio_pad_select_gpio(LED_PIN);
  esp_rom_gpio_pad_select_gpio(SENSOR_PIN);

  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(SENSOR_PIN, GPIO_MODE_INPUT);

  DHT11_init(SENSOR_PIN);

  // Inicializa o NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  conexaoWifiSemaphore = xSemaphoreCreateBinary();
  conexaoMQTTSemaphore = xSemaphoreCreateBinary();
  wifi_start();

  xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
  xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);


  // while(true){
  //   // Blink LED
  //   gpio_set_level(LED_PIN, led_mode);
  //   led_mode = !led_mode;

  //   // Read sensor
  //   sensor_read = DHT11_read();
  //   printf("Temperature: %d C\n", sensor_read.temperature);
  //   printf("Humidade: %d %%\n\n", sensor_read.humidity);
  //   fflush(stdout);

  //   // Sleep for 1s
  //   vTaskDelay(1000 / portTICK_PERIOD_MS);
  // }
}
