# UDP RECEIVE FUNCTIONS - HƯỚNG DẪN SỬ DỤNG

## Tổng quan

Đã thêm các hàm nhận UDP vào project ESP32, cho phép nhận dữ liệu từ máy tính hoặc thiết bị khác qua mạng WiFi.

## Các hàm đã thêm

### 1. `int availableUDPData()`

- Kiểm tra có dữ liệu UDP đang chờ không
- **Trả về**: Số bytes có sẵn để đọc, 0 nếu không có dữ liệu

### 2. `int receiveUDPData(char* buffer, int bufferSize)`

- Nhận dữ liệu UDP vào buffer
- **Tham số**:
  - `buffer`: Buffer để chứa dữ liệu
  - `bufferSize`: Kích thước buffer
- **Trả về**: Số bytes nhận được, 0 nếu không có dữ liệu, -1 nếu lỗi

### 3. `void handleUDPReceive()`

- Tự động nhận và xử lý dữ liệu UDP (non-blocking)
- Parse các format đặc biệt như "CMD:" và "DATA:"
- In kết quả ra Serial Monitor

## Cách sử dụng

### Cách 1: Đơn giản (Khuyên dùng)

```cpp
void loop() {
    handleUDPReceive(); // Tự động nhận và xử lý
    // Code khác...
}
```

### Cách 2: Kiểm soát thủ công

```cpp
void loop() {
    if (availableUDPData() > 0) {
        char buffer[256];
        int bytesRead = receiveUDPData(buffer, sizeof(buffer));

        if (bytesRead > 0) {
            Serial.printf("Nhận được: %s\n", buffer);

            // Xử lý dữ liệu tại đây
            if (strcmp(buffer, "LED_ON") == 0) {
                // Bật LED
            }
        }
    }
}
```

## Test và Demo

### 1. Upload code

- Sử dụng `main.cpp` đã được cập nhật
- Hoặc sử dụng `udp_receive_example.cpp` để test riêng

### 2. Chạy test script

```bash
# Từ máy tính, chạy:
python udp_sender_test.py [IP_ESP32]

# Ví dụ:
python udp_sender_test.py 192.168.1.100
```

### 3. Kiểm tra kết quả

- Mở Serial Monitor (115200 baud)
- Xem messages nhận được từ ESP32

## Format dữ liệu đặc biệt

ESP32 sẽ parse các format sau:

### Commands

```
CMD:LED_ON      -> Xử lý lệnh LED_ON
CMD:RESTART     -> Xử lý lệnh RESTART
```

### Data

```
DATA:temp=25.5  -> Xử lý dữ liệu nhiệt độ
DATA:hum=60     -> Xử lý dữ liệu độ ẩm
```

### Tin nhắn thông thường

```
Hello ESP32!    -> Hiển thị như tin nhắn bình thường
PING           -> Echo message
```

## Cấu hình

### Port cấu hình

- **LOCAL_TOUCH_PORT**: 8001 (ESP32 lắng nghe)
- **TOUCH_SERVER_PORT**: 7043 (ESP32 gửi đến)

### Thay đổi cấu hình

Chỉnh sửa trong `src/udpconfig.cpp`:

```cpp
const int LOCAL_TOUCH_PORT = 8001;  // Port ESP32 nhận
const int TOUCH_SERVER_PORT = 7043; // Port ESP32 gửi
```

## Ví dụ output Serial Monitor

```
===== UDP RECEIVE TEST =====
Kết nối WiFi...
WiFi đã kết nối!
IP Address: 192.168.1.100

[UDP_TOUCH] Khởi tạo UDP Touch module...
[UDP_TOUCH] Local UDP Port: 8001
[UDP_TOUCH] Touch Server: 192.168.0.202:7043
[UDP_TOUCH] UDP Touch module sẵn sàng!

===== BẮT ĐẦU LẮNG NGHE UDP =====

[UDP_RECEIVE] Nhận 13 bytes từ 192.168.1.50:54231
[UDP_RECEIVE] Dữ liệu: Hello ESP32!
[UDP_RECEIVE] Tin nhắn: Hello ESP32!

[UDP_RECEIVE] Nhận 10 bytes từ 192.168.1.50:54231
[UDP_RECEIVE] Dữ liệu: CMD:LED_ON
[UDP_RECEIVE] Nhận lệnh: LED_ON

[UDP_RECEIVE] Nhận 18 bytes từ 192.168.1.50:54231
[UDP_RECEIVE] Dữ liệu: DATA:temperature=25.5
[UDP_RECEIVE] Nhận dữ liệu: temperature=25.5
```

## Lưu ý

- Các hàm nhận UDP là non-blocking, không làm treo chương trình
- Cần đảm bảo WiFi đã kết nối trước khi sử dụng
- Buffer size tối đa 255 bytes cho mỗi packet
- ESP32 sẽ tự động in thông tin về packets nhận được
