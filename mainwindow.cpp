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

    connect(ui->actionConnect, SIGNAL(triggered()), connectDialog, SLOT(show()));
    connect(connectDialog, SIGNAL(accepted()), this, SLOT(connectPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnectPort()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(onExit()));
    connect(serialPort, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(ui->btnValidate, &QPushButton::clicked, this, &MainWindow::okPosition);
    connect(ui->btnStop, &QPushButton::clicked, this, &MainWindow::stopPosition);
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(serialReceived()));

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
    if(serialPort->open(QIODevice::ReadWrite)) {
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
    QByteArray ok = ui->inputPosition->text().toLocal8Bit();
    serialPort->write(ok);
    serialPort->readAll();
}

void MainWindow::stopPosition()
{
    QByteArray stop = ui->positionOrdered->text().toLocal8Bit();
    serialPort->write(stop);
}

void MainWindow::serialReceived()
{
    QByteArray ba;
    ba=serialPort->readAll();
    qInfo()<<ba;
}
MainWindow::~MainWindow()
{
    delete ui;
}

