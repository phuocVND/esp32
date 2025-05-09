// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_log.h"
// #include "esp_event.h" 
// #include "esp_websocket_client.h"
// #include "websocket_client.h"

// static const char *TAG = "websocket_client";

// static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
//     esp_websocket_client_handle_t client = (esp_websocket_client_handle_t)handler_args;
//     esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;

//     switch (event_id) {
//         case WEBSOCKET_EVENT_CONNECTED:
//             ESP_LOGI(TAG, "WebSocket Connected");
//             // Send a test message after connection
//             esp_websocket_client_send_text(client, "Hello from ESP32!", strlen("Hello from ESP32!"), portMAX_DELAY);
//             break;
//         case WEBSOCKET_EVENT_DISCONNECTED:
//             ESP_LOGI(TAG, "WebSocket Disconnected");
//             break;
//         case WEBSOCKET_EVENT_DATA:
//             if (data->op_code == WS_TRANSPORT_OPCODES_TEXT) {
//                 ESP_LOGI(TAG, "Received text data: %.*s", data->data_len, (char *)data->data_ptr);
//             } else if (data->op_code == WS_TRANSPORT_OPCODES_BINARY) {
//                 ESP_LOGI(TAG, "Received binary data: %d bytes", data->data_len);
//             }
//             break;
//         case WEBSOCKET_EVENT_ERROR:
//             ESP_LOGE(TAG, "WebSocket Error");
//             break;
//         default:
//             ESP_LOGI(TAG, "Unhandled event: %d", event_id);
//             break;
//     }
// }

// void websocket_client_start(void) {
//     // WebSocket configuration
//     esp_websocket_client_config_t ws_cfg = {
//         .uri = "ws://echo.websocket.org", // Replace with your WebSocket server URI
//         .port = 80,
//         // For secure WSS, enable transport and certificates if needed
//         // .transport = WEBSOCKET_TRANSPORT_OVER_SSL,
//         // .cert_pem = (const char *)server_cert_pem_start,
//     };

//     // Initialize WebSocket client
//     esp_websocket_client_handle_t client = esp_websocket_client_init(&ws_cfg);
//     if (client == NULL) {
//         ESP_LOGE(TAG, "Failed to initialize WebSocket client");
//         return;
//     }

//     // Register event handler
//     esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);

//     // Start WebSocket client
//     esp_err_t ret = esp_websocket_client_start(client);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to start WebSocket client: %s", esp_err_to_name(ret));
//         esp_websocket_client_destroy(client);
//         return;
//     }

//     // Keep the task running to maintain the connection
//     while (1) {
//         // Periodically send a message (e.g., every 10 seconds)
//         if (esp_websocket_client_is_connected(client)) {
//             esp_websocket_client_send_text(client, "Ping from ESP32", strlen("Ping from ESP32"), portMAX_DELAY);
//         }
//         vTaskDelay(10000 / portTICK_PERIOD_MS);
//     }

//     // Cleanup (unreachable in this example, but included for completeness)
//     esp_websocket_client_stop(client);
//     esp_websocket_client_destroy(client);
// }