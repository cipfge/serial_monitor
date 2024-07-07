#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QShowEvent;
class QCloseEvent;
class QComboBox;
class QLineEdit;
class StatusWidget;

enum ReceivedDataFormat
{
    VIEW_RECEIVED_AS_ASCII,
    VIEW_RECEIVED_AS_HEX
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent*);
    void showEvent(QShowEvent*);

private slots:
    void showAboutQt();
    void showAboutApplication();
    void toggleConnection();
    void sendSerialData();
    void loadSerialPorts();
    void handleSerialPortError(QSerialPort::SerialPortError);
    void readSerialData();
    void showSerialPortSettings();
    void serialPortChanged(int);
    void clearReceivedData();
    void saveReceivedData();
    void changeReceivedFormatToAscii();
    void changeReceivedFormatToHex();

private:
    Ui::MainWindow *ui;
    QComboBox *m_cbxSerialPort;
    QLineEdit *m_editSendData;
    QSerialPort *m_serialPort;
    QByteArray m_receivedData;
    StatusWidget *m_statusWidget;
    ReceivedDataFormat m_receivedFormat;

    void openSerialPort();
    void closeSerialPort();
    void writeSerialData(const QByteArray &data);
    void toggleControls(bool);

    QByteArray parseSendData(const QString &str);
    QString parseReceivedData(const QByteArray &data, ReceivedDataFormat format);
    void changeReceivedFormat(ReceivedDataFormat format);
};
#endif // MAINWINDOW_HPP
