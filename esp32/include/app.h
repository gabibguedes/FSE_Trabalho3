#ifndef APP_H
#define APP_H

typedef enum app_mode {
  BATERY=1,
  ENERGY=2
} AppMode;

char app_room[20];
int app_is_registered;

void app_loop();
void init_app();

#endif