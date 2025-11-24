#include "main.h"

// ===== UDP TOUCH CONFIGURATION =====
const char* TOUCH_SERVER_IP = "192.168.0.159";
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
void sendTouchValue(const char* touchMessage) {
    if (!isUDPTouchReady()) {
        // Serial.println("[UDP_TOUCH] Cảnh báo: UDP Touch chưa sẵn sàng!");
        return;
    }
    
    // Kiểm tra message không null và không rỗng
    if (touchMessage == nullptr || strlen(touchMessage) == 0) {
        // Serial.println("[UDP_TOUCH] Cảnh báo: Message rỗng!");
        return;
    }
    
    // Gửi UDP packet
    touch_udp.beginPacket(touch_server_address, TOUCH_SERVER_PORT);
    touch_udp.write((uint8_t*)touchMessage, strlen(touchMessage));
    touch_udp.endPacket();
    
    // Serial.printf("[UDP_TOUCH] Gửi: %s -> %s:%d\n", touchMessage, TOUCH_SERVER_IP, TOUCH_SERVER_PORT);
}

void sendTouchValueInt(int touchValue) {
    // Tạo message từ số nguyên
    char message[32];
    snprintf(message, sizeof(message), "Raw Value: %d", touchValue);
    
    // Gọi hàm chính
    sendTouchValue(message);
}

// ===== UDP TOUCH UTILITY FUNCTIONS =====
bool isUDPTouchReady() {
    return (WiFi.status() == WL_CONNECTED);
}

// ===== UDP RECEIVE FUNCTIONS =====
int availableUDPData() {
    if (!isUDPTouchReady()) {
        return 0;
    }
    return touch_udp.parsePacket();
}

int receiveUDPData(char* buffer, int bufferSize) {
    if (!isUDPTouchReady()) {
        Serial.println("[UDP_RECEIVE] Lỗi: WiFi chưa kết nối!");
        return -1;
    }
    
    // ✅ KHÔNG gọi parsePacket() nữa - đã gọi ở availableUDPData() rồi
    // Chỉ đọc dữ liệu từ packet hiện tại
    int bytesRead = touch_udp.read(buffer, bufferSize - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0'; // Null terminate
        
        // In thông tin về packet nhận được
        IPAddress remoteIP = touch_udp.remoteIP();
        int remotePort = touch_udp.remotePort();
        
        Serial.printf("[UDP_RECEIVE] Nhận %d bytes từ %s:%d\n", 
                     bytesRead, remoteIP.toString().c_str(), remotePort);
        Serial.printf("[UDP_RECEIVE] Dữ liệu: %s\n", buffer);
    }
    
    return bytesRead;
}

void handleUDPReceive() {
    if (!isUDPTouchReady()) {
        return;
    }
    
    int packetSize = touch_udp.parsePacket();
    
    if (packetSize > 0) {
        Serial.printf("[DEBUG] Có packet size: %d bytes\n", packetSize);
        
        char buffer[256];
        int bytesRead = touch_udp.read(buffer, sizeof(buffer) - 1);
        
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            
            IPAddress remoteIP = touch_udp.remoteIP();
            int remotePort = touch_udp.remotePort();
            
            Serial.printf("[UDP_RECEIVE] Nhận %d bytes từ %s:%d\n", 
                         bytesRead, remoteIP.toString().c_str(), remotePort);
            Serial.printf("[UDP_RECEIVE] Dữ liệu: %s\n", buffer);
            
            String data = String(buffer);
            data.trim();
            
            // ✅ Xử lý lệnh THRESHOLD
            if (data.startsWith("THRESHOLD:")) {
                int colonPos = data.indexOf(':');
                String thresholdStr = data.substring(colonPos + 1);
                thresholdStr.trim();
                
                int thresholdValue = thresholdStr.toInt();
                
                // Format thành 5 chữ số với leading zeros
                char uartCommand[16];
                snprintf(uartCommand, sizeof(uartCommand), "T%05dX", thresholdValue);
                
                // Gửi qua UART
                sendUARTCommand(String(uartCommand));
                
                Serial.printf("[UDP_THRESHOLD] Nhận threshold: %d -> Gửi UART: %s\n", 
                             thresholdValue, uartCommand);
            }
            
            // ✅ Xử lý lệnh XILANH
            else if (data.startsWith("XILANH:")) {
                int colonPos = data.indexOf(':');
                String valueStr = data.substring(colonPos + 1);
                valueStr.trim();
                
                int xiLanhValue = valueStr.toInt();
                
                if (xiLanhValue == 1) {
                    digitalWrite(15, HIGH);
                    Serial.println("[UDP_XILANH] IO15 -> HIGH (Xi lanh BẬT)");
                } else if (xiLanhValue == 0) {
                    digitalWrite(15, LOW);
                    Serial.println("[UDP_XILANH] IO15 -> LOW (Xi lanh TẮT)");
                } else {
                    Serial.printf("[UDP_XILANH] Giá trị không hợp lệ: %d (chỉ chấp nhận 0 hoặc 1)\n", xiLanhValue);
                }
            }
            
            // ✅ Xử lý lệnh RECALIB
            else if (data.equals("RECALIB")) {
                Serial.println("[UDP_RECALIB] Bắt đầu quá trình tái hiệu chuẩn...");
                
                // Bước 1: Gửi "D"
                sendUARTCommand("D");
                Serial.println("[UDP_RECALIB] Bước 1: Gửi lệnh D");
                
                delay(100); // Delay ngắn giữa các lệnh
                
                // Bước 2: Gửi "A"  
                sendUARTCommand("A");
                Serial.println("[UDP_RECALIB] Bước 2: Gửi lệnh A - Chờ 10 giây...");
                
                // Bước 3: Đợi 10 giây rồi gửi "E"
                delay(10000); // 10 seconds
                
                sendUARTCommand("E");
                Serial.println("[UDP_RECALIB] Bước 3: Gửi lệnh E - Hoàn thành tái hiệu chuẩn!");
            }
            
            // ✅ Xử lý lệnh LED CONTROL
            else if (data.startsWith("LEDCTRL:")) {
                // Format: LEDCTRL:ALL,255,128,64 hoặc LEDCTRL:5,255,0,0
                int colonPos = data.indexOf(':');
                String params = data.substring(colonPos + 1);
                
                int commaPos1 = params.indexOf(',');
                int commaPos2 = params.indexOf(',', commaPos1 + 1);
                int commaPos3 = params.indexOf(',', commaPos2 + 1);
                
                if (commaPos1 > 0 && commaPos2 > 0 && commaPos3 > 0) {
                    String target = params.substring(0, commaPos1);
                    int r = params.substring(commaPos1 + 1, commaPos2).toInt();
                    int g = params.substring(commaPos2 + 1, commaPos3).toInt();
                    int b = params.substring(commaPos3 + 1).toInt();
                    
                    if (target.equals("ALL")) {
                        setAllLEDs(r, g, b);
                        Serial.printf("[UDP_LEDCTRL] Đặt tất cả LED: RGB(%d,%d,%d)\n", r, g, b);
                    } else {
                        int ledIndex = target.toInt();
                        setLEDRange(ledIndex, ledIndex, r, g, b);
                        Serial.printf("[UDP_LEDCTRL] Đặt LED[%d]: RGB(%d,%d,%d)\n", ledIndex, r, g, b);
                    }
                }
            }
            
            // ✅ Xử lý lệnh LED ON/OFF
            else if (data.startsWith("LED:")) {
                int colonPos = data.indexOf(':');
                String valueStr = data.substring(colonPos + 1);
                valueStr.trim();
                
                int ledValue = valueStr.toInt();
                
                if (ledValue == 1) {
                    enableLEDEffect(true);
                    Serial.println("[UDP_LED] LED Effect -> BẬT");
                } else if (ledValue == 0) {
                    enableLEDEffect(false);
                    Serial.println("[UDP_LED] LED Effect -> TẮT");
                }
            }
            
            // ✅ Xử lý lệnh DIRECTION
            else if (data.startsWith("DIR:")) {
                int colonPos = data.indexOf(':');
                String valueStr = data.substring(colonPos + 1);
                valueStr.trim();
                
                int direction = valueStr.toInt();
                
                if (direction == 1) {
                    setLEDDirection(1);
                    Serial.println("[UDP_LED] Direction -> UP (1)");
                } else if (direction == 0) {
                    setLEDDirection(-1);
                    Serial.println("[UDP_LED] Direction -> DOWN (-1)");
                }
            }
            
            // ✅ Xử lý lệnh CONFIG MODE
            else if (data.startsWith("CONFIG:")) {
                int colonPos = data.indexOf(':');
                String valueStr = data.substring(colonPos + 1);
                valueStr.trim();
                
                int configValue = valueStr.toInt();
                Serial.printf("[UDP_LED] Config Mode -> %s\n", configValue ? "BẬT" : "TẮT");
                
                // Có thể thêm logic xử lý config mode ở đây
                if (configValue == 1) {
                    // Chế độ config - có thể disable touch effect
                    Serial.println("[UDP_LED] Vào chế độ cấu hình LED");
                } else {
                    // Chế độ bình thường - enable touch effect
                    Serial.println("[UDP_LED] Về chế độ bình thường");
                }
            }
            
            // ✅ Xử lý lệnh RAINBOW EFFECT
            else if (data.equals("RAINBOW:START")) {
                Serial.println("[UDP_LED] Bắt đầu hiệu ứng Rainbow");
                
                // Tạo hiệu ứng rainbow đơn giản
                for (int i = 0; i < 256; i += 85) {
                    int r = (i < 85) ? 255 - i * 3 : (i < 170) ? 0 : (i - 170) * 3;
                    int g = (i < 85) ? i * 3 : (i < 170) ? 255 - (i - 85) * 3 : 0;
                    int b = (i < 85) ? 0 : (i < 170) ? (i - 85) * 3 : 255 - (i - 170) * 3;
                    
                    setAllLEDs(r, g, b);
                    delay(50);
                }
                
                Serial.println("[UDP_LED] Hoàn thành hiệu ứng Rainbow");
            }
            
            // ✅ Xử lý lệnh RGB đơn giản (format: "255 128 64")
            else if (data.indexOf(' ') > 0) {
                // Phân tích RGB từ format "r g b"
                int space1 = data.indexOf(' ');
                int space2 = data.indexOf(' ', space1 + 1);
                
                if (space1 > 0 && space2 > 0) {
                    int r = data.substring(0, space1).toInt();
                    int g = data.substring(space1 + 1, space2).toInt();
                    int b = data.substring(space2 + 1).toInt();
                    
                    // Áp dụng màu với touch effect
                    applyColorWithBrightness(true, r, g, b);
                    Serial.printf("[UDP_LED] Áp dụng màu RGB: (%d,%d,%d)\n", r, g, b);
                }
            }
        }
    } else {
        // Debug message mỗi 30 giây để giảm spam
        static unsigned long lastDebug = 0;
        if (millis() - lastDebug > 30000) {
            lastDebug = millis();
            Serial.println("[DEBUG] Đang chờ UDP packets...");
        }
    }
}