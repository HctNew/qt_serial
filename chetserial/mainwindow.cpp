#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"

#include <QLabel>
#include <QMessageBox>
#include <QString>
#include <QDateTime>


MainWindow::MainWindow(QWidget *parent):
      QMainWindow(parent),
      m_ui(new Ui::MainWindow),
      m_serialInfoStatus(new QLabel),
      m_txBytesStatus(new QLabel),
      m_rxBytesStatus(new QLabel),
      m_settings(new SettingsDialog(this)),
      m_serial(new QSerialPort(this))
{

    m_ui->setupUi(this);

    // 设置串口配置窗口为半模态，类似于置顶
    m_settings->setWindowModality(Qt::ApplicationModal);

    // 设置串口接收区和发送区的属性
    m_ui->sendTextEdit->setEnabled(true);
    m_ui->sendTextEdit->setShowSendEnable(false);
    m_ui->sendTextEdit->setTimeStampEnable(false);

    // 设置活动类控件状态
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);

    // 状态栏设置
    m_txCount = 0;
    m_rxCount = 0;
    m_rxBytesStatus->setMinimumSize(80, 20);
    m_txBytesStatus->setMinimumSize(80, 20);
    m_serialInfoStatus->setMinimumSize(240, 20);

    //addWidget 指定了 label 的父对象，所以不需要手动delete
    m_ui->statusBar->addWidget(m_serialInfoStatus, 1);  // 状态栏添加Label, stretch = 1, 按比例1：1拉伸，策略得看sizePolicy属性
    m_ui->statusBar->addWidget(m_txBytesStatus, 1);
    m_ui->statusBar->addWidget(m_rxBytesStatus, 1);

    showStatusMessage(m_serialInfoStatus, tr(" Disconnected"));
    showStatusMessage(m_txBytesStatus, tr(" Tx: 0 Bytes"));
    showStatusMessage(m_rxBytesStatus, tr(" Rx: 0 Bytes"));

    m_rxBytesStatus->setFrameShape(QFrame::Panel);       // 设置标签形状
    m_rxBytesStatus->setFrameShadow(QFrame::Sunken);     // 设置标签阴影
    m_serialInfoStatus->setFrameShape(QFrame::Panel);    // 设置标签形状
    m_serialInfoStatus->setFrameShadow(QFrame::Sunken);  // 设置标签阴影
    m_txBytesStatus->setFrameShape(QFrame::Panel);       // 设置标签形状
    m_txBytesStatus->setFrameShadow(QFrame::Sunken);     // 设置标签阴影

    //初始化菜单栏活动类控件的信号槽
    initActionsConnections();

    connect(m_serial,  &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial,  &QSerialPort::readyRead,     this, &MainWindow::readData);
}

MainWindow::~MainWindow()
{
    delete m_ui;
    //delete m_settings;
}

void MainWindow::clearScreen()
{
    m_txCount = 0;
    m_rxCount = 0;
    m_ui->recvTextEdit->clear();
    showStatusMessage(m_txBytesStatus, tr(" Tx: 0 Bytes"));
    showStatusMessage(m_rxBytesStatus, tr(" Rx: 0 Bytes"));
}

void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.databits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);

    // 以读写的方式打开
    if (m_serial->open(QIODevice::ReadWrite))
    {

        // 失能活动控件：connect和Configure，使能活动控件：disconnect
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);

        showStatusMessage(m_serialInfoStatus, tr(" [%1] OPENED: %2, %3bits")
                          .arg(p.name).arg(p.stringbaudRate).arg(p.stringDataBits)
                          , Qt::darkGreen);
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        showStatusMessage(m_serialInfoStatus, tr(" [%1] Open Error").arg(p.name), Qt::darkRed);
    }
}

void MainWindow::on_sendPushButton_clicked()
{
    if (m_serial->isOpen())
    {
        QByteArray byteArrayWrite;

        // 判断右键菜单是否选中 “Hex Mode”，十六进制发送
        if (m_ui->sendTextEdit->isHexModeChecked())
        {
            byteArrayWrite = str2Hex( formatInput(m_ui->sendTextEdit->toPlainText().toUtf8()) );
        }
        else
        {
            byteArrayWrite = m_ui->sendTextEdit->toPlainText().toUtf8();
        }

        m_serial->write(byteArrayWrite);

        // 判断右键菜单是否选中 “Show Send”，显示发送
        if (m_ui->recvTextEdit->isShowSendChecked())
        {
            // 将发送的数据显示在接受区
            showReadOrWriteData(byteArrayWrite, ShowWriteData);
        }

        // 更新状态栏：Tx:
        m_txCount += byteArrayWrite.length();
        showStatusMessage(m_txBytesStatus, tr(" Tx: %1 Bytes").arg(m_txCount));


    }
}

void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
    {
        m_serial->close();
    }

    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);

    showStatusMessage(m_serialInfoStatus, tr(" Disconnected"));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to"
                          "use the Qt Serial Port Module in modern GUI applications"
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}


void MainWindow::readData()
{
    QByteArray data = m_serial->readAll();

    if (!data.isEmpty())
    {
        showReadOrWriteData(data);
        m_rxCount += data.length();
        showStatusMessage(m_rxBytesStatus, tr(" Rx: %1 Bytes").arg(m_rxCount));
    }
}


void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError)
    {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}


void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect,    &QAction::triggered, this,          &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this,          &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit,       &QAction::triggered, this,          &MainWindow::close);
    connect(m_ui->actionConfigure,  &QAction::triggered, m_settings,    &SettingsDialog::show);
    connect(m_ui->actionClear,      &QAction::triggered, this,          &MainWindow::clearScreen);
    connect(m_ui->actionAbout,      &QAction::triggered, this,          &MainWindow::about);
    connect(m_ui->actionAboutQt,    &QAction::triggered, qApp,          &QApplication::aboutQt);
}


void MainWindow::showStatusMessage(QLabel *label, const QString &message, const QColor &acolor)
{
    QPalette pe;
    pe.setColor(QPalette::WindowText, acolor);
    label->setPalette(pe);

    label->setText(message);
}

QByteArray MainWindow::str2Hex(const QString & str)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();

    senddata.resize(len/2);
    char lstr,hstr;

    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = convertHexChar(hstr);
        lowhexdata = convertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}

char MainWindow::convertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}


QString MainWindow::formatInput(const QString& hexStr)
{
    int strlen = hexStr.length();
    QString tmp = hexStr;

    //将输入格式化，补满两位：0XFF
    if (strlen%2 != 0)
    {
        QString startStr = hexStr.left(strlen-1);
        QString endStr = hexStr.right(1);
        tmp = startStr + "0" + endStr;
    }

    return tmp;
}

void MainWindow::showReadOrWriteData(const QByteArray &data, uint8_t rdSelect)
{
    QString strDis;

    /** 使用十六进制显示 */
    if (m_ui->recvTextEdit->isHexModeChecked())
    {
        // 转化为HEX大写字符串
        QString strHex = data.toHex().toUpper();

        // HEX之间使用空格隔开
        for(int i=0; i<strHex.length(); i+=2)
        {
            QString st = strHex.mid(i,2);
            strDis += st;
            strDis += " ";
        }
    }
    else
    {
        strDis = tr(data);
    }

    /** 加上时间戳显示 */
    if (m_ui->recvTextEdit->isTimeStampChecked())
    {
        QDateTime  nowtime = QDateTime::currentDateTime();
        QString str = (rdSelect == ShowReadData)? "[RECV] " : "[SEND] ";
        strDis = "[" + nowtime.toString("hh:mm:ss:zzz") + "]" + str + strDis + "\r\n";
    }

    m_ui->recvTextEdit->showData(strDis);
}

