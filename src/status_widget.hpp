#ifndef STATUS_WIDGET_HPP
#define STATUS_WIDGET_HPP

#include <QWidget>

namespace Ui {
class StatusWidget;
}

class QSerialPortInfo;

class StatusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatusWidget(QWidget *parent = nullptr);
    ~StatusWidget();

    void setStatus(bool connected, const QSerialPortInfo &info);

public slots:
    void rtsChanged(bool set);
    void dtsChanged(bool set);

private:
    Ui::StatusWidget *ui;
};

#endif // STATUS_WIDGET_HPP
