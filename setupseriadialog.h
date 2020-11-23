#ifndef SETUPSERIADIALOG_H
#define SETUPSERIADIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSerialPort>

namespace Ui {
class SetupSeriaDialog;
}

class SetupSeriaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupSeriaDialog(QWidget *parent = 0);
    ~SetupSeriaDialog();
    static QSerialPort availableSettings;

private:
    Ui::SetupSeriaDialog *ui;
    void OnAcceptButton();
    void OnCanceltButton();

    QMap<QString, QSerialPort::Parity> mapParity;
    QMap<QString, QSerialPort::FlowControl> mapFlowControl;
    QMap<QString, QSerialPort::StopBits> mapStopBits;
    QStringList *currentSetting;
    QVector<int> currentSetting_;

};

#endif // SETUPSERIADIALOG_H
