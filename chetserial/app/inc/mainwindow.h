#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QDebug>
#include "optionsdialog.h"


// 使用QT命令空间
QT_BEGIN_NAMESPACE

class QLabel;
class QString;
class QDomDocument;

namespace Ui {
class MainWindow;
}

// 结束QT命名空间
QT_END_NAMESPACE

class Console;
class SettingsDialog;


#define XML_FILE        ("userconfig/config.xml")
#define XML_FILEDIR     ("userconfig")
#define XML_FILENAME    ("config.xml")

#define XML_NODE_LANGUAGE  ("language")

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readData();

    void handleError(QSerialPort::SerialPortError error);

    void on_sendPushButton_clicked();

    void on_actionOptions_triggered();

    void on_actionChinese_triggered();

    void on_actionEnglish_triggered();

    void on_actionClear_triggered();

    void on_actionAbout_triggered();

    void on_actionConnect_triggered();

    void on_actionDisconnect_triggered();

private:
    void initActionsConnections();
    void initOptions(OptionsDialog::Options options);


private:
    enum
    {
        ShowReadData,
        ShowWriteData
    };
    void showReadOrWriteData(const QByteArray &data, uint8_t rdSelect = ShowReadData);

    QByteArray str2Hex(const QString & str);
    char convertHexChar(char ch);
    QString formatInput(const QString& hexStr);

    void showStatusMessage(QLabel *label, const QString &message,
                           const QColor &acolor = Qt::black);

    bool initXml(void);
    bool xmlSaveLanguage(QString str);
    bool xmlLoadLanguage();
    bool xmlSaveSerialPortSettings();
    bool xmlReadSerialPortSettings();

private:

    Ui::MainWindow *m_ui        = nullptr;
    QLabel *m_serialInfoStatus  = nullptr;
    QLabel *m_txBytesStatus     = nullptr;
    QLabel *m_rxBytesStatus     = nullptr;
    SettingsDialog *m_settings  = nullptr;
    QSerialPort *m_serial       = nullptr;
    OptionsDialog *m_options    = nullptr;

    long m_rxCount;
    long m_txCount;
};
#endif // MAINWINDOW_H