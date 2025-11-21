#!/usr/bin/env python3
"""
UDP Touch Receiver
Chương trình Python nhận giá trị touch từ ESP32 qua UDP
Port: 7043
"""

import socket
import threading
import time
import sys
from datetime import datetime

class UDPTouchReceiver:
    def __init__(self, host='0.0.0.0', port=7043):
        """
        Khởi tạo UDP Touch Receiver
        
        Args:
            host (str): Địa chỉ IP để lắng nghe (0.0.0.0 = tất cả interfaces)
            port (int): Port để lắng nghe UDP packets
        """
        self.host = host
        self.port = port
        self.socket = None
        self.running = False
        self.touch_values = []
        self.last_value = None
        self.total_packets = 0
        
    def start_server(self):
        """Khởi động UDP server"""
        try:
            # Tạo UDP socket
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.socket.bind((self.host, self.port))
            self.running = True
            
            print(f"[UDP_RECEIVER] Server đã khởi động tại {self.host}:{self.port}")
            print(f"[UDP_RECEIVER] Đang chờ dữ liệu từ ESP32...")
            print(f"[UDP_RECEIVER] Nhấn Ctrl+C để dừng server")
            print("-" * 50)
            
            # Lắng nghe UDP packets
            while self.running:
                try:
                    # Nhận dữ liệu (buffer 1024 bytes)
                    data, addr = self.socket.recvfrom(1024)
                    self.process_received_data(data, addr)
                    
                except socket.timeout:
                    continue
                except Exception as e:
                    if self.running:
                        print(f"[UDP_RECEIVER] Lỗi khi nhận dữ liệu: {e}")
                        
        except KeyboardInterrupt:
            print("\n[UDP_RECEIVER] Dừng server...")
        except Exception as e:
            print(f"[UDP_RECEIVER] Lỗi khởi động server: {e}")
        finally:
            self.stop_server()
    
    def process_received_data(self, data, addr):
        """
        Xử lý dữ liệu nhận được
        
        Args:
            data (bytes): Dữ liệu nhận được
            addr (tuple): Địa chỉ IP và port của người gửi
        """
        try:
            # Decode message
            message = data.decode('utf-8').strip()
            timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
            
            self.total_packets += 1
            
            # Parse touch value từ message "TOUCH:123"
            if message.startswith("TOUCH:"):
                touch_value = int(message.split(":")[1])
                self.last_value = touch_value
                self.touch_values.append(touch_value)
                
                # Giới hạn history (chỉ giữ 100 giá trị gần nhất)
                if len(self.touch_values) > 100:
                    self.touch_values = self.touch_values[-100:]
                
                # Hiển thị thông tin
                print(f"[{timestamp}] Từ {addr[0]}:{addr[1]} | Touch: {touch_value:4d} | Total: {self.total_packets}")
                
                # Gọi callback xử lý touch value (có thể mở rộng)
                self.on_touch_received(touch_value, addr)
                
            else:
                print(f"[{timestamp}] Từ {addr[0]}:{addr[1]} | Raw: {message}")
                
        except Exception as e:
            print(f"[UDP_RECEIVER] Lỗi xử lý dữ liệu: {e}")
    
    def on_touch_received(self, touch_value, addr):
        """
        Callback khi nhận được touch value
        Có thể override method này để xử lý custom logic
        
        Args:
            touch_value (int): Giá trị touch nhận được
            addr (tuple): Địa chỉ IP và port của ESP32
        """
        # Ví dụ: Xử lý các mức touch khác nhau
        if touch_value == 0:
            status = "No Touch"
        elif touch_value < 50:
            status = "Light Touch"
        elif touch_value < 100:
            status = "Medium Touch"
        else:
            status = "Strong Touch"
            
        # In trạng thái touch (có thể comment để giảm spam)
        # print(f"    -> Status: {status}")
        
        # Thêm logic xử lý khác ở đây nếu cần
        # Ví dụ: Ghi vào file, gửi đến hệ thống khác, v.v.
    
    def stop_server(self):
        """Dừng UDP server"""
        self.running = False
        if self.socket:
            self.socket.close()
        print(f"[UDP_RECEIVER] Server đã dừng")
        print(f"[UDP_RECEIVER] Tổng số packets nhận được: {self.total_packets}")
        if self.last_value is not None:
            print(f"[UDP_RECEIVER] Giá trị cuối cùng: {self.last_value}")
    
    def get_statistics(self):
        """Lấy thống kê"""
        if not self.touch_values:
            return None
            
        return {
            'total_packets': self.total_packets,
            'last_value': self.last_value,
            'min_value': min(self.touch_values),
            'max_value': max(self.touch_values),
            'avg_value': sum(self.touch_values) / len(self.touch_values),
            'values_count': len(self.touch_values)
        }

def print_statistics_thread(receiver):
    """Thread hiển thị thống kê định kỳ"""
    while receiver.running:
        time.sleep(10)  # Hiển thị mỗi 10 giây
        if receiver.running:
            stats = receiver.get_statistics()
            if stats:
                print("\n" + "="*50)
                print(f"THỐNG KÊ (10s gần nhất):")
                print(f"Tổng packets: {stats['total_packets']}")
                print(f"Giá trị cuối: {stats['last_value']}")
                print(f"Min/Max/Avg: {stats['min_value']}/{stats['max_value']}/{stats['avg_value']:.1f}")
                print("="*50 + "\n")

def main():
    """Main function"""
    print("UDP Touch Receiver - Nhận dữ liệu touch từ ESP32")
    print("=" * 50)
    
    # Có thể thay đổi host và port nếu cần
    HOST = '0.0.0.0'  # Lắng nghe tất cả interfaces
    PORT = 7043       # Port ESP32 gửi đến
    
    # Tạo receiver instance
    receiver = UDPTouchReceiver(host=HOST, port=PORT)
    
    # Tạo thread hiển thị thống kê
    stats_thread = threading.Thread(target=print_statistics_thread, args=(receiver,), daemon=True)
    stats_thread.start()
    
    try:
        # Khởi động server (blocking call)
        receiver.start_server()
    except KeyboardInterrupt:
        print("\nĐang dừng server...")
    finally:
        receiver.stop_server()

if __name__ == "__main__":
    main()