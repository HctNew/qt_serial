#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QDebug>
#include <QLabel>
#include "optionsdialog.h"

#define XML_NODE_MAINWINDOW     ("mainwindow")
#define XML_NODE_LANGUAGE       ("language")
#define XML_LANGUAGE_DEFAULT    ("English")

// 使用QT命令空间
QT_BEGIN_NAMESPACE

class QString;
class QDomDocument;
class QRect;
class QTableWidgetItem;

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
    struct MainWindowSettings
    {
        QString m_language = QString(XML_LANGUAGE_DEFAULT);
    };


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool xmlInitMainWindow(const QString &xmlFile);
    bool xmlSaveMainWindow(const QString &xmlFile);
    bool xmlLoadMainWindow(const QString &xmlFile);

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

    void on_actionConfigure_triggered();

    void on_addToolButton_clicked();

    void on_subToolButton_clicked();

    void on_upToolButton_clicked();

    void on_downToolButton_clicked();

    void on_editPushButton_clicked();

    void tableWidgetButtonClicked(bool isHexChecked,
                                  bool isWrapChecked, const QString &data);


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
                           const QColor &acolor = Qt::black)
    {
        QPalette pe;
        pe.setColor(QPalette::WindowText, acolor);
        label->setPalette(pe);

        label->setText(message);
    };

    void updateOptions(OptionsDialog::Options options);

    void xmlInitLanguage(QDomElement &parentElem, QDomDocument & doc);
    void xmlSaveLanguage(QDomElement &parentElem);
    void xmlLoadLanguage(QDomElement &parentElem);

    void xmlInitWinCfg(QDomElement &parentElem, QDomDocument & doc);
    void xmlSaveWinCfg(QDomElement &parentElem);
    void xmlLoadWinCfg(QDomElement &parentElem);

protected:
    void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *m_ui        = nullptr;
    QLabel *m_serialInfoStatus  = nullptr;
    QLabel *m_txBytesStatus     = nullptr;
    QLabel *m_rxBytesStatus     = nullptr;
    SettingsDialog *m_settings  = nullptr;
    QSerialPort *m_serial       = nullptr;
    OptionsDialog *m_options    = nullptr;
    MainWindowSettings m_currentSettings;

    ulong m_rxCount;
    ulong m_txCount;
};
#endif // MAINWINDOW_H
