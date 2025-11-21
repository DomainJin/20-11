# UDP Touch Receiver - Hướng Dẫn Sử Dụng

## Mô tả
Chương trình Python nhận dữ liệu touch value từ ESP32 qua UDP port 7043.

## Cấu trúc Files
- `udp_touch_receiver.py` - Chương trình chính với đầy đủ tính năng
- `simple_udp_receiver.py` - Version đơn giản để test nhanh
- `requirements.txt` - Dependencies (không cần cho version cơ bản)

## Cách Sử Dụng

### 1. Chạy Version Đơn Giản (Recommended để test)
```bash
python simple_udp_receiver.py
```

### 2. Chạy Version Đầy Đủ Tính Năng
```bash
python udp_touch_receiver.py
```

## Thông Tin Kỹ Thuật

### ESP32 Configuration (trong udpconfig.cpp):
- **Server IP**: `192.168.0.202`
- **Server Port**: `7043`
- **Message Format**: `"TOUCH:123"` (với 123 là touch value)

### Python Receiver Configuration:
- **Listen IP**: `0.0.0.0` (tất cả interfaces)
- **Listen Port**: `7043`
- **Buffer Size**: `1024 bytes`

## Troubleshooting

### 1. Lỗi "Address already in use"
```bash
# Kiểm tra port đang được sử dụng
netstat -an | find "7043"

# Hoặc đổi port trong code Python
PORT = 7044  # thay vì 7043
```

### 2. Không nhận được dữ liệu
- Kiểm tra ESP32 và PC cùng network
- Kiểm tra IP address trong udpconfig.cpp
- Kiểm tra firewall Windows

### 3. Test kết nối
```bash
# Từ ESP32, có thể test bằng ping
ping 192.168.0.202

# Từ PC, có thể test UDP bằng netcat (nếu có)
# echo "TOUCH:123" | nc -u 192.168.0.202 7043
```

## Output Mẫu

### Simple Version:
```
[   1] Từ 192.168.0.100:8001 -> TOUCH:45
      Touch Value: 45
[   2] Từ 192.168.0.100:8001 -> TOUCH:67
      Touch Value: 67
```

### Full Version:
```
[14:30:15.123] Từ 192.168.0.100:8001 | Touch:   45 | Total: 1
[14:30:15.223] Từ 192.168.0.100:8001 | Touch:   67 | Total: 2

==================================================
THỐNG KÊ (10s gần nhất):
Tổng packets: 45
Giá trị cuối: 67
Min/Max/Avg: 12/156/73.4
==================================================
```

## Tùy Chỉnh

### Thay đổi Port:
```python
PORT = 7044  # Thay đổi port nếu cần
```

### Thay đổi Logic Xử Lý:
Chỉnh sửa method `on_touch_received()` trong `udp_touch_receiver.py`:
```python
def on_touch_received(self, touch_value, addr):
    # Thêm logic custom ở đây
    if touch_value > 100:
        print("Strong touch detected!")
        # Gửi signal đến hệ thống khác
        # Ghi vào file log
        # etc.
```

## Notes
- Chương trình tự động parse message format `"TOUCH:value"`
- Có thể mở rộng để nhận nhiều format khác
- Hỗ trợ multiple ESP32 cùng lúc
- Thread-safe cho việc mở rộng tính năng