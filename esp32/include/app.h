#ifndef APP_H
#define APP_H

typedef enum app_mode {
  BATERY=1,
  ENERGY=2
} AppMode;

void app_loop();

#endif