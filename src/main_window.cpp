#include "main_window.hpp"
#include "./ui_main_window.h"
#include <QShowEvent>
#include <QCloseEvent>
#include <QMessageBox>
#include <QComboBox>
#include <QLineEdit>
#include <QFont>
#include <QDir>
#include <QFile>
#include <QSerialPortInfo>
#include <QScrollBar>
#include <QFileDialog>
#include "settings_dialog.hpp"
#include "status_widget.hpp"
#include "version.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_receivedFormat(VIEW_RECEIVED_AS_ASCII)
{
    ui->setupUi(this);

    setWindowTitle(SM_VERSION_NAME);

    m_cbxSerialPort = new QComboBox(this);
    ui->tbSerialPort->insertWidget(ui->actionToggleConnection, m_cbxSerialPort);

    m_editSendData = new QLineEdit(this);
    ui->tbSendData->insertWidget(ui->actionSendData, m_editSendData);

    ui->receivedDataView->setReadOnly(true);
    ui->receivedDataView->setWordWrapMode(QTextOption::WrapAnywhere);

#ifdef Q_OS_WIN
    ui->receivedDataView->setFont(QFont("Consolas", 10));
    m_editSendData->setFont(QFont("Consolas", 10));
#else
    ui->receivedDataView->setFont(QFont("Monospaced", 10));
    m_editSendData->setFont(QFont("Monospaced", 10));
#endif

    m_serialPort = new QSerialPort(this);
    m_serialPort->setBaudRate(QSerialPort::Baud115200);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    m_statusWidget = new StatusWidget(this);
    ui->statusbar->addWidget(m_statusWidget);

    connect(ui->actionExitApplication, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(showAboutQt()));
    connect(ui->actionAboutApplication, SIGNAL(triggered()), this, SLOT(showAboutApplication()));
    connect(ui->actionToggleConnection, SIGNAL(triggered()), this, SLOT(toggleConnection()));
    connect(ui->actionSendData, SIGNAL(triggered()), this, SLOT(sendSerialData()));
    connect(m_editSendData, SIGNAL(editingFinished()), this, SLOT(sendSerialData()));
    connect(ui->actionLoadSerialPorts, SIGNAL(triggered()), this, SLOT(loadSerialPorts()));
    connect(ui->actionPortSettings, SIGNAL(triggered()), this, SLOT(showSerialPortSettings()));
    connect(ui->actionClearReceivedData, SIGNAL(triggered()), this, SLOT(clearReceivedData()));
    connect(ui->actionSaveReceivedData, SIGNAL(triggered()), this, SLOT(saveReceivedData()));
    connect(ui->actionViewReceivedAsAscii, SIGNAL(triggered()), this, SLOT(changeReceivedFormatToAscii()));
    connect(ui->actionViewReceivedAsHex, SIGNAL(triggered()), this, SLOT(changeReceivedFormatToHex()));
    connect(m_serialPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
            this, SLOT(handleSerialPortError(QSerialPort::SerialPortError)));
    connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(readSerialData()));
    connect(m_cbxSerialPort, SIGNAL(currentIndexChanged(int)), this, SLOT(serialPortChanged(int)));
    connect(m_serialPort, SIGNAL(requestToSendChanged(bool)), m_statusWidget, SLOT(rtsChanged(bool)));
    connect(m_serialPort, SIGNAL(dataTerminalReadyChanged(bool)), m_statusWidget, SLOT(dtsChanged(bool)));

    loadSerialPorts();
    toggleControls(false);
    changeReceivedFormatToAscii();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_serialPort->isOpen())
    {
        QMessageBox::StandardButton response;
        response = QMessageBox::question(this, "Exit Application",
            "Serial port is open, are you sure you want to exit?",
            QMessageBox::Yes | QMessageBox::No);

        if (response == QMessageBox::No)
        {
            event->ignore();
            return;
        }
    }

    closeSerialPort();
    event->accept();
}

void MainWindow::showAboutQt()
{
    QMessageBox::aboutQt(this, "About Qt - "
        + QStringLiteral(SM_VERSION_NAME));
}

void MainWindow::showAboutApplication()
{
    QFile aboutFile(":/serialmonitor/html/about.html");
    if (!aboutFile.open(QIODevice::ReadOnly))
        return;

    QString html = aboutFile.readAll();

    html.replace("{APPLICATION_NAME}", SM_VERSION_NAME);
    html.replace("{APPLICATION_VERSION}", SM_VERSION_NUMBER);
    html.replace("{QT_VERSION}", QT_VERSION_NUMBER);
    html.replace("{APPLICATION_COPYRIGHT}", SM_VERSION_COPYRIGHT);

    QMessageBox::about(this, "About "
        + QStringLiteral(SM_VERSION_NAME), html);
}

void MainWindow::toggleConnection()
{
    if (m_serialPort->isOpen())
        closeSerialPort();
    else
        openSerialPort();
}

void MainWindow::sendSerialData()
{
    if (m_editSendData->text().isEmpty()
        || m_editSendData->text() == "")
        return;

    QByteArray data = parseSendData(m_editSendData->text());
    writeSerialData(data);
    m_editSendData->clear();
}

void MainWindow::loadSerialPorts()
{
    m_cbxSerialPort->clear();

    const auto infoList = QSerialPortInfo::availablePorts();
    if (infoList.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "There are no serial ports on this computer!");
        return;
    }

    for (const QSerialPortInfo &info : infoList)
    {
        m_cbxSerialPort->addItem(QIcon(":/serialmonitor/icons/plug.png"),
            info.portName(), info.portName());
    }
}

void MainWindow::handleSerialPortError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError)
    {
        QMessageBox::critical(this, "Error", m_serialPort->errorString());
        closeSerialPort();
    }
}

void MainWindow::readSerialData()
{
    const QByteArray data = m_serialPort->readAll();
    m_receivedData.append(data);
    ui->receivedDataView->insertPlainText(parseReceivedData(data, m_receivedFormat));
    QScrollBar *scrollBar = ui->receivedDataView->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void MainWindow::showSerialPortSettings()
{
    SettingsDialog dialog(this);

    SettingsDialog::PortSettings settings;
    settings.baudRate = m_serialPort->baudRate();
    settings.parity = m_serialPort->parity();
    settings.dataBits = m_serialPort->dataBits();
    settings.stopBits = m_serialPort->stopBits();
    settings.flowControl = m_serialPort->flowControl();

    dialog.setSettings(settings);

    QString title = m_serialPort->portName().isEmpty() ? "Settings"
                    : m_serialPort->portName() + " - Settings";
    dialog.setWindowTitle(title);

    if (dialog.exec() == QDialog::Accepted)
    {
        settings = dialog.settings();
        m_serialPort->setBaudRate(settings.baudRate);
        m_serialPort->setParity(settings.parity);
        m_serialPort->setDataBits(settings.dataBits);
        m_serialPort->setStopBits(settings.stopBits);
        m_serialPort->setFlowControl(settings.flowControl);
    }
}

void MainWindow::serialPortChanged(int index)
{
    if (index == -1)
        return;

    const QString portName = m_cbxSerialPort->itemData(index).toString();
    m_serialPort->setPortName(portName);
    toggleControls(m_serialPort->isOpen());
}

void MainWindow::clearReceivedData()
{
    m_receivedData.clear();
    ui->receivedDataView->clear();
}

void MainWindow::saveReceivedData()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Received Data",
        QDir::homePath(), "Data File (*.dat)");

    QFile dataFile(fileName);
    if (!dataFile.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, "Error", "Unable to write to file " + fileName);
        return;
    }

    dataFile.write(m_receivedData);
}

void MainWindow::changeReceivedFormatToAscii()
{
    ui->actionViewReceivedAsAscii->setChecked(true);
    ui->actionViewReceivedAsHex->setChecked(false);
    changeReceivedFormat(VIEW_RECEIVED_AS_ASCII);
}

void MainWindow::changeReceivedFormatToHex()
{
    ui->actionViewReceivedAsAscii->setChecked(false);
    ui->actionViewReceivedAsHex->setChecked(true);
    changeReceivedFormat(VIEW_RECEIVED_AS_HEX);
}

void MainWindow::openSerialPort()
{
    if (m_serialPort->portName().isEmpty())
        return;

    if (!m_serialPort->open(QIODevice::ReadWrite))
    {
        QMessageBox::critical(this, "Error", "Unable to open serial port "
            + m_serialPort->portName());
        return;
    }

    toggleControls(true);
}

void MainWindow::closeSerialPort()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
        toggleControls(false);
    }
}

void MainWindow::writeSerialData(const QByteArray &data)
{
    if (!m_serialPort->isOpen())
        return;

    m_serialPort->write(data);
}

void MainWindow::toggleControls(bool connected)
{
    if (connected)
    {
        ui->actionToggleConnection->setText("Disconnect");
        ui->actionToggleConnection->setToolTip("Disconnect");
        ui->actionToggleConnection->setIcon(QIcon(":/serialmonitor/icons/disconnect.png"));
    }
    else
    {
        ui->actionToggleConnection->setText("Connect");
        ui->actionToggleConnection->setToolTip("Connect");
        ui->actionToggleConnection->setIcon(QIcon(":/serialmonitor/icons/connect.png"));
    }

    ui->actionLoadSerialPorts->setEnabled(!connected);
    ui->actionPortSettings->setEnabled(!connected);
    ui->actionSendData->setEnabled(connected);
    m_cbxSerialPort->setEnabled(!connected);
    m_editSendData->setEnabled(connected);

    QSerialPortInfo info(*m_serialPort);
    m_statusWidget->setStatus(connected, info);
}

QByteArray MainWindow::parseSendData(const QString &str)
{
    QByteArray data;
    QString strData = str;
    strData.replace("<CR>", QChar(13));
    strData.replace("<LF>", QChar(10));
    data = strData.toLatin1();
    return data;
}

QString MainWindow::parseReceivedData(const QByteArray &data, ReceivedDataFormat format)
{
    QString formatedData;
    switch (format)
    {
    case VIEW_RECEIVED_AS_ASCII:
        formatedData = QString(data);
        break;
    case VIEW_RECEIVED_AS_HEX:
        formatedData = QString(data.toHex(' '));
        formatedData.append(' ');
        break;
    }
    return formatedData;
}

void MainWindow::changeReceivedFormat(ReceivedDataFormat format)
{
    m_receivedFormat = format;
    ui->receivedDataView->clear();
    ui->receivedDataView->insertPlainText(parseReceivedData(m_receivedData, m_receivedFormat));
    QScrollBar *scrollBar = ui->receivedDataView->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
