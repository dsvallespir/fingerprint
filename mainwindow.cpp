
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setupseriadialog.h"
#include "serialthread.h"

#include <QSerialPortInfo>
#include <QtSerialPort/QSerialPort>

#include <QList>
#include <QDebug>

#include <QImage>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QMap>
#include <QMessageBox>

QSerialPort MainWindow::serial;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupView();
    setupSerial = new SetupSeriaDialog;


    mSerialThread = new SerialThread(this);

    connect(mSerialThread, &SerialThread::signalOne, this, &MainWindow::drawFinger);

    connect(ui->btnConnect, &QPushButton::pressed, this, &MainWindow::btnConnectPressed);
    connect(ui->btnEnrol, &QPushButton::pressed, this, &MainWindow::btnEnrolPressed);
    connect(ui->btnUpload, &QPushButton::pressed, this, &MainWindow::btnUploadPressed);
    connect(ui->btnExit, &QPushButton::pressed, this, &MainWindow::btnExitPressed);

    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::setupSerialPort);
    connect(setupSerial, &SetupSeriaDialog::accepted, this, &MainWindow::setupDialogHide);
    connect(mSerialThread, &SerialThread::loadingSerialBuffer, this, &MainWindow::setValueProgressBar);

    if(serial.portName() == 0){
        enableBtnConnect = false;
    }
    else
    {
        enableBtnConnect = true;
    }

}

/* A revisar prolijidad */

MainWindow::~MainWindow(){

    delete ui;
    serial.close();
    setupSerial->close();
    qDebug() << "QSerialPort: Puerto serie cerrado";

    qDebug() << "App: Destruyendo objetos antes de salir..";
    delete myLabel;
}


/* A revisar prolijidad */

void MainWindow::setupView(){

    QPushButton *btnEnrol = new QPushButton;
    QPushButton *btnUpload = new QPushButton;
    QPushButton *btnExit = new QPushButton;

    QHBoxLayout *layout = new QHBoxLayout;
    myLabel = new QLabel;

    layout->addWidget(myLabel);
    layout->addWidget(btnEnrol);
    layout->addWidget(btnUpload);
    layout->addWidget(btnExit);

    ui->btnConnect->setText("Connect");
    ui->btnEnrol->setText("Enrol");
    ui->btnUpload->setText("View Image");
    ui->btnExit->setText("Exit");

    ui->btnConnect->setEnabled(false);
    ui->btnEnrol->setEnabled(false);
    ui->btnUpload->setEnabled(false);

    ui->progressBar->setValue(0);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(73216);

}


/* Parece estar bien */
void MainWindow::setupSerialPort(){

    setupSerial->show();

}


/* Podría funcionar */

void MainWindow::setSerialPort(QVector<int> settings){

    qDebug() << "Port Number" << settings[0];
    qDebug() << "Baud Rate: " << settings[1];
    qDebug() << "Data bits: " << settings[2];
    qDebug() << "Stop bits: " << settings[3];
    qDebug() << "Flow Control: " << settings[4];
    qDebug() << "Parity: " << settings[5];

    serial.setPortName("COM" + QString::number(settings[0]));
    serial.setBaudRate((QSerialPort::BaudRate)settings[1]);
    serial.setDataBits((QSerialPort::DataBits)settings[2]);
    serial.setStopBits((QSerialPort::StopBits)settings[3]);
    serial.setFlowControl((QSerialPort::FlowControl)settings[4]);
    serial.setParity((QSerialPort::Parity)settings[5]);

    qDebug() << "Puerto cargado" << serial.portName();
}


/* A completar */

QVector<int> MainWindow::getSerialPort(){

    QVector<int> v;

    v.push_back(0);

    return v;
}


void MainWindow::dataWrited(){

    qDebug() << "Se han enviado los datos";

}

void MainWindow::readData(){

    mSerialThread->start();
}

void MainWindow::btnEnrolPressed(){

    qDebug() << "Ha presionado el botón de Enrolar";

    serial.write(colFingImg);
    SerialThread::setFingerPrintCommand(GEN_IMG);

    qDebug() << "Serial: Se envíado la petición 0xef01ffffffff010003010005";
}

void MainWindow::btnUploadPressed(){

    qDebug() << "Ha presionado el botón Obtener Imagen";

    serial.write(upFingImg);
    SerialThread::setFingerPrintCommand(UP_IMG);


    //    readTimer.start(9000);

    //acá deberíamos lanzar los threads
    // puede ser uno para atender el puerto serie
    // otro para cargar la progress bar
    //
    mSerialThread->run();

    qDebug() << "Serial: Se envíado la petición 0xef01ffffffff0100030a000e";

}

void MainWindow::btnExitPressed(){

    qDebug() << "Ha presionado el botón Salir";
    this->close();

}

void MainWindow::btnConnectPressed(){

    if(serial.isOpen() == false){

        qDebug() << "Puerto: " <<  serial.portName();
        qDebug() << "Baud Rate: " << serial.baudRate();
        qDebug() << "Data bits: " << serial.dataBits();
        qDebug() << "Stop bits: " << serial.stopBits();
        qDebug() << "Flow Control: " << serial.flowControl();

          // Abrir Puerto
        if(serial.open(QIODevice::ReadWrite)) {
            qDebug() << "QSerialPort: Puerto serie abierto";

            // Señales a utilizar
            connect(&serial, &QSerialPort::bytesWritten, this, &MainWindow::dataWrited);
            ui->btnConnect->setText("Desconectar");
            ui->btnEnrol->setEnabled(true);
            ui->btnUpload->setEnabled(true);

            //QProgressBar.setValue();
        }
        else {
            qDebug() << "QSerialPort: " << serial.errorString();
            QMessageBox::critical(this, "Error al conectar","No se ha configurado el puerto serie",QMessageBox::Ok);
        }
    }
    else if(serial.isOpen() == true){

        serial.close();

        qDebug() << "QSerialPort: Puerto serie cerrado";

        // Señales a utilizar
        connect(&serial, &QSerialPort::bytesWritten, this, &MainWindow::dataWrited);
        ui->btnConnect->setText("Conectar");
        ui->btnEnrol->setEnabled(false);
        ui->btnUpload->setEnabled(false);
    }


}

//int MainWindow::serialReadInProgress(){

//}


void MainWindow::setupDialogHide(){

    if(serial.portName() == 0){
        ui->btnConnect->setEnabled(false);
    }
    else{
        ui->btnConnect->setEnabled(true);

    }
}


void MainWindow::drawFinger(const QByteArray& imgBuff){

    QByteArray *imgBuffRev = new QByteArray;
    for(unsigned int i = imgBuff.count() ; i > 0; i--){
        imgBuffRev->append(imgBuff.at(i-1));
    }

    for(unsigned int i = 0 ; i*256 < (unsigned int)imgBuffRev->count(); i++){
        imgBuffRev->remove(i*256,2);
    }

    qDebug() << "ImgBuff: " << imgBuff.count() << " bytes.";
    qDebug() << "ImgBuffRev: " << imgBuff.count() << " bytes.";

    unsigned char *dataImgBuff = (unsigned char*)&imgBuffRev->data()[0];

    QImage *myImage = new QImage(dataImgBuff, 256, 288, QImage::Format_Grayscale8);
    ui->myLabel->setPixmap(QPixmap::fromImage(*myImage));
    ui->myLabel->show();

    mSerialThread->terminate();

  //  delete imgBuff;
    delete imgBuffRev;
//    delete buffRecv;

}


void MainWindow::setValueProgressBar(int value){

    ui->progressBar->setValue(value);

}
