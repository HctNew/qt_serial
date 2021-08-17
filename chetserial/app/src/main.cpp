#include "mainwindow.h"
#include <QApplication>

#include <QDebug>
#include <QFile>
#include <QDomDocument>
#include <QTranslator>
#include <QMessageBox>
#include "xmlhelper.h"

static bool LoadLanguage(void);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (false == LoadLanguage())
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
 * @return bool
 */
static bool LoadLanguage(void)
{
    QDomDocument doc;
    QTranslator *m_translator = new QTranslator;


    do
    {
        if (false == xmlHelper::xmlRead(XML_FILE, doc)) break;

        QDomElement  root  = doc.documentElement();      // 返回根节点
        QDomNodeList tmp   = root.elementsByTagName(QString(XML_NODE_LANGUAGE));
        QDomNode     node  = tmp.at(0).firstChild();


        // 如果xml里有 XML_NODE_LANGUAGE 配置并且配置为中文，则加载中文翻译包
        if ( (tmp.length() == 1) && (!node.isNull()) && (node.nodeValue() == QString("Chinese") ))
        {
            QString strLanguageFile(":/language/zh_CN.qm");
            if (QFile(strLanguageFile).exists())
            {
                m_translator->load(strLanguageFile);
                qApp->installTranslator(m_translator);
                m_translator->deleteLater(); // 使用deleteLater自动管理析构
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
        m_translator->deleteLater();
        return true;
    }

    return false;
}


