#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "esp_mqtt_client.h"

// Callback để xử lý dữ liệu MQTT nhận được
typedef void (*mqtt_data_callback_t)(const char *topic, const char *data);

// Hàm khởi tạo và chạy MQTT client
void mqtt_client_init(const char *broker_url, mqtt_data_callback_t callback);

// Hàm subscribe vào một topic
void mqtt_client_subscribe(const char *topic, int qos);

#endif // MQTT_CLIENT_H