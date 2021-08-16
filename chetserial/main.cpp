#include "mainwindow.h"
#include <QApplication>

#include <QDebug>
#include <QFile>
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString strLanguageFile;

    strLanguageFile = QString(":/language/zh_CN.qm");

    QTranslator *m_translator = new QTranslator;
    if (QFile(strLanguageFile).exists())
    {
        m_translator->load(strLanguageFile);
        qApp->installTranslator(m_translator);
    }
    else
    {
        qDebug() << "[houqd] authclient language file does not exists ...";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
