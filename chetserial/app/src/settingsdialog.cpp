#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "myintValidator.h"
#include "xmlhelper.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QSerialPortInfo>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QDir>
#include <QCloseEvent>



static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog),
    m_intValidator(new MyValidator(0, 4000000, this))
{
    // 设置UI，里面的配置就是settingsdialog.ui文件生成的ui_settingdialog.h里的class方法
    m_ui->setupUi(this);

    // 暂时用不到local echo
    m_ui->localEchoCheckBox->setEnabled(false);
    m_ui->localEchoCheckBox->setCheckable(false);

    // 选择插入Box策略：输入字符串不插入到Box下拉列表中
    m_ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(m_ui->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::showPortInfo);
    connect(m_ui->baudRateBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::checkCustomBaudRatePolicy);


    fillPortsParameters();
    fillPortsInfo();

    updateSettings();

}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

SettingsDialog::Settings SettingsDialog::settings() const
{
    return m_currentSettings;
}

/**
 * @brief Initialize serial settings in the xml file
 * @param xmlFile   xmlfile "filedir/filename"
 * @return
 */
bool SettingsDialog::xmlInitSerialSettings(const QString xmlFile)
{
    QDomDocument doc;

    if (false == xmlHelper::xmlRead(xmlFile, doc)) return false;

    QDomElement root = doc.documentElement();      // 返回根节点
    QDomNodeList nodeList = root.elementsByTagName(QString(XML_NODE_SERIAL));
    QDomElement parentElem = nodeList.at(0).toElement();

    // 如果没有 XML_NODE_SERIAL 这个节点，则创建一个
    if (parentElem.isNull())
    {
        parentElem = doc.createElement(QString(XML_NODE_SERIAL));
        root.appendChild(parentElem);
    }

    // 判断是否有子节点，没有的话则创建
    if (!parentElem.hasChildNodes())
    {

        QDomElement childElem  = doc.createElement(QStringLiteral("BaudRate"));
        QDomText    text       = doc.createTextNode(m_currentSettings.stringbaudRate);
        childElem.setAttribute("index", m_validInterfaceCfg.baudRateIndex);
        childElem.appendChild(text);
        parentElem.appendChild(childElem);

        childElem  = doc.createElement(QStringLiteral("DataBits"));
        text       = doc.createTextNode(m_currentSettings.stringDataBits);
        childElem.setAttribute("index", m_validInterfaceCfg.dataBitsIndex);
        childElem.appendChild(text);
        parentElem.appendChild(childElem);

        childElem  = doc.createElement(QStringLiteral("Parity"));
        text       = doc.createTextNode(m_currentSettings.stringParity);
        childElem.setAttribute("index", m_validInterfaceCfg.parityIndex);
        childElem.appendChild(text);
        parentElem.appendChild(childElem);

        childElem  = doc.createElement(QStringLiteral("StopBits"));
        text       = doc.createTextNode(m_currentSettings.stringStopBits);
        childElem.setAttribute("index", m_validInterfaceCfg.stopBitsIndex);
        childElem.appendChild(text);
        parentElem.appendChild(childElem);

        childElem  = doc.createElement(QStringLiteral("FlowControl"));
        text       = doc.createTextNode(m_currentSettings.stringFlowControl);
        childElem.setAttribute("index", m_validInterfaceCfg.flowControlIndex);
        childElem.appendChild(text);
        parentElem.appendChild(childElem);

        return xmlHelper::xmlWrite(xmlFile, doc);
    }

    return true;
}

bool SettingsDialog::xmlSaveSerialSettings(const QString xmlFile)
{
    QDomDocument doc;

    if (false == xmlHelper::xmlRead(xmlFile, doc)) return false;

    QDomElement root = doc.documentElement();      // 返回根节点
    QDomNodeList nodeList = root.elementsByTagName(QString(XML_NODE_SERIAL));
    QDomElement parentElem = nodeList.at(0).toElement();

    nodeList = parentElem.childNodes();

    nodeList.at(0).firstChild().setNodeValue(m_currentSettings.stringbaudRate);
    nodeList.at(0).toElement().setAttribute("index", m_ui->baudRateBox->currentIndex());

    nodeList.at(1).firstChild().setNodeValue(m_currentSettings.stringDataBits);
    nodeList.at(1).toElement().setAttribute("index", m_ui->dataBitsBox->currentIndex());

    nodeList.at(2).firstChild().setNodeValue(m_currentSettings.stringParity);
    nodeList.at(2).toElement().setAttribute("index", m_ui->parityBox->currentIndex());

    nodeList.at(3).firstChild().setNodeValue(m_currentSettings.stringStopBits);
    nodeList.at(3).toElement().setAttribute("index", m_ui->stopBitsBox->currentIndex());

    nodeList.at(4).firstChild().setNodeValue(m_currentSettings.stringFlowControl);
    nodeList.at(4).toElement().setAttribute("index", m_ui->flowControlBox->currentIndex());

    return xmlHelper::xmlWrite(xmlFile, doc);

}

bool SettingsDialog::xmlLoadSerialSettings(const QString xmlFile)
{
    QDomDocument doc;

    if (false == xmlHelper::xmlRead(xmlFile, doc)) return false;


    QDomElement  root       = doc.documentElement();      // 返回根节点
    QDomNodeList nodeList   = root.elementsByTagName(QString(XML_NODE_SERIAL));
    QDomElement  parentElem = nodeList.at(0).toElement();

    nodeList = parentElem.childNodes();


    m_ui->baudRateBox->setCurrentIndex(     nodeList.at(0).toElement().attribute("index").toInt());
    m_ui->baudRateBox->setCurrentText(      nodeList.at(0).firstChild().nodeValue() );
    m_ui->dataBitsBox->setCurrentIndex(     nodeList.at(1).toElement().attribute("index").toInt());
    m_ui->parityBox->setCurrentIndex(       nodeList.at(2).toElement().attribute("index").toInt());
    m_ui->stopBitsBox->setCurrentIndex(     nodeList.at(3).toElement().attribute("index").toInt());
    m_ui->flowControlBox->setCurrentIndex(  nodeList.at(4).toElement().attribute("index").toInt());

    updateSettings();

    return true;
}

/**
 * @brief 重写对话框的关闭窗口事件
 * @param event
 */
void SettingsDialog::closeEvent(QCloseEvent *event)
{
    // 还原界面显示
    m_ui->baudRateBox->setCurrentIndex(             m_validInterfaceCfg.baudRateIndex   );
    m_ui->baudRateBox->setCurrentText(              m_validInterfaceCfg.baudRateText    );
    m_ui->dataBitsBox->setCurrentIndex(             m_validInterfaceCfg.dataBitsIndex   );
    m_ui->parityBox->setCurrentIndex(               m_validInterfaceCfg.parityIndex     );
    m_ui->stopBitsBox->setCurrentIndex(             m_validInterfaceCfg.stopBitsIndex   );
    m_ui->flowControlBox->setCurrentIndex(          m_validInterfaceCfg.flowControlIndex);
    m_ui->serialPortInfoListBox->setCurrentIndex(   m_validInterfaceCfg.serialProtIndex );

    event->accept();
}


/** 显示串口信息 */
void SettingsDialog::showPortInfo(int idx)
{
    if (idx == -1) return;

    const QStringList list = m_ui->serialPortInfoListBox->itemData(idx).toStringList();

    // Description: %1, [%1 = arg(……)], 类似于printf
    m_ui->descriptionLabel->setText(    tr("Description: %1").arg(          list.count() > 1 ? list.at(1) : tr(blankString) ) );
    m_ui->manufacturerLabel->setText(   tr("Manufacture: %1").arg(          list.count() > 2 ? list.at(2) : tr(blankString) ) );
    m_ui->serialNumberLabel->setText(   tr("Serial number: %1").arg(        list.count() > 3 ? list.at(3) : tr(blankString) ) );
    m_ui->locationLabel->setText(       tr("Location: %1").arg(             list.count() > 4 ? list.at(4) : tr(blankString) ) );
    m_ui->vidLabel->setText(            tr("Vendor Identifier: %1").arg(    list.count() > 5 ? list.at(5) : tr(blankString) ) );
    m_ui->pidLabel->setText(            tr("Product Identifier: %1").arg(   list.count() > 6 ? list.at(6) : tr(blankString) ) );
}



void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    const bool isCustomBaudRate = !m_ui->baudRateBox->itemData(idx).isValid();
    m_ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate)
    {
        m_ui->baudRateBox->clearEditText();
        QLineEdit *edit = m_ui->baudRateBox->lineEdit();
        edit->setValidator(m_intValidator); // 设置整数范围:m_intValidator(0, 4000000)
    }
}


void SettingsDialog::fillPortsParameters()
{
    // 波特率-Example:
    //      Item: "9600" => Value: 9600
    m_ui->baudRateBox->addItem(QStringLiteral("9600"),   QSerialPort::Baud9600);
    m_ui->baudRateBox->addItem(QStringLiteral("19200"),  QSerialPort::Baud19200);
    m_ui->baudRateBox->addItem(QStringLiteral("38400"),  QSerialPort::Baud38400);
    m_ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    m_ui->baudRateBox->addItem(tr("Custom"));

    // 数据位
    m_ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    m_ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    m_ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    m_ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    m_ui->dataBitsBox->setCurrentIndex(3);//设置默认Index为3，即Data8

    // 校验位
    m_ui->parityBox->addItem(tr("None"),  QSerialPort::NoParity);
    m_ui->parityBox->addItem(tr("Even"),  QSerialPort::EvenParity);
    m_ui->parityBox->addItem(tr("Odd"),   QSerialPort::OddParity);
    m_ui->parityBox->addItem(tr("Mark"),  QSerialPort::MarkParity);
    m_ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    // 停止位
    m_ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    m_ui->stopBitsBox->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
#endif
    m_ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    // 流控制
    m_ui->flowControlBox->addItem(tr("None"),       QSerialPort::NoFlowControl);
    m_ui->flowControlBox->addItem(tr("RTS/CTS"),    QSerialPort::HardwareControl);
    m_ui->flowControlBox->addItem(tr("XON/XOFF"),   QSerialPort::SoftwareControl);
}

void SettingsDialog::fillPortsInfo()
{
    m_ui->serialPortInfoListBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        QStringList list;
        description  = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();

        /* QString::number(info.vendorIdentifier(), 16)-----转化为16进制的字符串 */
        list << info.portName()
             << (!description.isEmpty()   ? description  : blankString)
             << (!manufacturer.isEmpty()  ? manufacturer : blankString)
             << (!serialNumber.isEmpty()  ? serialNumber : blankString)
             << (info.systemLocation())
             << (info.vendorIdentifier()  ? QString::number(info.vendorIdentifier(),  16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        //将串口名 list.first()即info.protName()添加到Box下来列表
        m_ui->serialPortInfoListBox->addItem(list.first(), list);
    }
}

void SettingsDialog::updateSettings()
{
    /********************** 1、串口名字 ********************************/
    m_currentSettings.name = m_ui->serialPortInfoListBox->currentText();

    /******************* 2、波特率值 + 字符串形式 *************************/
    if (m_ui->baudRateBox->currentIndex() == 4)
    {
        // Index < 4 是系统默认定义的几个固定的波特率，9600，19200，38400，115200
        m_currentSettings.baudRate = m_ui->baudRateBox->currentText().toInt();
    }
    else
    {
        // 使用强制类型转换将 QVariant 转换成 QSerialPort::BaudRate
        m_currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    m_ui->baudRateBox->itemData(m_ui->baudRateBox->currentIndex()).toInt());
    }

    // 波特率值字符串形式，
    // 使用m_ui->baudRateBox->itemText，当波特率采用custom的时候，状态栏会显示custom，而不会显示正确的波特率，所以
    // 使用QString::number将波特率转换为字符串显示
    m_currentSettings.stringbaudRate = QString::number(m_currentSettings.baudRate);
    //m_currentSettings.stringbaudRate = m_ui->baudRateBox->itemText(m_ui->baudRateBox->currentIndex());


    /********************************** 3、数据位 ********************************************/
    m_currentSettings.databits = static_cast<QSerialPort::DataBits>(
                        m_ui->dataBitsBox->itemData(m_ui->dataBitsBox->currentIndex()).toInt());
    m_currentSettings.stringDataBits = m_ui->dataBitsBox->currentText();


    /********************************** 4、校验位 ********************************************/
    m_currentSettings.parity = static_cast<QSerialPort::Parity>(
                        m_ui->parityBox->itemData(m_ui->parityBox->currentIndex()).toInt());
    m_currentSettings.stringParity = m_ui->parityBox->currentText();

    /********************************** 5、停止位 ********************************************/
    m_currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                        m_ui->stopBitsBox->itemData(m_ui->stopBitsBox->currentIndex()).toInt());
    m_currentSettings.stringStopBits = m_ui->stopBitsBox->currentText();

    /********************************** 6、流控制 ********************************************/
    m_currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                        m_ui->flowControlBox->itemData(m_ui->flowControlBox->currentIndex()).toInt());
    m_currentSettings.stringFlowControl = m_ui->flowControlBox->currentText();

    /********************************** 7、本地回显 ********************************************/
    //m_currentSettings.localEchoEnabled = m_ui->localEchoCheckBox->isChecked();

    m_validInterfaceCfg.baudRateIndex    = m_ui->baudRateBox->currentIndex();
    m_validInterfaceCfg.baudRateText     = m_ui->baudRateBox->currentText();
    m_validInterfaceCfg.dataBitsIndex    = m_ui->dataBitsBox->currentIndex();
    m_validInterfaceCfg.parityIndex      = m_ui->parityBox->currentIndex();
    m_validInterfaceCfg.stopBitsIndex    = m_ui->stopBitsBox->currentIndex();
    m_validInterfaceCfg.flowControlIndex = m_ui->flowControlBox->currentIndex();
    m_validInterfaceCfg.serialProtIndex = m_ui->serialPortInfoListBox->currentIndex();
}


void SettingsDialog::on_applyButton_clicked()
{
    updateSettings();

    // 返回accept信号给exec()
    accept();
}
