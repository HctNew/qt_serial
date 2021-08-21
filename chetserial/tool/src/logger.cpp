#include "logger.h"

#include <QFile>
#include <QDir>
#include <QDate>
#include <QTextStream>

void logger::write(const QString &fileDir, const QString &msg)
{
    QDir dir;
    QString currentPath;

    currentPath = dir.currentPath();

    if (!dir.exists(fileDir))
    {
        dir.mkpath(fileDir);
    }

    dir.setCurrent(fileDir);

    // 获取年月日作为文件名
    QDate   nowDate  = QDate::currentDate();
    QString fileName = nowDate.toString("yyyy-MM-dd") + ".txt";

    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream text_stream(&file);
    text_stream << msg;
    file.flush();
    file.close();

    dir.setCurrent(currentPath);
}
