#include "setupseriadialog.h"
#include "ui_setupseriadialog.h"
#include "mainwindow.h"

#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QString>
#include <QStringList>

SetupSeriaDialog::SetupSeriaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupSeriaDialog)
{
    ui->setupUi(this);

    QStringList *strlBaudRate = new QStringList;
    QStringList *strlDataBits = new QStringList;
    QStringList *strlStopBits = new QStringList;
    QStringList *strlPort = new QStringList;
    QStringList *strlParity = new QStringList;
    QStringList *strlFlowControl = new QStringList;

    currentSetting = new QStringList;

    bool enableButtonFlag;

    connect(ui->btnAccept, &QPushButton::pressed, this, &SetupSeriaDialog::OnAcceptButton);
    connect(ui->btnCancel, &QPushButton::pressed, this, &SetupSeriaDialog::OnCanceltButton);

    enableButtonFlag = false;

    *strlBaudRate << "9600" << "19200" << "28800" << "38400" << "48000" << "57600"
                  << "67200" << "76800" << "86400" << "96000" << "105600" << "115200" ;

    *strlDataBits << "5" << "6" << "7" << "8" << "9";
    *strlStopBits << "1" << "1.5" << "2";
    *strlParity   << "No Parity" << "Even" << "Odd" << "Space" << "Mark";
    *strlFlowControl << "None" << "Software Control" << "Hardware Control";

    ui->cbxBaudRate->addItems(*strlBaudRate);
    ui->cbxDataBits->addItems(*strlDataBits);
    ui->cbxStopBits->addItems(*strlStopBits);
    ui->cbxParity->addItems(*strlParity);
    ui->cbxFlowCtrl->addItems(*strlFlowControl);

    ui->cbxBaudRate->setCurrentIndex(strlBaudRate->indexOf("115200"));
    ui->cbxDataBits->setCurrentIndex(strlDataBits->indexOf("8"));
    ui->cbxStopBits->setCurrentIndex(strlStopBits->indexOf("1"));
    ui->cbxParity->setCurrentIndex(strlParity->indexOf("No Parity"));
    ui->cbxFlowCtrl->setCurrentIndex(strlFlowControl->indexOf("None"));

    mapStopBits[strlStopBits->at(0)] = QSerialPort::OneStop;
    mapStopBits[strlStopBits->at(1)] = QSerialPort::OneAndHalfStop;
    mapStopBits[strlStopBits->at(2)] = QSerialPort::TwoStop;

    mapParity[strlParity->at(0)] = QSerialPort::NoParity;
    mapParity[strlParity->at(1)] = QSerialPort::EvenParity;
    mapParity[strlParity->at(2)] = QSerialPort::OddParity;
    mapParity[strlParity->at(3)] = QSerialPort::SpaceParity;
    mapParity[strlParity->at(4)] = QSerialPort::MarkParity;

    mapFlowControl[strlFlowControl->at(0)] = QSerialPort::NoFlowControl;
    mapFlowControl[strlFlowControl->at(1)] = QSerialPort::SoftwareControl;
    mapFlowControl[strlFlowControl->at(2)] = QSerialPort::HardwareControl;

    QList<QSerialPortInfo> portInfoList = QSerialPortInfo::availablePorts();

    if(portInfoList.count() == 1){
        qDebug() << "QSerialPortInfo: Un puerto encontrado:";
        enableButtonFlag = true;
    }
    else if(portInfoList.count() > 1){
        qDebug() << "QSerialPortInfo: " << portInfoList.count() << " puertos encontrados";
        enableButtonFlag = true;
    }
    else {
        qDebug() << "QSerialPortInfo: No se encontraron puertos";
    }

    // Setear puerto
    foreach(QSerialPortInfo info, portInfoList){
        qDebug() << info.portName() << "\n"
                 << info.description() << "\n"
                 << info.manufacturer() << "\n";
                 //   serial.setPort(info);
        *strlPort << info.portName();
    }
    ui->cbxPort->addItems(*strlPort);

    qDebug() << "QSerialPortInfo: Nombre de puerto serie seteado";

    if(enableButtonFlag == true){
        ui->btnCancel->setEnabled(true);
        ui->btnAccept->setEnabled(true);
    }
    else{
        ui->btnCancel->setEnabled(true);
        ui->btnAccept->setEnabled(false);
    }

}

SetupSeriaDialog::~SetupSeriaDialog()
{
    delete ui;
}

void SetupSeriaDialog::OnAcceptButton(){

    QString portNameLabel = ui->cbxPort->currentText();
    if(portNameLabel.startsWith("COM") == true){
        portNameLabel.remove("COM");
        currentSetting_.push_back(portNameLabel.toInt());
        currentSetting_.push_back(ui->cbxBaudRate->currentText().toInt());
        currentSetting_.push_back(ui->cbxDataBits->currentText().toInt());
        currentSetting_.push_back(mapStopBits.value(ui->cbxStopBits->currentText()));
        currentSetting_.push_back(mapFlowControl.value(ui->cbxFlowCtrl->currentText()));
        currentSetting_.push_back(mapParity.value(ui->cbxParity->currentText()));

        qDebug() << "QSerialPortInfo: Nombre de puerto serie seteado" << currentSetting_[0];
    }
    else{
        qDebug() << "Ha ocurrido un error estableciendo el puerto";

    }

    qDebug() << "Port Number" << currentSetting_[0];
    qDebug() << "Baud Rate: " << currentSetting_[1];
    qDebug() << "Data bits: " << currentSetting_[2];
    qDebug() << "Stop bits: " << currentSetting_[3];
    qDebug() << "Flow Control: " << currentSetting_[4];
    qDebug() << "Parity: " << currentSetting_[5];

    MainWindow::setSerialPort(currentSetting_);
    SetupSeriaDialog::hide();
    accept();

}


void SetupSeriaDialog::OnCanceltButton(){
    SetupSeriaDialog::hide();
}
