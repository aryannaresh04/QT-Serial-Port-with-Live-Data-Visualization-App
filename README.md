# QT Serial Port with Live Data Visualization

This Qt application provides real-time communication with serial devices, live data visualization using charts, and logging capabilities. It is built using Qt Creator and utilizes various Qt modules for GUI, serial communication, charting, and file handling.

## Features

- Connect to available serial ports
- Read incoming data in real-time
- Display live data using Qt Charts
- Log received data to file
- Simulate serial ports using `socat`
- Handshaking before data transfer
- Supports virtual ports for testing (e.g., `/dev/ttys01`, `/dev/ttys02`)

## Libraries and APIs Used

- Qt Creator: Development environment
- Qt Core:
  - `QSerialPort`: Serial communication
  - `QFile`, `QTextStream`: File I/O and logging
  - `QDateTime`: Timestamps
- Qt Widgets: GUI components
  - `QMainWindow`, `QComboBox`, `QPushButton`, `QTextEdit`, `QLineEdit`, `QMessageBox`
- Qt Charts (QtCharts):
  - `QChart`, `QLineSeries`, `QValueAxis`, `QChartView`
- Qt GUI:
  - `QTimer`: Periodic operations
- Command-line Utility:
  - `socat`: For creating and testing virtual serial ports

## Screenshots

Example screenshots from the application:

| Data Transfer via socat | Real-Time Chart View |
|-------------------------|----------------------|
| ![Socat Screenshot](screenshots/socat.png) | ![Chart Screenshot](screenshots/chartview.png) |


## How to Use

### Prerequisites

- Qt 6.x installed
- Qt Creator IDE or CMake
- Virtual serial ports or physical serial devices
- [socat](http://www.dest-unreach.org/socat/) installed (for simulation)

### Running the Application

1. Clone the repository:

   ```bash
   git clone https://github.com/aryannaresh04/QT-Serial-Port-with-Live-Data-Visualization-App.git
   cd QT-Serial-Port-with-Live-Data-Visualization-App
