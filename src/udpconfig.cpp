#include "udpconfig.h"
#include <Arduino.h>

// ===== UDP TOUCH CONFIGURATION =====
const char* TOUCH_SERVER_IP = "192.168.0.202";
const int TOUCH_SERVER_PORT = 7043;
const int LOCAL_TOUCH_PORT = 8001;

// ===== GLOBAL UDP TOUCH OBJECTS =====
WiFiUDP touch_udp;
IPAddress touch_server_address;

// ===== UDP TOUCH INITIALIZATION =====
bool initUDPTouch() {
    Serial.println("[UDP_TOUCH] Khởi tạo UDP Touch module...");
    
    // Kiểm tra WiFi trước khi khởi tạo UDP
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[UDP_TOUCH] Lỗi: WiFi chưa kết nối!");
        return false;
    }
    
    // Khởi tạo UDP
    if (!touch_udp.begin(LOCAL_TOUCH_PORT)) {
        Serial.println("[UDP_TOUCH] Lỗi: Không thể khởi tạo UDP!");
        return false;
    }
    
    // Thiết lập địa chỉ Touch Server
    touch_server_address.fromString(TOUCH_SERVER_IP);
    
    Serial.printf("[UDP_TOUCH] Local UDP Port: %d\n", LOCAL_TOUCH_PORT);
    Serial.printf("[UDP_TOUCH] Touch Server: %s:%d\n", TOUCH_SERVER_IP, TOUCH_SERVER_PORT);
    Serial.println("[UDP_TOUCH] UDP Touch module sẵn sàng!");
    
    return true;
}

// ===== UDP TOUCH FUNCTIONS =====
void sendTouchValue(int touchValue) {
    if (!isUDPTouchReady()) {
        Serial.println("[UDP_TOUCH] Cảnh báo: UDP Touch chưa sẵn sàng!");
        return;
    }
    
    // Tạo message đơn giản chứa touch value
    char message[32];
    snprintf(message, sizeof(message), "TOUCH:%d", touchValue);
    
    // Gửi UDP packet
    touch_udp.beginPacket(touch_server_address, TOUCH_SERVER_PORT);
    touch_udp.write((uint8_t*)message, strlen(message));
    touch_udp.endPacket();
    
    // Serial.printf("[UDP_TOUCH] Gửi: %s -> %s:%d\n", message, TOUCH_SERVER_IP, TOUCH_SERVER_PORT);
}

// ===== UDP TOUCH UTILITY FUNCTIONS =====
bool isUDPTouchReady() {
    return (WiFi.status() == WL_CONNECTED);
}