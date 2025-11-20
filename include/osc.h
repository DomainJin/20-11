#ifndef OSC_H
#define OSC_H

#include <main.h>


// ===== OSC CONFIGURATION =====
extern const char* RESOLUME_IP;
extern const int RESOLUME_PORT;
extern const int LOCAL_UDP_PORT;

// ===== GLOBAL OSC OBJECTS =====
extern WiFiUDP osc_udp;
extern IPAddress osc_resolume_address;

// ===== OSC FUNCTION DECLARATIONS =====

/**
 * Khởi tạo OSC module
 * @return true nếu khởi tạo thành công, false nếu thất bại
 */
bool initOSC();

/**
 * Tạo OSC message
 * @param buffer Buffer để lưu trữ message
 * @param length Con trỏ để lưu độ dài message
 * @param address Địa chỉ OSC
 * @param value Giá trị float cần gửi
 */
void createOSCMessage(uint8_t* buffer, int* length, const char* address, float value);


/**
 * Gửi OSC message tới Resolume
 * @param address Địa chỉ OSC
 * @param value Giá trị float
 */
void sendOSC(const char* address, float value);

/**
 * Kích hoạt clip trong Resolume
 * @param layer Số layer
 * @param clip Số clip
 * @param value Giá trị kích hoạt
 */
void activateClip(int layer, int clip, int value);

/**
 * Thiết lập hướng phát clip
 * @param layer Số layer (1-based)
 * @param clip Số clip (1-based)
 * @param value 0.0=reverse, 1.0=forward
 */
void setDirClip(float value);  // Sửa thành float

/**
 * Khởi tạo clips ban đầu
 */
void sendResolumeInit();

/**
 * Kích hoạt clips chính
 */
void sendResolumeEnable();

/**
 * Phát ngược clips
 */
void sendResolumeBack(int n);

/**
 * Dừng/tạm dừng clips
 */
void sendResolumePause();

/**
 * Kích hoạt clips chính
 */
void sendResolumeMain();
/**
 * Kiểm tra trạng thái kết nối OSC
 * @return true nếu OSC sẵn sàng, false nếu chưa kết nối
 */
bool isOSCReady();

#endif // OSC_H
