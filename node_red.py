import sys
from datetime import datetime

# Get command from Node-RED
command = ' '.join(sys.argv[1:]).strip()

if command:
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    log_entry = f"[{timestamp}] {command}"
    
    # Print for Node-RED debug
    print(log_entry)
    
    # Write to manual command file for unified controller to read
    with open("/home/pi/manual_command.txt", "a") as f:
        f.write(log_entry + "\n")
    
    # Also log to terminal output file
    with open("/home/pi/terminal_output.log", "a") as f:
        f.write(log_entry + "\n")
else:
    print("[ERROR] No command received.")