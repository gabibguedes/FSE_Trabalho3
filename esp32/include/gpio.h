#ifndef GPIO_H
#define GPIO_H

#define BUTTON_PIN 0
#define LED_PIN 2
#define SENSOR_PIN 4

void change_led(int level);
int get_button_level();
void set_button_gpio();
void set_all_gpios();
#endif