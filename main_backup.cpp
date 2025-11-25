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
    pinMode(2, OUTPUT);
    pinMode(15, OUTPUT);
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
    initLED();  // ✅ THÊM DÒNG NÀY
    
    Serial.println("Tất cả modules đã sẵn sàng!");
}

void loop() {
    // Test LED cơ bản trước
    static unsigned long lastToggle = 0;
    static int testPhase = 0;
    
    if (millis() - lastToggle >= 3000) {
        lastToggle = millis();
        testPhase++;
        
        switch(testPhase % 4) {
            case 0:
                Serial.println("[TEST] LED TẮT");
                clearAllLEDs();
                break;
            case 1:
                Serial.println("[TEST] LED XANH");
                setAllLEDs(0, 255, 0);
                break;
            case 2:
                Serial.println("[TEST] LED ĐỎ");
                setAllLEDs(255, 0, 0);
                break;
            case 3:
                Serial.println("[TEST] LED XANH DƯƠNG");
                setAllLEDs(0, 0, 255);
                break;
        }
    }
    
    // Comment touch logic để test LED trước
    /*
    handleUARTData();
    handleUDPReceive();
    touchActive = isTouchActive();
    touchDuration = getTouchDuration();
    applyColorWithBrightness(touchActive, 0, 255, 0);
    */
}