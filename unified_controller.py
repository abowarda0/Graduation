import serial
import time
import os
import threading
from datetime import datetime

SERIAL_PORT = "/dev/ttyACM0"
BAUD_RATE = 9600

# Global variables
arduino = None
current_detection_status = "unknown"
last_detection_status = ""

def initialize_arduino():
    """Initialize Arduino connection"""
    global arduino
    try:
        arduino = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)
        print("[INFO] Arduino connection established.")
        return True
    except Exception as e:
        print(f"[ERROR] Arduino connection failed: {e}")
        return False

def send_to_arduino(command, source=""):
    """Send command to Arduino with error handling"""
    global arduino
    if arduino:
        try:
            arduino.write((command + "\n").encode())
            print(f"[{source}] Sent to Arduino: {command}")
            return True
        except Exception as e:
            print(f"[ERROR] Failed to send to Arduino: {e}")
            return False
    return False

def get_current_detection_status():
    """Read current detection status from file"""
    try:
        if os.path.exists("current_status.txt"):
            with open("current_status.txt", "r") as f:
                return f.read().strip()
    except:
        pass
    return "unknown"

def monitor_detection():
    """Monitor detection file and send automatic commands"""
    global current_detection_status, last_detection_status
    
    file_path = "realtime.txt"
    last_size = 0
    
    print("[INFO] Starting detection monitoring thread...")
    
    while True:
        try:
            # Update current status from status file
            current_detection_status = get_current_detection_status()
            
            # Check detection log file for changes
            if os.path.exists(file_path):
                with open(file_path, "r") as f:
                    f.seek(last_size)
                    new_lines = f.readlines()
                    last_size = f.tell()
                    
                    for line in reversed(new_lines):
                        line = line.strip()
                        if not line:
                            continue
                        
                        parts = line.split(",")
                        if len(parts) >= 2:
                            status_from_file = parts[1].strip()
                            
                            if status_from_file in ["near", "far"]:
                                if status_from_file != last_detection_status:
                                    send_to_arduino(status_from_file, "AUTO-DETECTION")
                                    last_detection_status = status_from_file
                                break
                            elif status_from_file == "no human" and last_detection_status != "no human":
                                send_to_arduino("no human", "AUTO-DETECTION")
                                last_detection_status = "no human"
                                break
            
            time.sleep(0.1)
            
        except Exception as e:
            print(f"[ERROR] Detection monitoring: {e}")
            time.sleep(1)

def monitor_manual_commands():
    """Monitor for manual commands from Node-RED"""
    global current_detection_status
    
    manual_command_file = "/home/pi/manual_command.txt"
    last_size = 0
    
    print("[INFO] Starting manual command monitoring thread...")
    
    while True:
        try:
            if os.path.exists(manual_command_file):
                with open(manual_command_file, "r") as f:
                    f.seek(last_size)
                    new_lines = f.readlines()
                    last_size = f.tell()
                    
                    for line in new_lines:
                        line = line.strip()
                        if not line:
                            continue
                        
                        # Parse timestamp and command
                        if "] " in line:
                            command = line.split("] ")[-1].strip()
                            
                            # Check if manual control is allowed
                            if current_detection_status == "no human":
                                # Map Node-RED commands to Arduino commands
                                arduino_command = map_manual_command(command)
                                if arduino_command:
                                    send_to_arduino(arduino_command, "MANUAL")
                                    print(f"[MANUAL] Command '{command}' executed (Status: {current_detection_status})")
                            else:
                                print(f"[MANUAL] Command '{command}' BLOCKED (Status: {current_detection_status})")
            
            time.sleep(0.1)
            
        except Exception as e:
            print(f"[ERROR] Manual command monitoring: {e}")
            time.sleep(1)

def map_manual_command(command):
    """Map Node-RED commands to Arduino commands"""
    mapping = {
        "forward": "moveforward",
        "backward": "movebackward",
        "left": "turnleft", 
        "right": "turnright",
        "stop": "stopcar"
    }
    return mapping.get(command.lower())

def main():
    """Main function to run the unified controller"""
    print("[INFO] Starting Unified Arduino Controller...")
    
    if not initialize_arduino():
        print("[ERROR] Cannot start without Arduino connection!")
        return
    
    # Start monitoring threads
    detection_thread = threading.Thread(target=monitor_detection, daemon=True)
    manual_thread = threading.Thread(target=monitor_manual_commands, daemon=True)
    
    detection_thread.start()
    manual_thread.start()
    
    print("[INFO] All monitoring threads started. Press Ctrl+C to exit.")
    
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\n[EXIT] Shutting down unified controller...")
    finally:
        if arduino:
            arduino.close()
            print("[INFO] Arduino connection closed.")

if _name_ == "_main_":
    main()