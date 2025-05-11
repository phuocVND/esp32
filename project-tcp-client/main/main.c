#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_connect.h"
#include "tcp_client.h"

void app_main(void) {
    // Khởi tạo và kết nối Wi-Fi
    if (wifi_connect_init() != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to initialize Wi-Fi");
        return;
    }

    if (wifi_wait_for_connection() != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to connect to Wi-Fi");
        return;
    }

    // Khởi tạo TCP client
    if (tcp_client_init() != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to initialize TCP client");
        return;
    }

    // Gửi dữ liệu
    const char *message = "Hello, TCP Server!";
    if (tcp_client_send(message) != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to send data");
    }

    // Nhận dữ liệu
    char rx_buffer[128];
    if (tcp_client_receive(rx_buffer, sizeof(rx_buffer)) != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to receive data");
    } else {
        ESP_LOGI("MAIN", "Server response: %s", rx_buffer);
    }

    // // Đóng kết nối TCP
    // tcp_client_close();

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Giữ chương trình chạy
    }
}