#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    fillPortParameters();

    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::fillPortParameters()
{
    ui->cbxBaudRate->addItem(QStringLiteral("1200"), QSerialPort::Baud1200);
    ui->cbxBaudRate->addItem(QStringLiteral("1400"), QSerialPort::Baud2400);
    ui->cbxBaudRate->addItem(QStringLiteral("4800"), QSerialPort::Baud4800);
    ui->cbxBaudRate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->cbxBaudRate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->cbxBaudRate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->cbxBaudRate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->cbxBaudRate->setCurrentIndex(6);

    ui->cbxParity->addItem(tr("None"), QSerialPort::NoParity);
    ui->cbxParity->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->cbxParity->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->cbxParity->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->cbxParity->addItem(tr("Space"), QSerialPort::SpaceParity);
    ui->cbxParity->setCurrentIndex(0);

    ui->cbxDataBits->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->cbxDataBits->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->cbxDataBits->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->cbxDataBits->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->cbxDataBits->setCurrentIndex(3);

    ui->cbxStopBits->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->cbxStopBits->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->cbxStopBits->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    ui->cbxStopBits->setCurrentIndex(0);

    ui->cbxFlowControl->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->cbxFlowControl->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->cbxFlowControl->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
    ui->cbxFlowControl->setCurrentIndex(0);
}

SettingsDialog::PortSettings SettingsDialog::settings() const
{
    PortSettings settings;
    settings.baudRate = static_cast<QSerialPort::BaudRate>(
        ui->cbxBaudRate->itemData(ui->cbxBaudRate->currentIndex()).toInt());
    settings.parity = static_cast<QSerialPort::Parity>(
        ui->cbxParity->itemData(ui->cbxParity->currentIndex()).toInt());
    settings.dataBits = static_cast<QSerialPort::DataBits>(
        ui->cbxDataBits->itemData(ui->cbxDataBits->currentIndex()).toInt());
    settings.stopBits = static_cast<QSerialPort::StopBits>(
        ui->cbxStopBits->itemData(ui->cbxStopBits->currentIndex()).toInt());
    settings.flowControl = static_cast<QSerialPort::FlowControl>(
        ui->cbxFlowControl->itemData(ui->cbxFlowControl->currentIndex()).toInt());
    return settings;
}

void SettingsDialog::setSettings(const PortSettings &portSettings)
{
    ui->cbxBaudRate->setCurrentIndex(ui->cbxBaudRate->findData(portSettings.baudRate));
    ui->cbxParity->setCurrentIndex(ui->cbxParity->findData(portSettings.parity));
    ui->cbxDataBits->setCurrentIndex(ui->cbxDataBits->findData(portSettings.dataBits));
    ui->cbxStopBits->setCurrentIndex(ui->cbxStopBits->findData(portSettings.stopBits));
    ui->cbxFlowControl->setCurrentIndex(ui->cbxFlowControl->findData(portSettings.flowControl));
}
