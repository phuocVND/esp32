#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "wifi_connect.h"
#include "tcp_client.h"
#include "led_control.h"
#include "servo_control.h"

// Định nghĩa số servo
#define NUM_SERVOS 6

// Biến toàn cục để lưu trữ góc cho các servo
static short servo_angles[NUM_SERVOS] = {0};
// Semaphore để đồng bộ hóa truy cập servo_angles
static SemaphoreHandle_t angles_mutex;

// Task điều khiển LED
void led_task(void *arg) {
    while (1) {
        led1_control(true);
        // vTaskDelay(500 / portTICK_PERIOD_MS);
        led1_control(false);
        led2_control(true);
        // vTaskDelay(500 / portTICK_PERIOD_MS);
        led2_control(false);
        led3_control(true);
        // vTaskDelay(500 / portTICK_PERIOD_MS);
        led3_control(false);
        // vTaskDelay(pdMS_TO_TICKS(10));
    }
}
typedef struct {
    short servo_index;
    short servo_angle;
} servo_param_t;
// Task điều khiển Servo
void control_servo(void *arg) {
    servo_param_t *param = (servo_param_t *)arg;
    while (1) {
            servo_set_angle(param->servo_index, param->servo_angle); 
            vTaskDelay(pdMS_TO_TICKS(20));
        }
}

// Task điều khiển Servo
void servo_task(void *arg) {
    static servo_param_t params[NUM_SERVOS];
    short *angles = (short *)arg;
    
    for (int i = 0; i < NUM_SERVOS; i++) {
        params[i].servo_index = i;
        params[i].servo_angle = angles[i];
        xTaskCreate(control_servo, "control_servo", 2048, &params[i], 8, NULL);
    }
    vTaskDelete(NULL); // xóa servo_task khi đã hoàn thành nhiệm vụ
}

// Task xử lý TCP
void tcp_task(void *param) {
    short *shared_array = (short *)param;
    while (1) {
        if (xSemaphoreTake(angles_mutex, portMAX_DELAY) == pdTRUE) {
            if (tcp_client_receive_ints(shared_array) != ESP_OK) {
                ESP_LOGW("TCPGiovan", "Failed to receive int array");
                xSemaphoreGive(angles_mutex);
                break; // Thoát nếu lỗi nhận dữ liệu
            }
            ESP_LOGI("TCP_TASK", "Received int array");
            xSemaphoreGive(angles_mutex);
        }
        // vTaskDelay(pdMS_TO_TICKS(500)); // Đợi 500ms
    }
    tcp_client_close();
    vTaskDelete(NULL);
}

void app_main(void) {
    // Khởi tạo LED và servo
    init_leds();
    init_servos();

    // Khởi tạo Wi-Fi
    if (wifi_connect_init() != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to initialize Wi-Fi");
        return;
    }

    // Thử lại kết nối Wi-Fi
    int retry_count = 0;
    const int max_retries = 5;
    while (wifi_wait_for_connection() != ESP_OK && retry_count < max_retries) {
        ESP_LOGW("MAIN", "Failed to connect to Wi-Fi, retrying (%d/%d)...", retry_count + 1, max_retries);
        retry_count++;
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    if (retry_count >= max_retries) {
        ESP_LOGE("MAIN", "Failed to connect to Wi-Fi after %d attempts", max_retries);
        return;
    }
    ESP_LOGI("MAIN", "Wi-Fi connected successfully");

    // Khởi tạo TCP client
    if (tcp_client_init() != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to initialize TCP client");
        return;
    }

    const char *message = "Hello, TCP Server!";
    if (tcp_client_send(message) != ESP_OK) {
        ESP_LOGE("TCP_TASK", "Failed to send data");
    }

    char rx_buffer[128];
    if (tcp_client_receive(rx_buffer, sizeof(rx_buffer)) != ESP_OK) {
        ESP_LOGE("TCP_TASK", "Failed to receive string data");
    } else {
        ESP_LOGI("TCP_TASK", "Server response: %s", rx_buffer);
    }
    
    // Tạo semaphore
    angles_mutex = xSemaphoreCreateMutex();
    if (angles_mutex == NULL) {
        ESP_LOGE("MAIN", "Failed to create mutex");
        return;
    }

    // // Tạo task LED
    // TaskHandle_t led_handle;
    // if (xTaskCreate(led_task, "led_task", 4096, NULL, 5, &led_handle) != pdPASS) {
    //     ESP_LOGE("MAIN", "Failed to create led_task");
    //     return;
    // }
    TaskHandle_t tcp_handle;

    // Tạo task TCP
    if (xTaskCreate(tcp_task, "tcp_task", 4096, servo_angles, 5, &tcp_handle) != pdPASS) {
        ESP_LOGE("MAIN", "Failed to create tcp_task");
        return;
    }

    // Tạo task Servo
    TaskHandle_t servo_handle;
    if (xTaskCreate(servo_task, "control_servo", 4096, servo_angles, 5, &servo_handle) != pdPASS) {
        ESP_LOGE("MAIN", "Failed to create servo_task");
        return;
    }


    xTaskCreate(servo_task, "control_servo", 4096, servo_angles, 5, &servo_handle);
    xTaskCreate(servo_task, "control_servo", 4096, servo_angles, 5, &servo_handle);
    xTaskCreate(servo_task, "control_servo", 4096, servo_angles, 5, &servo_handle);
    xTaskCreate(servo_task, "control_servo", 4096, servo_angles, 5, &servo_handle);
    xTaskCreate(servo_task, "control_servo", 4096, servo_angles, 5, &servo_handle);
}
