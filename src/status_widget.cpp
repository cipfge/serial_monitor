#include "status_widget.hpp"
#include "ui_status_widget.h"
#include <QSerialPortInfo>

StatusWidget::StatusWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusWidget)
{
    ui->setupUi(this);
}

StatusWidget::~StatusWidget()
{
    delete ui;
}

void StatusWidget::setStatus(bool connected, const QSerialPortInfo &info)
{
    QString statusText;
    QString statusIcon;

    if (connected)
    {
        statusText = "Connected";
        statusIcon = ":/serialmonitor/icons/connect.png";
    }
    else
    {
        statusText = "Disconnected";
        statusIcon = ":/serialmonitor/icons/disconnect.png";
    }

    ui->lbStatusIcon->setPixmap(QPixmap(statusIcon));
    ui->lbStatusText->setText(statusText);
    ui->lbSerialPort->setText(info.portName());
    ui->lbSeialPortInfo->setText(info.manufacturer()
        + " - " + info.description());
}

void StatusWidget::rtsChanged(bool set)
{
    QString path = set ? ":/serialmonitor/icons/status_green.png"
        : ":/serialmonitor/icons/status_red.png";
    ui->lbRtsStatus->setPixmap(QPixmap(path));
}

void StatusWidget::dtsChanged(bool set)
{
    QString path = set ? ":/serialmonitor/icons/status_green.png"
        : ":/serialmonitor/icons/status_red.png";
    ui->lbDtsStatus->setPixmap(QPixmap(path));
}
