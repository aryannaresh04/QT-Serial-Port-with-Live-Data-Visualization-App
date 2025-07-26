#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QFile>
#include <QTimer>

#include<QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonConnect_clicked();
    void on_pushButtonSend_clicked();
    void on_pushButtonSendFile_clicked();
    void on_pushButtonRecieveFile_clicked();
    void readSerialData();
    void startAutoTransfer();
    void sendNextLine();
    void setupChart();

private:
    Ui::MainWindow *ui;

    // Serial communication
    QSerialPort *serial;
    void listAvailablePorts();
    QString incomingBuffer;

    // File handling
    QFile receiveFile;
    QFile sendFile;
    QStringList sendLines;
    int currentLineIndex = 0;
    bool receivingToFile = false;
    //bool autoTransferTriggered = false;

    // Auto-send timer
    QTimer *autoSendTimer = nullptr;

    // Chart components
    QChartView *chartView = nullptr;
    QChart *chart = nullptr;
    QLineSeries *series = nullptr;
    QValueAxis *axisX = nullptr;
    QValueAxis *axisY = nullptr;
    qint64 chartX = 0;

};

#endif // MAINWINDOW_H
