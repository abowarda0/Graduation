import torch
import cv2
from datetime import datetime

# Load YOLOv5s model
model = torch.hub.load('yolov5', 'yolov5s', source='local')
model.conf = 0.5

cap = cv2.VideoCapture(0, cv2.CAP_V4L2)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

print("[INFO] Starting YOLOv5 detection...")

with open("realtime.txt", "w") as log_file, open("current_status.txt", "w") as status_file:
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        
        results = model(frame)
        df = results.pandas().xyxy[0]
        now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        person_found = False
        current_status = "no human"
        
        for _, row in df.iterrows():
            label = row['name']
            confidence = row['confidence']
            
            if label == 'person':
                person_found = True
                x = int(row['xmin'])
                y = int(row['ymin'])
                w = int(row['xmax'] - row['xmin'])
                h = int(row['ymax'] - row['ymin'])
                area = w * h
                
                if area > 22500:
                    current_status = "near"
                else:
                    current_status = "far"
                
                log_file.write(f"{now},{current_status},{confidence:.2f},{x},{y},{w},{h},{area}\n")
                log_file.flush()
                
                cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
                cv2.putText(frame, f"{current_status} {confidence:.2f}", (x, y-10),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
                break
        
        if not person_found:
            log_file.write(f"{now},no human,0,0,0,0,0,0\n")
            log_file.flush()
        
        # Write current status to status file for other scripts to read
        status_file.seek(0)
        status_file.write(current_status)
        status_file.truncate()
        status_file.flush()
        
        cv2.imshow("YOLOv5 Detection", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

cap.release()
cv2.destroyAllWindows()