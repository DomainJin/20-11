#ifndef UDP_CONFIG_H
#define UDP_CONFIG_H

#include <WiFi.h>
#include <WiFiUdp.h>

// ===== UDP TOUCH CONFIGURATION =====
extern const char* TOUCH_SERVER_IP;
extern const int TOUCH_SERVER_PORT;
extern const int LOCAL_TOUCH_PORT;

// ===== GLOBAL UDP TOUCH OBJECTS =====
extern WiFiUDP touch_udp;
extern IPAddress touch_server_address;

// ===== UDP TOUCH FUNCTION DECLARATIONS =====

/**
 * Khởi tạo UDP Touch module
 * @return true nếu khởi tạo thành công, false nếu thất bại
 */
bool initUDPTouch();

/**
 * Gửi touch value qua UDP đến server
 * @param touchValue Giá trị touch cần gửi
 */
void sendTouchValue(int touchValue);

/**
 * Kiểm tra trạng thái kết nối UDP Touch
 * @return true nếu UDP Touch sẵn sàng, false nếu chưa kết nối
 */
bool isUDPTouchReady();

#endif // UDP_CONFIG_H