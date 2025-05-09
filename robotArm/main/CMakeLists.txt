idf_component_register(SRCS 
                    # "hello_world_main.c"
                    "main.c" 
                    "led_control.c"
                    "servo_control.c"
                    "wifi_connect.c"
                    # "websocket_client.c"
                    # "tcp_client.c"
                    "mqtt_client.c"
                    INCLUDE_DIRS "."
                    
                    # REQUIRES lwip esp_netif
)
