#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

// 使用QT命令空间
QT_BEGIN_NAMESPACE

class QLabel;
class QString;

namespace Ui {
class MainWindow;
}

// 结束QT命名空间
QT_END_NAMESPACE

class Console;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void readData();

    void handleError(QSerialPort::SerialPortError error);

    void on_sendPushButton_clicked();


private:
    void initActionsConnections();

private:
    enum
    {
        ShowReadData,
        ShowWriteData
    };
    void showStatusMessage(const QString &message,
                           const QString& styleSheet  = QStringLiteral("Color:black"));

    QByteArray str2Hex(const QString & str);
    char convertHexChar(char ch);
    QString formatInput(const QString& hexStr);

    void showReadOrWriteData(const QByteArray &data, uint8_t rdSelect = ShowReadData);

private:

    Ui::MainWindow *m_ui = nullptr;
    QLabel *m_status = nullptr;
    SettingsDialog *m_settings = nullptr;
    QSerialPort *m_serial = nullptr;
    QString m_str;
};
#endif // MAINWINDOW_H
