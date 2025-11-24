#!/usr/bin/env python3
"""
UDP Sender Test Script
Script để test gửi dữ liệu UDP đến ESP32
"""

import socket
import time
import sys

def main():
    # Cấu hình
    ESP32_IP = "192.168.0.43"  # Thay bằng IP của ESP32
    ESP32_PORT = 8001           # Port ESP32 đang lắng nghe
    
    if len(sys.argv) > 1:
        ESP32_IP = sys.argv[1]
    
    print(f"Gửi dữ liệu UDP đến ESP32 tại {ESP32_IP}:{ESP32_PORT}")
    print("Nhấn Ctrl+C để dừng")
    print("-" * 50)
    
    try:
        # Tạo UDP socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        messages = [
            "Hello ESP32!",
            "CMD:LED_ON",
            "CMD:LED_OFF", 
            "DATA:temperature=25.5",
            "DATA:humidity=60",
            "Test message from Python",
            "PING",
            "CMD:RESTART"
        ]
        
        count = 0
        while True:
            # Chọn message
            message = messages[count % len(messages)]
            count += 1
            
            # Gửi UDP packet
            sock.sendto(message.encode('utf-8'), (ESP32_IP, ESP32_PORT))
            print(f"[{count:3d}] Gửi: {message}")
            
            time.sleep(2)  # Gửi mỗi 2 giây
            
    except KeyboardInterrupt:
        print(f"\nĐã gửi {count} messages")
        print("Tạm biệt!")
    except Exception as e:
        print(f"Lỗi: {e}")
    finally:
        if 'sock' in locals():
            sock.close()

if __name__ == "__main__":
    main()