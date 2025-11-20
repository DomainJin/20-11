#include <Arduino.h>
#include "main.h"
#include "osc.h"
#include "uart.h"

// Cấu hình WiFi
const char* ssid = "Cube Touch";
const char* password = "admin123";

// Cấu hình OSC
const char* resolume_ip = "192.168.0.202";
const int resolume_port = 7000;

WiFiUDP udp;
IPAddress resolume_address;

bool touchActive = false;
unsigned long touchDuration = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("\nKhởi động ESP32 OSC Client...");
  
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
  
  // Khởi tạo OSC module
  initOSC();
  
  // Khởi tạo UART module
  initUART();
  
  Serial.println("Sẵn sàng gửi OSC đến Resolume!");
  Serial.println("Nhấn phím bất kỳ trong Serial Monitor để gửi lệnh OSC");
  Serial.println("Lệnh: status | test | send:DATA | hoặc Enter");
}

void loop() {
    // Xử lý dữ liệu UART từ PIC
    handleUARTData();
    touchActive = isTouchActive();
    touchDuration = getTouchDuration();
    sendResolumeInit();
    delay(1000);
    sendResolumeEnable();
    delay(1000);
    sendResolumeBack(5);
    delay(1000);
    sendResolumeMain();
    delay(5000);
}