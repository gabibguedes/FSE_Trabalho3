#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "dht11.h"

#define LED_PIN 2
#define SENSOR_PIN 15

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

  while(true){
    sensor_read = DHT11_read();
    gpio_set_level(LED_PIN, led_mode);
    led_mode = !led_mode;

    printf("Temperature: %d C\n", sensor_read.temperature);
    printf("Humidade: %d %%\n\n", sensor_read.humidity);
    fflush(stdout);

    // Sleep for 1s
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
