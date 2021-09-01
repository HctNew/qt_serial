#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"
#include "xmlhelper.h"
#include "logger.h"
#include "customtablewidget.h"

#include <QDesktopWidget>
#include <QLabel>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QDir>

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent):
      QMainWindow(parent),
      m_ui(new Ui::MainWindow),
      m_serialInfoStatus(new QLabel),
      m_txBytesStatus(new QLabel),
      m_rxBytesStatus(new QLabel),
      m_settings(new SettingsDialog(this)),
      m_serial(new QSerialPort(this)),
      m_options(new OptionsDialog(this))
{

    m_ui->setupUi(this);

    m_ui->tableWidget->xmlInitCmdList(XML_FILE);
    m_ui->tableWidget->xmlLoadCmdList(XML_FILE);
    m_settings->xmlInitSerialSettings(XML_FILE);
    m_settings->xmlLoadSerialSettings(XML_FILE);
    m_options->xmlInitOptions(XML_FILE);
    m_options->xmlLoadOptions(XML_FILE);
    xmlInitMainWindow(XML_FILE);
    xmlLoadMainWindow(XML_FILE);


    // 设置串口接收区和发送区的属性
    m_ui->sendTextEdit->setEnabled(true);
    m_ui->sendTextEdit->setShowSendEnable(false);
    m_ui->sendTextEdit->setTimeStampEnable(false);
    m_ui->recvTextEdit->setAutoSendWrapEnable(false);
    m_ui->recvTextEdit->setStyleSheet("QTextEdit{border:2px solid gray;}");
    m_ui->sendTextEdit->setStyleSheet("QTextEdit{border:2px solid gray;}");

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
    m_ui->statusBar->addWidget(m_txBytesStatus,    1);
    m_ui->statusBar->addWidget(m_rxBytesStatus,    1);

    showStatusMessage(m_serialInfoStatus, tr(" Disconnected"));
    showStatusMessage(m_txBytesStatus,    tr(" Tx: 0 Bytes"));
    showStatusMessage(m_rxBytesStatus,    tr(" Rx: 0 Bytes"));

    m_rxBytesStatus->setFrameShape(QFrame::Panel);       // 设置标签形状
    m_rxBytesStatus->setFrameShadow(QFrame::Sunken);     // 设置标签阴影
    m_txBytesStatus->setFrameShape(QFrame::Panel);       // 设置标签形状
    m_txBytesStatus->setFrameShadow(QFrame::Sunken);     // 设置标签阴影
    m_serialInfoStatus->setFrameShape(QFrame::Panel);    // 设置标签形状
    m_serialInfoStatus->setFrameShadow(QFrame::Sunken);  // 设置标签阴影

    // 初始化tableWidget
    m_ui->editPushButton->setText("Edit");
    m_ui->tableWidget->setEditEnable(false);
    m_ui->addToolButton->setVisible(false);
    m_ui->subToolButton->setVisible(false);
    m_ui->horizontalLayout_3->setAlignment(Qt::AlignRight);

    //初始化菜单栏活动类控件以及串口的信号槽
    connect(m_ui->actionQuit,       &QAction::triggered,                this,   &MainWindow::close);
    connect(m_ui->actionAboutQt,    &QAction::triggered,                qApp,   &QApplication::aboutQt);
    connect(m_serial,               &QSerialPort::errorOccurred,        this,   &MainWindow::handleError);
    connect(m_serial,               &QSerialPort::readyRead,            this,   &MainWindow::readData);
    connect(m_ui->tableWidget,      &CustomTableWidget::commandClicked, this,   &MainWindow::tableWidgetButtonClicked);

    updateOptions(m_options->options());
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

bool MainWindow::xmlInitMainWindow(const QString &xmlFile)
{
    QDomDocument doc;

    if (false == xmlHelper::xmlRead(xmlFile, doc)) return false;

    QDomElement root = doc.documentElement();      // 返回根节点
    QDomNodeList nodeList = root.elementsByTagName(QString(XML_NODE_MAINWINDOW));
    QDomElement parentElem = nodeList.at(0).toElement();

    // 如果没有 XML_NODE_SERIAL 这个节点，则创建一个
    if (parentElem.isNull())
    {
        parentElem = doc.createElement(QString(XML_NODE_MAINWINDOW));
        root.appendChild(parentElem);
    }


    xmlInitLanguage(parentElem, doc);
    xmlInitWinCfg(parentElem,doc);

    return xmlHelper::xmlWrite(xmlFile, doc);

    return true;
}

bool MainWindow::xmlSaveMainWindow(const QString &xmlFile)
{
    QDomDocument doc;

    if (false == xmlHelper::xmlRead(xmlFile, doc)) return false;

    QDomElement  root       = doc.documentElement();      // 返回根节点
    QDomNodeList nodeList   = root.elementsByTagName(QString(XML_NODE_MAINWINDOW));
    QDomElement  parentElem = nodeList.at(0).toElement();

    xmlSaveLanguage(parentElem);
    xmlSaveWinCfg(parentElem);

    return xmlHelper::xmlWrite(xmlFile, doc);
}

bool MainWindow::xmlLoadMainWindow(const QString &xmlFile)
{
    QDomDocument doc;

    if (false == xmlHelper::xmlRead(xmlFile, doc)) return false;

    QDomElement  root       = doc.documentElement();      // 返回根节点
    QDomNodeList nodeList   = root.elementsByTagName(QString(XML_NODE_MAINWINDOW));
    QDomElement  parentElem = nodeList.at(0).toElement();   // options元素

    xmlLoadLanguage(parentElem);
    xmlLoadWinCfg(parentElem);

    return true;
}


void MainWindow::on_sendPushButton_clicked()
{
    if (!m_serial->isOpen()) return;

    QByteArray byteArrayWrite = m_ui->sendTextEdit->toPlainText().toUtf8();

    if (m_ui->sendTextEdit->isAutoSendWrapChecked() &&
        !m_ui->sendTextEdit->isHexModeChecked() )
    {
        byteArrayWrite += QString("\r\n").toUtf8();

    }

    // 判断右键菜单是否选中 “Hex Mode”，十六进制发送
    if (m_ui->sendTextEdit->isHexModeChecked())
    {
        byteArrayWrite = str2Hex( formatInput(byteArrayWrite) );
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

/**
 * @brief read serial data and show in the recvEdit
 */
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
        on_actionDisconnect_triggered();//关闭串口
    }
}

/**
 * @brief initialize options dialog settings
 * @param options
 */
void MainWindow::updateOptions(OptionsDialog::Options options)
{
    m_ui->recvTextEdit->setFont(options.m_font);
    m_ui->sendTextEdit->setFont(options.m_font);

    // 需要使用QPalette来配置，不能使用setTextColor，因为setTextColor没有使lineEdit配置前输入的内容字体颜色生效。
    QPalette pe = m_ui->recvTextEdit->palette();
    pe.setColor(QPalette::Text, options.m_textColor);       // 字体颜色
    pe.setColor(QPalette::Base, options.m_backgroundColor); // lineEdit背景色
    m_ui->recvTextEdit->setPalette(pe);
    //m_ui->sendTextEdit->setPalette(pe);

}

void MainWindow::showStatusMessage(QLabel *label, const QString &message, const QColor &acolor)
{
    QPalette pe;
    pe.setColor(QPalette::WindowText, acolor);
    label->setPalette(pe);

    label->setText(message);
}

void MainWindow::xmlInitLanguage(QDomElement &parentElem, QDomDocument & doc)
{
    QDomNodeList nodeList = parentElem.elementsByTagName("language");
    if (nodeList.count() == 0)
    {
        // 加入 “Display” 元素
        QDomElement childElem = doc.createElement("language");
        parentElem.appendChild(childElem);

        QDomText text = doc.createTextNode(XML_LANGUAGE_DEFAULT);
        childElem.appendChild(text);
    }


}

void MainWindow::xmlSaveLanguage(QDomElement &parentElem)
{
    // 找出“language”元素
    QDomNodeList nodeList = parentElem.elementsByTagName("language");

    // 因为我设计的时候不允许同级存在同名的节点，所以第一个就是我们想要的“找出“language”元素”元素节点
    nodeList.at(0).firstChild().setNodeValue(m_currentSettings.m_language);
}

void MainWindow::xmlLoadLanguage(QDomElement &parentElem)
{
    // 找出“language”元素
    QDomNodeList nodeList = parentElem.elementsByTagName("language");

    m_currentSettings.m_language = nodeList.at(0).firstChild().nodeValue();

    // 如果xml里有 XML_NODE_LANGUAGE 配置并且配置为中文
    if ( m_currentSettings.m_language == QString("Chinese") )
    {
        m_ui->actionChinese->setChecked(true);
        m_ui->actionEnglish->setChecked(false);
    }
    else
    {
        m_ui->actionChinese->setChecked(false);
        m_ui->actionEnglish->setChecked(true);
    }
}

void MainWindow::xmlInitWinCfg(QDomElement &parentElem, QDomDocument & doc)
{
    QDomNodeList nodeList = parentElem.elementsByTagName("winCfg");
    if (nodeList.count() == 0)
    {
        QDomElement elem, childElem;


        childElem = doc.createElement("winCfg");
        parentElem.appendChild(childElem);

        // winPosition
        elem = doc.createElement("winPosition");
        QDesktopWidget *pDesk = QApplication::desktop();
        elem.setAttribute("x", (pDesk->width() - this->width()) / 2);
        elem.setAttribute("y", (pDesk->height() - this->height()) / 2);
        elem.setAttribute("width", this->width());
        elem.setAttribute("height", this->height());
        childElem.appendChild(elem);

        // contextMenu
        QDomElement contextElem = doc.createElement("contextMenu");
        childElem.appendChild(contextElem);

        elem = doc.createElement("HEX");
        elem.setAttribute("RW", "R");
        elem.setAttribute("Enable", m_ui->recvTextEdit->isHexModeChecked());
        contextElem.appendChild(elem);

        elem = doc.createElement("HEX");
        elem.setAttribute("RW", "W");
        elem.setAttribute("Enable", m_ui->sendTextEdit->isHexModeChecked());
        contextElem.appendChild(elem);

        elem = doc.createElement("TimeStamp");
        elem.setAttribute("Enable", m_ui->recvTextEdit->isTimeStampChecked());
        contextElem.appendChild(elem);

        elem = doc.createElement("ShowSend");
        elem.setAttribute("Enable", m_ui->recvTextEdit->isShowSendChecked());
        contextElem.appendChild(elem);

        elem = doc.createElement("SendWrap");
        elem.setAttribute("Enable", m_ui->sendTextEdit->isAutoSendWrapChecked());
        contextElem.appendChild(elem);
    }

}
void MainWindow::xmlSaveWinCfg(QDomElement &parentElem)
{
    QDomElement elem, childElem;

    // 找出“winPosition”元素
    QDomNodeList nodeList = parentElem.elementsByTagName("winCfg");
    childElem = nodeList.at(0).toElement();


    nodeList = childElem.elementsByTagName("winPosition");
    elem     = nodeList.at(0).toElement();

    elem.setAttribute("x", this->pos().x());
    elem.setAttribute("y", this->pos().y());
    elem.setAttribute("width", this->width());
    elem.setAttribute("height", this->height());

    nodeList = childElem.elementsByTagName("contextMenu");
    elem     = nodeList.at(0).toElement();

    nodeList = elem.childNodes();
    nodeList.at(0).toElement().setAttribute("Enable", m_ui->recvTextEdit->isHexModeChecked());
    nodeList.at(1).toElement().setAttribute("Enable", m_ui->sendTextEdit->isHexModeChecked());
    nodeList.at(2).toElement().setAttribute("Enable", m_ui->recvTextEdit->isTimeStampChecked());
    nodeList.at(3).toElement().setAttribute("Enable", m_ui->recvTextEdit->isShowSendChecked());
    nodeList.at(4).toElement().setAttribute("Enable", m_ui->sendTextEdit->isAutoSendWrapChecked());


}
void MainWindow::xmlLoadWinCfg(QDomElement &parentElem)
{
    QDomElement childElem, elem;
    QDomNodeList nodeList;

    // 找出“language”元素
    nodeList  = parentElem.elementsByTagName("winCfg");
    childElem = nodeList.at(0).toElement();

    nodeList = childElem.elementsByTagName("winPosition");
    elem     = nodeList.at(0).toElement();

    this->move(elem.attribute("x").toInt(), elem.attribute("y").toInt());
    this->resize(elem.attribute("width").toInt(), elem.attribute("height").toInt());

    nodeList = childElem.elementsByTagName("contextMenu");
    elem     = nodeList.at(0).toElement();
    nodeList = elem.childNodes();

    m_ui->recvTextEdit->setHexModeChecked(      nodeList.at(0).toElement().attribute("Enable").toInt());
    m_ui->sendTextEdit->setHexModeChecked(      nodeList.at(1).toElement().attribute("Enable").toInt());
    m_ui->recvTextEdit->setTimeStampChecked(    nodeList.at(2).toElement().attribute("Enable").toInt());
    m_ui->recvTextEdit->setShowSendChecked(     nodeList.at(3).toElement().attribute("Enable").toInt());
    m_ui->sendTextEdit->setAutoSendWrapChecked( nodeList.at(4).toElement().attribute("Enable").toInt());

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    xmlSaveMainWindow(XML_FILE);
    m_ui->tableWidget->xmlSaveCmdList(XML_FILE);

    qApp->quit();
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
    else return (ch);
}

/**
 * @brief 格式化HEX，保留2个占位符显示
 * @param hexStr
 * @return QString
 */
QString MainWindow::formatInput(const QString& hexStr)
{
//    int strlen = hexStr.length();
//    QString tmp = hexStr;

//    //将输入格式化，补满两位：0XFF
//    if (strlen%2 != 0)
//    {
//        QString startStr = hexStr.left(strlen-1);
//        QString endStr = hexStr.right(1);
//        tmp = startStr + "0" + endStr;
//    }

//    return tmp;

    QRegExp rx("([a-fA-F0-9]{2})([\\s][a-fA-F0-9]{2})*[\\s]?"); //正则表达式
    QRegularExpression re;

    bool match = rx.exactMatch(hexStr);//验证文本
    if(match == true)
    {
        //表达式正确
        return hexStr;
    }
    else
    {
        QMessageBox::warning(this, tr("Format Error"), tr("<b>Hexformat:</b> [xx xx xx]<br>"
                                                          "<b>Example:</b> [0x12 0x4 0x56]<br>"
                                                          "<b>Input:  </b>12 04 56"));
        return QString("");
    }


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
        QString str = (rdSelect == ShowReadData)? tr("[RECV] ") : tr("[SEND] ");
        strDis = "[" + nowtime.toString("hh:mm:ss:zzz") + "]" + str + strDis + "\r\n";
    }

    m_ui->recvTextEdit->showData(strDis);

    if (m_options->isRecordLog())
    {
        logger::write(m_options->logFilePath(), strDis);
    }
}

void MainWindow::on_actionOptions_triggered()
{

    if (m_options->exec() == m_options->Accepted)
    {
        updateOptions(m_options->options());
        m_options->xmlSaveOptions(XML_FILE);
    }
}

void MainWindow::on_actionChinese_triggered()
{
    if (!m_ui->actionChinese->isChecked())
    {
        m_ui->actionChinese->setChecked(true);
        return;
    }

    m_ui->actionEnglish->setChecked(false);

    m_currentSettings.m_language = QString("Chinese");
    if (true == xmlSaveMainWindow(XML_FILE))
    {
        QMessageBox::information(this, tr("Language Setting"),
                                  tr("The changed will take effect when the program is restarted."));
    }
    else
    {
        QMessageBox::warning(this, tr("Warinning"),
                                 tr("Invalid operating."));
    }
}

void MainWindow::on_actionEnglish_triggered()
{
    if (!m_ui->actionEnglish->isChecked())
    {
        m_ui->actionEnglish->setChecked(true);
        return;
    }

    m_ui->actionChinese->setChecked(false);

    m_currentSettings.m_language = QString("English");
    if (true == xmlSaveMainWindow(XML_FILE))
    {
        QMessageBox::information(this, tr("Language Setting"),
                                 tr("The changed will take effect when the program is restarted."));
    }
    else
    {
        QMessageBox::warning(this, tr("Warinning"),
                                 tr("Invalid operating."));
    }
}

void MainWindow::on_actionClear_triggered()
{
    m_txCount = 0;
    m_rxCount = 0;
    m_ui->recvTextEdit->clear();
    showStatusMessage(m_txBytesStatus, tr(" Tx: 0 Bytes"));
    showStatusMessage(m_rxBytesStatus, tr(" Rx: 0 Bytes"));
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About"),
                       tr("<b>Author:</b> Chet <br>"
                          "<b>Mail:  </b> hctlovelife@foxmail.com<br><br>"
                          "Welcome to use my serial assistance!"));
}

void MainWindow::on_actionConnect_triggered()
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

void MainWindow::on_actionDisconnect_triggered()
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

void MainWindow::on_actionConfigure_triggered()
{
    if ( m_settings->exec() == QDialog::Accepted)
    {
        m_settings->xmlSaveSerialSettings(XML_FILE);
    }
}


void MainWindow::on_addToolButton_clicked()
{
    CustomTableWidget::CmdStruct cmdData;
    cmdData.m_id = m_ui->tableWidget->currentRow()+1;
    m_ui->tableWidget->addItemRow(cmdData);
}

void MainWindow::on_subToolButton_clicked()
{
    m_ui->tableWidget->removeItemRow(m_ui->tableWidget->currentRow());
}

void MainWindow::on_upToolButton_clicked()
{
    int row = m_ui->tableWidget->currentRow();
    m_ui->tableWidget->moveItemRow(row, row-1);
}

void MainWindow::on_downToolButton_clicked()
{
    int row = m_ui->tableWidget->currentRow();
    m_ui->tableWidget->moveItemRow(row, row+1);
}

void MainWindow::on_editPushButton_clicked()
{
    if (m_ui->editPushButton->text() == QStringLiteral("Edit"))
    {
        m_ui->editPushButton->setText("Save");
        m_ui->tableWidget->setEditEnable(true);
        m_ui->addToolButton->setVisible(true);
        m_ui->subToolButton->setVisible(true);
    }
    else
    {
        m_ui->editPushButton->setText("Edit");
        m_ui->tableWidget->setEditEnable(false);
        m_ui->addToolButton->setVisible(false);
        m_ui->subToolButton->setVisible(false);
    }
}

void MainWindow::tableWidgetButtonClicked(bool isHexChecked, bool isWrapChecked, const QString &data)
{
    if (!m_serial->isOpen()) return;

    QByteArray byteArrayWrite = data.toUtf8();

    if (isWrapChecked && !isHexChecked )
    {
        byteArrayWrite += QString("\r\n").toUtf8();

    }

    // 判断右键菜单是否选中 “Hex Mode”，十六进制发送
    if (isHexChecked)
    {
        byteArrayWrite = str2Hex( formatInput(byteArrayWrite) );
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


