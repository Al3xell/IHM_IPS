#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QSerialPort>
#include <QDialog>

namespace Ui {
class ConnectDialog;
}

class QIntValidator;

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = nullptr);
    ~ConnectDialog();

    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
    };

    Settings settings() const;

private slots:
    void showPortInfo(int idx);
    void apply();
    void cancel();

private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();

private:
    Ui::ConnectDialog *ui = nullptr;
    Settings currentSettings;
    QIntValidator *intValidator = nullptr;
};


#endif // CONNECTDIALOG_H
