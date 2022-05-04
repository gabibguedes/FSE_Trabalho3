#include "nvs.h"

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#define TAG "NVS"
#define MAX_NVS_STR_LEN 20
#define NVS_NAME "local_storage"
#define NVS_KEY "room"

char *read_nvs_room(){
  nvs_handle_t nvs_handle;
  esp_err_t ret_val;
  char *saved_data = malloc(MAX_NVS_STR_LEN);
  size_t len;

  ret_val = nvs_open(NVS_NAME, NVS_READWRITE, &nvs_handle);
  if (ret_val != ESP_OK) {
    ESP_LOGE("NVS", "Error (%s) opening NVS handle for Write", esp_err_to_name(ret_val));
  } else {
    printf("opening NVS Read handle Done \r\n");
    ret_val = nvs_get_str(nvs_handle,NVS_KEY, saved_data, &len);

    if (ret_val == ESP_OK){
      ESP_LOGW("NVS", "*****(%s) Can read/get value: %s", esp_err_to_name(ret_val), saved_data);
    }else{
      ESP_LOGE("NVS", "Error (%s) Can not read/get value: %s", esp_err_to_name(ret_val), saved_data);
      return "";
    }

    ret_val = nvs_commit(nvs_handle);
    if (ret_val != ESP_OK) {
      ESP_LOGE("NVS", "Error (%s) Can not commit - read", esp_err_to_name(ret_val));
    } else {
      ESP_LOGI("NVS", "Read Commit Done!");
    }
  }

  nvs_close(nvs_handle);

  return saved_data;
}

void store_room_nvs(char *stringVal) {
  nvs_handle_t nvs_handle;
  esp_err_t ret_val;

  ret_val = nvs_open(NVS_NAME, NVS_READWRITE, &nvs_handle);
  if (ret_val != ESP_OK) {
    ESP_LOGE("NVS", "Error (%s) opening NVS handle for Write", esp_err_to_name(ret_val));
  } else {
    printf("opening NVS Write handle Done \r\n");
    ret_val = nvs_set_str(nvs_handle, NVS_KEY, stringVal);
    if (ret_val != ESP_OK) {
      ESP_LOGE("NVS", "Error (%s) Can not write/set value: %s", esp_err_to_name(ret_val), stringVal);
    }

    ret_val = nvs_commit(nvs_handle);
    if (ret_val != ESP_OK) {
      ESP_LOGE("NVS", "Error (%s) Can not commit - write", esp_err_to_name(ret_val));
    } else {
      ESP_LOGI("NVS", "Write Commit Done!");
    }
  }

  nvs_close(nvs_handle);
}
