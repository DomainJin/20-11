#include "osc.h"
#include <string.h>
#include <main.h>

// ===== Variables =====


// ===== OSC CONFIGURATION =====

const char* RESOLUME_IP = "192.168.0.202";
const int RESOLUME_PORT = 7000;
const int LOCAL_UDP_PORT = 8000;

// ===== GLOBAL OSC OBJECTS =====

WiFiUDP osc_udp;
IPAddress osc_resolume_address;

// ===== OSC INITIALIZATION =====

bool initOSC() {
    Serial.println("[OSC] Khởi tạo OSC module...");
    
    // Kiểm tra WiFi trước khi khởi tạo UDP
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[OSC] Lỗi: WiFi chưa kết nối!");
        return false;
    }
    
    // Khởi tạo UDP
    if (!osc_udp.begin(LOCAL_UDP_PORT)) {
        Serial.println("[OSC] Lỗi: Không thể khởi tạo UDP!");
        return false;
    }
    
    // Thiết lập địa chỉ Resolume
    osc_resolume_address.fromString(RESOLUME_IP);
    
    Serial.printf("[OSC] UDP Port: %d\n", LOCAL_UDP_PORT);
    Serial.printf("[OSC] Resolume Address: %s:%d\n", RESOLUME_IP, RESOLUME_PORT);
    Serial.println("[OSC] OSC module sẵn sàng!");
    
    return true;
}

// ===== OSC MESSAGE FUNCTIONS =====

// Hàm tạo thông điệp OSC cho 1 giá trị float
void createOSCMessage(uint8_t* buffer, int* length, const char* address, float value) {
    int pos = 0;
    int addrLen = strlen(address);

    // 1. Địa chỉ OSC, kết thúc bằng null và padding về 4 byte
    memcpy(buffer + pos, address, addrLen);
    pos += addrLen;
    buffer[pos++] = 0;
    while (pos % 4 != 0) buffer[pos++] = 0;

    // 2. Type tag ",f" (float), rồi null và padding về 4 byte
    int typetagStart = pos;
    buffer[pos++] = ',';
    buffer[pos++] = 'f';
    buffer[pos++] = 0;
    while ((pos - typetagStart) % 4 != 0) buffer[pos++] = 0;

    // 3. Dữ liệu float dạng big-endian
    uint32_t val;
    memcpy(&val, &value, sizeof(float));
    buffer[pos++] = (val >> 24) & 0xFF;
    buffer[pos++] = (val >> 16) & 0xFF;
    buffer[pos++] = (val >> 8) & 0xFF;
    buffer[pos++] = val & 0xFF;

    *length = pos;
}

// Hàm gửi OSC qua UDP
void sendOSC(const char* address, float value) {
    uint8_t buffer[256];
    int length;
    createOSCMessage(buffer, &length, address, value);

    if (osc_udp.beginPacket(osc_resolume_address, RESOLUME_PORT) == 0) {
        Serial.println("[OSC] Không thể bắt đầu gói UDP!");
        return;
    }
    osc_udp.write(buffer, length);
    osc_udp.endPacket();
    Serial.printf("[OSC] Đã gửi OSC: %s = %f\n", address, value);
}

void setDirClip(float value) {
    char address[128];
    snprintf(address, sizeof(address), "/composition/selectedclip/transport/position/behaviour/playdirection");
    sendOSC(address, value); 
}

// ===== OSC HELPER FUNCTIONS =====


void activateClip(int layer, int clip, int value) {
    // Tạo OSC address: /composition/layers/{layer}/clips/{clip}/connect
    char address[128];
    snprintf(address, sizeof(address), "/composition/layers/%d/clips/%d/connect", layer, clip);
    Serial.printf("[OSC] Kích hoạt Clip: Layer %d, Clip %d\n", layer, clip);
    sendOSC(address, value);
}

void setDirClip(int layer, int clip, int value) {
    // Tạo OSC address: /composition/layers/{layer}/clips/{clip}/connect
    char address[128];
    snprintf(address, sizeof(address), "/composition/layers/%d/clips/%d/transport/position/behaviour/playdirection", layer, clip);
    Serial.printf("[OSC] Kích hoạt Clip: Layer %d, Clip %d\n", layer, clip);
    sendOSC(address, value);
}

void sendResolumeInit(){
    activateClip(1, 1, 1);
    activateClip(2, 1, 1);
    activateClip(3, 1, 1);
}

void sendResolumeEnable(){
    activateClip(1, 2, 1);
    activateClip(2, 2, 1);
    activateClip(3, 2, 1);
}

void sendResolumeBack(int n){
    for(int i = 0; i <=n; i++){
    setDirClip(1,2,0);
    setDirClip(2,2,0);
    setDirClip(3,2,0);
    }
    
    // setDirClip(2, 2, 0);
    // setDirClip(3, 2, 0); //hiệu ứng màn led không lùi
}

void sendResolumePause(){
    setDirClip(1, 2, 1);
    setDirClip(2, 2, 1);
    setDirClip(3, 2, 0); //hiệu ứng màn led không lùi
}

void sendResolumeMain(){
    activateClip(1, 3, 1);
    activateClip(2, 3, 1);
    activateClip(3, 3, 1);
}

bool isOSCReady() {
    return WiFi.status() == WL_CONNECTED;
}