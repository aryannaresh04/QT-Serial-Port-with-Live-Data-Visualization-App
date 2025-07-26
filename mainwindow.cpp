#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QPainter>
#include <QDir>

#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort(this))
{
    ui->setupUi(this);

    //setupChart();
    setupChart();
    listAvailablePorts();
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
}

MainWindow::~MainWindow() {
    if (serial->isOpen())
        serial->close();
    delete ui;
}

void MainWindow::listAvailablePorts() {
    ui->comboBoxPort->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        ui->comboBoxPort->addItem(info.portName());
    }
    // Manual ports
    ui->comboBoxPort->addItem("/dev/ttys001");
    ui->comboBoxPort->addItem("/dev/ttys002");
}

void MainWindow::on_pushButtonConnect_clicked() {
    if (serial->isOpen()) {
        serial->close();
        ui->pushButtonConnect->setText("Connect");
        ui->textEditRead->append("🔌 Disconnected.");
        return;
    }

    QString portName = ui->comboBoxPort->currentText();
    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);

    QString selectedFlow = ui->comboBoxFlowControl->currentText();
    if (selectedFlow == "None") {
        serial->setFlowControl(QSerialPort::NoFlowControl);
    } else if (selectedFlow == "Hardware (RTS/CTS)") {
        serial->setFlowControl(QSerialPort::HardwareControl);
    } else if (selectedFlow == "Software (XON/XOFF)") {
        serial->setFlowControl(QSerialPort::SoftwareControl);
    } else {
        ui->textEditRead->append("⚠ Unknown flow control option!");
        return;
    }

    if (serial->open(QIODevice::ReadWrite)) {
        ui->pushButtonConnect->setText("Disconnect");
        ui->textEditRead->append("✅ Handshaking complete. Connected to " + portName + " using " + selectedFlow);
        ui->textEditRead->append("📤 Starting auto-transfer...");
        QTimer::singleShot(300, this, &MainWindow::startAutoTransfer);
    } else {
        ui->textEditRead->append("❌ Failed to connect.");
    }
}

void MainWindow::on_pushButtonSend_clicked() {
    if (serial->isOpen()) {
        QString data = ui->lineEditSend->text() + "\n";
        serial->write(data.toUtf8());

        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        QString displayText = QString("<span style='color:green;'>[%1] [You] %2</span>").arg(timestamp, ui->lineEditSend->text());
        ui->textEditRead->append(displayText);
        ui->lineEditSend->clear();
    }
}

/*void MainWindow::readSerialData() {
    QByteArray rawData = serial->readAll();

    if (receivingToFile && receiveFile.isOpen()) {
        receiveFile.write(rawData);
    }

    incomingBuffer += QString::fromUtf8(rawData);
    while (incomingBuffer.contains('\n')) {
        int newlineIndex = incomingBuffer.indexOf('\n');
        QString line = incomingBuffer.left(newlineIndex).trimmed();
        incomingBuffer.remove(0, newlineIndex + 1);

        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        QString displayText = QString("<span style='color:blue;'>[%1] [Peer] %2</span>")
                                  .arg(timestamp, line);
        ui->textEditRead->append(displayText);

        // Attempt to convert line to a number and plot it
        bool ok;
        double value = line.toDouble(&ok);
        if (ok) {
            series->append(chartX++, value);

            // Scroll X-axis to keep last 100 points visible
            if (chartX > 100)
                axisX->setRange(chartX - 100, chartX);

            // Optionally auto-adjust Y range
            if (value < axisY->min() || value > axisY->max()) {
                axisY->setRange(qMin(axisY->min(), value), qMax(axisY->max(), value));
            }
        }
    }
}*/

/*void MainWindow::readSerialData() {
    QByteArray rawData = serial->readAll();

    if (receivingToFile && receiveFile.isOpen()) {
        receiveFile.write(rawData);
    }

    incomingBuffer += QString::fromUtf8(rawData);
    while (incomingBuffer.contains('\n')) {
        int newlineIndex = incomingBuffer.indexOf('\n');
        QString line = incomingBuffer.left(newlineIndex).trimmed();
        incomingBuffer.remove(0, newlineIndex + 1);

        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        QString displayText = QString("<span style='color:blue;'>[%1] [Peer] %2</span>")
                                  .arg(timestamp, line);
        ui->textEditRead->append(displayText);
    }
}this is the 1st iteration*/

void MainWindow::readSerialData() {
    QByteArray rawData = serial->readAll();

    if (receivingToFile && receiveFile.isOpen()) {
        receiveFile.write(rawData);
    }

    incomingBuffer += QString::fromUtf8(rawData);
    while (incomingBuffer.contains('\n')) {
        int newlineIndex = incomingBuffer.indexOf('\n');
        QString line = incomingBuffer.left(newlineIndex).trimmed();
        incomingBuffer.remove(0, newlineIndex + 1);

        // Log the received line to the text area
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        QString displayText = QString("<span style='color:blue;'>[%1] [Peer] %2</span>")
                                  .arg(timestamp, line);
        ui->textEditRead->append(displayText);

        // Try to parse the line as a number for charting
        bool ok;
        double value = line.toDouble(&ok);
        if (ok && series) {  // Only plot if the series exists and parsing succeeded
            series->append(chartX++, value);
            // Scroll X-axis to keep last 100 points visible
            if (chartX > 100)
                axisX->setRange(chartX - 100, chartX);
            // Optionally auto-adjust Y range
            if (value < axisY->min() || value > axisY->max())
                axisY->setRange(std::min(axisY->min(), value), std::max(axisY->max(), value));
        }
    }
}


void MainWindow::on_pushButtonSendFile_clicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select File to Send");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Cannot open file for reading.");
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    if (serial->isOpen()) {
        serial->write(fileData);
        ui->textEditRead->append("📤 Sent file: " + filePath + " (" + QString::number(fileData.size()) + " bytes)");
    } else {
        QMessageBox::warning(this, "Error", "Serial port is not open.");
    }

    ui->lineEditFilePath->setText(filePath);
}

void MainWindow::on_pushButtonRecieveFile_clicked() {
    if (!receivingToFile) {
        QString filePath = QFileDialog::getSaveFileName(this, "Save Received Data");
        if (filePath.isEmpty()) return;

        receiveFile.setFileName(filePath);
        if (!receiveFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QMessageBox::warning(this, "Error", "Cannot open file for writing.");
            return;
        }

        receivingToFile = true;
        ui->textEditRead->append("💾 Started saving to: " + filePath);
        ui->pushButtonRecieveFile->setText("Stop Saving");
    } else {
        receiveFile.close();
        receivingToFile = false;
        ui->textEditRead->append("🛑 Stopped saving received data.");
        ui->pushButtonRecieveFile->setText("Receive File");
    }
}

void MainWindow::startAutoTransfer() {
    QString portName = serial->portName();

    QString sendFilePath, receiveFilePath;
    if (portName.contains("ttys001")) {
        sendFilePath = "/Users/Aryan/Documents/SerialPortTxt/send1.txt";
        receiveFilePath = "/Users/Aryan/Documents/SerialPortTxt/received1.txt";
    } else if (portName.contains("ttys002")) {
        sendFilePath = "/Users/Aryan/Documents/SerialPortTxt/send2.txt";
        receiveFilePath = "/Users/Aryan/Documents/SerialPortTxt/received2.txt";
    } else {
        ui->textEditRead->append("⚠ Unknown port, cannot auto-transfer.");
        return;
    }

    // Load send file
    sendFile.setFileName(sendFilePath);
    if (!sendFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->textEditRead->append("❌ Failed to open send file.");
        return;
    }

    QTextStream in(&sendFile);
    sendLines = in.readAll().split('\n', Qt::SkipEmptyParts);
    sendFile.close();
    currentLineIndex = 0;

    // Prepare receive file
    receiveFile.setFileName(receiveFilePath);
    if (!receiveFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        ui->textEditRead->append("❌ Failed to open receive file.");
        return;
    }
    receivingToFile = true;

    ui->textEditRead->append("💾 Saving received data to: " + receiveFilePath);
    ui->textEditRead->append("📤 Sent data from: " + sendFilePath);

    // Start periodic sending
    autoSendTimer = new QTimer(this);
    connect(autoSendTimer, &QTimer::timeout, this, &MainWindow::sendNextLine);
    autoSendTimer->start(1000); // 1 second interval
}

void MainWindow::sendNextLine() {
    if (!serial->isOpen() || currentLineIndex >= sendLines.size()) return;

    QString line = sendLines[currentLineIndex++] + "\n";
    serial->write(line.toUtf8());

    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString displayText = QString("<span style='color:green;'>[%1] [You] %2</span>").arg(timestamp, line.trimmed());
    ui->textEditRead->append(displayText);
}

void MainWindow::setupChart() {
    series = new QLineSeries();
    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);

    axisX = new QValueAxis;
    axisX->setLabelFormat("%d");
    axisX->setTitleText("Sample");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisY = new QValueAxis;
    axisY->setRange(0, 100); // Adjust as needed
    axisY->setTitleText("Value");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->verticalLayoutChart->addWidget(chartView);
}


/*void MainWindow::setupChart() {
    series = new QtCharts::QLineSeries();

    chart = new QtCharts::QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Live Serial Data");

    axisX = new QtCharts::QValueAxis;
    axisX->setLabelFormat("%d");
    axisX->setTitleText("Time");

    axisY = new QtCharts::QValueAxis;
    axisY->setRange(0, 100);  // Set range based on expected data
    axisY->setTitleText("Value");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayoutChart->addWidget(chartView);  // Assuming you added a vertical layout in the UI
}*/
