/*
 * UDP RECEIVE EXAMPLE
 * Ví dụ cách sử dụng hàm nhận UDP trong ESP32
 * 
 * Để test:
 * 1. Upload code này lên ESP32
 * 2. Chạy script Python udp_sender_test.py từ máy tính
 * 3. Xem kết quả trên Serial Monitor
 */

#include <Arduino.h>
#include <WiFi.h>
#include "udpconfig.h"

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

void setup() {
    Serial.begin(115200);
    Serial.println("\n===== UDP RECEIVE TEST =====");
    
    // Kết nối WiFi
    Serial.println("Kết nối WiFi...");
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi đã kết nối!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    // Khởi tạo UDP
    if (initUDPTouch()) {
        Serial.println("UDP Touch module sẵn sàng để nhận dữ liệu!");
    } else {
        Serial.println("Lỗi khởi tạo UDP Touch!");
    }
    
    Serial.println("\n===== BẮT ĐẦU LẮNG NGHE UDP =====");
}

void loop() {
    // Cách 1: Sử dụng handleUDPReceive() - Đơn giản nhất
    handleUDPReceive();
    
    /* 
    // Cách 2: Kiểm tra và nhận thủ công
    if (availableUDPData() > 0) {
        char buffer[256];
        int bytesRead = receiveUDPData(buffer, sizeof(buffer));
        
        if (bytesRead > 0) {
            Serial.printf("Nhận được: %s\n", buffer);
            
            // Phản hồi lại (echo)
            sendTouchValueInt(123); // Gửi lại một giá trị test
        }
    }
    */
    
    // Gửi heartbeat mỗi 5 giây
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 5000) {
        sendTouchValueInt(999); // Gửi heartbeat
        lastHeartbeat = millis();
        Serial.println("[HEARTBEAT] Gửi heartbeat...");
    }
    
    delay(10); // Tránh spam CPU
}