#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
#include <stddef.h>
#include <stdbool.h>
// Định nghĩa TAG và hằng số (thay vì dùng ESP-IDF)
#define TCP_TAG "TCP_CLIENT"
#define TCP_SERVER_IP "192.168.1.213" // Thay bằng IP server của bạn
#define TCP_SERVER_PORT 8080
#define NUM_INTS 5
typedef struct {
    int id;
    float value;
    char name[32];
} MyData;

void send_array(int client_socket, float* arr, size_t size);
bool recv_array(int client_socket, int* arr, size_t size);
void send_struct(int client_socket, const MyData* data);
bool recv_struct(int client_socket, MyData* data);
int tcp_client_init(void);
int tcp_client_send(const char *data);
int tcp_client_receive_ints(int *int_array);
int tcp_client_receive(char *buffer, size_t buffer_size);
void tcp_client_close(void);

#endif