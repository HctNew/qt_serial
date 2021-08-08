#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"

#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent):
      QMainWindow(parent),
      m_ui(new Ui::MainWindow),
      m_status(new QLabel),
      m_console(new Console),
      m_settings(new SettingsDialog),
      m_serial(new QSerialPort(this))
{
    m_ui->setupUi(this);

    // 失能终端，设置中心窗口对象为 m_console
    m_console->setEnabled(false);
    setCentralWidget(m_console);

    // 设置活动类控件状态
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);

    // 暂时不知道怎么用的
    m_ui->statusBar->addWidget(m_status);

    //初始化菜单栏活动类控件的信号槽
    initActionsConnections();

    connect(m_serial,  &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial,  &QSerialPort::readyRead,     this, &MainWindow::readData);
    connect(m_console, &Console::getData,           this, &MainWindow::writeData);

}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_settings;
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
//        m_ui->sendPlainTextEdit->setEnabled(true);
//        m_console->setLocalEchoEnabled(p.localEchoEnabled);
        m_console->setEnabled(true);
        m_console->setLocalEchoEnabled(p.localEchoEnabled);

        // 失能活动控件：connect和Configure，使能活动控件：disconnect
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);

        showStatusMessage(tr("Connected to %1: %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringbaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
    {
        m_serial->close();
    }

    m_console->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);

    showStatusMessage(tr("Disconnected"));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to"
                          "use the Qt Serial Port Module in modern GUI applications"
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

void MainWindow::writeData(const QByteArray &data)
{
    m_serial->write(data);
}

void MainWindow::readData()
{
    const QByteArray data = m_serial->readAll();

    m_console->putData(data);
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
    connect(m_ui->actionConnect,    &QAction::triggered, this,       &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this,       &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit,       &QAction::triggered, this,       &MainWindow::close);
    connect(m_ui->actionConfigure,  &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionClear,      &QAction::triggered, m_console,  &Console::clear);
    connect(m_ui->actionAbout,      &QAction::triggered, this,       &MainWindow::about);
    connect(m_ui->actionAboutQt,    &QAction::triggered, qApp,       &QApplication::aboutQt);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}