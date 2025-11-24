/*
 * UDP SEND STRING EXAMPLE
 * Demo cách sử dụng hàm sendTouchValue mới với string
 */

#include <Arduino.h>
#include <WiFi.h>
#include "udpconfig.h"

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

void setup() {
    Serial.begin(115200);
    Serial.println("\n===== UDP SEND STRING TEST =====");
    
    // Kết nối WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    
    // Khởi tạo UDP
    initUDPTouch();
    
    Serial.println("===== DEMO GỬI STRING =====");
}

void loop() {
    // Demo gửi các loại string khác nhau
    
    // 1. Gửi message đơn giản
    sendTouchValue("Hello from ESP32!");
    delay(2000);
    
    // 2. Gửi JSON data
    sendTouchValue("{\"sensor\":\"touch\", \"value\":128, \"timestamp\":1234567890}");
    delay(2000);
    
    // 3. Gửi command
    sendTouchValue("CMD:LED_ON");
    delay(2000);
    
    // 4. Gửi formatted string
    char buffer[64];
    float temperature = 25.6;
    int humidity = 65;
    snprintf(buffer, sizeof(buffer), "SENSOR:temp=%.1f,hum=%d", temperature, humidity);
    sendTouchValue(buffer);
    delay(2000);
    
    // 5. Gửi số nguyên (dùng hàm wrapper)
    sendTouchValueInt(42);
    delay(2000);
    
    // 6. Gửi raw data
    sendTouchValue("RAW:010203040506070809");
    delay(2000);
    
    Serial.println("--- Chu kỳ gửi hoàn tất ---");
    delay(3000);
}