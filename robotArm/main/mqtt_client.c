#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt_client.h"

static const char *TAG = "mqtt_client";
static mqtt_data_callback_t data_callback = NULL;
static esp_mqtt_client_handle_t mqtt_client = NULL;

/*
 * @brief Event handler for MQTT events
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        // Subscribe vào các topic mặc định
        msg_id = esp_mqtt_client_subscribe(client, "/led/control", 0);
        ESP_LOGI(TAG, "Subscribed to /led/control, msg_id=%d", msg_id);
        msg_id = esp_mqtt_client_subscribe(client, "/servo/control", 0);
        ESP_LOGI(TAG, "Subscribed to /servo/control, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);

        // Gọi callback để xử lý dữ liệu
        if (data_callback) {
            char topic[32];
            char data[32];
            snprintf(topic, event->topic_len + 1, "%.*s", event->topic_len, event->topic);
            snprintf(data, event->data_len + 1, "%.*s", event->data_len, event->data);
            data_callback(topic, data);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGE(TAG, "TCP transport error: esp-tls=%d, tls-stack=%d, socket=%d",
                     event->error_handle->esp_tls_last_esp_err,
                     event->error_handle->esp_tls_stack_err,
                     event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

/*
 * @brief MQTT task
 */
static void mqtt_task(void *arg)
{
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)arg;
    esp_mqtt_client_start(client);
    vTaskDelay(portMAX_DELAY); // Giữ task chạy
}

/*
 * @brief Initialize MQTT client
 */
void mqtt_client_init(const char *broker_url, mqtt_data_callback_t callback)
{
    data_callback = callback;

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = broker_url,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    // Tạo task cho MQTT
    xTaskCreate(mqtt_task, "mqtt_task", 4096, mqtt_client, 5, NULL);
}

/*
 * @brief Subscribe to a topic
 */
void mqtt_client_subscribe(const char *topic, int qos)
{
    if (mqtt_client) {
        esp_mqtt_client_subscribe(mqtt_client, topic, qos);
        ESP_LOGI(TAG, "Subscribed to %s, QoS=%d", topic, qos);
    } else {
        ESP_LOGE(TAG, "MQTT client not initialized");
    }
}