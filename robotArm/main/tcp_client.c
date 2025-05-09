// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_log.h"
// #include <sys/socket.h>
// #include <netdb.h>
// #include "tcp_client.h"

// static const char *TAG = "tcp_client";

// // Task TCP client
// static void tcp_client_task(void *pvParameters) {
//     char rx_buffer[128] = {0};
//     esp_netif_t *netif = (esp_netif_t *)pvParameters;
//     const char *netif_name = esp_netif_get_desc(netif);

//     // Định nghĩa hostname và port
//     const char *host_name = "example.com"; // Thay bằng hostname của bạn
//     const int host_port = 80;              // Thay bằng port của bạn
//     const char *payload = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";

//     // DNS lookup
//     struct addrinfo hints = {
//         .ai_family = AF_INET,
//         .ai_socktype = SOCK_STREAM,
//     };
//     struct addrinfo *res;
//     int err = getaddrinfo(host_name, NULL, &hints, &res);
//     if (err != 0 || res == NULL) {
//         ESP_LOGE(TAG, "DNS lookup failed for %s: err=%d res=%p", host_name, err, res);
//         if (res) freeaddrinfo(res);
//         vTaskDelete(NULL);
//         return;
//     }

//     struct sockaddr_in dest_addr;
//     memcpy(&dest_addr.sin_addr, &((struct sockaddr_in *)(res->ai_addr))->sin_addr, sizeof(dest_addr.sin_addr));
//     dest_addr.sin_family = AF_INET;
//     dest_addr.sin_port = htons(host_port);
//     freeaddrinfo(res);

//     // Tạo socket
//     int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
//     if (sock < 0) {
//         ESP_LOGE(TAG, "\"%s\" Unable to create socket: errno %d", netif_name, errno);
//         goto tcp_client_fail;
//     }
//     ESP_LOGI(TAG, "\"%s\" Socket created", netif_name);

//     // Kết nối tới host
//     int ret = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
//     if (ret != 0) {
//         ESP_LOGE(TAG, "\"%s\" Socket unable to connect: errno %d", netif_name, errno);
//         goto tcp_client_fail;
//     }
//     ESP_LOGI(TAG, "\"%s\" Successfully connected", netif_name);

//     // Gửi yêu cầu HTTP GET
//     ret = send(sock, payload, strlen(payload), 0);
//     if (ret < 0) {
//         ESP_LOGE(TAG, "\"%s\" Error occurred during sending: errno %d", netif_name, errno);
//         goto tcp_client_fail;
//     }

//     // Nhận phản hồi
//     ret = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
//     if (ret < 0) {
//         ESP_LOGE(TAG, "\"%s\" Error occurred during receiving: errno %d", netif_name, errno);
//     } else if (ret > 0) {
//         rx_buffer[ret] = 0; // Null-terminate dữ liệu nhận được
//         ESP_LOGI(TAG, "\"%s\" Received %d bytes", netif_name, ret);
//         ESP_LOGI(TAG, "%s", rx_buffer);
//     } else {
//         ESP_LOGE(TAG, "\"%s\" Closed connection during receiving", netif_name);
//     }

// tcp_client_fail:
//     close(sock);
//     vTaskDelete(NULL);
// }

// // Hàm khởi động TCP client
// void tcp_client_start(esp_netif_t *netif) {
//     xTaskCreate(tcp_client_task, "tcp_client_task", 4096, netif, 5, NULL);
// }