#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSerialPort>

#define XML_NODE_SERIAL ("serialParam")

QT_BEGIN_NAMESPACE

namespace Ui {
class SettingsDialog;
}

class QCloseEvent;

QT_END_NAMESPACE

class MyValidator;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    struct Settings
    {
        QString name;
        qint32 baudRate;
        QString stringbaudRate;
        QSerialPort::DataBits databits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };

    struct InterfaceSettings
    {
        quint8 baudRateIndex;
        quint8 dataBitsIndex;
        quint8 parityIndex;
        quint8 stopBitsIndex;
        quint8 flowControlIndex;
        quint8 serialProtIndex;
        QString baudRateText;

    };

    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    Settings settings() const;

    bool xmlInitSerialSettings(const QString xmlFile);
    bool xmlSaveSerialSettings(const QString xmlFile);
    bool xmlLoadSerialSettings(const QString xmlFile);

protected:
    void closeEvent(QCloseEvent *event) override;


private slots:
    void showPortInfo(int idx);
    void checkCustomBaudRatePolicy(int idx);

    void on_applyButton_clicked();

private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();

private:
    Ui::SettingsDialog *m_ui = nullptr;
    Settings m_currentSettings;
    MyValidator *m_intValidator = nullptr;
    InterfaceSettings m_validInterfaceCfg;

};

#endif // SETTINGSDIALOG_H
