#!/usr/bin/env python3
"""
Simple UDP Touch Receiver
Version đơn giản để test nhận dữ liệu từ ESP32
"""

import socket
import sys

def main():
    # Cấu hình
    HOST = '0.0.0.0'  # Lắng nghe tất cả interfaces  
    PORT = 7043       # Port ESP32 gửi đến
    
    print(f"Khởi động UDP Receiver tại {HOST}:{PORT}")
    print("Chờ dữ liệu từ ESP32...")
    print("Nhấn Ctrl+C để dừng")
    print("-" * 40)
    
    try:
        # Tạo UDP socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind((HOST, PORT))
        
        packet_count = 0
        
        while True:
            # Nhận dữ liệu
            data, addr = sock.recvfrom(1024)
            packet_count += 1
            
            # Decode và hiển thị
            message = data.decode('utf-8').strip()
            print(f"[{packet_count:4d}] Từ {addr[0]}:{addr[1]} -> {message}")
            
            # Parse touch value nếu đúng format
            if message.startswith("TOUCH:"):
                try:
                    touch_value = int(message.split(":")[1])
                    print(f"      Touch Value: {touch_value}")
                except:
                    pass
                    
    except KeyboardInterrupt:
        print(f"\nĐã nhận {packet_count} packets")
        print("Tạm biệt!")
    except Exception as e:
        print(f"Lỗi: {e}")
    finally:
        if 'sock' in locals():
            sock.close()

if __name__ == "__main__":
    main()