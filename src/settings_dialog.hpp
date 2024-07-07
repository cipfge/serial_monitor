#ifndef SETTINGS_DIALOG_HPP
#define SETTINGS_DIALOG_HPP

#include <QDialog>
#include <QSerialPort>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    struct PortSettings
    {
        qint32 baudRate;
        QSerialPort::Parity parity;
        QSerialPort::DataBits dataBits;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
    };

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    PortSettings settings() const;
    void setSettings(const PortSettings &portSettings);

private:
    Ui::SettingsDialog *ui;

    void fillPortParameters();
};

#endif // SETTINGS_DIALOG_HPP
