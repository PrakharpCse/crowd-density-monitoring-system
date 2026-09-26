# Portable Crowd Density Monitoring System

An IoT-based crowd monitoring system that detects entry and exit activity using an ESP32 and IR sensors, sends the data to a Flask backend, and displays real-time crowd information through a web dashboard.

## Features

- Real-time entry and exit counting
- Live crowd calculation
- ESP32 + IR sensor integration
- Flask REST API
- Interactive web dashboard
- Crowd history logging
- Crowd capacity indicator
- Data visualization using Chart.js
- CSV/history download
- API-key based ESP32 communication

## Technologies Used

- ESP32
- IR Sensors
- Python
- Flask
- HTML5
- CSS3
- JavaScript
- Chart.js

## Project Structure

```text
Crowd Dashboard/
├── app.py
├── requirements.txt
├── README.md
├── .gitignore
├── esp32/
│   └── crowd_counter.ino
├── static/
│   ├── script.js
│   ├── style.css
│   └── images/
│       └── img1.jpeg
└── templates/
    └── index.html