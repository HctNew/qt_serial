#include "mainwindow.h"
#include <QApplication>

#include <QDebug>
#include <QFile>
#include <QDomDocument>
#include <QTranslator>
#include <QMessageBox>
#include "xmlhelper.h"

static bool loadLanguage(QObject *parent);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    xmlHelper::initXml();
    if (false == loadLanguage(&a))
    {
        QMessageBox::critical(nullptr, QObject::tr("Error"),
                                      QObject::tr("Load language Failed!"));
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}



/**
 * @brief load xml language setting
 * @param parent    指定下父对象，方便退出时析构QTranslator对象，避免内存泄漏
 * @return
 */
static bool loadLanguage(QObject *parent)
{
    QDomDocument doc;
    QTranslator *m_translator = new QTranslator(parent);

    do
    {
        if (false == xmlHelper::xmlRead(XML_FILE, doc)) break;

        QDomElement  root       = doc.documentElement();      // 返回根节点
        QDomNodeList nodeList   = root.elementsByTagName(QString(XML_NODE_MAINWINDOW));

        nodeList  = nodeList.at(0).toElement().elementsByTagName("language");

        QDomNode node = nodeList.at(0).firstChild();

        // 如果xml里有 XML_NODE_LANGUAGE 配置并且配置为中文，则加载中文翻译包
        if ((!node.isNull()) && (node.nodeValue() != QString(XML_LANGUAGE_DEFAULT) ))
        {
            QString strLanguageFile(":/language/zh_CN.qm");
            if (QFile(strLanguageFile).exists())
            {
                m_translator->load(strLanguageFile);
                qApp->installTranslator(m_translator);

                // 虽然可以用deleteLater自动管理内存析构，但这里不合适，因为当主函数中的MainWindow初始化完成
                // 后，m_tanslator就暂时没有翻译的作用了，系统就会自动调用delete析构，但是如果程序运行起来后
                // 有些需要实时翻译的文本就不起作用了。
                //m_translator->deleteLater();
                return true;
            }
        }

    }while(0);


    QString strLanguageFile(":/language/en_CN.qm");
    // 否则加载英文翻译包
    if (QFile(strLanguageFile).exists())
    {
        m_translator->load(strLanguageFile);
        qApp->installTranslator(m_translator);
        //m_translator->deleteLater();
        return true;
    }


    return false;
}




