#include "tcp_client.h"

static int sock = -1; // Socket file descriptor



// Gửi mảng float
void send_array(int client_socket, float* arr, size_t size) {
    ssize_t bytes_sent = send(client_socket, arr, size * sizeof(float), 0);
    if (bytes_sent == -1) {
        ESP_LOGE(TCP_TAG, "Error sending array: %s", strerror(errno));
    } else {
        ESP_LOGI(TCP_TAG, "Array sent successfully!");
    }
}

// Nhận mảng int
bool recv_array(int client_socket, int* arr, size_t size) {
    ssize_t bytes_received = recv(client_socket, arr, size * sizeof(int), 0);
    if (bytes_received > 0) {
        ESP_LOGI(TCP_TAG, "Array received successfully!");
        for (size_t i = 0; i < size; ++i) {
            ESP_LOGI(TCP_TAG, "arr[%d] = %d", i, arr[i]);
        }
        return true;
    } else if (bytes_received == 0) {
        ESP_LOGW(TCP_TAG, "Client disconnected!");
        return false;
    } else {
        ESP_LOGE(TCP_TAG, "Error receiving array: %s", strerror(errno));
        return false;
    }
}

// Gửi struct
void send_struct(int client_socket, const MyData* data) {
    ssize_t bytes_sent = send(client_socket, data, sizeof(MyData), 0);
    if (bytes_sent == -1) {
        ESP_LOGE(TCP_TAG, "Error sending struct: %s", strerror(errno));
    } else {
        ESP_LOGI(TCP_TAG, "Struct sent successfully!");
    }
}

// Nhận struct
bool recv_struct(int client_socket, MyData* data) {
    ssize_t bytes_received = recv(client_socket, data, sizeof(MyData), 0);
    if (bytes_received > 0) {
        ESP_LOGI(TCP_TAG, "Struct received successfully!");
        ESP_LOGI(TCP_TAG, "ID: %d", data->id);
        ESP_LOGI(TCP_TAG, "Value: %f", data->value);
        ESP_LOGI(TCP_TAG, "Name: %s", data->name);
        return true;
    } else if (bytes_received == 0) {
        ESP_LOGW(TCP_TAG, "Client disconnected!");
        return false;
    } else {
        ESP_LOGE(TCP_TAG, "Error receiving struct: %s", strerror(errno));
        return false;
    }
}


// Khởi tạo và kết nối tới server
esp_err_t tcp_client_init(void) {
    struct sockaddr_in server_addr;
    struct addrinfo hints, *res;

    // Thiết lập hints cho getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // Chuyển đổi port thành chuỗi
    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", TCP_SERVER_PORT);

    // Lấy thông tin địa chỉ server
    int err = getaddrinfo(TCP_SERVER_IP, port_str, &hints, &res);
    if (err != 0 || res == NULL) {
        ESP_LOGE(TCP_TAG, "DNS lookup failed err=%d res=%p", err, res);
        if (res) freeaddrinfo(res);
        return ESP_FAIL;
    }

    // Tạo socket
    sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock < 0) {
        ESP_LOGE(TCP_TAG, "Failed to create socket: %d", errno);
        freeaddrinfo(res);
        return ESP_FAIL;
    }

    // Kết nối tới server
    if (connect(sock, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TCP_TAG, "Socket connect failed: %d", errno);
        close(sock);
        sock = -1;
        freeaddrinfo(res);
        return ESP_FAIL;
    }

    ESP_LOGI(TCP_TAG, "Connected to server %s:%d", TCP_SERVER_IP, TCP_SERVER_PORT);
    freeaddrinfo(res);
    return ESP_OK;
}

// Gửi dữ liệu tới server
esp_err_t tcp_client_send(const char *data) {
    if (sock < 0) {
        ESP_LOGE(TCP_TAG, "Socket not initialized");
        return ESP_FAIL;
    }

    int len = send(sock, data, strlen(data), 0);
    if (len < 0) {
        ESP_LOGE(TCP_TAG, "Error occurred during sending: %d", errno);
        return ESP_FAIL;
    }

    ESP_LOGI(TCP_TAG, "Sent %d bytes: %s", len, data);
    return ESP_OK;
}

// Nhận dữ liệu từ server
esp_err_t tcp_client_receive(char *buffer, size_t buffer_size) {
    if (sock < 0) {
        ESP_LOGE(TCP_TAG, "Socket not initialized");
        return ESP_FAIL;
    }

    int len = recv(sock, buffer, buffer_size - 1, 0);
    if (len < 0) {
        ESP_LOGE(TCP_TAG, "Error occurred during receiving: %d", errno);
        return ESP_FAIL;
    } else if (len == 0) {
        ESP_LOGW(TCP_TAG, "Connection closed by server");
        return ESP_FAIL;
    }

    buffer[len] = '\0'; // Thêm ký tự kết thúc chuỗi
    ESP_LOGI(TCP_TAG, "Received %d bytes: %s", len, buffer);
    return ESP_OK;
}

// Đóng kết nối
void tcp_client_close(void) {
    if (sock >= 0) {
        close(sock);
        sock = -1;
        ESP_LOGI(TCP_TAG, "Socket closed");
    }
}