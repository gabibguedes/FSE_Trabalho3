#include "gpio.h"

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "dht11.h"

void change_led(int level){
  gpio_set_level(LED_PIN, level);
}

int get_button_level(){
  // TODO: Make Led dimerable
  return gpio_get_level(BUTTON_PIN);
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