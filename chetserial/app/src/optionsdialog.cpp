#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "xmlhelper.h"

#include <QDebug>
#include <QFontDialog>
#include <QColorDialog>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QDir>

#define XML_NODE_OPTIONS    ("options")

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::OptionsDialog)
{
    m_ui->setupUi(this);

    // 配置Tab栏 “Display”
    m_currentOptions.m_font = m_ui->fontLineEdit->font();
    m_currentOptions.m_font.setStyleName("Regular");
    m_currentOptions.m_textColor.setRgb(0, 0, 0);
    m_currentOptions.m_backgroundColor.setRgb(255, 255, 255);
    m_uncertainOptions = m_currentOptions;

    updateOptions();
}

void OptionsDialog::updateOptions()
{

    QString str =   m_currentOptions.m_font.family() + ", " +
                    m_currentOptions.m_font.styleName() +
                    ", " + QString::number(m_currentOptions.m_font.pointSize()) + "pt";
    m_ui->fontLineEdit->setText(str);

    str = "background-color: rgb(%1, %2, %3);";
    m_ui->textColorToolButton->setStyleSheet(str.arg(m_currentOptions.m_textColor.red()).
                                             arg(m_currentOptions.m_textColor.green()).
                                             arg(m_currentOptions.m_textColor.blue())
                                             );

    str = "background-color: rgb(%1, %2, %3);";
    m_ui->bgColorToolButton->setStyleSheet(str.arg(m_currentOptions.m_backgroundColor.red()).
                                             arg(m_currentOptions.m_backgroundColor.green()).
                                             arg(m_currentOptions.m_backgroundColor.blue())
                                           );
}

void OptionsDialog::xmlInitDisplaySettings(QDomElement &parentElem, QDomDocument & doc)
{
    // 加入 “Display” 元素
    QDomElement childElem = doc.createElement("Display");
    parentElem.appendChild(childElem);

    // Display.childNode.at(0)
    QDomElement fontStyleElem = doc.createElement("FontStyle");
    fontStyleElem.setAttribute("string",  m_currentOptions.m_font.toString());
    childElem.appendChild(fontStyleElem);

    // Display.childNode.at(1)
    QDomElement colorElem   = doc.createElement("Color");
    QDomElement elem        = doc.createElement("text");
    elem.setAttribute("rgb", QString::number(m_currentOptions.m_textColor.rgb(), 16));
    colorElem.appendChild(elem); // Color.childNode.at(0)

    elem = doc.createElement("background");
    elem.setAttribute("rgb", QString::number(m_currentOptions.m_backgroundColor.rgb(), 16));
    colorElem.appendChild(elem); // Color.childNode.at(1)
    childElem.appendChild(colorElem);
}

void OptionsDialog::xmlSaveDisPlaySettings(QDomElement &parentElem)
{
    QDomElement  elem;
    QDomNodeList nodeList;

    nodeList = parentElem.elementsByTagName("Display"); // 找出“Display”元素
    nodeList = nodeList.at(0).childNodes();             // 因为我设计的时候不允许同级存在同名的节点，所以第一个就是我们想要的“Display”元素节点

    /** index[0] font */
    elem = nodeList.at(0).toElement();
    elem.setAttribute("string", m_currentOptions.m_font.toString());

    /** index[1] color */
    nodeList = nodeList.at(1).childNodes();
    nodeList.at(0).toElement().setAttribute("rgb", QString::number(m_currentOptions.m_textColor.rgb(), 16));         // text color
    nodeList.at(1).toElement().setAttribute("rgb", QString::number(m_currentOptions.m_backgroundColor.rgb(), 16));   // background color
}

void OptionsDialog::xmlLoadDisPlaySettings(const QDomElement &parentElem)
{
    QDomElement  elem;
    QDomNodeList nodeList;

    nodeList = parentElem.elementsByTagName("Display"); // 找出“Display”元素
    nodeList = nodeList.at(0).childNodes();             // 因为我设计的时候不允许同级存在同名的节点，所以第一个就是我们想要的“Display”元素节点

    /** index[0] font */
    elem = nodeList.at(0).toElement();
    m_currentOptions.m_font.fromString(elem.attribute("string"));

    /** index[1] color */
    nodeList = nodeList.at(1).childNodes();

    elem = nodeList.at(0).toElement();  // text color
    m_currentOptions.m_textColor = QColor::fromRgb(elem.attribute("rgb").toUInt(nullptr, 16));

    elem = nodeList.at(1).toElement();  // background color
    m_currentOptions.m_backgroundColor = QColor::fromRgb(elem.attribute("rgb").toUInt(nullptr, 16));
}


OptionsDialog::~OptionsDialog()
{
    delete m_ui;
}

bool OptionsDialog::xmlInitOptions(const QString xmlFile)
{
    QDomDocument doc;

    if (false == xmlHelper::xmlRead(xmlFile, doc)) return false;

    QDomElement root = doc.documentElement();      // 返回根节点
    QDomNodeList nodeList = root.elementsByTagName(QString(XML_NODE_OPTIONS));
    QDomElement parentElem = nodeList.at(0).toElement();

    // 如果没有 XML_NODE_SERIAL 这个节点，则创建一个
    if (parentElem.isNull())
    {
        parentElem = doc.createElement(QString(XML_NODE_OPTIONS));
        root.appendChild(parentElem);
    }

    if (!parentElem.hasChildNodes())
    {
        xmlInitDisplaySettings(parentElem, doc);

        return xmlHelper::xmlWrite(xmlFile, doc);
    }

    return true;
}

bool OptionsDialog::xmlSaveOptions(const QString xmlFile)
{
    QDomDocument doc;

    if (false == xmlHelper::xmlRead(xmlFile, doc)) return false;

    QDomElement  root       = doc.documentElement();      // 返回根节点
    QDomNodeList nodeList   = root.elementsByTagName(QString(XML_NODE_OPTIONS));
    QDomElement  parentElem = nodeList.at(0).toElement();   // options元素

    xmlSaveDisPlaySettings(parentElem);

    return xmlHelper::xmlWrite(xmlFile, doc);
}

bool OptionsDialog::xmlLoadOptions(const QString xmlFile)
{
    QDomDocument doc;

    if (false == xmlHelper::xmlRead(xmlFile, doc)) return false;

    QDomElement  root       = doc.documentElement();      // 返回根节点
    QDomNodeList nodeList   = root.elementsByTagName(QString(XML_NODE_OPTIONS));
    QDomElement  parentElem = nodeList.at(0).toElement();   // options元素

    xmlLoadDisPlaySettings(parentElem);

    m_uncertainOptions = m_currentOptions;
    updateOptions();

    return true;
}

void OptionsDialog::on_fontConfigButton_clicked()
{
    bool ok = true;
    QFont font = QFontDialog::getFont(&ok, m_currentOptions.m_font, this, tr("Font"));
    if (ok)
    {
        QString str = font.family() + ", " + font.styleName() + ", " + QString::number(font.pointSize()) + "pt";
        m_ui->fontLineEdit->setText(str);
        m_uncertainOptions.m_font = font;
    }

}

void OptionsDialog::on_okButton_clicked()
{
    m_currentOptions = m_uncertainOptions;

    accept();
}

void OptionsDialog::on_cancelButton_clicked()
{
    if (m_isChanged)
    {
        m_isChanged = false;
        updateOptions();
        accept();
    }
    else
    {
        updateOptions();
        reject();
    }
}

void OptionsDialog::on_applyButton_clicked()
{
    m_currentOptions = m_uncertainOptions;
    m_isChanged = true;
}

void OptionsDialog::on_textColorToolButton_clicked()
{
    QColor color = QColorDialog::getColor(m_currentOptions.m_textColor, this,
                                          tr("Text Color"), QColorDialog::ShowAlphaChannel);
    if (color != QColor::Invalid)
    {
        QString str = "background-color: rgb(%1, %2, %3);";
        m_ui->textColorToolButton->setStyleSheet(str.arg(color.red()).
                                                 arg(color.green()).
                                                 arg(color.blue())
                                                 );

        m_uncertainOptions.m_textColor = color;
    }


}

void OptionsDialog::on_bgColorToolButton_clicked()
{
    QColor color = QColorDialog::getColor(m_currentOptions.m_backgroundColor, nullptr,
                                          tr("Background Color"), QColorDialog::ShowAlphaChannel);
    if (color != QColor::Invalid)
    {
        QString str = "background-color: rgb(%1, %2, %3);";
        m_ui->bgColorToolButton->setStyleSheet(str.arg(color.red()).
                                                 arg(color.green()).
                                                 arg(color.blue())
                                                 );

        m_uncertainOptions.m_backgroundColor = color;
    }
}

void OptionsDialog::on_defaultColorButton_clicked()
{
    m_uncertainOptions.m_textColor.setRgb(0, 0, 0);
    m_uncertainOptions.m_backgroundColor.setRgb(255, 255, 255);

    QString str = "background-color: rgb(%1, %2, %3);";
    m_ui->textColorToolButton->setStyleSheet(str.arg(m_uncertainOptions.m_textColor.red()).
                                             arg(m_uncertainOptions.m_textColor.green()).
                                             arg(m_uncertainOptions.m_textColor.blue())
                                             );

    str = "background-color: rgb(%1, %2, %3);";
    m_ui->bgColorToolButton->setStyleSheet(str.arg(m_uncertainOptions.m_backgroundColor.red()).
                                             arg(m_uncertainOptions.m_backgroundColor.green()).
                                             arg(m_uncertainOptions.m_backgroundColor.blue())
                                           );
}

void OptionsDialog::closeEvent(QCloseEvent *event)
{


    if (m_isChanged)
    {
        m_isChanged = false;
        setResult(Accepted); // 设置退出窗口后返回的结果值
    }
    else
    {
        setResult(Rejected); // 设置退出窗口后返回的结果值
    }

    updateOptions();

    Q_UNUSED(event);
    //event->accept(); // 默认就是accept()，所以这里可以不同手动accept()
}
