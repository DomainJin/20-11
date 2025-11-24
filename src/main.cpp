#include <Arduino.h>
#include "main.h"

// Cấu hình WiFi
const char* ssid = "Cube Touch";
const char* password = "admin123";

// Cấu hình OSC
const char* resolume_ip = "192.168.0.241";
const int resolume_port = 7000;

// Cấu hình OSC
const char* server_ip = "192.168.0.202";
const int server_port = 7000;

WiFiUDP udp;
IPAddress resolume_address;

bool touchActive = false;
unsigned long touchDuration = 0;
bool sendEnableOnce = false;
bool sendBackOnce = false;
bool sendMainOnce = false;
bool sendInitOnce = false;
int mainRunMillis = 0;

int valuetouch = 0;
int operationTime = 2000;    // 2 giây để kích hoạt main effect
int mainEffectime = 6000;    // 6 giây main effect chạy



void setup() {
    Serial.begin(115200);
    Serial.println("\nKhởi động ESP32 OSC Client...");
    
    // Cấu hình pin IO15 cho xi lanh
    pinMode(15, OUTPUT);
    digitalWrite(15, LOW);
    
    // Kết nối WiFi
    WiFi.begin(ssid, password);
    Serial.print("Đang kết nối WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi đã kết nối!");
    Serial.print("Địa chỉ IP: ");
    Serial.println(WiFi.localIP());
    
    // Khởi tạo các modules
    initOSC();
    initUART();
    initUDPTouch();
    initLED();
    
    Serial.println("Tất cả modules đã sẵn sàng!");
}

void loop() {
    // Xử lý dữ liệu UART và UDP
    handleUARTData();
    handleUDPReceive();
    
    // Lấy trạng thái touch hiện tại
    touchActive = isTouchActive();
    touchDuration = getTouchDuration();

    if(touchActive){
        sendEnableOnce = true;
    }
    
    if(sendEnableOnce){
        sendResolumeEnable();
        sendEnableOnce = false;
    }

    if(!touchActive && touchDuration < operationTime){
        sendBackOnce = true;
    }
    if(sendBackOnce){
        sendResolumeBack(1);
        sendBackOnce = false;
    }
    if(touchActive && touchDuration >= operationTime){
        sendMainOnce = true;
        mainRunMillis = millis();
    }
    if(sendMainOnce){
        sendResolumeMain();
        sendMainOnce = false;
    }
    if(!touchActive && (millis() - mainRunMillis) >= mainEffectime){
        sendInitOnce = true;
    }
    if(sendInitOnce){
        sendResolumeInit();
        sendInitOnce = false;
    }
}