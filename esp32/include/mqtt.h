#ifndef MQTT_H
#define MQTT_H

void mqtt_start();

void send_mqtt_message(char *topico, char *mensagem);
void send_int_message(char *topico, int value);
void send_float_message(char *topico, float value);

#endif
