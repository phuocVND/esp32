#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include "esp_log.h"
#include "esp_err.h"
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>
// Cấu hình cho TCP client
#define TCP_SERVER_IP   "192.168.1.88" // Địa chỉ IP của server
#define TCP_SERVER_PORT 8080           // Cổng server
#define TCP_TAG         "TCP_CLIENT"

typedef struct {
    int id;
    float value;
    char name[32];
} MyData;


void send_array(int client_socket, float* arr, size_t size);
bool recv_array(int client_socket, int* arr, size_t size);
void send_struct(int client_socket, const MyData* data);
bool recv_struct(int client_socket, MyData* data);

// Hàm khởi tạo và kết nối TCP client
esp_err_t tcp_client_init(void);

// Hàm gửi dữ liệu qua TCP
esp_err_t tcp_client_send(const char *data);

// Hàm nhận dữ liệu từ server
esp_err_t tcp_client_receive(char *buffer, size_t buffer_size);

// Hàm đóng kết nối
void tcp_client_close(void);

#endif // TCP_CLIENT_H