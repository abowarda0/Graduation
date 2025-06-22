# Smart Pneumatic Car Bumper

An AI-Vision Enabled Pre-Impact Safety System developed as a graduation project for the Mechatronics Department at Mansoura University.

##  Project Overview

The Smart Pneumatic Car Bumper is an active vehicle safety system designed to mitigate the effects of low-speed collisions. By integrating real-time computer vision with a rapid-deployment pneumatic bumper, the system can detect potential obstacles, such as pedestrians, and physically extend a protective cushion before an impact occurs. This project addresses the gap in vehicle safety for urban, low-speed environments where collisions are frequent but often overlooked by traditional safety systems like airbags.

##  Key Features

- **Real-time Pedestrian Detection**: Utilizes the YOLOv5 AI model to identify pedestrians in the vehicle's path
- **Automatic Bumper Deployment**: Deploys a pneumatic bumper to absorb impact force when a collision is predicted
- **Dual Control Modes**: Features both a manual control mode via a web dashboard and a fully autonomous safety mode
- **Intelligent Safety Override**: The autonomous system automatically stops the vehicle and deploys the bumper if a threat is detected, overriding any manual user input to ensure safety

##  Hardware Components

### Control & Processing
- **Control Unit**: Raspberry Pi 4 Model B
- **Microcontroller**: Arduino UNO
- **Vision**: Standard USB Webcam

### Motion & Power
- **Motors**: 2 x 12V DC Wiper Motors
- **Motor Drivers**: 2 x BTS7960B H-Bridge
- **Power**: 2 x 12V 9Ah Batteries

### Pneumatic System
- Air Compressor & Air Tank
- Pneumatic Cylinder with Fast Return Spring
- 5/2 Solenoid Valve
- **Control Circuit**: 2-Channel Relay Module, Delay Timer

### Mechanical Components
- Custom Steel Chassis
- 4 Rubber Wheels
- UCP 204 Bearings
- 428H Chains & Gears

##  Software & Technologies

- **OS**: Raspberry Pi OS
- **Primary Languages**: Python, C++ (Arduino)
- **AI Model**: YOLOv5s
- **Dashboard**: Node-RED with node-red-dashboard

### Core Libraries
- PyTorch (for YOLOv5)
- OpenCV (for video capture)
- pyserial (for RPi-Arduino communication)

## System Setup & Installation

### 1. Raspberry Pi Setup

Clone the repository:
```bash
git clone https://github.com/your-username/Graduation.git
cd Graduation
```

Set up Python environment:
```bash
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```
*Note: requirements.txt should include torch, opencv-python-headless, pandas, pyserial*

Download YOLOv5: The detection script assumes YOLOv5 is in a local directory. Follow official YOLOv5 instructions to set it up.

### 2. Arduino Setup

1. Open the Arduino IDE
2. Connect the Arduino UNO to your computer
3. Select the correct Board ("Arduino Uno") and Port from the Tools menu
4. Open the `arduino_firmware/arduino_firmware.ino` file from this repository
5. Click the "Upload" button to flash the firmware to the Arduino

### 3. Node-RED Setup

Install the dashboard node:
```bash
npm install node-red-dashboard
```

1. Start Node-RED and open the editor in your browser (e.g., `http://<raspberry_pi_ip>:1880`)
2. Go to Menu > Import and import the `nodered_flow.json` file from this repository
3. Click "Deploy"

##  How to Run the System

1. **Power On**: Ensure all hardware components, including the Raspberry Pi, Arduino, motors, and pneumatic system, are powered on correctly

2. **Start the AI Detection**: Open a terminal on the Raspberry Pi, activate the Python virtual environment, and run the detection script:
   ```bash
   source venv/bin/activate
   python detection.py
   ```

3. **Start the Unified Controller**: Open a second terminal, activate the environment, and run the controller script:
   ```bash
   source venv/bin/activate
   python unified_controller.py
   ```

4. **Use the Dashboard**: Access the manual control dashboard in a web browser at `http://<raspberry_pi_ip>:1880/ui`

##  Performance & Results

- **System Response Time**: The average end-to-end response time from AI detection to bumper deployment is under 400 milliseconds
- **Effective Reaction Distance**: At a typical parking speed of 5 km/h, this quick response time translates to a vehicle travel distance of less than 50 cm before the system reacts
- **Impact Reduction**: The system is designed to reduce force transmission to car body, and testing showed a significant reduction in impact forces

##  Future Work

- **AI & Latency Optimization**: Reduce system latency by converting the model to TensorFlow Lite (TFLite) and using a hardware accelerator like Google Coral
- **Sensor Fusion**: Integrate ultrasonic sensors for early detection and improved reliability in all weather conditions
- **Advanced Control**: Replace the simpler relay-only system with more advanced microcontroller logic for smoother control
- **Material Science**: Use lighter materials for commercial applications

##  Contributors & Team

This project was developed by:
- **Mohamed Samir Soliman Abo Warda**
- **Raghad Salah Mahmoud**

##  Acknowledgments

We extend our sincere gratitude to our supervisors, **Dr. Abeer Tawkol** and **Eng. Ahmed Ramadan**, for their invaluable guidance and support throughout this project.

---

*Developed at Mansoura University - Mechatronics Department*
