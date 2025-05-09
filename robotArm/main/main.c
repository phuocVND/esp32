#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_control.h"
#include "servo_control.h"
#include "wifi_connect.h"
// #include "websocket.h"
// Task điều khiển LED
void led_task(void *arg) {
    while (1) {
        led1_control(true);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        led1_control(false);
        led2_control(true);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        led2_control(false);
        led3_control(true);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        led3_control(false);
    }
}

// Task điều khiển Servo
void servo_task(void *arg) {
    ServoParams *params = (ServoParams *)arg; // Lấy tham số
    uint8_t servo_id = params->servo_id;
    uint32_t *angles = params->angles;
    uint32_t delay_ms = params->delay_ms;

    while (1) {
        for (int i = 0; i < NUM_ANGLES; i++) {
            servo_set_angle(servo_id, angles[i]);
            vTaskDelay(delay_ms / portTICK_PERIOD_MS);
        }
    }
}

// Hàm chính
void app_main(void) {
    // Khởi tạo LED, Servo và Wi-Fi
    init_leds();
    init_servos();
    wifi_init();
    // websocket_init();
    // Tạo task cho LED
    TaskHandle_t led_handle;
    if (xTaskCreate(led_task, "led_task", 2048, NULL, 5, &led_handle) != pdPASS) {
        printf("Failed to create led_task\n");
        return;
    }

    // Tạo task cho 6 servo
    static ServoParams servo_params[NUM_SERVOS] = {
        {0, {0, 90, 180}, 1000},   // Servo 0: 0°, 90°, 180°, 1s
        {1, {45, 90, 135}, 1500},  // Servo 1: 45°, 90°, 135°, 1.5s
        {2, {0, 45, 90}, 800},     // Servo 2: 0°, 45°, 90°, 0.8s
        {3, {90, 135, 180}, 1200}, // Servo 3: 90°, 135°, 180°, 1.2s
        {4, {0, 90, 180}, 1000},   // Servo 4: 0°, 90°, 180°, 1s
        {5, {45, 90, 135}, 1500},  // Servo 5: 45°, 90°, 135°, 1.5s
    };

    TaskHandle_t servo_handles[NUM_SERVOS];
    char task_name[16];
    for (int i = 0; i < NUM_SERVOS; i++) {
        snprintf(task_name, sizeof(task_name), "servo_%d", i);
        if (xTaskCreate(servo_task, task_name, 2048, &servo_params[i], 5, &servo_handles[i]) != pdPASS) {
            printf("Failed to create %s\n", task_name);
            return;
        }
    }
}