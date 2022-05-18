#include <QMessageBox>
#include <QSerialPort>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serialPort(new QSerialPort(this))
{
    ui->setupUi(this);
    this->setWindowTitle("IHM IPS");
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    connectDialog = new ConnectDialog();

    connect(ui->actionConnect, &QAction::triggered, connectDialog, &ConnectDialog::show);
    connect(connectDialog, &QDialog::accepted, this, &MainWindow::connectPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::disconnectPort);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onExit);
    connect(serialPort, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(ui->btnValidate, &QPushButton::clicked, this, &MainWindow::okPosition);
    connect(ui->btnStop, &QPushButton::clicked, this, &MainWindow::stopPosition);
    connect(serialPort,&QSerialPort::readyRead,this,&MainWindow::serialReceived);

}


void MainWindow::onExit()
{
    QMessageBox warning;
    warning.setText("Are you sure ?");
    warning.setIcon(QMessageBox::Warning);
    warning.addButton(QMessageBox::Ok);
    warning.addButton(QMessageBox::Cancel);
    int answer = warning.exec();
    if(answer == 0x00000400)
    {
        this->close();
    }
}

void MainWindow::connectPort()
{
    ConnectDialog::Settings p = connectDialog->settings();
    serialPort->setPortName(p.name);
    serialPort->setBaudRate(p.baudRate);
    serialPort->setDataBits(p.dataBits);
    serialPort->setParity(p.parity);
    serialPort->setStopBits(p.stopBits);
    serialPort->setFlowControl(p.flowControl);
    serialPort->open(QIODevice::ReadWrite);
    if(serialPort->isOpen()) {

        serialPort->setDataTerminalReady(false);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        QMessageBox info;
        info.setText("Connect to : "+p.name);
        info.setIcon(QMessageBox::Information);
        info.addButton(QMessageBox::Ok);
        info.exec();
        this->setWindowTitle("IHM IPS : " + p.name);
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), serialPort->errorString());
        this->setWindowTitle("IHM IPS");
    }
}

void MainWindow::disconnectPort()
{

    if(serialPort->isOpen())
    {
        serialPort->close();
        this->setWindowTitle("IHM IPS");
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
        serialPort->close();

        QMessageBox info;
        info.setText("Disconnected");
        info.setIcon(QMessageBox::Information);
        info.addButton(QMessageBox::Ok);
        info.exec();
    }

}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serialPort->errorString());
        disconnectPort();
    }
}

void MainWindow::okPosition()
{
    if(serialPort->isOpen()){
        QString okPosition = "ok:"+ui->positionOrdered->text();
        QByteArray ok = okPosition.toUtf8();
        serialPort->write(ok);
    }
}

void MainWindow::stopPosition()
{
    if(serialPort->isOpen()){
        QString stopPosition = "stop:"+ui->positionOrdered->text();
        QByteArray stop = stopPosition.toUtf8();
        serialPort->write(stop);
    }
}
void MainWindow::serialReceived()
{
    serialPort->bytesAvailable();
    QByteArray ba=serialPort->readAll();
    QString text = QString(ba);
    QStringList list = text.split(":");
    qInfo() << ba;
}
MainWindow::~MainWindow()
{
    delete ui;
}

