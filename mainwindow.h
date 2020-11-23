#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QByteArray>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

#include "setupseriadialog.h"
#include "serialthread.h"
#include "fingerprint.h"


namespace Ui {
class MainWindow;
}

class SerialThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);

    const QByteArray readSysParam = QByteArray::fromHex("ef01ffffffff0100030f0013"); //0xef01ffffffff0100030f0013
    const QByteArray colFingImg = QByteArray::fromHex("ef01ffffffff010003010005"); //0xef01fffffffe010003010005
    const QByteArray upFingImg = QByteArray::fromHex("ef01ffffffff0100030a000e");   // 0xef01ffffffff0100030b000e
   // QByteArray upFingImg = QByteArray::fromHex("ef01ffffffff0100030b000f");     // 0xef01ffffffff0100030b000f

    const QByteArray set57kBaudRate = QByteArray::fromHex("ef01ffffffff0100050e0406001e");
    const QByteArray set115kBaudRate = QByteArray::fromHex("ef01ffffffff0100050e040c0024");
    const QByteArray setDataBits256 = QByteArray::fromHex("ef01ffffffff0100050e0603001D");
    const QByteArray setDataBits128 = QByteArray::fromHex("ef01ffffffff0100050e0602001C");

    const QByteArray readValidTempNum = QByteArray::fromHex("ef01ffffffff0100031d0021");

    const QByteArray emptyFingerLibrary = QByteArray::fromHex("ef01ffffffff0100030d0011");


    void serialPortConnection();

    static void setSerialPort(QVector<int> settings);
    QVector<int> getSerialPort();

    static QSerialPort serial;
    unsigned int prevCmd;

//signals:

    //int serialReadInProgress();


    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QTimer readTimer;

    QLabel *myLabel;
    QLabel myLabel2, myLabel3, myLabel4, myLabel5;
    SetupSeriaDialog *setupSerial;
    bool enableBtnConnect;

    SerialThread *mSerialThread;

    void readSysParameters();


    //QByteArray imgBuff;

private slots:
     void readData();
     void dataWrited();
     void btnEnrolPressed();
     void btnUploadPressed();
     void btnExitPressed();
     void btnConnectPressed();
     void setupView();
     void setupSerialPort();
     void setupDialogHide();
     void drawFinger(const QByteArray&);
     void setValueProgressBar(int);
};

#endif // MAINWINDOW_H
                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
